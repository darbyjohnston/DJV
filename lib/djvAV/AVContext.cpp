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

#include <djvAV/AVContext.h>

#include <djvAV/CineonPlugin.h>
#include <djvAV/DPXPlugin.h>
#include <djvAV/IFFPlugin.h>
#include <djvAV/IFLPlugin.h>
#include <djvAV/Image.h>
#include <djvAV/IO.h>
#include <djvAV/LUTPlugin.h>
#include <djvAV/OpenGLImage.h>
#include <djvAV/PICPlugin.h>
#include <djvAV/PPMPlugin.h>
#include <djvAV/RLAPlugin.h>
#include <djvAV/SGIPlugin.h>
#include <djvAV/TargaPlugin.h>
#include <djvAV/WAVPlugin.h>
#if defined(JPEG_FOUND)
#include <djvAV/JPEGPlugin.h>
#endif // JPEG_FOUND
#if defined(PNG_FOUND)
#include <djvAV/PNGPlugin.h>
#endif // PNG_FOUND
#if defined(TIFF_FOUND)
#include <djvAV/TIFFPlugin.h>
#endif // TIFF_FOUND
#if defined(OPENEXR_FOUND)
#include <djvAV/OpenEXRPlugin.h>
#endif // OPENEXR_FOUND
#if defined(FFMPEG_FOUND)
#include <djvAV/FFmpegPlugin.h>
#endif // FFMPEG_FOUND

#include <djvCore/DebugLog.h>
#include <djvCore/Error.h>

#include <QCoreApplication>
#include <QMetaType>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLDebugLogger>
#include <QScopedPointer>
#include <QVector>

#include <sstream>

namespace djv
{
    namespace AV
    {
        struct AVContext::Private
        {
            QScopedPointer<QOffscreenSurface> offscreenSurface;
            QScopedPointer<QOpenGLContext> openGLContext;
            QScopedPointer<QOpenGLDebugLogger> openGLDebugLogger;
            QScopedPointer<IOFactory> ioFactory;
        };

