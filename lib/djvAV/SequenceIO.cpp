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
                //! \todo [1.0 S] Should this be configurable?
                const size_t threadCount = 4;

            } // namespace

            struct ISequenceRead::Private
            {
                std::promise<Info> infoPromise;
                std::condition_variable queueCV;
                Time::Timestamp seek = -1;
                std::thread thread;
                std::atomic<bool> running;
            };

            void ISequenceRead::_init(
                const std::string & fileName,
                const std::shared_ptr<ResourceSystem>& resourceSystem,
                const std::shared_ptr<LogSystem>& logSystem)
            {
                IRead::_init(fileName, resourceSystem, logSystem);
                _speed = Time::Speed();
                _p->running = true;
                _p->thread = std::thread(
                    [this]
                {
                    DJV_PRIVATE_PTR();
                    FileSystem::FileInfo fileInfo(_fileName);
                    fileInfo.evalSequence();
                    Frame::Index frameIndex = Frame::Invalid;
                    if (fileInfo.isSequenceValid())
                    {
                        _frames = Frame::toFrames(fileInfo.getSequence());
                        if (_frames.size())
                        {
                            frameIndex = 0;
                            _duration = Time::frameToTimestamp(_frames.size(), _speed);
                        }
                    }

                    Frame::Number frameNumber = Frame::Invalid;
                    if (frameIndex != Frame::Invalid)
                    {
                        frameNumber = Frame::getFrame(_frames, frameIndex);
                    }
                    Info info;
                    try
                    {
                        info = _readInfo(fileInfo.getFileName(frameNumber));
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
                        {
                            _logSystem->log("djv::AV::ISequenceRead", e.what(), LogLevel::Error);
                        }
                        _logSystem->log("djv::AV::ISequenceRead", e.what(), LogLevel::Error);
                    }

                    const auto timeout = Time::getValue(Time::TimerValue::Fast);
                    while (p.running)
                    {
                        size_t read = 0;
                        Time::Timestamp seek = -1;
                        {
                            std::unique_lock<std::mutex> lock(_mutex);
                            if (p.queueCV.wait_for(
                                lock,
                                std::chrono::milliseconds(timeout),
                                [this]
                            {
                                return (_videoQueue.isFinished() ? false : (_videoQueue.getFrameCount() < _videoQueue.getMax())) || _p->seek != -1;
                            }))
                            {
                                read = std::min(_videoQueue.getMax() - _videoQueue.getFrameCount(), threadCount);
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
                            frameIndex = Time::timestampToFrame(seek, _speed);
                            /*{
                                std::stringstream ss;
                                ss << _fileName << ": seek " << frameIndex;
                                _logSystem->log("djv::AV::IO::ISequenceRead", ss.str());
                            }*/
                        }
                        struct Future
                        {
                            Frame::Number frameNumber = Frame::Invalid;
                            Time::Timestamp pts = 0;
                            std::shared_ptr<Image::Image> image;
                        };
                        std::vector<std::future<Future> > futures;
                        for (size_t i = 0; i < read && (Frame::Invalid == frameIndex || frameIndex < _frames.size()); ++i)
                        {
                            Frame::Number frameNumber = Frame::Invalid;
                            if (frameIndex != Frame::Invalid)
                            {
                                frameNumber = Frame::getFrame(_frames, frameIndex);
                                ++frameIndex;
                            }
                            Time::Timestamp pts = Time::frameToTimestamp(frameNumber, _speed);
                            /*{
                                std::stringstream ss;
                                ss << _fileName << ": read frame " << pts;
                                _logSystem->log("djv::AV::IO::ISequenceRead", ss.str());
                            }*/
                            auto fileName = fileInfo.getFileName(frameNumber);
                            futures.push_back(std::async(
                                std::launch::async,
                                [this, frameNumber, pts, fileName]
                                {
                                    Future out;
                                    out.frameNumber = frameNumber;
                                    out.pts = pts;
                                    try
                                    {
                                        out.image = _readImage(fileName);
                                    }
                                    catch (const std::exception& e)
                                    {
                                        _logSystem->log("djv::AV::ISequenceRead", e.what(), LogLevel::Error);
                                    }
                                    return out;
                                }));
                        }

                        for (auto& future : futures)
                        {
                            const auto result = future.get();
                            if (result.image)
                            {
                                std::lock_guard<std::mutex> lock(_mutex);
                                _videoQueue.addFrame(result.pts, result.image);
                            }
                        }

                        if (Frame::Invalid == frameIndex || frameIndex >= static_cast<Frame::Index>(_frames.size()))
                        {
                            std::lock_guard<std::mutex> lock(_mutex);
                            _videoQueue.setFinished(true);
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

            void ISequenceRead::_finish()
            {
                DJV_PRIVATE_PTR();
                p.running = false;
                if (p.thread.joinable())
                {
                    p.thread.join();
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
                const std::string& fileName,
                const Info& info,
                const std::shared_ptr<ResourceSystem>& resourceSystem,
                const std::shared_ptr<LogSystem>& logSystem)
            {
                IWrite::_init(fileName, info, resourceSystem, logSystem);

                DJV_PRIVATE_PTR();

                _info = info;
                if (_info.video.size())
                {
                    _imageInfo = _info.video[0].info;
                }

                p.fileInfo.setPath(fileName);
                p.fileInfo.evalSequence();
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
                                        images.push_back(frame.second);
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
                                        std::stringstream s;
                                        s << DJV_TEXT("The file") <<
                                            " '" << fileName << "' " << DJV_TEXT("cannot be written") << ".";
                                        throw std::runtime_error(s.str());
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
                                        _logSystem->log("djv::AV::ISequenceWrite", result.errorString, LogLevel::Error);
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
