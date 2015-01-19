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

//! \file djvCoreApplication.cpp

#include <djvCoreApplication.h>

#include <djvAssert.h>
#include <djvDebugLog.h>
#include <djvError.h>
#include <djvErrorUtil.h>
#include <djvFileInfoUtil.h>
#include <djvMath.h>
#include <djvMemory.h>
#include <djvSystem.h>
#include <djvTime.h>

#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>

//------------------------------------------------------------------------------
// djvApplicationEnum
//------------------------------------------------------------------------------

const QStringList & djvApplicationEnum::exitValueLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvApplicationEnum", "Default") <<
        qApp->translate("djvApplicationEnum", "Error") <<
        qApp->translate("djvApplicationEnum", "Help") <<
        qApp->translate("djvApplicationEnum", "Info") <<
        qApp->translate("djvApplicationEnum", "About");
    
    DJV_ASSERT(EXIT_VALUE_COUNT == data.count());
    
    return data;
}

//------------------------------------------------------------------------------
// djvAbstractCoreApplication::P
//------------------------------------------------------------------------------

struct djvAbstractCoreApplication::P
{
    P() :
        exitValue(djvApplicationEnum::EXIT_DEFAULT),
        endline  (false),
        separator(false),
        debugLog (false)
    {}

    djvApplicationEnum::EXIT_VALUE exitValue;
    QString                        commandLineName;
    bool                           endline;
    bool                           separator;
    bool                           debugLog;
};

//------------------------------------------------------------------------------
// AbstractCoreApplication
//------------------------------------------------------------------------------

QString djvAbstractCoreApplication::_name;

namespace
{

djvAbstractCoreApplication * _global = 0;

} // namespace

