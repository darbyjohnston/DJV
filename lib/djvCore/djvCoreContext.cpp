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

//! \file djvCoreContext.cpp

#include <djvCoreContext.h>

#include <djvAssert.h>
#include <djvDebugLog.h>
#include <djvError.h>
#include <djvErrorUtil.h>
#include <djvFileInfoUtil.h>
#include <djvMath.h>
#include <djvMemory.h>
#include <djvSequence.h>
#include <djvSystem.h>
#include <djvTime.h>

#include <QCoreApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QScopedPointer>
#include <QTranslator>

//------------------------------------------------------------------------------
// djvCoreContextPrivate
//------------------------------------------------------------------------------

struct djvCoreContextPrivate
{
    djvCoreContextPrivate() :
        endline  (false),
        separator(false),
        debugLog (new djvDebugLog)
    {}

    bool                        endline;
    bool                        separator;
    QScopedPointer<djvDebugLog> debugLog;
};

//------------------------------------------------------------------------------
// djvCoreContext
//------------------------------------------------------------------------------

djvCoreContext::djvCoreContext(QObject * parent) :
    QObject(parent),
    _p(new djvCoreContextPrivate)
{
    //DJV_DEBUG("djvCoreContext::djvCoreContext");
    
    // Register meta types.
    
    qRegisterMetaType<djvFileInfo>("djvFileInfo");
    qRegisterMetaType<djvFileInfoList>("djvFileInfoList");
    qRegisterMetaType<djvSequence>("djvSequence");
    qRegisterMetaType<djvSequence::COMPRESS>("djvSequence::COMPRESS");
    
    // Load translators.
    
    QTranslator * qtTranslator = new QTranslator(this);
    qtTranslator->load("qt_" + QLocale::system().name(),
        QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    qApp->installTranslator(qtTranslator);
    
    loadTranslator("djvCore");
}

djvCoreContext::~djvCoreContext()
{
    //DJV_DEBUG("djvCoreContext::~djvCoreContext");
    
    delete _p;
}

bool djvCoreContext::commandLine(int & argc, char ** argv)
{
    QStringList args;
    
    for (int i = 1; i < argc; ++i)
    {
        args += argv[i];
    }

    DJV_LOG(debugLog(), "djvCoreContext",
        QString("Command line: %1").arg(djvStringUtil::addQuotes(args).join(", ")));
    DJV_LOG(debugLog(), "djvCoreContext", "");

    try
    {
        if (! commandLineParse(args))
        {
            return false;
        }
    }
    catch (const QString & error)
    {
        printError(
            qApp->translate("djvCoreContext",
                "Cannot parse the command line argument: %1").arg(error));
        
        return false;
    }
    
    DJV_LOG(debugLog(), "djvCoreContext", "Information:");
    DJV_LOG(debugLog(), "djvCoreContext", "");
    DJV_LOG(debugLog(), "djvCoreContext", info());
    
    return true;
}

QString djvCoreContext::doc() const
{
    return djvFileInfoUtil::fixPath(qApp->applicationDirPath() + "/../doc/" +
        "djvDocumentation.html");
}

QString djvCoreContext::info() const
{
    static const QString label = qApp->translate(
        "djvCoreContext",
"General\n"
"\n"
"    Version: %1\n"
"    Time units: %2\n"
"    Default speed: %3\n"
"    Maximum sequence size: %4\n"
"\n"
"System\n"
"\n"
"    %5\n"
"    Endian: %6\n"
"    Locale: %7\n"
"    Search path: %8\n"
"    Qt version: %9\n");

    return QString(label).
        arg(DJV_PROJECT_NAME).
        arg(djvStringUtil::label(djvTime::units()).join(", ")).
        arg(djvStringUtil::label(djvSpeed::speed()).join(", ")).
        arg(djvStringUtil::label(djvSequence::maxFrames()).join(", ")).
        arg(djvSystem::info()).
        arg(djvStringUtil::label(djvMemory::endian()).join(", ")).
        arg(QLocale::system().name()).
        arg(djvStringUtil::addQuotes(djvSystem::searchPath()).join(", ")).
        arg(qVersion());
}

QString djvCoreContext::about() const
{
    static const QString label = qApp->translate(
        "djvCoreContext",
"DJV Imaging, Version: %1\n"
"\n"
"http://djv.sourceforge.net\n"
"\n"
"Made in Berkeley, CA, USA\n"
"\n"
"Copyright (c) 2004-2015 Darby Johnston\n"
"All rights reserved.\n"
"\n"
"Redistribution and use in source and binary forms, with or without "
"modification, are permitted provided that the following conditions are "
"met:\n"
"\n"
"* Redistributions of source code must retain the above copyright notice, "
"this list of conditions, and the following disclaimer.\n"
"\n"
"* Redistributions in binary form must reproduce the above copyright "
"notice, this list of conditions, and the following disclaimer in the "
"documentation and/or other materials provided with the distribution.\n"
"\n"
"* Neither the names of the copyright holders nor the names of any "
"contributors may be used to endorse or promote products derived from this "
"software without specific prior written permission.\n"
"\n"
"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "
"\"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT "
"LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A "
"PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER "
"OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, "
"EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, "
"PROCUREMENT OF SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR "
"PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF "
"LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING "
"NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS "
"SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n"
"\n"
"Portions of this software are copyright (in alphabetical order):\n"
"\n"
"    Copyright (c) 2004-2015 Darby Johnston\n"
"    All rights reserved.\n"
"\n"
"    Copyright (c) 2008 Alan Jones\n"
"    All rights reserved.\n"
"\n"
"    Copyright (c) 2008-2009 Mikael Sundell\n"
"    All rights reserved.\n"
"\n"
"This software is based in part on the works of (in alphabetical order):\n"
"\n"
"    CMake\n"
"    http://www.cmake.org\n"
"    Copyright 2000-2011 Kitware, Inc., Insight Software Consortium\n"
"    All rights reserved.\n"
"\n"
"    FFmpeg\n"
"    http://www.ffmpeg.org\n"
"    Licensed under the LGPLv2.1\n"
"    http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html\n"
"\n"
"    GLM\n"
"    https://glm.g-truc.net/0.9.9/index.html\n"
"    Copyright (c) 2005-2017 G-Truc Creation\n"
"\n"
"    glbinding\n"
"    https://github.com/cginternals/glbinding\n"
"    Copyright (c) 2014-2015 Computer Graphics Systems Group at the Hasso-Plattner-Institute and CG Internals GmbH, Germany\n"
"\n"
"    libjpeg\n"
"    http://www.ijg.org\n"
"    Copyright (c) 1991-2012, Thomas G. Lane, Guido Vollbeding\n"
"\n"
"    libpng\n"
"    http://www.libpng.org\n"
"    Copyright (c) 2004, 2006-2012 Glenn Randers-Pehrson\n"
"\n"
"    libtiff\n"
"    http://www.libtiff.org\n"
"    Copyright (c) 1988-1997 Sam Leffler\n"
"    Copyright (c) 1991-1997 Silicon Graphics, Inc.\n"
"\n"
"    OpenAL\n"
"    https://www.openal.org\n"
"    Copyright (C) 1999-2000 by Loki Software\n"
"\n"
"    OpenEXR\n"
"    http://www.openexr.com\n"
"    Copyright (c) 2006, Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.\n"
"\n"
"    Qt\n"
"    http://qt-project.org\n"
"    Copyright (c) 2013 Digia Plc and/or its subsidiary(-ies)\n"
"\n"
"    zlib\n"
"    http://www.zlib.net/\n"
"    Copyright (c) 1995-2013 Jean-loup Gailly and Mark Adler\n");

    return QString(label).arg(DJV_PROJECT_NAME);
}
    
void djvCoreContext::print(const QString & string, bool newLine, int indent)
{
    consolePrint(string, newLine, indent);
}

void djvCoreContext::printSeparator()
{
    const_cast<djvCoreContext *>(this)->_p->separator = true;
}
    
djvDebugLog * djvCoreContext::debugLog() const
{
    return _p->debugLog.data();
}

void djvCoreContext::printMessage(const QString & string)
{
    print(string);
}

void djvCoreContext::printError(const djvError & error)
{
    const QStringList list = djvErrorUtil::format(error);
    
    for (int i = list.count() - 1; i >= 0; --i)
    {
        print(list[i]);
    }
}

void djvCoreContext::loadTranslator(const QString & baseName)
{
    DJV_LOG(debugLog(), "djvCoreContext",
        QString("Searching for translator: \"%1\"").arg(baseName));

    const QString fileName = djvSystem::findFile(
        QString("%1_%2.qm").arg(baseName).arg(QLocale::system().name()));
    
    if (! fileName.isEmpty())
    {
        DJV_LOG(debugLog(), "djvCoreContext",
            QString("Found translator: \"%1\"").arg(fileName));

        QTranslator * qTranslator = new QTranslator(qApp);
        qTranslator->load(fileName);
        
        qApp->installTranslator(qTranslator);
    }
    
    DJV_LOG(debugLog(), "djvCoreContext", "");
}

bool djvCoreContext::commandLineParse(QStringList & in) throw (QString)
{
    //DJV_DEBUG("djvCoreContext::commandLineParse");
    //DJV_DEBUG_PRINT("in = " << in);

    QStringList tmp;
    QString     arg;

    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            // General options.

            if (
                qApp->translate("djvCoreContext", "-time_units") == arg)
            {
                djvTime::UNITS value = static_cast<djvTime::UNITS>(0);
                in >> value;
                djvTime::setUnits(value);
            }
            else if (
                qApp->translate("djvCoreContext", "-default_speed") == arg)
            {
                djvSpeed::FPS value = static_cast<djvSpeed::FPS>(0);
                in >> value;
                djvSpeed::setSpeed(value);
            }
            else if (
                qApp->translate("djvCoreContext", "-max_sequence_frames") == arg)
            {
                qint64 value = static_cast<qint64>(0);
                in >> value;
                djvSequence::setMaxFrames(value);
            }

            else if (
                qApp->translate("djvCoreContext", "-debug_log") == arg)
            {
                Q_FOREACH(const QString & message, debugLog()->messages())
                {
                    printMessage(message);
                }
                
                connect(
                    debugLog(),
                    SIGNAL(message(const QString &)),
                    SLOT(debugLogCallback(const QString &)));
            }

            else if (
                qApp->translate("djvCoreContext", "-help") == arg ||
                qApp->translate("djvCoreContext", "-h") == arg)
            {
                print("\n" + commandLineHelp());
                
                return false;
            }
            else if (
                qApp->translate("djvCoreContext", "-info") == arg)
            {
                print("\n" + info());
                
                return false;
            }
            else if (
                qApp->translate("djvCoreContext", "-about") == arg)
            {
                print("\n" + about());
                
                return false;
            }

#if defined(DJV_OSX)

            else if (
                qApp->translate("djvCoreContext", "-psn_") == arg.mid(0, 5))
            {
                //! \todo Ignore the Mac OS process id command line argument.
                //! Is this still necessary?
            }

#endif

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

QString djvCoreContext::commandLineHelp() const
{
    static const QString label = qApp->translate(
        "djvCoreContext",
"\n"
"General Options\n"
"\n"
"    -time_units (value)\n"
"        Set the time units. Options = %1. Default = %2.\n"
"    -default_speed (value)\n"
"        Set the default speed. Options = %3. Default = %4.\n"
"    -max_sequence_frames (value)\n"
"        Set the maximum number of frames a sequence can hold. Default = %5.\n"
"    -debug_log\n"
"        Print debug log messages.\n"
"    -help, -h\n"
"        Show the help message.\n"
"    -info\n"
"        Show the information message.\n"
"    -about\n"
"        Show the about message.\n");

    return QString(label).
        arg(djvTime::unitsLabels().join(", ")).
        arg(djvStringUtil::label(djvTime::units()).join(", ")).
        arg(djvSpeed::fpsLabels().join(", ")).
        arg(djvStringUtil::label(djvSpeed::speed()).join(", ")).
        arg(djvStringUtil::label(djvSequence::maxFrames()).join(", "));
}

void djvCoreContext::consolePrint(const QString & string, bool newline, int indent)
{
    if (_p->separator)
    {
        djvSystem::print("");

        const_cast<djvCoreContext *>(this)->_p->separator = false;
    }

    djvSystem::print(string, newline, indent);

    const_cast<djvCoreContext *>(this)->_p->endline = ! newline;
}

void djvCoreContext::debugLogCallback(const QString & string)
{
    consolePrint(string);
}


