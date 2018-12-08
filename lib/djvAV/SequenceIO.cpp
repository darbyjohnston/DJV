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
            namespace
            {
                const size_t timeout = 10;

            } // namespace

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
                    while (_queue && _p->running)
                    {
                        bool read = false;
                        Timestamp seek = -1;
                        {
                            std::unique_lock<std::mutex> lock(_queue->getMutex());
                            if (_p->queueCV.wait_for(
                                lock,
                                std::chrono::milliseconds(timeout),
                                [this]
                            {
                                return (_queue->getVideoCount() < _queue->getVideoMax()) || _p->seek != -1;
                            }))
                            {
                                read = true;
                                if (_p->seek != -1)
                                {
                                    seek = _p->seek;
                                    _p->seek = -1;
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
                                    _p->infoPromise.set_value(info);
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
                _p->thread.join();
            }

            std::future<Info> ISequenceRead::getInfo()
            {
                return _p->infoPromise.get_future();
            }

            void ISequenceRead::seek(Timestamp value)
            {
                std::lock_guard<std::mutex> lock(_queue->getMutex());
                _p->seek = value;
                _p->queueCV.notify_one();
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

                _p->fileInfo.setPath(fileName);
                _p->fileInfo.evalSequence();
                if (_p->fileInfo.isSequenceValid())
                {
                    auto sequence = _p->fileInfo.getSequence();
                    if (sequence.ranges.size())
                    {
                        sequence.sort();
                        _p->frameNumber = sequence.ranges[0].min;
                    }
                }
                _p->info = info;

                _p->offscreenSurface.reset(new QOffscreenSurface);
                QSurfaceFormat surfaceFormat = QSurfaceFormat::defaultFormat();
                surfaceFormat.setSwapBehavior(QSurfaceFormat::SingleBuffer);
                surfaceFormat.setSamples(1);
                _p->offscreenSurface->setFormat(surfaceFormat);
                _p->offscreenSurface->create();

                _p->openGLContext.reset(new QOpenGLContext);
                _p->openGLContext->setFormat(surfaceFormat);
                _p->openGLContext->create();
                _p->openGLContext->moveToThread(this);

                _p->openGLDebugLogger.reset(new QOpenGLDebugLogger);
                connect(
                    _p->openGLDebugLogger.data(),
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
                _p->running = true;
                _p->openGLContext->makeCurrent(_p->offscreenSurface.data());
                if (_p->openGLContext->format().testOption(QSurfaceFormat::DebugContext))
                {
                    _p->openGLDebugLogger->initialize();
                    _p->openGLDebugLogger->startLogging();
                }
                _start();
                while (_p->running)
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
                                _p->running = false;
                            }
                        }
                    }
                    if (image)
                    {
                        if (auto context = _context.lock())
                        {
                            try
                            {
                                const auto fileName = _p->fileInfo.getFileName(_p->frameNumber);
                                if (_p->frameNumber != Core::Frame::Invalid)
                                {
                                    ++_p->frameNumber;
                                }

                                //std::stringstream ss;
                                //ss << "Writing: " << fileName;
                                //context->log("djv::AV::IO::ISequenceWrite", ss.str());

                                _open(fileName, _p->info);
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
                        msleep(timeout);
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
