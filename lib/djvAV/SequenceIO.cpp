// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/SequenceIO.h>

#include <djvGL/ImageConvert.h>

#include <djvAV/SpeedFunc.h>

#include <djvSystem/Context.h>
#include <djvSystem/File.h>
#include <djvSystem/FileInfo.h>
#include <djvSystem/LogSystem.h>
#include <djvSystem/Path.h>
#include <djvSystem/TextSystem.h>
#include <djvSystem/TimerFunc.h>

#include <djvCore/OSFunc.h>
#include <djvCore/String.h>
#include <djvCore/StringFormat.h>

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
                Math::Frame::Number frame = Math::Frame::invalid;
                std::shared_ptr<Image::Image> image;
            };

            struct ISequenceRead::Private
            {
                Math::Frame::Number frame = Math::Frame::invalid;
                std::promise<Info> infoPromise;
                std::vector<std::future<Future> > cacheFutures;
                std::condition_variable queueCV;
                Direction direction = Direction::Forward;
                Math::Frame::Number seek = Math::Frame::invalid;
                std::thread thread;
                std::atomic<bool> running;
                std::chrono::steady_clock::time_point infoTimer;
            };

            void ISequenceRead::_init(
                const System::File::Info& fileInfo,
                const ReadOptions& options,
                const std::shared_ptr<System::TextSystem>& textSystem,
                const std::shared_ptr<System::ResourceSystem>& resourceSystem,
                const std::shared_ptr<System::LogSystem>& logSystem)
            {
                IRead::_init(fileInfo, options, textSystem, resourceSystem, logSystem);
                _speed = fromSpeed(getDefaultSpeed());
                _p->running = true;
                _p->thread = std::thread(
                    [this]
                {
                    DJV_PRIVATE_PTR();

                    // Get the sequence.
                    size_t sequenceFrameCount = 0;
                    p.frame = Math::Frame::invalid;
                    if (System::File::Type::Sequence == _fileInfo.getType())
                    {
                        _sequence = _fileInfo.getSequence();
                        sequenceFrameCount = _sequence.getFrameCount();
                        if (sequenceFrameCount)
                        {
                            p.frame = 0;
                        }
                    }

                    // Read the file information.
                    Math::Frame::Number frameNumber = Math::Frame::invalid;
                    if (sequenceFrameCount)
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
                            _logSystem->log("djv::AV::IO::ISequenceRead", e.what(), System::LogLevel::Error);
                        }
                    }

                    // Start looping...
                    p.infoTimer = std::chrono::steady_clock::now();
                    const auto timeout = System::getTimerValue(System::TimerValue::VeryFast);
                    while (p.running)
                    {
                        // Update the options.
                        size_t threadCount = 4;
                        bool playback = false;
                        bool loop = false;
                        InOutPoints inOutPoints;
                        bool cacheEnabled = false;
                        size_t cacheMaxByteCount = 0;
                        {
                            std::lock_guard<std::mutex> lock(_mutex);
                            threadCount = _threadCount;
                            playback = _playback;
                            loop = _loop;
                            inOutPoints = _inOutPoints;
                            cacheEnabled = _cacheEnabled;
                            cacheMaxByteCount = _cacheMaxByteCount;
                        }
                        if (!cacheEnabled)
                        {
                            _cache.clear();
                        }
                        if (info.video.size() && _options.layer < info.video.size())
                        {
                            const size_t dataByteCount = info.video[_options.layer].getDataByteCount();
                            _cache.setMax(dataByteCount ? (cacheMaxByteCount / dataByteCount) : 0);
                            _cache.setSequenceSize(info.videoSequence.getFrameCount());
                            _cache.setInOutPoints(inOutPoints);
                        }
                        else
                        {
                            _cache.setMax(0);
                        }

                        // Check to see if there is work to be done.
                        size_t queueCount = 0;
                        Math::Frame::Number seek = Math::Frame::invalid;
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
                                if (p.seek != Math::Frame::invalid)
                                {
                                    seek = p.seek;
                                    p.seek = Math::Frame::invalid;
                                    _videoQueue.setFinished(false);
                                    _videoQueue.clearFrames();
                                }
                            }
                        }
                        if (seek != Math::Frame::invalid)
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
                            read = _readQueue(queueCount, loop, cacheEnabled);
                        }

                        // Fill the cache.
                        if (cacheEnabled)
                        {
                            _readCache(playback ? (threadCount / 2) : threadCount, inOutPoints);
                        }

                        // Update information.
                        const auto now = std::chrono::steady_clock::now();
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

            void ISequenceRead::seek(Math::Frame::Number value, Direction direction)
            {
                DJV_PRIVATE_PTR();
                {
                    std::lock_guard<std::mutex> lock(_mutex);
                    p.seek = value;
                    _direction = direction;
                }
                p.queueCV.notify_one();
            }

            bool ISequenceRead::hasCache() const
            {
                return _sequence.getFrameCount() > 1;
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
                const bool seek = _p->seek != Math::Frame::invalid;
                const bool direction = _p->direction != _direction;
                return queue || seek || direction;
            }

            size_t ISequenceRead::_getQueueCount(size_t threadCount) const
            {
                const size_t queueMax = _videoQueue.getMax() - _videoQueue.getCount();
                return std::min(queueMax, threadCount);
            }

            std::future<ISequenceRead::Future> ISequenceRead::_getFuture(Math::Frame::Number i, std::string fileName)
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
                            _logSystem->log(
                                "djv::AV::ISequenceRead",
                                String::Format("{0}: {1}").arg(fileName).arg(e.what()),
                                System::LogLevel::Error);
                        }
                        return out;
                    });
            }

            size_t ISequenceRead::_readQueue(size_t count, bool loop, bool cacheEnabled)
            {
                DJV_PRIVATE_PTR();

                // Get frames to be added to the queue.
                const size_t sequenceFrameCount = _sequence.getFrameCount();
                std::vector<std::pair<Math::Frame::Number, std::shared_ptr<Image::Image> > > images;
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
                        if (sequenceFrameCount)
                        {
                            if (p.frame >= 0 && p.frame < sequenceFrameCount)
                            {
                                const Math::Frame::Number frameNumber = _sequence.getFrame(p.frame);
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

                    if (sequenceFrameCount)
                    {
                        switch (p.direction)
                        {
                        case Direction::Forward:
                            ++p.frame;
                            if (loop && p.frame >= sequenceFrameCount)
                            {
                                p.frame = 0;
                            }
                            break;
                        case Direction::Reverse:
                            --p.frame;
                            if (loop && p.frame < 0)
                            {
                                p.frame = sequenceFrameCount - 1;
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
                    images.push_back(std::make_pair(result.frame, result.image));
                    if (cacheEnabled)
                    {
#if defined(DJV_MMAP)
                        result.image->detach();
#endif // DJV_MMAP
                        _cache.add(result.frame, result.image);
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

                if (Math::Frame::invalid == p.frame || p.frame < 0 || p.frame >= static_cast<Math::Frame::Number>(sequenceFrameCount))
                {
                    std::lock_guard<std::mutex> lock(_mutex);
                    _videoQueue.setFinished(true);
                }

                return futures.size();
            }

            void ISequenceRead::_readCache(size_t count, const AV::IO::InOutPoints& inOutPoints)
            {
                DJV_PRIVATE_PTR();

                // Get frames to be added to the cache.
                Math::Frame::Number frame = Math::Frame::invalid;
                {
                    std::lock_guard<std::mutex> lock(_mutex);
                    if (_videoQueue.getCount())
                    {
                        frame = _videoQueue.getFrame().frame;
                    }
                }
                if (count > 0 && frame != Math::Frame::invalid)
                {
                    const size_t sequenceFrameCount = _sequence.getFrameCount();
                    const auto range = inOutPoints.getRange(sequenceFrameCount);
                    _cache.setDirection(p.direction);
                    _cache.setCurrentFrame(frame);
                    const size_t readBehind = _cache.getReadBehind();
                    switch (p.direction)
                    {
                    case Direction::Forward:
                    {
                        for (size_t i = 0; i < readBehind; ++i)
                        {
                            --frame;
                            if (frame < range.getMin())
                            {
                                frame = range.getMax();
                            }
                        }
                        const size_t max = std::min(_cache.getMax(), sequenceFrameCount);
                        for (size_t i = 0; i < max && p.cacheFutures.size() < count; ++i)
                        {
                            if (!_cache.contains(frame))
                            {
                                const std::string fileName = _fileInfo.getFileName(_sequence.getFrame(frame));
                                p.cacheFutures.push_back(_getFuture(frame, fileName));
                            }
                            ++frame;
                            if (frame > range.getMax())
                            {
                                frame = range.getMin();
                            }
                        }
                        break;
                    }
                    case Direction::Reverse:
                    {
                        for (size_t i = 0; i < readBehind; ++i)
                        {
                            ++frame;
                            if (frame > range.getMax())
                            {
                                frame = range.getMin();
                            }
                        }
                        const size_t max = std::min(_cache.getMax(), sequenceFrameCount);
                        for (Math::Frame::Number i = 0; i < max && p.cacheFutures.size() < count; ++i)
                        {
                            if (!_cache.contains(frame))
                            {
                                const std::string fileName = _fileInfo.getFileName(_sequence.getFrame(frame));
                                p.cacheFutures.push_back(_getFuture(frame, fileName));
                            }
                            --frame;
                            if (frame < range.getMin())
                            {
                                frame = range.getMax();
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
#if defined(DJV_MMAP)
                        result.image->detach();
#endif // DJV_MMAP
                        _cache.add(result.frame, result.image);
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
                System::File::Info fileInfo;
                Math::Frame::Number frameNumber = Math::Frame::invalid;
                GLFWwindow * glfwWindow = nullptr;
                std::shared_ptr<GL::ImageConvert> convert;
                std::thread thread;
                std::atomic<bool> running;
            };

            void ISequenceWrite::_init(
                const System::File::Info& fileInfo,
                const Info& info,
                const WriteOptions& options,
                const std::shared_ptr<System::TextSystem>& textSystem,
                const std::shared_ptr<System::ResourceSystem>& resourceSystem,
                const std::shared_ptr<System::LogSystem>& logSystem)
            {
                IWrite::_init(fileInfo, info, options, textSystem, resourceSystem, logSystem);

                DJV_PRIVATE_PTR();

                _info = info;
                if (_info.video.size())
                {
                    _imageInfo = _info.video[0];
                }

                p.fileInfo = fileInfo;
                if (System::File::Type::Sequence == p.fileInfo.getType())
                {
                    auto sequence = p.fileInfo.getSequence();
                    if (sequence.isValid())
                    {
                        p.frameNumber = sequence.getRanges()[0].getMin();
                    }
                }

#if defined(DJV_GL_ES2)
                glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else // DJV_GL_ES2
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
                glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif // DJV_GL_ES2
                glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
                int env = 0;
                if (OS::getIntEnv("DJV_GL_DEBUG", env) && env != 0)
                {
                    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
                }
                p.glfwWindow = glfwCreateWindow(100, 100, "djv::IO::ISequenceWrite", NULL, NULL);
                if (!p.glfwWindow)
                {
                    throw System::File::Error(_textSystem->getText(DJV_TEXT("error_glfw_window_creation")));
                }

                p.running = true;
                p.thread = std::thread(
                    [this]
                {
                    DJV_PRIVATE_PTR();
                    try
                    {
                        glfwMakeContextCurrent(p.glfwWindow);
#if defined(DJV_GL_ES2)
                        if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
#else // DJV_GL_ES2
                        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
#endif // DJV_GL_ES2
                        {
                            throw System::File::Error(_textSystem->getText(DJV_TEXT("error_glad_init")));
                        }

                        p.convert = GL::ImageConvert::create(_textSystem, _resourceSystem);

                        const auto timeout = System::getTimerValue(System::TimerValue::VeryFast);
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
                                    if (p.frameNumber != Math::Frame::invalid)
                                    {
                                        ++p.frameNumber;
                                    }
                                    auto image = images[i];
                                    const Image::Type imageType = _getImageType(image->getType());
                                    if (Image::Type::None == imageType)
                                    {
                                        throw System::File::Error(String::Format("{0}: {1}").
                                            arg(fileName).
                                            arg(_textSystem->getText(DJV_TEXT("error_unsupported_image_type"))));
                                    }
                                    const Image::Layout imageLayout = _getImageLayout();
                                    if (imageType != image->getType() || imageLayout != image->getLayout())
                                    {
                                        const Image::Info imageInfo(image->getSize(), imageType, imageLayout);
                                        auto tmp = Image::Image::create(imageInfo);
                                        tmp->setTags(image->getTags());
                                        p.convert->process(*image, imageInfo, *tmp);
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
                                        _logSystem->log(
                                            "djv::AV::ISequenceWrite",
                                            String::Format("{0}: {1}").arg(result.fileName).arg(result.errorString),
                                            System::LogLevel::Error);
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
                    catch (const std::exception& e)
                    {
                        _logSystem->log("djv::AV::ISequenceWrite", e.what(), System::LogLevel::Error);
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
