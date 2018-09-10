//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

#include <djvGraphics/GraphicsContext.h>

#include <djvGraphics/CineonPlugin.h>
#include <djvGraphics/DPXPlugin.h>
#include <djvGraphics/IFFPlugin.h>
#include <djvGraphics/IFLPlugin.h>
#include <djvGraphics/Image.h>
#include <djvGraphics/ImageIO.h>
#include <djvGraphics/LUTPlugin.h>
#include <djvGraphics/OpenGLImage.h>
#include <djvGraphics/PICPlugin.h>
#include <djvGraphics/PPMPlugin.h>
#include <djvGraphics/TargaPlugin.h>
#include <djvGraphics/RLAPlugin.h>
#include <djvGraphics/SGIPlugin.h>
#if defined(JPEG_FOUND)
#include <djvGraphics/JPEGPlugin.h>
#endif // JPEG_FOUND
#if defined(PNG_FOUND)
#include <djvGraphics/PNGPlugin.h>
#endif // PNG_FOUND
#if defined(TIFF_FOUND)
#include <djvGraphics/TIFFPlugin.h>
#endif // TIFF_FOUND
#if defined(OPENEXR_FOUND)
#include <djvGraphics/OpenEXRPlugin.h>
#endif // OPENEXR_FOUND
#if defined(FFMPEG_FOUND)
#include <djvGraphics/FFmpegPlugin.h>
#endif // FFMPEG_FOUND

#include <djvCore/DebugLog.h>
#include <djvCore/Error.h>

#include <QCoreApplication>
#include <QMetaType>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QScopedPointer>
#include <QVector>

#include <glbinding/Version.h>
#include <glbinding-aux/Meta.h>
#include <glbinding-aux/types_to_string.h>

#include <sstream>

using namespace gl;

//------------------------------------------------------------------------------
// djvGraphicsContext::Private
//------------------------------------------------------------------------------

struct djvGraphicsContext::Private
{
    QScopedPointer<QOffscreenSurface> offscreenSurface;
    QScopedPointer<QOpenGLContext> openGlContext;
    QScopedPointer<djvImageIOFactory> imageIOFactory;
};

//------------------------------------------------------------------------------
// djvGraphicsContext
//------------------------------------------------------------------------------

djvGraphicsContext::djvGraphicsContext(QObject * parent) :
    djvCoreContext(parent),
    _p(new Private)
{
    //DJV_DEBUG("djvGraphicsContext::djvGraphicsContext");
    
    // Register meta types.
    qRegisterMetaType<djvImage>("djvImage");
    qRegisterMetaType<djvImageIOInfo>("djvImageIOInfo");
    
    // Create the default OpenGL context.
    DJV_LOG(debugLog(), "djvGraphicsContext",
        "Creating the default OpenGL context...");

    _p->offscreenSurface.reset(new QOffscreenSurface);
    QSurfaceFormat surfaceFormat;
    surfaceFormat.setSamples(1);
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
    surfaceFormat.setSwapBehavior(QSurfaceFormat::SingleBuffer);
    _p->offscreenSurface->setFormat(surfaceFormat);
    _p->offscreenSurface->create();
    
    _p->openGlContext.reset(new QOpenGLContext);
    _p->openGlContext->setFormat(surfaceFormat);
    _p->openGlContext->create();
    _p->openGlContext->makeCurrent(_p->offscreenSurface.data());

    glbinding::initialize([this](const char* name)
    {
        return _p->openGlContext->getProcAddress(QByteArray::fromStdString(name));
    });
    for (const glbinding::Version & v : glbinding::aux::Meta::versions())
    {
        std::stringstream s;
        s << v;
        DJV_LOG(debugLog(), "djvGraphicsContext", QString("glbinding version: %1").arg(QString::fromLatin1(s.str().c_str())));
    }
    DJV_LOG(debugLog(), "djvGraphicsContext", "");

    //! Create the image I/O plugins.
    DJV_LOG(debugLog(), "djvGraphicsContext", "Loading image I/O plugins...");

    _p->imageIOFactory.reset(new djvImageIOFactory(this));
    _p->imageIOFactory->addPlugin(new djvCineonPlugin(this));
    _p->imageIOFactory->addPlugin(new djvDPXPlugin(this));
    _p->imageIOFactory->addPlugin(new djvIFFPlugin(this));
    _p->imageIOFactory->addPlugin(new djvIFLPlugin(this));
    _p->imageIOFactory->addPlugin(new djvLUTPlugin(this));
    _p->imageIOFactory->addPlugin(new djvPICPlugin(this));
    _p->imageIOFactory->addPlugin(new djvPPMPlugin(this));
    _p->imageIOFactory->addPlugin(new djvRLAPlugin(this));
    _p->imageIOFactory->addPlugin(new djvSGIPlugin(this));
    _p->imageIOFactory->addPlugin(new djvTargaPlugin(this));
#if defined(JPEG_FOUND)
    _p->imageIOFactory->addPlugin(new djvJPEGPlugin(this));
#endif // JPEG_FOUND
#if defined(PNG_FOUND)
    _p->imageIOFactory->addPlugin(new djvPNGPlugin(this));
#endif // PNG_FOUND
#if defined(TIFF_FOUND)
    _p->imageIOFactory->addPlugin(new djvTIFFPlugin(this));
#endif // TIFF_FOUND
#if defined(OPENEXR_FOUND)
    _p->imageIOFactory->addPlugin(new djvOpenEXRPlugin(this));
#endif // OPENEXR_FOUND
#if defined(FFMPEG_FOUND)
    _p->imageIOFactory->addPlugin(new djvFFmpegPlugin(this));
#endif // FFMPEG_FOUND

    DJV_LOG(debugLog(), "djvGraphicsContext", "");
    DJV_LOG(debugLog(), "djvGraphicsContext", "Information:");
    DJV_LOG(debugLog(), "djvGraphicsContext", "");
    DJV_LOG(debugLog(), "djvGraphicsContext", info());
}

