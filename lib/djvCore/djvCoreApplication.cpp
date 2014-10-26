//------------------------------------------------------------------------------
// Copyright (c) 2004-2014 Darby Johnston
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

#include <djvError.h>
#include <djvErrorUtil.h>
#include <djvFileInfoUtil.h>
#include <djvMath.h>
#include <djvMemory.h>
#include <djvStringUtil.h>
#include <djvSystem.h>
#include <djvTime.h>

//------------------------------------------------------------------------------
// djvAbstractCoreApplication::P
//------------------------------------------------------------------------------

struct djvAbstractCoreApplication::P
{
    P() :
        exitValue(EXIT_VALUE_DEFAULT),
        endline  (false),
        separator(false)
    {}

    EXIT_VALUE  exitValue;
    QString     commandLineName;
    bool        endline;
    bool        separator;
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

    // Store the command line arguments.

    if (argc)
    {
        _p->commandLineName = argv[0];
    }

    for (int i = 1; i < argc; ++i)
    {
        _commandLineArgs << QString(argv[i]);
    }

    //DJV_DEBUG_PRINT("commandLine = " << _commandLineArgs);
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
        case EXIT_VALUE_HELP:  printMessage(commandLineHelp()); break;
        case EXIT_VALUE_INFO:  printMessage(info()); break;
        case EXIT_VALUE_ABOUT: printMessage(about()); break;

        default: break;
    }
    
    return _p->exitValue;
}

djvAbstractCoreApplication::EXIT_VALUE djvAbstractCoreApplication::exitValue() const
{
    return _p->exitValue;
}

void djvAbstractCoreApplication::setExitValue(EXIT_VALUE exitValue)
{
    _p->exitValue = exitValue;
}

const QString & djvAbstractCoreApplication::name()
{
    return _name;
}