djvAbstractCoreApplication::djvAbstractCoreApplication(
    const QString & name,
    int &           argc,
    char **         argv) throw (djvError) :
    _p(new P)
{
    _global = this;
    
    _name = name;

    //DJV_DEBUG("djvAbstractCoreApplication::djvAbstractCoreApplication");
    //DJV_DEBUG_PRINT("name = " << _p->name);
    
    QTranslator * qtTranslator = new QTranslator(qApp);
    qtTranslator->load("qt_" + QLocale::system().name(),
        QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    qApp->installTranslator(qtTranslator);

    loadTranslator("djvCore");

    // Store the command line arguments.

    if (argc > 0)
    {
        _p->commandLineName = argv[0];
    }

    for (int i = 1; i < argc; ++i)
    {
        _commandLineArgs << QString(argv[i]);
    }

    //DJV_DEBUG_PRINT("commandLine = " << _commandLineArgs);

    DJV_LOG("djvAbstractCoreApplication",
        QString("Command line name: \"%1\"").arg(_p->commandLineName));
    DJV_LOG("djvAbstractCoreApplication",
        QString("Command line arguments: %1").
            arg(djvStringUtil::addQuotes(_commandLineArgs).join(", ")));
    DJV_LOG("djvAbstractCoreApplication", "");

    DJV_LOG("djvAbstractCoreApplication", "Information:");
    DJV_LOG("djvAbstractCoreApplication", "");
    DJV_LOG("djvAbstractCoreApplication", info());
}

djvAbstractCoreApplication::~djvAbstractCoreApplication()
{
    //DJV_DEBUG("djvAbstractCoreApplication::~djvAbstractCoreApplication");
    
    delete _p;
}

int djvAbstractCoreApplication::run()
{
    switch (_p->exitValue)
    {
        case djvApplicationEnum::EXIT_HELP:
        
            printMessage("\n" + commandLineHelp(), 1);
            
            break;

        case djvApplicationEnum::EXIT_INFO:
        
            printMessage("\n" + info(), 1);
            
            break;

        case djvApplicationEnum::EXIT_ABOUT:
        
            printMessage("\n" + about(), 1);
            
            break;

        default: break;
    }
    
    QStringList tmp;
    tmp << _p->exitValue;
    DJV_LOG("djvAbstractCoreApplication",
        QString("Exit value: %1").arg(tmp.join(", ")));
    
    return _p->exitValue;
}

djvApplicationEnum::EXIT_VALUE djvAbstractCoreApplication::exitValue() const
{
    return _p->exitValue;
}

void djvAbstractCoreApplication::setExitValue(djvApplicationEnum::EXIT_VALUE in)
{
    _p->exitValue = in;
}

const QString & djvAbstractCoreApplication::name()
{
    return _name;
}

QString djvAbstractCoreApplication::docPath() const
{
    const QString docPath =
#if defined(DJV_WINDOWS)
        "../doc";
#elif defined(DJV_OSX)
        "../doc";
#else
        "../doc";
#endif

    return djvFileInfoUtil::fixPath(qApp->applicationDirPath() + "/" + docPath);
}

QString djvAbstractCoreApplication::doc() const
{
    return docPath() + "djvHelp.html";
}

QString djvAbstractCoreApplication::info() const
{
    static const QString label = qApp->translate(
        "djvAbstractCoreApplication",
"General\n"
"\n"
"    Version: %1\n"
"    System: %2\n"
"    Endian: %3\n"
"    Locale: %4\n"
"    Search path: %5\n"
"    Time units: %6\n"
"    Default speed: %7\n"
"    Maximum sequence size: %8\n");

    return QString(label).
        arg(DJV_PACKAGE_NAME).
        arg(djvSystem::info()).
        arg(djvStringUtil::label(djvMemory::endian()).join(", ")).
        arg(QLocale::system().name()).
        arg(djvStringUtil::addQuotes(djvSystem::searchPath()).join(", ")).
        arg(djvStringUtil::label(djvTime::units()).join(", ")).
        arg(djvStringUtil::label(djvSpeed::speed()).join(", ")).
        arg(djvStringUtil::label(djvSequence::maxFrames()).join(", "));
}

QString djvAbstractCoreApplication::about() const
{
    static const QString label = qApp->translate(
        "djvAbstractCoreApplication",
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
"    GLEW\n"
"    http://glew.sourceforge.net\n"
"    Copyright (c) 2002-2007, Milan Ikits <milan ikits[]ieee org>\n"
"    Copyright (c) 2002-2007, Marcelo E. Magallon <mmagallo[]debian org>\n"
"    Copyright (c) 2002, Lev Povalahev\n"
"    All rights reserved.\n"
"\n"
"    libjpeg\n"
"    http://www.ijg.org\n"
"    Copyright (c) 1991-2012, Thomas G. Lane, Guido Vollbeding\n"
"\n"
"    libpng\n"
"    http://www.libpng.org\n"
"    Copyright (c) 2004, 2006-2012 Glenn Randers-Pehrson\n"
"\n"
"    libquicktime\n"
"    http://libquicktime.sourceforge.net\n"
"    Copyright (c) 2002 Heroine Virtual Ltd.\n"
"    Copyright (c) 2002-2007 Members of the libquicktime project\n"
"\n"
"    libtiff\n"
"    http://www.libtiff.org\n"
"    Copyright (c) 1988-1997 Sam Leffler\n"
"    Copyright (c) 1991-1997 Silicon Graphics, Inc.\n"
"\n"
"    OpenEXR\n"
"    http://www.openexr.com\n"
"    Copyright (c) 2006, Industrial Light & Magic, a division of Lucasfilm"
"Entertainment Company Ltd.\n"
"\n"
"    PortAudio\n"
"    http://www.portaudio.com\n"
"    Copyright (c) 1999-2006 Ross Bencina and Phil Burk\n"
"\n"
"    Qt\n"
"    http://qt-project.org\n"
"    Copyright (c) 2013 Digia Plc and/or its subsidiary(-ies)\n"
"\n"
"    zlib\n"
"    http://www.zlib.net/\n"
"    Copyright (c) 1995-2013 Jean-loup Gailly and Mark Adler\n");

    return QString(label).arg(DJV_PACKAGE_NAME);
}

void djvAbstractCoreApplication::printMessage(const QString & string, int indent) const
{
    print(string, true, indent);
}

void djvAbstractCoreApplication::printError(const djvError & error) const
{
    if (_p->endline)
    {
        djvSystem::print("");

        const_cast<djvAbstractCoreApplication *>(this)->_p->endline = false;
    }

    djvErrorUtil::print(error);
}

void djvAbstractCoreApplication::print(const QString & string, bool newline, int indent) const
{
    if (_p->separator)
    {
        djvSystem::print("");

        const_cast<djvAbstractCoreApplication *>(this)->_p->separator = false;
    }

    djvSystem::print(string, newline, indent);

    const_cast<djvAbstractCoreApplication *>(this)->_p->endline = ! newline;
}

void djvAbstractCoreApplication::printSeparator() const
{
    const_cast<djvAbstractCoreApplication *>(this)->_p->separator = true;
}

void djvAbstractCoreApplication::commandLine(QStringList & in) throw (QString)
{
    //DJV_DEBUG("djvAbstractCoreApplication::commandLine");
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
                qApp->translate("djvAbstractCoreApplication", "-time_units") == arg)
            {
                djvTime::UNITS value = static_cast<djvTime::UNITS>(0);
                in >> value;
                djvTime::setUnits(value);
            }
            else if (
                qApp->translate("djvAbstractCoreApplication", "-default_speed") == arg)
            {
                djvSpeed::FPS value = static_cast<djvSpeed::FPS>(0);
                in >> value;
                djvSpeed::setSpeed(value);
            }
            else if (
                qApp->translate("djvAbstractCoreApplication", "-max_sequence_frames") == arg)
            {
                qint64 value = static_cast<qint64>(0);
                in >> value;
                djvSequence::setMaxFrames(value);
            }

            else if (
                qApp->translate("djvAbstractCoreApplication", "-debug_log") == arg)
            {
                _p->debugLog = true;
            }

            else if (
                qApp->translate("djvAbstractCoreApplication", "-help") == arg ||
                qApp->translate("djvAbstractCoreApplication", "-h") == arg)
            {
                setExitValue(djvApplicationEnum::EXIT_HELP);
            }
            else if (
                qApp->translate("djvAbstractCoreApplication", "-info") == arg)
            {
                setExitValue(djvApplicationEnum::EXIT_INFO);
            }
            else if (
                qApp->translate("djvAbstractCoreApplication", "-about") == arg)
            {
                setExitValue(djvApplicationEnum::EXIT_ABOUT);
            }

#if defined(DJV_OSX)

            else if (
                qApp->translate("djvAbstractCoreApplication", "-psn_") == arg.mid(0, 5))
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
}

