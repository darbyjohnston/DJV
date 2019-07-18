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

#include <djvCore/Cache.h>
#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/LogSystem.h>
#include <djvCore/OS.h>
#include <djvCore/Path.h>
#include <djvCore/String.h>
#include <djvCore/Timer.h>

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
                const size_t threadCount = 4;

                typedef Memory::Cache<Frame::Index, std::shared_ptr<Image::Image> > MemoryCache;

                std::vector<Time::TimestampRange> _getCachedTimestamps(const MemoryCache& cache, const Time::Speed& speed)
                {
                    std::vector<Time::TimestampRange> out;
                    auto frames = cache.getKeys();
                    const size_t size = frames.size();
                    if (size)
                    {
                        std::sort(frames.begin(), frames.end());
                        Frame::Index rangeStart = frames[0];
                        Frame::Index prevFrame = frames[0];
                        size_t i = 1;
                        for (; i < size; prevFrame = frames[i], ++i)
                        {
                            if (frames[i] != prevFrame + 1)
                            {
                                const Time::Timestamp t0 = frameToTimestamp(rangeStart, speed);
                                const Time::Timestamp t1 = frameToTimestamp(prevFrame, speed);
                                out.push_back(Time::TimestampRange(t0, t1));
                                rangeStart = frames[i];
                            }
                        }
                        if (size > 1)
                        {
                            Time::Timestamp t0 = frameToTimestamp(rangeStart, speed);
                            Time::Timestamp t1 = frameToTimestamp(prevFrame, speed);
                            out.push_back(Time::TimestampRange(t0, t1));
                        }
                        else
                        {
                            Time::Timestamp t = frameToTimestamp(rangeStart, speed);
                            out.push_back(Time::TimestampRange(t));
                        }
                    }
                    return out;
                }

            } // namespace

            struct ISequenceRead::Future
            {
                Frame::Index frameIndex = Frame::Invalid;
                std::shared_ptr<Image::Image> image;
            };

            struct ISequenceRead::Private
            {
                Frame::Index frameIndex = 0;
                std::promise<Info> infoPromise;
                bool cacheEnabled = false;
                size_t cacheMax = 0;
                std::vector<Time::TimestampRange> cachedTimestamps;
                MemoryCache cache;
                std::vector<std::future<Future> > cacheFutures;
                std::condition_variable queueCV;
                Time::Timestamp seek = -1;
                std::thread thread;
                std::atomic<bool> running;
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

                    // Get the list of frames.
                    p.frameIndex = Frame::Invalid;
                    if (_fileInfo.isSequenceValid())
                    {
                        _frames = Frame::toFrames(_fileInfo.getSequence());
                        if (_frames.size())
                        {
                            p.frameIndex = 0;
                            const int64_t f = Time::scale(1, _speed.swap(), Time::getTimebaseRational());
                            _duration = f * _frames.size();
                        }
                    }

                    // Read the file information.
                    Frame::Number frameNumber = Frame::Invalid;
                    if (_frames.size())
                    {
                        frameNumber = Frame::getFrame(_frames, p.frameIndex);
                    }
                    Info info;
                    std::string fileName = _fileInfo.getFileName(frameNumber);
                    try
                    {
                        info = _readInfo(fileName);
                        p.infoPromise.set_value(info);
                    }
                    catch (const std::exception & e)
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
                        catch (const std::exception & e)
                        {}
                    }

                    // Start looping...
                    const auto timeout = Time::getValue(Time::TimerValue::Fast);
                    while (p.running)
                    {
                        // Update the cache options.
                        bool cacheEnabled = false;
                        size_t cacheMax = 0;
                        {
                            std::lock_guard<std::mutex> lock(_mutex);
                            cacheEnabled = p.cacheEnabled;
                            cacheMax = p.cacheMax;
                        }
                        if (!cacheEnabled)
                        {
                            p.cache.clear();
                        }
                        if (info.video.size() && _options.layer < info.video.size())
                        {
                            const size_t dataByteCount = info.video[_options.layer].info.getDataByteCount();
                            p.cache.setMax(cacheMax / dataByteCount);
                        }
                        else
                        {
                            p.cache.setMax(0);
                        }
                        p.cachedTimestamps = _getCachedTimestamps(p.cache, _speed);

                        // Check to see if the queue needs to be filled up or a seek occurred.
                        size_t queueCount = 0;
                        Time::Timestamp seek = -1;
                        {
                            std::unique_lock<std::mutex> lock(_mutex);
                            if (p.queueCV.wait_for(
                                lock,
                                std::chrono::milliseconds(timeout),
                                [this]
                                {
                                    const bool fillQueue = (_videoQueue.getFrameCount() < _videoQueue.getMax()) && !_videoQueue.isFinished();
                                    return fillQueue || (_p->seek != -1);
                                }))
                            {
                                const size_t queueMax = _videoQueue.getMax() - _videoQueue.getFrameCount();
                                const size_t frameMax = _frames.size() ? (_frames.size() - static_cast<size_t>(p.frameIndex)) : 1;
                                queueCount = std::min(std::min(queueMax, threadCount / 2), frameMax);
                                if (p.seek != -1)
                                {
                                    seek = p.seek;
                                    p.seek = -1;
                                    _videoQueue.setFinished(false);
                                    _videoQueue.clearFrames();
                                }
                            }
                        }
                        if (seek != -1)
                        {
                            p.frameIndex = Time::timestampToFrame(seek, _speed);
                            /*{
                                std::stringstream ss;
                                ss << _fileName << ": seek " << p.frameIndex;
                                _logSystem->log("djv::AV::IO::ISequenceRead", ss.str());
                            }*/
                        }

                        // Fill the queue.
                        const size_t read = _readQueue(queueCount, cacheEnabled);

                        // Fill the cache.
                        if (cacheEnabled)
                        {
                            _readCache(threadCount / 2 - read);
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

            void ISequenceRead::seek(Time::Timestamp value)
            {
                DJV_PRIVATE_PTR();
                {
                    std::lock_guard<std::mutex> lock(_mutex);
                    p.seek = value;
                }
                p.queueCV.notify_one();
            }

            bool ISequenceRead::hasCache() const
            {
                return true;
            }

            bool ISequenceRead::isCacheEnabled() const
            {
                return _p->cacheEnabled;
            }

            void ISequenceRead::setCacheEnabled(bool value)
            {
                std::lock_guard<std::mutex> lock(_mutex);
                _p->cacheEnabled = value;
            }

            size_t ISequenceRead::getCacheMax() const
            {
                return _p->cacheMax;
            }

            void ISequenceRead::setCacheMax(size_t value)
            {
                std::lock_guard<std::mutex> lock(_mutex);
                _p->cacheMax = value;
            }

            std::vector<Core::Time::TimestampRange> ISequenceRead::getCachedTimestamps()
            {
                std::lock_guard<std::mutex> lock(_mutex);
                return _p->cachedTimestamps;
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

            std::future<ISequenceRead::Future> ISequenceRead::_getFuture(Frame::Index i, std::string fileName)
            {
                return std::async(
                    std::launch::async,
                    [this, i, fileName]
                    {
                        Future out;
                        out.frameIndex = i;
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
                std::map<Frame::Index, std::shared_ptr<Image::Image> > images;
                std::vector<std::future<Future> > futures;
                for (size_t i = 0; i < count; ++i)
                {
                    const Time::Timestamp timestamp = frameToTimestamp(_frames.size() ? p.frameIndex : 0, _speed);
                    /*{
                        std::stringstream ss;
                        ss << _fileName << ": read timestamp " << timestamp;
                        _logSystem->log("djv::AV::IO::ISequenceRead", ss.str());
                    }*/

                    std::shared_ptr<Image::Image> cachedImage;
                    if (cacheEnabled && p.cache.get(p.frameIndex, cachedImage))
                    {
                        images[timestamp] = cachedImage;
                    }
                    else
                    {
                        Frame::Number frameNumber = Frame::Invalid;
                        if (_frames.size())
                        {
                            frameNumber = Frame::getFrame(_frames, p.frameIndex);
                        }
                        const std::string fileName = _fileInfo.getFileName(frameNumber);
                        futures.push_back(_getFuture(p.frameIndex, fileName));
                    }

                    if (_frames.size())
                    {
                        ++p.frameIndex;
                    }
                }

                // Get the results.
                for (auto& future : futures)
                {
                    const auto result = future.get();
                    if (result.image)
                    {
                        const Time::Timestamp timestamp = frameToTimestamp(result.frameIndex != Frame::Invalid ? result.frameIndex : 0, _speed);
                        images[timestamp] = result.image;
                        if (cacheEnabled)
                        {
                            result.image->detach();
                            p.cache.add(result.frameIndex, result.image);
                        }
                    }
                }

                // Add the frames to the queue.
                for (const auto& i : images)
                {
                    std::lock_guard<std::mutex> lock(_mutex);
                    if (_videoQueue.getFrameCount() >= _videoQueue.getMax())
                    {
                        break;
                    }
                    _videoQueue.addFrame(VideoFrame(i.first, i.second));
                }

                if (Frame::Invalid == p.frameIndex || p.frameIndex >= static_cast<Frame::Index>(_frames.size()))
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
                if (count > 0 &&
                    p.cacheFutures.size() < threadCount / 2 &&
                    _frames.size() > 1 &&
                    p.cache.getSize() < _frames.size() &&
                    p.cache.getSize() < p.cache.getMax())
                {
                    Frame::Index i = 0;
                    for (; i < _frames.size() && p.cacheFutures.size() < count; ++i)
                    {
                        if (!p.cache.contains(i))
                        {
                            const std::string fileName = _fileInfo.getFileName(Frame::getFrame(_frames, i));
                            p.cacheFutures.push_back(_getFuture(i, fileName));
                        }
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
                            result.image->detach();
                            p.cache.add(result.frameIndex, result.image);
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
                Frame::Number frameNumber = Frame::Invalid;
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
                    throw std::runtime_error(ss.str());
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
                            throw std::runtime_error(ss.str());
                        }

                        p.convert = Image::Convert::create(_resourceSystem);

                        const auto timeout = Time::getValue(Time::TimerValue::Fast);
                        while (p.running)
                        {
                            std::vector<std::shared_ptr<Image::Image> > images;
                            {
                                std::unique_lock<std::mutex> lock(_mutex, std::try_to_lock);
                                if (lock.owns_lock())
                                {
                                    while (_videoQueue.hasFrames() && images.size() < threadCount)
                                    {
                                        auto frame = _videoQueue.popFrame();
                                        images.push_back(frame.image);
                                    }

                                    if (!_videoQueue.hasFrames() && _videoQueue.isFinished())
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
                                    if (p.frameNumber != Frame::Invalid)
                                    {
                                        ++p.frameNumber;
                                    }
                                    auto image = images[i];
                                    const Image::Type imageType = _getImageType(image->getType());
                                    if (Image::Type::None == imageType)
                                    {
                                        std::stringstream ss;
                                        ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be written") << ".";
                                        throw std::runtime_error(ss.str());
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

        } // namespace IO
    } // namespace AV
} // namespace djv
