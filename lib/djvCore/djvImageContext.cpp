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

//! \file djvImageContext.cpp

#include <djvImageContext.h>

#include <djvDebugLog.h>
#include <djvError.h>
#include <djvImage.h>
#include <djvImageIo.h>
#include <djvOpenGlContext.h>
#include <djvOpenGlImage.h>

#include <QCoreApplication>
#include <QMetaType>
#include <QScopedPointer>
#include <QVector>

//------------------------------------------------------------------------------
// djvImageContextPrivate
//------------------------------------------------------------------------------

struct djvImageContextPrivate
{
    QScopedPointer<djvOpenGlContextFactory> openGlContextFactory;
    QScopedPointer<djvOpenGlContext>        openGlContext;
    QScopedPointer<djvImageIoFactory>       imageIoFactory;
};

//------------------------------------------------------------------------------
// djvImageContext
//------------------------------------------------------------------------------

djvImageContext::djvImageContext(QObject * parent) :
    djvCoreContext(parent),
    _p(new djvImageContextPrivate)
{
    //DJV_DEBUG("djvImageContext::djvImageContext");
    
    // Register meta types.
    
    qRegisterMetaType<djvImage>("djvImage");
    qRegisterMetaType<djvImageIoInfo>("djvImageIoInfo");
    
    // Create the default OpenGL context.

    DJV_LOG(debugLog(), "djvImageContext",
        "Creating the default OpenGL context...");

    _p->openGlContextFactory.reset(new djvOpenGlContextFactory(this));

    _p->openGlContext.reset(_p->openGlContextFactory->create());

    DJV_LOG(debugLog(), "djvImageContext", "");

    //! Create the image I/O plugins.

    DJV_LOG(debugLog(), "djvImageContext", "Loading image I/O plugins...");

    _p->imageIoFactory.reset(new djvImageIoFactory(this));

    DJV_LOG(debugLog(), "djvImageContext", "");
    DJV_LOG(debugLog(), "djvImageContext", "Information:");
    DJV_LOG(debugLog(), "djvImageContext", "");
    DJV_LOG(debugLog(), "djvImageContext", info());
}

djvImageContext::~djvImageContext()
{
    //DJV_DEBUG("djvImageContext::~djvImageContext");

#if defined(DJV_WINDOWS)

    //! \todo On Windows deleting the image factory causes the application
    //! to hang on exit.

    _p->imageIoFactory.take();

#endif // DJV_WINDOWS

    delete _p;
}

djvImageIoFactory * djvImageContext::imageIoFactory() const
{
    return _p->imageIoFactory.data();
}
    
djvOpenGlContextFactory * djvImageContext::openGlContextFactory() const
{
    return _p->openGlContextFactory.data();
}

djvOpenGlContext * djvImageContext::openGlContext() const
{
    return _p->openGlContext.data();
}

QString djvImageContext::info() const
{
    static const QString label = qApp->translate("djvImageContext",
"%1"
"\n"
"OpenGL\n"
"\n"
"    Vendor: %2\n"
"    Renderer: %3\n"
"    Version: %4\n"
"    Render filter: %5, %6\n"
"\n"
"Image I/O\n"
"\n"
"    %7\n");

    return QString(label).
        arg(djvCoreContext::info()).
        arg(_p->openGlContext->vendor()).
        arg(_p->openGlContext->renderer()).
        arg(_p->openGlContext->version()).
        arg(djvStringUtil::label(djvOpenGlImageFilter::filter().min).join(", ")).
        arg(djvStringUtil::label(djvOpenGlImageFilter::filter().mag).join(", ")).
        arg(_p->imageIoFactory->names().join(", "));
}

bool djvImageContext::commandLineParse(QStringList & in) throw (QString)
{
    //DJV_DEBUG("djvImageContext::commandLineParse");
    //DJV_DEBUG_PRINT("in = " << in);

    if (! djvCoreContext::commandLineParse(in))
        return false;

    Q_FOREACH (djvPlugin * plugin, _p->imageIoFactory->plugins())
    {
        djvImageIo * io = static_cast<djvImageIo *>(plugin);
        
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

            if (qApp->translate("djvImageContext", "-render_filter") == arg)
            {
                djvOpenGlImageFilter value;
                in >> value;
                djvOpenGlImageFilter::setFilter(value);
            }

            else if (qApp->translate("djvImageContext", "-render_filter_high") == arg)
            {
                djvOpenGlImageFilter::setFilter(
                    djvOpenGlImageFilter::filterHighQuality());
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

QString djvImageContext::commandLineHelp() const
{
    QString imageIoHelp;

    Q_FOREACH(djvPlugin * plugin, _p->imageIoFactory->plugins())
    {
        djvImageIo * io = static_cast<djvImageIo *>(plugin);
        
        imageIoHelp += io->commandLineHelp();
    }

    static const QString label = qApp->translate("djvImageContext",
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
        arg(imageIoHelp).
        arg(djvOpenGlImageFilter::filterLabels().join(", ")).
        arg(djvStringUtil::label(djvOpenGlImageFilter::filter().min).join(", ")).
        arg(djvStringUtil::label(djvOpenGlImageFilter::filter().mag).join(", ")).
        arg(djvStringUtil::label(djvOpenGlImageFilter::filterHighQuality().min).join(", ")).
        arg(djvStringUtil::label(djvOpenGlImageFilter::filterHighQuality().mag).join(", ")).
        arg(djvCoreContext::commandLineHelp());
}
