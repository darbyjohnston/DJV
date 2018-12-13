//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvAV/FFmpeg.h>
#include <djvAV/PixelProcess.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/OS.h>
#include <djvCore/Path.h>
#include <djvCore/String.h>
#include <djvCore/Timer.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            struct ISequenceRead::Private
            {
                std::promise<Info> infoPromise;
                std::condition_variable queueCV;
                Timestamp seek = -1;
                std::thread thread;
                std::atomic<bool> running;
            };

            void ISequenceRead::_init(
                const std::string & fileName,
                const std::shared_ptr<Queue> & queue,
                Context * context)
            {
                IRead::_init(fileName, queue, context);

                _p->running = true;
                _p->thread = std::thread(
                    [this, fileName, context]
                {
                    DJV_PRIVATE_PTR();
                    try
                    {
                        FileInfo fileInfo(fileName);
                        fileInfo.evalSequence();
                        Frame::Index frameIndex = Frame::Invalid;
                        if (fileInfo.isSequenceValid())
                        {
                            _frames = Frame::toFrames(fileInfo.getSequence());
                            if (_frames.size())
                            {
                                frameIndex = 0;
                                _duration = _speed.getDuration() * _frames.size();
                            }
                        }

                        Frame::Number frameNumber = Frame::Invalid;
                        if (frameIndex != Frame::Invalid)
                        {
                            frameNumber = Frame::getFrame(_frames, frameIndex);
                        }
                        p.infoPromise.set_value(_readInfo(fileInfo.getFileName(frameNumber)));

                        const auto timeout = Timer::getValue(Timer::Value::Fast);
                        while (_queue && p.running)
                        {
                            bool read = false;
                            Timestamp seek = -1;
                            {
                                std::unique_lock<std::mutex> lock(_queue->getMutex());
                                if (p.queueCV.wait_for(
                                    lock,
                                    std::chrono::milliseconds(timeout),
                                    [this]
                                {
                                    return (_queue->getVideoCount() < _queue->getVideoMax()) || _p->seek != -1;
                                }))
                                {
                                    read = true;
                                    if (p.seek != -1)
                                    {
                                        seek = p.seek;
                                        p.seek = -1;
                                        _queue->setFinished(false);
                                        _queue->clear();
                                    }
                                }
                            }
                            if (seek != -1)
                            {
                                //! \todo File sequence seeking?
                            }
                            if (read)
                            {
                                std::shared_ptr<Image> image;
                                Timestamp pts = 0;
                                Frame::Number frameNumber = Frame::Invalid;
                                if (frameIndex != Frame::Invalid)
                                {
                                    frameNumber = Frame::getFrame(_frames, frameIndex);
                                    ++frameIndex;
                                }
                                auto fileName = fileInfo.getFileName(frameNumber);

                                AVRational r;
                                r.num = _speed.getDuration();
                                r.den = _speed.getScale();
                                pts = av_rescale_q(frameNumber * _speed.getDuration(), r, FFmpeg::getTimeBaseQ());

                                /*{
                                    std::stringstream ss;
                                    ss << _fileName << ": read frame " << pts;
                                    context->log("djv::AV::IO::ISequenceRead", ss.str());
                                }*/

                                image = _readImage(fileName);

                                {
                                    std::lock_guard<std::mutex> lock(_queue->getMutex());
                                    _queue->addVideo(pts, image);
                                }

                                if (Frame::Invalid == frameIndex || frameIndex >= static_cast<Frame::Index>(_frames.size()))
                                {
                                    std::lock_guard<std::mutex> lock(_queue->getMutex());
                                    _queue->setFinished(true);
                                    if (_queue->hasCloseOnFinish())
                                    {
                                        p.running = false;
                                    }
                                }
                            }
                        }
                    }
                    catch (const std::exception & e)
                    {
                        p.infoPromise.set_value(Info());
                        context->log("djv::AV::ISequenceRead", e.what(), LogLevel::Error);
                    }
                });
            }
            
            ISequenceRead::ISequenceRead() :
                _p(new Private)
            {}

            ISequenceRead::~ISequenceRead()
            {
                DJV_PRIVATE_PTR();
                p.running = false;
                if (p.thread.joinable())
                {
                    p.thread.join();
                }
            }

            bool ISequenceRead::isRunning() const
            {
                return _p->running;
            }

            std::future<Info> ISequenceRead::getInfo()
            {
                return _p->infoPromise.get_future();
            }

            void ISequenceRead::seek(Timestamp value)
            {
                DJV_PRIVATE_PTR();
                std::lock_guard<std::mutex> lock(_queue->getMutex());
                p.seek = value;
                p.queueCV.notify_one();
            }

            struct ISequenceWrite::Private
            {
                FileInfo fileInfo;
                Frame::Number frameNumber = Frame::Invalid;
                GLFWwindow * glfwWindow = nullptr;
                std::thread thread;
                std::atomic<bool> running;
            };

            void ISequenceWrite::_init(
                const std::string & fileName,
                const Info & info,
                const std::shared_ptr<Queue> & queue,
                Context * context)
            {
                IWrite::_init(fileName, info, queue, context);

                _info = info;
                if (_info.video.size())
                {
                    _pixelInfo = _info.video[0].info;
                }

                DJV_PRIVATE_PTR();
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

                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
                glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
                if (!OS::getEnv("DJV_OPENGL_DEBUG").empty())
                {
                    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
                }
                p.glfwWindow = glfwCreateWindow(100, 100, context->getName().c_str(), NULL, NULL);
                if (!p.glfwWindow)
                {
                    std::stringstream ss;
                    ss << DJV_TEXT("Cannot create GLFW window.");
                    throw std::runtime_error(ss.str());
                }

                p.running = true;
                p.thread = std::thread(
                    [this, context]
                {
                    DJV_PRIVATE_PTR();
                    try
                    {
                        glfwMakeContextCurrent(p.glfwWindow);
                        glbinding::initialize(glfwGetProcAddress);

                        _convert = Pixel::Convert::create(context);

                        const auto timeout = Timer::getValue(Timer::Value::Fast);
                        while (p.running)
                        {
                            std::shared_ptr<Image> image;
                            {
                                std::unique_lock<std::mutex> lock(_queue->getMutex(), std::try_to_lock);
                                if (lock.owns_lock())
                                {
                                    if (_queue->hasVideo())
                                    {
                                        image = _queue->getVideo().second;
                                        _queue->popVideo();
                                    }
                                    else if (_queue->isFinished())
                                    {
                                        p.running = false;
                                    }
                                }
                            }
                            if (image)
                            {
                                const auto fileName = p.fileInfo.getFileName(p.frameNumber);
                                if (p.frameNumber != Frame::Invalid)
                                {
                                    ++p.frameNumber;
                                }

                                /*{
                                    std::stringstream ss;
                                    ss << "Writing: " << fileName;
                                    context->log("djv::AV::IO::ISequenceWrite", ss.str());
                                }*/

                                _write(fileName, image);
                            }
                            else
                            {
                                std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
                            }
                        }

                        _convert.reset();
                    }
                    catch (const std::exception & e)
                    {
                        context->log("djv::AV::ISequenceWrite", e.what(), LogLevel::Error);
                    }
                });
            }

            ISequenceWrite::ISequenceWrite() :
                _p(new Private)
            {}

            ISequenceWrite::~ISequenceWrite()
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

            bool ISequenceWrite::isRunning() const
            {
                return _p->running;
            }

        } // namespace IO
    } // namespace AV
} // namespace djv
