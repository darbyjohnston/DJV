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

#include <djvAV/PNG.h>

#include <djvCore/Context.h>
#include <djvCore/FileIO.h>
#include <djvCore/FileInfo.h>

#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLDebugLogger>

#include <sstream>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace PNG
            {
                namespace
                {
                    const size_t timeout = 10;

                } // namespace

                struct Write::Private
                {
                    Core::FileInfo fileInfo;
                    Core::Frame::Number frameNumber = Core::Frame::Invalid;
                    Pixel::Info info;
                    QScopedPointer<QOffscreenSurface> offscreenSurface;
                    QScopedPointer<QOpenGLContext> openGLContext;
                    QScopedPointer<QOpenGLDebugLogger> openGLDebugLogger;
                    std::atomic<bool> running;

                    FILE * f = nullptr;
                    png_structp png = nullptr;
                    png_infop pngInfo = nullptr;
                    ErrorStruct pngError;
                };

                void Write::_init(
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

                    const auto & videoInfo = info.getVideo();
                    if (!videoInfo.size())
                    {
                        std::stringstream s;
                        s << pluginName << " cannot write: " << fileName;
                        throw std::runtime_error(s.str());
                    }
                    Pixel::Type pixelType = Pixel::Type::None;
                    switch (videoInfo[0].getInfo().getType())
                    {
                    case Pixel::Type::L_U8:
                    case Pixel::Type::L_U16:
                    case Pixel::Type::LA_U8:
                    case Pixel::Type::LA_U16:
                    case Pixel::Type::RGB_U8:
                    case Pixel::Type::RGB_U16:
                    case Pixel::Type::RGBA_U8:
                    case Pixel::Type::RGBA_U16: pixelType = videoInfo[0].getInfo().getType(); break;
                    case Pixel::Type::L_U32:
                    case Pixel::Type::L_F16:
                    case Pixel::Type::L_F32:    pixelType = Pixel::Type::L_U16; break;
                    case Pixel::Type::LA_U32:
                    case Pixel::Type::LA_F16:
                    case Pixel::Type::LA_F32:   pixelType = Pixel::Type::LA_U16; break;
                    case Pixel::Type::RGB_U32:
                    case Pixel::Type::RGB_F16:
                    case Pixel::Type::RGB_F32:  pixelType = Pixel::Type::RGB_U16; break;
                    case Pixel::Type::RGBA_U32:
                    case Pixel::Type::RGBA_F16:
                    case Pixel::Type::RGBA_F32: pixelType = Pixel::Type::RGBA_U16; break;
                    default: break;
                    }
                    if (Pixel::Type::None == pixelType)
                    {
                        std::stringstream s;
                        s << pluginName << " cannot write: " << fileName;
                        throw std::runtime_error(s.str());
                    }
                    _p->info = Pixel::Info(videoInfo[0].getInfo().getSize(), pixelType);

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
                        &Write::_debugLogMessage);
                }

                Write::Write() :
                    _p(new Private)
                {}

                Write::~Write()
                {
                    _p->running = false;
                }

                std::shared_ptr<Write> Write::create(const std::string & fileName, const Info & info, const std::shared_ptr<Queue> & queue, const std::shared_ptr<Core::Context> & context)
                {
                    auto out = std::shared_ptr<Write>(new Write);
                    out->_init(fileName, info, queue, context);
                    out->start();
                    return out;
                }

                namespace
                {
                    bool pngScanline(png_structp png, const uint8_t* in)
                    {
                        if (setjmp(png_jmpbuf(png)))
                            return false;
                        png_write_row(png, (png_byte *)in);
                        return true;
                    }

                    bool pngEnd(png_structp png, png_infop pngInfo)
                    {
                        if (setjmp(png_jmpbuf(png)))
                            return false;
                        png_write_end(png, pngInfo);
                        return true;
                    }

                } // namespace

                void Write::run()
                {
                    _p->openGLContext->makeCurrent(_p->offscreenSurface.data());
                    if (_p->openGLContext->format().testOption(QSurfaceFormat::DebugContext))
                    {
                        _p->openGLDebugLogger->initialize();
                        _p->openGLDebugLogger->startLogging();
                    }
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
                                    const std::string fileName = _p->fileInfo.getFileName(_p->frameNumber);
                                    if (_p->frameNumber != Core::Frame::Invalid)
                                    {
                                        ++_p->frameNumber;
                                    }

                                    std::stringstream ss;
                                    ss << "Writing: " << fileName;
                                    context->log("djv::AV::IO::PNG::Write", ss.str());

                                    std::shared_ptr<Pixel::Data> pixelData(image);
                                    if (pixelData->getInfo() != _p->info)
                                    {
                                        pixelData = pixelData->convert(_p->info.getType(), context);
                                    }

                                    _open(fileName);
                                    const auto & size = pixelData->getSize();
                                    for (int y = 0; y < size.y; ++y)
                                    {
                                        if (!pngScanline(_p->png, pixelData->getData(y)))
                                        {
                                            std::stringstream s;
                                            s << pluginName << " cannot write: " << _fileName << ": " << _p->pngError.msg;
                                            throw std::runtime_error(s.str());
                                        }
                                    }
                                    if (!pngEnd(_p->png, _p->pngInfo))
                                    {
                                        std::stringstream s;
                                        s << pluginName << " cannot write: " << _fileName << ": " << _p->pngError.msg;
                                        throw std::runtime_error(s.str());
                                    }
                                }
                                catch (const std::exception & e)
                                {
                                    context->log("djv::AV::IO::PNG::Write", e.what(), Core::LogLevel::Error);
                                }
                                _close();
                            }
                        }
                        else
                        {
                            std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
                        }
                    }
                }

                void Write::_debugLogMessage(const QOpenGLDebugMessage & message)
                {
                    if (auto context = _context.lock())
                    {
                        context->log("djv::AV::IO::PNG::Write", message.message().toStdString());
                    }
                }

                namespace
                {
                    bool pngOpen(
                        FILE *              f,
                        png_structp         png,
                        png_infop *         pngInfo,
                        const Pixel::Info & info)
                    {
                        if (setjmp(png_jmpbuf(png)))
                        {
                            return false;
                        }
                        *pngInfo = png_create_info_struct(png);
                        if (!*pngInfo)
                        {
                            return false;
                        }
                        png_init_io(png, f);

                        int colorType = 0;
                        switch (info.getGLFormat())
                        {
                        case GL_R: colorType = PNG_COLOR_TYPE_GRAY; break;
                        case GL_RG: colorType = PNG_COLOR_TYPE_GRAY_ALPHA; break;
                        case GL_RGB: colorType = PNG_COLOR_TYPE_RGB; break;
                        case GL_RGBA: colorType = PNG_COLOR_TYPE_RGB_ALPHA; break;
                        default: break;
                        }

                        png_set_IHDR(
                            png,
                            *pngInfo,
                            info.getWidth(),
                            info.getHeight(),
                            static_cast<int>(Pixel::getBitDepth(info.getType())),
                            colorType,
                            PNG_INTERLACE_NONE,
                            PNG_COMPRESSION_TYPE_DEFAULT,
                            PNG_FILTER_TYPE_DEFAULT);
                        png_write_info(png, *pngInfo);
                        return true;
                    }

                } // namespace

                void Write::_open(const std::string& fileName)
                {
                    _p->png = png_create_write_struct(
                        PNG_LIBPNG_VER_STRING,
                        &_p->pngError,
                        djvPngError,
                        djvPngWarning);
                    if (!_p->png)
                    {
                        std::stringstream s;
                        s << pluginName << " cannot open: " << fileName << ": " << _p->pngError.msg;
                        throw std::runtime_error(s.str());
                    }
                    _p->f = Core::fopen(fileName.c_str(), "wb");
                    if (!_p->f)
                    {
                        std::stringstream s;
                        s << pluginName << " cannot open: " << fileName;
                        throw std::runtime_error(s.str());
                    }
                    if (!pngOpen(_p->f, _p->png, &_p->pngInfo, _p->info))
                    {
                        std::stringstream s;
                        s << pluginName << " cannot open: " << fileName << ": " << _p->pngError.msg;
                        throw std::runtime_error(s.str());
                    }
                    if (Pixel::getBitDepth(_p->info.getType()) > 8 && Core::Memory::Endian::LSB == Core::Memory::getEndian())
                    {
                        png_set_swap(_p->png);
                    }
                }

                void Write::_close()
                {
                    if (_p->png || _p->pngInfo)
                    {
                        png_destroy_write_struct(
                            _p->png ? &_p->png : nullptr,
                            _p->pngInfo ? &_p->pngInfo : nullptr);
                        _p->png = nullptr;
                        _p->pngInfo = nullptr;
                    }
                    if (_p->f)
                    {
                        fclose(_p->f);
                        _p->f = nullptr;
                    }
                }

            } // namespace PNG
        } // namespace IO
    } // namespace AV
} // namespace djv