        AVContext::AVContext(int & argc, char ** argv, QObject * parent) :
            Core::CoreContext(argc, argv, parent),
            _p(new Private)
        {
            //DJV_DEBUG("AVContext::AVContext");

            // Register meta types.
            qRegisterMetaType<Image>("djv::AV::Image");
            qRegisterMetaType<IOInfo>("djv::AV::IOInfo");

            // Create the default OpenGL context.
            DJV_LOG(debugLog(), "djv::AV::AVContext", "Creating the default OpenGL context...");

            QSurfaceFormat defaultFormat;
            defaultFormat.setRenderableType(QSurfaceFormat::OpenGL);
            defaultFormat.setMajorVersion(4);
            defaultFormat.setMinorVersion(1);
            defaultFormat.setProfile(QSurfaceFormat::CoreProfile);
            //! \todo Document this environment variable.
            if (Core::System::env("DJV_OPENGL_DEBUG").size())
            {
                defaultFormat.setOption(QSurfaceFormat::DebugContext);
            }
            QSurfaceFormat::setDefaultFormat(defaultFormat);

            _p->offscreenSurface.reset(new QOffscreenSurface);
            QSurfaceFormat surfaceFormat = QSurfaceFormat::defaultFormat();
            surfaceFormat.setSwapBehavior(QSurfaceFormat::SingleBuffer);
            surfaceFormat.setSamples(1);
            _p->offscreenSurface->setFormat(surfaceFormat);
            _p->offscreenSurface->create();
            _p->openGLContext.reset(new QOpenGLContext);
            _p->openGLContext->setFormat(surfaceFormat);
            if (!_p->openGLContext->create())
            {
                throw Core::Error(
                    "djv::AV::AVContext",
                    qApp->translate("djv::AV::AVContext", "Cannot create OpenGL context, found version %1.%2").
                    arg(_p->openGLContext->format().majorVersion()).arg(_p->openGLContext->format().minorVersion()));
            }
            _p->openGLContext->makeCurrent(_p->offscreenSurface.data());
            DJV_LOG(debugLog(), "djv::AV::AVContext",
                QString("OpenGL context valid = %1").arg(_p->openGLContext->isValid()));
            DJV_LOG(debugLog(), "djv::AV::AVContext",
                QString("OpenGL version = %1.%2").
                arg(_p->openGLContext->format().majorVersion()).
                arg(_p->openGLContext->format().minorVersion()));
            if (!_p->openGLContext->versionFunctions<QOpenGLFunctions_3_3_Core>())
            {
                throw Core::Error(
                    "djv::AV::AVContext",
                    qApp->translate("djv::AV::AVContext", "Cannot find OpenGL 3.3 functions, found version %1.%2").
                    arg(_p->openGLContext->format().majorVersion()).arg(_p->openGLContext->format().minorVersion()));
            }

            _p->openGLDebugLogger.reset(new QOpenGLDebugLogger);
            connect(
                _p->openGLDebugLogger.data(),
                &QOpenGLDebugLogger::messageLogged,
                this,
                &AVContext::debugLogMessage);
            if (_p->openGLContext->format().testOption(QSurfaceFormat::DebugContext))
            {
                _p->openGLDebugLogger->initialize();
                _p->openGLDebugLogger->startLogging();
            }

            //! Create the I/O plugins.
            DJV_LOG(debugLog(), "djv::AV::AVContext", "Loading I/O plugins...");

            _p->ioFactory.reset(new IOFactory(this));
            _p->ioFactory->addPlugin(new CineonPlugin(this));
            _p->ioFactory->addPlugin(new DPXPlugin(this));
            _p->ioFactory->addPlugin(new IFFPlugin(this));
            _p->ioFactory->addPlugin(new IFLPlugin(this));
            _p->ioFactory->addPlugin(new LUTPlugin(this));
            _p->ioFactory->addPlugin(new PICPlugin(this));
            _p->ioFactory->addPlugin(new PPMPlugin(this));
            _p->ioFactory->addPlugin(new RLAPlugin(this));
            _p->ioFactory->addPlugin(new SGIPlugin(this));
            _p->ioFactory->addPlugin(new TargaPlugin(this));
            _p->ioFactory->addPlugin(new WAVPlugin(this));
#if defined(JPEG_FOUND)
            _p->ioFactory->addPlugin(new JPEGPlugin(this));
#endif // JPEG_FOUND
#if defined(PNG_FOUND)
            _p->ioFactory->addPlugin(new PNGPlugin(this));
#endif // PNG_FOUND
#if defined(TIFF_FOUND)
            _p->ioFactory->addPlugin(new TIFFPlugin(this));
#endif // TIFF_FOUND
#if defined(OPENEXR_FOUND)
            _p->ioFactory->addPlugin(new OpenEXRPlugin(this));
#endif // OPENEXR_FOUND
#if defined(FFMPEG_FOUND)
            _p->ioFactory->addPlugin(new FFmpegPlugin(this));
#endif // FFMPEG_FOUND

            DJV_LOG(debugLog(), "djv::AV::AVContext", "Information:");
            DJV_LOG(debugLog(), "djv::AV::AVContext", info());
        }

        AVContext::~AVContext()
        {
            //DJV_DEBUG("AVContext::~AVContext");
#if defined(DJV_WINDOWS)
            //! \todo On Windows deleting the factory causes the application
            //! to hang on exit.
            _p->ioFactory.take();
#endif // DJV_WINDOWS
        }

        QPointer<IOFactory> AVContext::ioFactory() const
        {
            return _p->ioFactory.data();
        }

        QPointer<QOpenGLContext> AVContext::openGLContext() const
        {
            return _p->openGLContext.data();
        }

        void AVContext::makeGLContextCurrent()
        {
            _p->openGLContext->makeCurrent(_p->offscreenSurface.data());
        }

