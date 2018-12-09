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

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/Path.h>
#include <djvCore/String.h>
#include <djvCore/Timer.h>

#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLDebugLogger>

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
                const std::shared_ptr<Core::Context> & context)
            {
                IRead::_init(fileName, queue, context);
                IRead::_init(fileName, queue, context);
                _p->thread = std::thread(
                    [this, fileName]
                {
                    DJV_PRIVATE_PTR();
                    Core::FileInfo fileInfo(fileName);
                    fileInfo.evalSequence();

                    AV::Speed speed;
                    Duration duration = 0;
                    std::vector<Core::Frame::Number> frames;
                    Core::Frame::Index frameIndex = Core::Frame::Invalid;
                    if (fileInfo.isSequenceValid())
                    {
                        frames = Core::Frame::toFrames(fileInfo.getSequence());
                        if (frames.size())
                        {
                            frameIndex = 0;
                            duration = speed.getDuration() * frames.size();
                        }
                    }

                    _start();
                    const auto timeout = Core::Timer::getValue(Core::Timer::Value::Fast);
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
                            try
                            {
                                Core::Frame::Number frameNumber = Core::Frame::Invalid;
                                if (frameIndex != Core::Frame::Invalid)
                                {
                                    frameNumber = Core::Frame::getFrame(frames, frameIndex);
                                    ++frameIndex;
                                }
                                auto fileName = fileInfo.getFileName(frameNumber);

                                AVRational r;
                                r.num = speed.getDuration();
                                r.den = speed.getScale();
                                pts = av_rescale_q(frameNumber * speed.getDuration(), r, FFmpeg::getTimeBaseQ());

                                if (auto context = _context.lock())
                                {
                                    //std::stringstream ss;
                                    //ss << _fileName << ", read frame: " << pts;
                                    //context->log("djv::AV::IO::ISequenceRead", ss.str());
                                }

                                const auto info = _open(fileName, speed, duration);
                                if (Core::Frame::Invalid == frameIndex || 0 == frameIndex)
                                {
                                    p.infoPromise.set_value(info);
                                }
                                image = _read();
                                _close();
                            }
                            catch (const Core::Error & error)
                            {
                                if (auto context = _context.lock())
                                {
                                    context->log("djv::AV::IO::ISequenceRead", error.what(), Core::LogLevel::Error);
                                }
                            }
                            if (image)
                            {
                                std::lock_guard<std::mutex> lock(_queue->getMutex());
                                _queue->addVideo(pts, image);
                            }
                            if (Core::Frame::Invalid == frameIndex || frameIndex >= static_cast<Core::Frame::Index>(frames.size()))
                            {
                                std::lock_guard<std::mutex> lock(_queue->getMutex());
                                _queue->setFinished(true);
                            }
                        }
                    }
                    _exit();
                });
            }
            
            ISequenceRead::ISequenceRead() :
                _p(new Private)
            {}

            ISequenceRead::~ISequenceRead()
            {
                if (_p->thread.joinable())
                {
                    _p->thread.join();
                }
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
                Core::FileInfo fileInfo;
                Core::Frame::Number frameNumber = Core::Frame::Invalid;
                Info info;
                QScopedPointer<QOffscreenSurface> offscreenSurface;
                QScopedPointer<QOpenGLContext> openGLContext;
                QScopedPointer<QOpenGLDebugLogger> openGLDebugLogger;
                std::atomic<bool> running;
            };

            void ISequenceWrite::_init(
                const std::string & fileName,
                const Info & info,
                const std::shared_ptr<Queue> & queue,
                const std::shared_ptr<Core::Context> & context)
            {
                IWrite::_init(fileName, info, queue, context);

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
                p.info = info;

                p.offscreenSurface.reset(new QOffscreenSurface);
                QSurfaceFormat surfaceFormat = QSurfaceFormat::defaultFormat();
                surfaceFormat.setSwapBehavior(QSurfaceFormat::SingleBuffer);
                surfaceFormat.setSamples(1);
                p.offscreenSurface->setFormat(surfaceFormat);
                p.offscreenSurface->create();

                p.openGLContext.reset(new QOpenGLContext);
                p.openGLContext->setFormat(surfaceFormat);
                p.openGLContext->create();
                p.openGLContext->moveToThread(this);

                p.openGLDebugLogger.reset(new QOpenGLDebugLogger);
                connect(
                    p.openGLDebugLogger.data(),
                    &QOpenGLDebugLogger::messageLogged,
                    this,
                    &ISequenceWrite::_debugLogMessage);
                
                start();
            }

            ISequenceWrite::ISequenceWrite() :
                _p(new Private)
            {}

            ISequenceWrite::~ISequenceWrite()
            {
                _p->running = false;
                wait();
            }

            void ISequenceWrite::run()
            {
                DJV_PRIVATE_PTR();
                p.running = true;
                p.openGLContext->makeCurrent(p.offscreenSurface.data());
                if (p.openGLContext->format().testOption(QSurfaceFormat::DebugContext))
                {
                    p.openGLDebugLogger->initialize();
                    p.openGLDebugLogger->startLogging();
                }
                _start();
                const auto timeout = Core::Timer::getValue(Core::Timer::Value::Fast);
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
                        if (auto context = _context.lock())
                        {
                            try
                            {
                                const auto fileName = p.fileInfo.getFileName(p.frameNumber);
                                if (p.frameNumber != Core::Frame::Invalid)
                                {
                                    ++p.frameNumber;
                                }

                                //std::stringstream ss;
                                //ss << "Writing: " << fileName;
                                //context->log("djv::AV::IO::ISequenceWrite", ss.str());

                                _open(fileName, p.info);
                                _write(image);
                                _close();
                            }
                            catch (const std::exception & e)
                            {
                                context->log("djv::AV::IO::ISequenceWrite", e.what(), Core::LogLevel::Error);
                            }
                        }
                    }
                    else
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
                    }
                }
                _exit();
            }

            void ISequenceWrite::_debugLogMessage(const QOpenGLDebugMessage & message)
            {
                if (auto context = _context.lock())
                {
                    context->log("djv::AV::IO::ISequenceWrite", message.message().toStdString());
                }
            }

        } // namespace IO
    } // namespace AV
} // namespace djv
