//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvAV/SequenceIO.h>

#include <djvAV/ImageConvert.h>

#include <djvCore/Context.h>
#include <djvCore/FileSystem.h>
#include <djvCore/FileInfo.h>
#include <djvCore/LogSystem.h>
#include <djvCore/OS.h>
#include <djvCore/Path.h>
#include <djvCore/String.h>
#include <djvCore/Timer.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <future>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace
            {
                //! \todo Should this be configurable?
                const double infoTimeout = 0.5;

            } // namespace

            struct ISequenceRead::Future
            {
                Frame::Number frame = Frame::invalid;
                std::shared_ptr<Image::Image> image;
            };

            struct ISequenceRead::Private
            {
                Frame::Number frame = Frame::invalid;
                std::promise<Info> infoPromise;
                std::vector<std::future<Future> > cacheFutures;
                std::condition_variable queueCV;
                Direction direction = Direction::Forward;
                Frame::Number seek = Frame::invalid;
                std::thread thread;
                std::atomic<bool> running;
                std::chrono::system_clock::time_point infoTimer;
            };

            void ISequenceRead::_init(
                const FileSystem::FileInfo & fileInfo,
                const ReadOptions& options,
                const std::shared_ptr<ResourceSystem>& resourceSystem,
                const std::shared_ptr<LogSystem>& logSystem)
            {
                IRead::_init(fileInfo, options, resourceSystem, logSystem);
                _speed = Time::Speed();
                _p->running = true;
                _p->thread = std::thread(
                    [this]
                {
                    DJV_PRIVATE_PTR();

                    // Get the sequence.
                    size_t sequenceSize = 0;
                    p.frame = Frame::invalid;
                    if (_fileInfo.isSequenceValid())
                    {
                        _sequence = _fileInfo.getSequence();
                        sequenceSize = _sequence.getSize();
                        if (sequenceSize)
                        {
                            p.frame = 0;
                        }
                    }

                    // Read the file information.
                    Frame::Number frameNumber = Frame::invalid;
                    if (sequenceSize)
                    {
                        frameNumber = _sequence.getFrame(0);
                    }
                    Info info;
                    std::string fileName = _fileInfo.getFileName(frameNumber);
                    try
                    {
                        info = _readInfo(fileName);
                        info.fileName = _fileInfo.getFileName();
                        p.infoPromise.set_value(info);
                    }
                    catch (const std::exception&)
                    {
                        try
                        {
                            {
                                std::lock_guard<std::mutex> lock(_mutex);
                                _videoQueue.setFinished(true);
                                _audioQueue.setFinished(true);
                            }
                            p.running = false;
                            p.infoPromise.set_exception(std::current_exception());
                        }
                        catch (const std::exception& e)
                        {
                            _logSystem->log("djv::AV::IO::ISequenceRead", e.what(), LogLevel::Error);
                        }
                    }

                    // Start looping...
                    p.infoTimer = std::chrono::system_clock::now();
                    const auto timeout = Time::getValue(Time::TimerValue::Fast);
                    while (p.running)
                    {
                        // Update the options.
                        size_t threadCount = 4;
                        bool playback = false;
                        bool inOutPointsEnabled = false;
                        Frame::Index inPoint = Frame::invalid;
                        Frame::Index outPoint = Frame::invalid;
                        bool cacheEnabled = false;
                        size_t cacheMaxByteCount = 0;
                        {
                            std::lock_guard<std::mutex> lock(_mutex);
                            threadCount = _threadCount;
                            playback = _playback;
                            inOutPointsEnabled = _inOutPointsEnabled;
                            inPoint = _inPoint;
                            outPoint = _outPoint;
                            cacheEnabled = _cacheEnabled;
                            cacheMaxByteCount = _cacheMaxByteCount;
                        }
                        if (!cacheEnabled)
                        {
                            _cache.clear();
                        }
                        if (info.video.size() && _options.layer < info.video.size())
                        {
                            const size_t dataByteCount = info.video[_options.layer].info.getDataByteCount();
                            _cache.setMax(dataByteCount ? (cacheMaxByteCount / dataByteCount) : 0);
                            _cache.setSequenceSize(info.video[_options.layer].sequence.getSize());
                            _cache.setInOutPoints(inOutPointsEnabled, inPoint, outPoint);
                        }
                        else
                        {
                            _cache.setMax(0);
                        }

                        // Check to see if there is work to be done.
                        size_t queueCount = 0;
                        Frame::Number seek = Frame::invalid;
                        {
                            std::unique_lock<std::mutex> lock(_mutex);
                            if (p.queueCV.wait_for(
                                lock,
                                std::chrono::milliseconds(timeout),
                                [this]
                                {
                                    return _hasWork();
                                }))
                            {
                                queueCount = _getQueueCount(playback ? (threadCount / 2) : 1);
                                if (p.direction != _direction)
                                {
                                    p.direction = _direction;
                                    _videoQueue.setFinished(false);
                                    _videoQueue.clearFrames();
                                }
                                if (p.seek != Frame::invalid)
                                {
                                    seek = p.seek;
                                    p.seek = Frame::invalid;
                                    _videoQueue.setFinished(false);
                                    _videoQueue.clearFrames();
                                }
                            }
                        }
                        if (seek != Frame::invalid)
                        {
                            p.frame = seek;
                            /*{
                                std::stringstream ss;
                                ss << _fileName << ": seek " << p.frame;
                                _logSystem->log("djv::AV::IO::ISequenceRead", ss.str());
                            }*/
                        }

                        // Fill the queue.
                        size_t read = 0;
                        if (queueCount > 0)
                        {
                            read = _readQueue(queueCount, cacheEnabled);
                        }

                        // Fill the cache.
                        if (cacheEnabled)
                        {
                            _readCache(playback ? (threadCount / 2) : threadCount);
                        }

                        // Update information.
                        const auto now = std::chrono::system_clock::now();
                        std::chrono::duration<double> delta = now - p.infoTimer;
                        if (delta.count() > infoTimeout)
                        {
                            p.infoTimer = now;
                            size_t cacheByteCount = _cache.getTotalByteCount();
                            auto cacheSequence = _cache.getSequence();
                            auto cachedFrames = _cache.getFrames();
                            {
                                std::lock_guard<std::mutex> lock(_mutex);
                                _cacheByteCount = cacheByteCount;
                                _cacheSequence = cacheSequence;
                                _cachedFrames = std::move(cachedFrames);
                            }
                        }
                    }

                    p.running = false;
                });
            }
            
            ISequenceRead::ISequenceRead() :
                _p(new Private)
            {}

            ISequenceRead::~ISequenceRead()
            {}

            bool ISequenceRead::isRunning() const
            {
                return _p->running;
            }

            std::future<Info> ISequenceRead::getInfo()
            {
                return _p->infoPromise.get_future();
            }

            void ISequenceRead::seek(Frame::Number value, Direction direction)
            {
                DJV_PRIVATE_PTR();
                {
                    std::lock_guard<std::mutex> lock(_mutex);
                    p.seek = value;
                    _direction = direction;
                }
                p.queueCV.notify_one();
            }

            void ISequenceRead::_finish()
            {
                DJV_PRIVATE_PTR();
                p.running = false;
                if (p.thread.joinable())
                {
                    //! \todo How do we safely detach the thread here so we don't block?
                    p.thread.join();
                }
            }

            bool ISequenceRead::_hasWork() const
            {
                const bool queue = (_videoQueue.getCount() < _videoQueue.getMax()) && !_videoQueue.isFinished();
                const bool seek = _p->seek != Frame::invalid;
                const bool direction = _p->direction != _direction;
                return queue || seek || direction;
            }

            size_t ISequenceRead::_getQueueCount(size_t threadCount) const
            {
                const size_t queueMax = _videoQueue.getMax() - _videoQueue.getCount();
                return std::min(queueMax, threadCount);
            }

            std::future<ISequenceRead::Future> ISequenceRead::_getFuture(Frame::Number i, std::string fileName)
            {
                return std::async(
                    std::launch::async,
                    [this, i, fileName]
                    {
                        Future out;
                        out.frame = i;
                        try
                        {
                            out.image = _readImage(fileName);
                        }
                        catch (const std::exception& e)
                        {
                            std::stringstream ss;
                            ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be read") << ". " << e.what();
                            _logSystem->log("djv::AV::ISequenceRead", ss.str(), LogLevel::Error);
                        }
                        return out;
                    });
            }

            size_t ISequenceRead::_readQueue(size_t count, bool cacheEnabled)
            {
                DJV_PRIVATE_PTR();

                // Get frames to be added to the queue.
                const size_t sequenceSize = _sequence.getSize();
                std::vector<std::pair<Frame::Number, std::shared_ptr<Image::Image> > > images;
                std::vector<std::future<Future> > futures;
                for (size_t i = 0; i < count; ++i)
                {
                    std::shared_ptr<Image::Image> cachedImage;
                    if (cacheEnabled && _cache.get(p.frame, cachedImage))
                    {
                        images.push_back(std::make_pair(p.frame, cachedImage));
                    }
                    else
                    {
                        if (sequenceSize)
                        {
                            if (p.frame >= 0 && p.frame < sequenceSize)
                            {
                                const Frame::Number frameNumber = _sequence.getFrame(p.frame);
                                const std::string fileName = _fileInfo.getFileName(frameNumber);
                                futures.push_back(_getFuture(p.frame, fileName));
                            }
                        }
                        else
                        {
                            const std::string fileName = _fileInfo.getFileName();
                            futures.push_back(_getFuture(p.frame, fileName));
                        }
                    }

                    if (sequenceSize)
                    {
                        switch (p.direction)
                        {
                        case Direction::Forward:
                            ++p.frame;
                            if (p.frame >= sequenceSize)
                            {
                                p.frame = 0;
                            }
                            break;
                        case Direction::Reverse:
                            --p.frame;
                            if (p.frame < 0)
                            {
                                p.frame = sequenceSize - 1;
                            }
                            break;
                        default: break;
                        }
                    }
                }

                // Get the results.
                for (auto& future : futures)
                {
                    const auto result = future.get();
                    if (result.image)
                    {
                        images.push_back(std::make_pair(result.frame, result.image));
                        if (cacheEnabled)
                        {
#if defined(DJV_MMAP)
                            result.image->detach();
#endif // DJV_MMAP
                            _cache.add(result.frame, result.image);
                        }
                    }
                }

                // Add the frames to the queue.
                {
                    std::lock_guard<std::mutex> lock(_mutex);
                    for (const auto& i : images)
                    {
                        if (_videoQueue.getCount() >= _videoQueue.getMax())
                        {
                            break;
                        }
                        _videoQueue.addFrame(VideoFrame(i.first, i.second));
                    }
                }

                if (Frame::invalid == p.frame || p.frame < 0 || p.frame >= static_cast<Frame::Number>(sequenceSize))
                {
                    std::lock_guard<std::mutex> lock(_mutex);
                    _videoQueue.setFinished(true);
                }

                return futures.size();
            }

            void ISequenceRead::_readCache(size_t count)
            {
                DJV_PRIVATE_PTR();

                // Get frames to be added to the cache.
                Frame::Number frame = Frame::invalid;
                {
                    std::lock_guard<std::mutex> lock(_mutex);
                    if (_videoQueue.getCount())
                    {
                        frame = _videoQueue.getFrame().frame;
                    }
                }
                if (count > 0 && frame != Frame::invalid)
                {
                    _cache.setDirection(p.direction);
                    _cache.setCurrentFrame(frame);
                    const size_t readBehind = _cache.getReadBehind();
                    switch (p.direction)
                    {
                    case Direction::Forward:
                    {
                        const size_t sequenceSize = _sequence.getSize();
                        for (size_t i = 0; i < readBehind; ++i)
                        {
                            --frame;
                            if (frame < 0)
                            {
                                frame = sequenceSize - 1;
                            }
                        }
                        const size_t max = std::min(_cache.getMax(), sequenceSize);
                        for (size_t i = 0; i < max && p.cacheFutures.size() < count; ++i)
                        {
                            if (!_cache.contains(frame))
                            {
                                const std::string fileName = _fileInfo.getFileName(_sequence.getFrame(frame));
                                p.cacheFutures.push_back(_getFuture(frame, fileName));
                            }
                            ++frame;
                            if (frame >= sequenceSize)
                            {
                                frame = 0;
                            }
                        }
                        break;
                    }
                    case Direction::Reverse:
                    {
                        const size_t sequenceSize = _sequence.getSize();
                        for (size_t i = 0; i < readBehind; ++i)
                        {
                            ++frame;
                            if (frame >= sequenceSize)
                            {
                                frame = 0;
                            }
                        }
                        const size_t max = std::min(_cache.getMax(), sequenceSize);
                        for (Frame::Number i = 0; i < max && p.cacheFutures.size() < count; ++i)
                        {
                            if (!_cache.contains(frame))
                            {
                                const std::string fileName = _fileInfo.getFileName(_sequence.getFrame(frame));
                                p.cacheFutures.push_back(_getFuture(frame, fileName));
                            }
                            --frame;
                            if (frame < 0)
                            {
                                frame = sequenceSize - 1;
                            }
                        }
                        break;
                    }
                    default: break;
                    }
                }

                // Get the results.
                auto i = p.cacheFutures.begin();
                while (i != p.cacheFutures.end())
                {
                    if (i->valid() &&
                        i->wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        const auto result = i->get();
                        if (result.image)
                        {
#if defined(DJV_MMAP)
                            result.image->detach();
#endif // DJV_MMAP
                            _cache.add(result.frame, result.image);
                        }
                        i = p.cacheFutures.erase(i);
                    }
                    else
                    {
                        ++i;
                    }
                }
            }

            struct ISequenceWrite::Private
            {
                FileSystem::FileInfo fileInfo;
                Frame::Number frameNumber = Frame::invalid;
                GLFWwindow * glfwWindow = nullptr;
                std::shared_ptr<Image::Convert> convert;
                std::thread thread;
                std::atomic<bool> running;
            };

            void ISequenceWrite::_init(
                const FileSystem::FileInfo& fileInfo,
                const Info& info,
                const WriteOptions& options,
                const std::shared_ptr<ResourceSystem>& resourceSystem,
                const std::shared_ptr<LogSystem>& logSystem)
            {
                IWrite::_init(fileInfo, info, options, resourceSystem, logSystem);

                DJV_PRIVATE_PTR();

                _info = info;
                if (_info.video.size())
                {
                    _imageInfo = _info.video[0].info;
                }

                p.fileInfo = fileInfo;
                if (p.fileInfo.isSequenceValid())
                {
                    auto sequence = p.fileInfo.getSequence();
                    if (sequence.ranges.size())
                    {
                        sequence.sort();
                        p.frameNumber = sequence.ranges[0].min;
                    }
                }

#if defined(DJV_OPENGL_ES2)
                glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else // DJV_OPENGL_ES2
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
                glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif // DJV_OPENGL_ES2
                glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
                if (OS::getIntEnv("DJV_OPENGL_DEBUG") != 0)
                {
                    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
                }
                p.glfwWindow = glfwCreateWindow(100, 100, "djv::IO::ISequenceWrite", NULL, NULL);
                if (!p.glfwWindow)
                {
                    std::stringstream ss;
                    ss << DJV_TEXT("Cannot create GLFW window.");
                    throw FileSystem::Error(ss.str());
                }

                p.running = true;
                p.thread = std::thread(
                    [this]
                {
                    DJV_PRIVATE_PTR();
                    try
                    {
                        glfwMakeContextCurrent(p.glfwWindow);
#if defined(DJV_OPENGL_ES2)
                        if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
#else
                        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
#endif
                        {
                            std::stringstream ss;
                            ss << "Cannot initialize GLAD.";
                            throw FileSystem::Error(ss.str());
                        }

                        p.convert = Image::Convert::create(_resourceSystem);

                        const auto timeout = Time::getValue(Time::TimerValue::VeryFast);
                        while (p.running)
                        {
                            std::vector<std::shared_ptr<Image::Image> > images;
                            {
                                std::unique_lock<std::mutex> lock(_mutex, std::try_to_lock);
                                if (lock.owns_lock())
                                {
                                    while (!_videoQueue.isEmpty() && images.size() < _threadCount)
                                    {
                                        auto frame = _videoQueue.popFrame();
                                        images.push_back(frame.image);
                                    }
                                    if (_videoQueue.isEmpty() && _videoQueue.isFinished())
                                    {
                                        p.running = false;
                                    }
                                }
                            }
                            if (images.size())
                            {
                                struct Future
                                {
                                    std::string fileName;
                                    bool error = false;
                                    std::string errorString;
                                };
                                std::vector<std::future<Future> > futures;
                                for (size_t i = 0; i < images.size(); ++i)
                                {
                                    const auto fileName = p.fileInfo.getFileName(p.frameNumber);
                                    /*{
                                        std::stringstream ss;
                                        ss << "Writing: " << fileName;
                                        _logSystem->log("djv::AV::IO::ISequenceWrite", ss.str());
                                    }*/
                                    if (p.frameNumber != Frame::invalid)
                                    {
                                        ++p.frameNumber;
                                    }
                                    auto image = images[i];
                                    const Image::Type imageType = _getImageType(image->getType());
                                    if (Image::Type::None == imageType)
                                    {
                                        std::stringstream ss;
                                        ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be written") << ".";
                                        throw FileSystem::Error(ss.str());
                                    }
                                    const Image::Layout imageLayout = _getImageLayout();
                                    if (imageType != image->getType() || imageLayout != image->getLayout())
                                    {
                                        const Image::Info info(image->getSize(), imageType, imageLayout);
                                        auto tmp = Image::Image::create(info);
                                        tmp->setTags(image->getTags());
                                        p.convert->process(*image, info, *tmp);
                                        image = tmp;
                                    }
                                    futures.push_back(std::async(
                                        std::launch::async,
                                        [this, fileName, image]
                                        {
                                            Future out;
                                            out.fileName = fileName;
                                            try
                                            {
                                                _write(fileName, image);
                                            }
                                            catch (const std::exception& e)
                                            {
                                                out.error = true;
                                                out.errorString = e.what();
                                            }
                                            return out;
                                        }));
                                }
                                for (auto& future : futures)
                                {
                                    const auto result = future.get();
                                    if (result.error)
                                    {
                                        std::stringstream ss;
                                        ss << DJV_TEXT("The file") << " '" << result.fileName << "' " <<
                                            DJV_TEXT("cannot be written") << ". " << result.errorString;
                                        _logSystem->log("djv::AV::ISequenceWrite", ss.str(), LogLevel::Error);
                                        p.running = false;
                                    }
                                }
                            }
                            else
                            {
                                std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
                            }
                        }

                        p.convert.reset();
                    }
                    catch (const std::exception & e)
                    {
                        _logSystem->log("djv::AV::ISequenceWrite", e.what(), LogLevel::Error);
                    }

                    p.running = false;
                });
            }

            ISequenceWrite::ISequenceWrite() :
                _p(new Private)
            {}

            ISequenceWrite::~ISequenceWrite()
            {}

            bool ISequenceWrite::isRunning() const
            {
                return _p->running;
            }

            Image::Type ISequenceWrite::_getImageType(Image::Type value) const
            {
                return value;
            }

            Image::Layout ISequenceWrite::_getImageLayout() const
            {
                return Image::Layout();
            }

            void ISequenceWrite::_finish()
            {
                DJV_PRIVATE_PTR();
                p.running = false;
                if (p.thread.joinable())
                {
                    //! \todo How do we safely detach the thread here so we don't block?
                    p.thread.join();
                }
                if (p.glfwWindow)
                {
                    glfwDestroyWindow(p.glfwWindow);
                }
            }

            ISequencePlugin::~ISequencePlugin()
            {}

            bool ISequencePlugin::canSequence() const
            {
                return true;
            }

        } // namespace IO
    } // namespace AV
} // namespace djv