        QString AVContext::info() const
        {
            static const QString label = qApp->translate("djv::AV::AVContext",
                "%1"
                "\n"
                "OpenGL\n"
                "\n"
                "    Version: %2.%3\n"
                "    Render filter: %4, %5\n"
                "\n"
                "I/O\n"
                "\n"
                "    Plugins: %6\n");
            QStringList filterMinLabel;
            filterMinLabel << OpenGLImageFilter::filter().min;
            QStringList filterMagLabel;
            filterMagLabel << OpenGLImageFilter::filter().mag;
            return QString(label).
                arg(Core::CoreContext::info()).
                arg(_p->openGLContext->format().majorVersion()).
                arg(_p->openGLContext->format().minorVersion()).
                arg(filterMinLabel.join(", ")).
                arg(filterMagLabel.join(", ")).
                arg(_p->ioFactory->names().join(", "));
        }

        bool AVContext::commandLineParse(QStringList & in)
        {
            //DJV_DEBUG("AVContext::commandLineParse");
            //DJV_DEBUG_PRINT("in = " << in);

            if (!Core::CoreContext::commandLineParse(in))
                return false;

            Q_FOREACH(Core::Plugin * plugin, _p->ioFactory->plugins())
            {
                auto io = static_cast<IOPlugin *>(plugin);
                io->commandLine(in);
            }

            QStringList tmp;
            QString     arg;
            try
            {
                while (!in.isEmpty())
                {
                    in >> arg;

                    // OpenGL options.
                    if (qApp->translate("djv::AV::AVContext", "-render_filter") == arg)
                    {
                        OpenGLImageFilter value;
                        in >> value;
                        OpenGLImageFilter::setFilter(value);
                    }
                    else if (qApp->translate("djv::AV::AVContext", "-render_filter_high") == arg)
                    {
                        OpenGLImageFilter::setFilter(OpenGLImageFilter::filterHighQuality());
                    }

                    // Leftovers.
                    else
                    {
                        tmp << arg;
                    }
                }
            }
            catch (const QString &)
            {
                in = tmp;
                throw QString(arg);
            }

            in = tmp;
            return true;
        }

        QString AVContext::commandLineHelp() const
        {
            QString ioHelp;
            Q_FOREACH(Core::Plugin * plugin, _p->ioFactory->plugins())
            {
                auto io = static_cast<IOPlugin *>(plugin);
                ioHelp += io->commandLineHelp();
            }
            static const QString label = qApp->translate("djv::AV::AVContext",
                "%1"
                "\n"
                "OpenGL Options\n"
                "\n"
                "    -render_filter (zoom out) (zoom in)\n"
                "        Set the render filter: %2. Default = %3, %4.\n"
                "    -render_filter_high\n"
                "        Set the render filter to high quality settings (%5, %6).\n"
                "%7");
            QStringList filterMinLabel;
            filterMinLabel << OpenGLImageFilter::filter().min;
            QStringList filterMagLabel;
            filterMagLabel << OpenGLImageFilter::filter().mag;
            QStringList filterHighQualityMinLabel;
            filterHighQualityMinLabel << OpenGLImageFilter::filterHighQuality().min;
            QStringList filterHighQualityMagLabel;
            filterHighQualityMagLabel << OpenGLImageFilter::filterHighQuality().mag;
            return QString(label).
                arg(ioHelp).
                arg(OpenGLImageFilter::filterLabels().join(", ")).
                arg(filterMinLabel.join(", ")).
                arg(filterMagLabel.join(", ")).
                arg(filterHighQualityMinLabel.join(", ")).
                arg(filterHighQualityMagLabel.join(", ")).
                arg(Core::CoreContext::commandLineHelp());
        }

        void AVContext::debugLogMessage(const QOpenGLDebugMessage & message)
        {
            DJV_LOG(debugLog(), "djv::AV::AVContext", message.message());
        }

    } // namespace AV
} // namespace djv