djvGraphicsContext::~djvGraphicsContext()
{
    //DJV_DEBUG("djvGraphicsContext::~djvGraphicsContext");
#if defined(DJV_WINDOWS)
    //! \todo On Windows deleting the image factory causes the application
    //! to hang on exit.
    _p->imageIOFactory.take();
#endif // DJV_WINDOWS
}

djvImageIOFactory * djvGraphicsContext::imageIOFactory() const
{
    return _p->imageIOFactory.data();
}

QOpenGLContext * djvGraphicsContext::openGlContext() const
{
    return _p->openGlContext.data();
}

QString djvGraphicsContext::info() const
{
    static const QString label = qApp->translate("djvGraphicsContext",
"%1"
"\n"
"OpenGL\n"
"\n"
"    Version: %2.%3\n"
"    Render filter: %4, %5\n"
"\n"
"Image I/O\n"
"\n"
"    Plugins: %6\n");
    return QString(label).
        arg(djvCoreContext::info()).
        arg(_p->openGlContext->format().majorVersion()).
        arg(_p->openGlContext->format().minorVersion()).
        arg(djvStringUtil::label(djvOpenGLImageFilter::filter().min).join(", ")).
        arg(djvStringUtil::label(djvOpenGLImageFilter::filter().mag).join(", ")).
        arg(_p->imageIOFactory->names().join(", "));
}

bool djvGraphicsContext::commandLineParse(QStringList & in) throw (QString)
{
    //DJV_DEBUG("djvGraphicsContext::commandLineParse");
    //DJV_DEBUG_PRINT("in = " << in);

    if (! djvCoreContext::commandLineParse(in))
        return false;

    Q_FOREACH (djvPlugin * plugin, _p->imageIOFactory->plugins())
    {
        djvImageIO * io = static_cast<djvImageIO *>(plugin);
        io->commandLine(in);
    }

    QStringList tmp;
    QString     arg;
    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            // OpenGL options.
            if (qApp->translate("djvGraphicsContext", "-render_filter") == arg)
            {
                djvOpenGLImageFilter value;
                in >> value;
                djvOpenGLImageFilter::setFilter(value);
            }
            else if (qApp->translate("djvGraphicsContext", "-render_filter_high") == arg)
            {
                djvOpenGLImageFilter::setFilter(
                    djvOpenGLImageFilter::filterHighQuality());
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

QString djvGraphicsContext::commandLineHelp() const
{
    QString imageIOHelp;
    Q_FOREACH(djvPlugin * plugin, _p->imageIOFactory->plugins())
    {
        djvImageIO * io = static_cast<djvImageIO *>(plugin);
        
        imageIOHelp += io->commandLineHelp();
    }
    static const QString label = qApp->translate("djvGraphicsContext",
"%1"
"\n"
"OpenGL Options\n"
"\n"
"    -render_filter (zoom out) (zoom in)\n"
"        Set the render filter. Options = %2. Default = %3, %4.\n"
"    -render_filter_high\n"
"        Set the render filter to high quality settings (%5, %6).\n"
"%7");
    return QString(label).
        arg(imageIOHelp).
        arg(djvOpenGLImageFilter::filterLabels().join(", ")).
        arg(djvStringUtil::label(djvOpenGLImageFilter::filter().min).join(", ")).
        arg(djvStringUtil::label(djvOpenGLImageFilter::filter().mag).join(", ")).
        arg(djvStringUtil::label(djvOpenGLImageFilter::filterHighQuality().min).join(", ")).
        arg(djvStringUtil::label(djvOpenGLImageFilter::filterHighQuality().mag).join(", ")).
        arg(djvCoreContext::commandLineHelp());
}