const QString & djvAbstractCoreApplication::commandLineName() const
{
    return _p->commandLineName;
}

QString djvAbstractCoreApplication::commandLineHelp() const
{
    static const QString label = qApp->translate(
        "djvAbstractCoreApplication",
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

djvAbstractCoreApplication * djvAbstractCoreApplication::global()
{
    return _global;
}

const QStringList & djvAbstractCoreApplication::errorLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvAbstractCoreApplication",
            "Cannot parse the command line argument: %1");
    
    DJV_ASSERT(ERROR_COUNT == data.count());
    
    return data;
}

void djvAbstractCoreApplication::loadTranslator(const QString & baseName)
{
    DJV_LOG("djvAbstractCoreApplication",
        QString("Searching for translator: \"%1\"").arg(baseName));

    const QString fileName = djvSystem::findFile(
        QString("%1_%2.qm").arg(baseName).arg(QLocale::system().name()));
    
    if (! fileName.isEmpty())
    {
        DJV_LOG("djvAbstractCoreApplication",
            QString("Found translator: \"%1\"").arg(fileName));

        QTranslator * qTranslator = new QTranslator(qApp);
        qTranslator->load(fileName);
        
        qApp->installTranslator(qTranslator);
    }
    
    DJV_LOG("djvAbstractCoreApplication", "");
}

bool djvAbstractCoreApplication::hasDebugLog() const
{
    return _p->debugLog;
}

//------------------------------------------------------------------------------
// djvCoreApplication
//------------------------------------------------------------------------------

djvCoreApplication::djvCoreApplication(const QString & name, int & argc, char ** argv)
    throw (djvError) :
    QCoreApplication(argc, argv),
    djvAbstractCoreApplication(name, argc, argv)
{
    //DJV_DEBUG("djvCoreApplication");
    
    setOrganizationName("djv.sourceforge.net");

    setApplicationName(name);
}

//------------------------------------------------------------------------------

_DJV_STRING_OPERATOR_LABEL(
    djvApplicationEnum::EXIT_VALUE,
    djvApplicationEnum::exitValueLabels())

