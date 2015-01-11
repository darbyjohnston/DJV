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

//! \file djvImageApplication.cpp

#include <djvImageApplication.h>

#include <djvDebugLog.h>
#include <djvError.h>
#include <djvImageIo.h>
#include <djvOpenGlContext.h>
#include <djvOpenGlImage.h>

#include <QVector>

//------------------------------------------------------------------------------
// djvAbstractImageApplication::P
//------------------------------------------------------------------------------

struct djvAbstractImageApplication::P
{
    P() :
        context(0)
    {}
    
    djvOpenGlContext * context;
};

//------------------------------------------------------------------------------
// djvAbstractImageApplication
//------------------------------------------------------------------------------

djvAbstractImageApplication::djvAbstractImageApplication(
    const QString & name,
    int &           argc,
    char **         argv) throw (djvError) :
    djvAbstractCoreApplication(name, argc, argv),
    _p(new P)
{
    //DJV_DEBUG("djvAbstractImageApplication::djvAbstractImageApplication");
    //DJV_DEBUG_PRINT("name = " << name);
    
    // Create the default OpenGL context.

    DJV_LOG("djvAbstractImageApplication",
        "Creating the default OpenGL context...");

    _p->context = djvOpenGlContextFactory::create();

    DJV_LOG("djvAbstractImageApplication", "");

    //! Force image I/O plugin loading.

    DJV_LOG("djvAbstractImageApplication", "Loading image I/O plugins...");

    djvImageIoFactory::global();

    //DJV_DEBUG_PRINT("image I/O base = " <<
    //    djvImageIoBaseFactory::global()->plugins().count());
    //DJV_DEBUG_PRINT("image load = " <<
    //    ImageLoadFactory::global()->plugins().count());
    //DJV_DEBUG_PRINT("image save = " <<
    //    ImageSaveFactory::global()->plugins().count());

    DJV_LOG("djvAbstractImageApplication", "");
    DJV_LOG("djvAbstractImageApplication", "Information:");
    DJV_LOG("djvAbstractImageApplication", "");
    DJV_LOG("djvAbstractImageApplication", info());
}

djvAbstractImageApplication::~djvAbstractImageApplication()
{
    //DJV_DEBUG("djvAbstractImageApplication::~djvAbstractImageApplication");

    delete _p->context;
    
    delete _p;
}

djvOpenGlContext * djvAbstractImageApplication::context()
{
    return _p->context;
}

QString djvAbstractImageApplication::info() const
{
    static const QString label = qApp->translate("djvAbstractImageApplication",
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
"    Plugins: %7\n");

    return QString(label).
        arg(djvAbstractCoreApplication::info()).
        arg(_p->context->vendor()).
        arg(_p->context->renderer()).
        arg(_p->context->version()).
        arg(djvStringUtil::label(djvOpenGlImageFilter::filter().min).join(", ")).
        arg(djvStringUtil::label(djvOpenGlImageFilter::filter().mag).join(", ")).
        arg(djvImageIoFactory::global()->names().join(", "));
}

void djvAbstractImageApplication::commandLine(QStringList & in) throw (QString)
{
    //DJV_DEBUG("djvAbstractImageApplication::commandLine");
    //DJV_DEBUG_PRINT("in = " << in);

    djvAbstractCoreApplication::commandLine(in);

    if (djvAbstractCoreApplication::exitValue() !=
        djvApplicationEnum::EXIT_DEFAULT)
        return;
    
    Q_FOREACH (djvPlugin * plugin, djvImageIoFactory::global()->plugins())
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

            if (
                qApp->translate("djvAbstractImageApplication", "-render_filter") == arg)
            {
                djvOpenGlImageFilter value;
                in >> value;
                djvOpenGlImageFilter::setFilter(value);
            }

            else if (qApp->translate("djvAbstractImageApplication", "-render_filter_high") == arg)
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
}

QString djvAbstractImageApplication::commandLineHelp() const
{
    QString imageIoHelp;

    Q_FOREACH(djvPlugin * plugin, djvImageIoFactory::global()->plugins())
    {
        djvImageIo * io = static_cast<djvImageIo *>(plugin);
        
        imageIoHelp += io->commandLineHelp();
    }

    static const QString label = qApp->translate("djvAbstractImageApplication",
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
        arg(djvAbstractCoreApplication::commandLineHelp());
}

//------------------------------------------------------------------------------
// djvImageApplication
//------------------------------------------------------------------------------

djvImageApplication::djvImageApplication(const QString & name, int & argc, char ** argv)
    throw (djvError) :
    QCoreApplication(argc, argv),
    djvAbstractImageApplication(name, argc, argv)
{
    setOrganizationName("djv.sourceforge.net");
    
    setApplicationName(name);
}