QString djvAbstractCoreApplication::docPath() const
{
    //! \todo Allow the use of an optional environment variable to override
    //! the default documentation path.
    
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

namespace
{

const QString labelInfo =
"\n"
" General\n"
"\n"
"     Version: %1\n"
"     System: %2\n"
"     Endian: %3\n"
"     Search path: %4\n"
"     Time units: %5\n"
"     Default speed: %6\n"
"     Maximum sequence size: %7\n";

} // namespace

QString djvAbstractCoreApplication::info() const
{
    return QString(labelInfo).
        arg(DJV_PACKAGE_NAME).
        arg(djvSystem::info()).
        arg(djvStringUtil::label(djvMemory::endian()).join(", ")).
        arg(djvSystem::searchPath().join(", ")).
        arg(djvStringUtil::label(djvTime::units()).join(", ")).
        arg(djvStringUtil::label(djvSpeed::speed()).join(", ")).
        arg(djvStringUtil::label(djvSequence::maxFrames()).join(", "));
}

namespace
{

const QString labelAbout =
"\n"
" DJV Imaging, Version: %1\n"
"\n"
" http://djv.sourceforge.net\n"
"\n"
" Made in Berkeley, CA, USA\n"
"\n"
" Copyright (c) 2004-2014 Darby Johnston\n"
" All rights reserved.\n"
"\n"
" Redistribution and use in source and binary forms, with or without"
" modification, are permitted provided that the following conditions are"
" met:\n"
"\n"
" * Redistributions of source code must retain the above copyright notice,"
" this list of conditions, and the following disclaimer.\n"
"\n"
" * Redistributions in binary form must reproduce the above copyright"
" notice, this list of conditions, and the following disclaimer in the"
" documentation and/or other materials provided with the distribution.\n"
"\n"
" * Neither the names of the copyright holders nor the names of any"
" contributors may be used to endorse or promote products derived from this"
" software without specific prior written permission.\n"
"\n"
" THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS"
" \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT"
" LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A"
" PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER"
" OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,"
" EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,"
" PROCUREMENT OF SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR"
" PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF"
" LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING"
" NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS"
" SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n"
"\n"
" Portions of this software are copyright (in alphabetical order):\n"
"\n"
"     Copyright (c) 2004-2014 Darby Johnston\n"
"     All rights reserved.\n"
"\n"
"     Copyright (c) 2008 Alan Jones\n"
"     All rights reserved.\n"
"\n"
"     Copyright (c) 2008-2009 Mikael Sundell\n"
"     All rights reserved.\n"
"\n"
" This software is based in part on the works of (in alphabetical order):\n"
"\n"
"     CMake\n"
"     http://www.cmake.org\n"
"     Copyright 2000-2011 Kitware, Inc., Insight Software Consortium\n"
"     All rights reserved.\n"
"\n"
"     FFmpeg\n"
"     http://www.ffmpeg.org\n"
"     Licensed under the LGPLv2.1\n"
"     http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html\n"
"\n"
"     GLEW\n"
"     http://glew.sourceforge.net\n"
"     Copyright (c) 2002-2007, Milan Ikits <milan ikits[]ieee org>\n"
"     Copyright (c) 2002-2007, Marcelo E. Magallon <mmagallo[]debian org>\n"
"     Copyright (c) 2002, Lev Povalahev\n"
"     All rights reserved.\n"
"\n"
"     libjpeg\n"
"     http://www.ijg.org\n"
"     Copyright (c) 1991-2012, Thomas G. Lane, Guido Vollbeding\n"
"\n"
"     libpng\n"
"     http://www.libpng.org\n"
"     Copyright (c) 2004, 2006-2012 Glenn Randers-Pehrson\n"
"\n"
"     libquicktime\n"
"     http://libquicktime.sourceforge.net\n"
"     Copyright (c) 2002 Heroine Virtual Ltd.\n"
"     Copyright (c) 2002-2007 Members of the libquicktime project\n"
"\n"
"     libtiff\n"
"     http://www.libtiff.org\n"
"     Copyright (c) 1988-1997 Sam Leffler\n"
"     Copyright (c) 1991-1997 Silicon Graphics, Inc.\n"
"\n"
"     OpenEXR\n"
"     http://www.openexr.com\n"
"     Copyright (c) 2006, Industrial Light & Magic, a division of Lucasfilm"
" Entertainment Company Ltd.\n"
"\n"
"     PortAudio\n"
"     http://www.portaudio.com\n"
"     Copyright (c) 1999-2006 Ross Bencina and Phil Burk\n"
"\n"
"     Qt\n"
"     http://qt-project.org\n"
"     Copyright (c) 2013 Digia Plc and/or its subsidiary(-ies)\n"
"\n"
"     zlib\n"
"     http://www.zlib.net/\n"
"     Copyright (c) 1995-2013 Jean-loup Gailly and Mark Adler\n";

} // namespace

QString djvAbstractCoreApplication::about() const
{
    return QString(labelAbout).arg(DJV_PACKAGE_NAME);
}

void djvAbstractCoreApplication::printMessage(const QString & string) const
{
    print(string);
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

void djvAbstractCoreApplication::print(const QString & string, bool newline) const
{
    if (_p->separator)
    {
        djvSystem::print("");

        const_cast<djvAbstractCoreApplication *>(this)->_p->separator = false;
    }

    djvSystem::print(string, newline);

    const_cast<djvAbstractCoreApplication *>(this)->_p->endline = ! newline;
}

void djvAbstractCoreApplication::printSeparator() const
{
    const_cast<djvAbstractCoreApplication *>(this)->_p->separator = true;
}

void djvAbstractCoreApplication::commandLine(QStringList & in) throw (djvError)
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

            if ("-time_units" == arg)
            {
                djvTime::UNITS value = static_cast<djvTime::UNITS>(0);
                in >> value;
                djvTime::setUnits(value);
            }
            else if ("-default_speed" == arg)
            {
                djvSpeedEnum::FPS value = static_cast<djvSpeedEnum::FPS>(0);
                in >> value;
                djvSpeed::setSpeed(value);
            }
            else if ("-max_sequence_frames" == arg)
            {
                qint64 value = static_cast<qint64>(0);
                in >> value;
                djvSequence::setMaxFrames(value);
            }

            else if ("-help" == arg || "-h" == arg)
            {
                setExitValue(EXIT_VALUE_HELP);
            }

            else if ("-info" == arg)
            {
                setExitValue(EXIT_VALUE_INFO);
            }
            else if ("-about" == arg)
            {
                setExitValue(EXIT_VALUE_ABOUT);
            }

#if defined(DJV_OSX)

            else if ("-psn_" == arg.mid(0, 5))
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
        
        throw djvError(arg);
    }

    in = tmp;
}

const QString & djvAbstractCoreApplication::commandLineName() const
{
    return _p->commandLineName;
}

namespace
{

const QString commandLineHelpLabel =
"\n"
" General Options\n"
"\n"
"     -time_units (value)\n"
"         Set the time units. Options = %1. Default = %2.\n"
"     -default_speed (value)\n"
"         Set the default speed. Options = %3. Default = %4.\n"
"     -max_sequence_frames (value)\n"
"         Set the maximum number of frames a sequence can hold. Default = %5.\n"
"     -help, -h\n"
"         Show the help message.\n"
"     -info\n"
"         Show the information message.\n"
"     -about\n"
"         Show the about message.\n";

} // namespace

QString djvAbstractCoreApplication::commandLineHelp() const
{
    return QString(commandLineHelpLabel).
        arg(djvTime::unitsLabels().join(", ")).
        arg(djvStringUtil::label(djvTime::units()).join(", ")).
        arg(djvSpeedEnum::fpsLabels().join(", ")).
        arg(djvStringUtil::label(djvSpeed::speed()).join(", ")).
        arg(djvStringUtil::label(djvSequence::maxFrames()).join(", "));
}

const QString djvAbstractCoreApplication::errorCommandLine =
    "Cannot parse the command line argument: %1";

djvAbstractCoreApplication * djvAbstractCoreApplication::global()
{
    return _global;
}

//------------------------------------------------------------------------------
// djvCoreApplication
//------------------------------------------------------------------------------

djvCoreApplication::djvCoreApplication(const QString & name, int & argc, char ** argv)
    throw (djvError) :
    QCoreApplication(argc, argv),
    djvAbstractCoreApplication(name, argc, argv)
{
    setOrganizationName("djv.sourceforge.net");

    setApplicationName(name);
}

