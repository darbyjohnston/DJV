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

//! \file djv_info.cpp

#include <djv_info.h>

#include <djvError.h>
#include <djvErrorUtil.h>
#include <djvFileInfoUtil.h>
#include <djvImageIo.h>
#include <djvStringUtil.h>
#include <djvSystem.h>
#include <djvTime.h>
#include <djvVectorUtil.h>

#include <QDir>

//------------------------------------------------------------------------------
// djvInfoApplication
//------------------------------------------------------------------------------

djvInfoApplication::djvInfoApplication(int argc, char ** argv) throw (djvError) :
    djvImageApplication("djv_info", argc, argv),
    _info    (true),
    _verbose (false),
    _filePath(false),
    _sequence(djvSequence::COMPRESS_RANGE),
    _recurse (false),
    _columns (djvSystem::terminalWidth())
{
    //DJV_DEBUG("djvInfoApplication::djvInfoApplication");

    // Parse the command line.

    try
    {
        commandLine(_commandLineArgs);
    }
    catch (const djvError & error)
    {
        printError(djvError(QString(errorCommandLine).arg(error.string())));
        
        setExitValue(EXIT_VALUE_ERROR);
    }

    if (exitValue() != EXIT_VALUE_DEFAULT)
        return;

    // Convert the command line inputs:
    //
    // * Match wildcards
    // * Expand sequences

    djvFileInfoList list;

    for (int i = 0; i < _input.count(); ++i)
    {
        //DJV_DEBUG_PRINT("input = " << _input[i]);

        djvFileInfo file(_input[i]);

        // Match wildcards.

        if (_sequence && file.isSequenceWildcard())
        {
            file = djvFileInfoUtil::sequenceWildcardMatch(
                file,
                djvFileInfoUtil::list(file.path(), _sequence));

            //DJV_DEBUG_PRINT("  wildcard match = " << file);
        }

        // Is this is a sequence?

        if (_sequence && file.isSequenceValid())
        {
            file.setType(djvFileInfo::SEQUENCE);

            //DJV_DEBUG_PRINT("sequence = " << file);
        }

        // Expand the sequence.

        QStringList tmp = djvFileInfoUtil::expandSequence(file);

        for (int j = 0; j < tmp.count(); ++j)
        {
            file = djvFileInfo(tmp[j]);

            if (_sequence && file.isSequenceValid())
            {
                file.setType(djvFileInfo::SEQUENCE);
            }

            list += file;
        }
    }

    //DJV_DEBUG_PRINT("list = " << list);

    // Compress the inputs into sequences.

    //djvFileInfoUtil::compressSequence(list, _sequence);

    //DJV_DEBUG_PRINT("list = " << list);

    // Print the files.

    for (int i = 0; i < list.count(); ++i)
    {
        if (djvFileInfo::DIRECTORY == list[i].type())
        {
            try
            {
                printDirectory(
                    list[i],
                    ((list.count() > 1) || _recurse) && ! _filePath);
            }
            catch (const djvError & error)
            {
                printError(error);
                
                setExitValue(EXIT_VALUE_ERROR);
            }
        }
        else
        {
            try
            {
                printItem(list[i], _filePath, _info);
            }
            catch (const djvError & error)
            {
                printError(error);
                
                setExitValue(EXIT_VALUE_ERROR);
            }
        }
    }
}

void djvInfoApplication::commandLine(QStringList & in) throw (djvError)
{
    djvImageApplication::commandLine(in);

    if (exitValue() != EXIT_VALUE_DEFAULT)
        return;

    if (in.isEmpty())
    {
        setExitValue(EXIT_VALUE_HELP);
        
        return;
    }

    QString arg;

    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            // Parse the options.

            if ("-x_info" == arg || "-xi" == arg)
            {
                _info = false;
            }
            else if ("-verbose" == arg || "-v" == arg)
            {
                _verbose = true;
            }
            else if ("-file_path" == arg || "-fp" == arg)
            {
                _filePath = true;
            }
            else if ("-seq" == arg || "-q" == arg)
            {
                in >> _sequence;
            }
            else if ("-recurse" == arg || "-r" == arg)
            {
                _recurse = true;
            }
            else if ("-columns" == arg || "-c" == arg)
            {
                in >> _columns;
            }

            // Parse the arguments.

            else
            {
                _input += arg;
            }
        }
    }
    catch (const QString &)
    {
        throw djvError(arg);
    }

    if (! _input.count())
    {
        throw djvError("Input");
    }
}

namespace
{

const QString commandLineHelpLabel =
"\n"
" djv_info\n"
"\n"
"     This application provides a command line tool for displaying information "
"about images and movies.\n"
"\n"
"     Example output:\n"
"\n"
"     yesterdayview.mov                    640x424:1.51 RGB U8 00:02:00:01@12\n"
"     dlad.dpx                          2048x1556:1.32 RGB U10 00:00:00:01@24\n"
"     render0001-1000.exr                                      00:00:41:16@24\n"
"         0: A,B,G,R                                     720x480:1.5 RGBA F16\n"
"         1: Ambient                                      720x480:1.5 RGB F16\n"
"         2: Diffuse                                      720x480:1.5 RGB F16\n"
"         3: Specular                                     720x480:1.5 RGB F16\n"
"         4: Z                                              720x480:1.5 L F32\n"
"\n"
"     Key:\n"
"\n"
"     (name)     (width)x(height):(aspect) (format) (type) (duration)@(speed)\n"
"\n"
"     Layer key:\n"
"\n"
"     (name)                                               (duration)@(speed)\n"
"         (layer): (name)           (width)x(height):(aspect) (format) (type)\n"
"\n"
" Usage\n"
"\n"
"     djv_info (image|directory)... [option]...\n"
"\n"
" Options\n"
"\n"
"     -x_info, -xi\n"
"         Don't show image information, only file names.\n"
"     -verbose, -v\n"
"         Show verbose image information.\n"
"     -file_path, -fp\n"
"         Show file path names.\n"
"     -seq, -q (value)\n"
"         Set file sequencing. Options = %1. Default = %2.\n"
"     -recurse, -r\n"
"         Descend into sub-directories.\n"
"     -columns, -c (value)\n"
"         Set the number of columns for formatting output. A value of zero"
" disables formatting.\n"
"%3"
"\n"
" Examples\n"
"\n"
"     Display image information:\n"
"     > djv_info image.sgi image2.sgi\n"
"\n"
"     Display image sequence information:\n"
"     > djv_info image.1-100.sgi\n"
"\n"
"     Display information about all images within a directory:\n"
"     > djv_info ~/pics\n";

} // namespace

QString djvInfoApplication::commandLineHelp() const
{
    return QString(commandLineHelpLabel).
        arg(djvSequence::compressLabels().join(", ")).
        arg(djvStringUtil::label(_sequence).join(", ")).
        arg(djvImageApplication::commandLineHelp());
}

void djvInfoApplication::printItem(const djvFileInfo & in, bool path, bool info)
    throw (djvError)
{
    //DJV_DEBUG("djvInfoApplication::printItem");
    //DJV_DEBUG_PRINT("in = " << in);

    // Open the file.

    djvImageIoInfo _info;

    {
        QScopedPointer<djvImageLoad> load(
            djvImageIoFactory::global()->load(in, _info));
    
        if (! load.data())
            return;
    }
    
    // Print the file.

    const QString name = in.fileName(-1, path);

    if (info && ! _verbose)
    {
        if (1 == _info.layerCount())
        {
            // Print single layer information.

            const QString str = QString("%1x%2:%3 %4 %5@%6").
                arg(_info[0].size.x).
                arg(_info[0].size.y).
                arg(djvVectorUtil::aspect(_info[0].size), 0, 'f', 2).
                arg(djvStringUtil::label(_info[0].pixel).join(", ")).
                arg(djvTime::unitsToString(
                    _info.sequence.frames.count(),
                    _info.sequence.speed)).
                arg(djvSpeed::speedToFloat(_info.sequence.speed), 0, 'f', 2);

            print(QString("%1 %2").
                arg(name).
                arg(str, _columns - name.length() - 2));
        }
        else
        {
            // Print time information.

            const QString str = QString("%1@%2").
                arg(djvTime::unitsToString(
                    _info.sequence.frames.count(),
                    _info.sequence.speed)).
                arg(djvSpeed::speedToFloat(_info.sequence.speed), 0, 'f', 2);

            print(QString("%1 %2").
                arg(name).
                arg(str, _columns - name.length() - 2));

            // Print each layer's information.

            for (int i = 0; i < _info.layerCount(); ++i)
            {
                const QString nameString = QString("    %1. %2").
                    arg(i).
                    arg(_info[i].layerName);

                const QString infoString = QString("%1x%2:%3 %4").
                    arg(_info[i].size.x).
                    arg(_info[i].size.y).
                    arg(djvVectorUtil::aspect(_info[i].size), 0, 'f', 2).
                    arg(djvStringUtil::label(_info[i].pixel).join(", "));

                print(QString("%1 %2").
                    arg(nameString).
                    arg(infoString, _columns - nameString.length() - 2));
            }
        }
    }
    else if (info && _verbose)
    {
        // Print verbose informaton.

        print(name);

        for (int i = 0; i < _info.layerCount(); ++i)
        {
            print(QString("Layer = %1").arg(_info[i].layerName));
            print(QString("  Width = %1").arg(_info[i].size.x));
            print(QString("  Height = %1").arg(_info[i].size.y));
            print(QString("  Aspect = %1").
                arg(djvVectorUtil::aspect(_info[i].size), 0, 'f', 2));
            print(QString("  Pixel = %1").
                arg(djvStringUtil::label(_info[i].pixel).join(", ")));
        }

        print(QString("Start = %1").
            arg(djvTime::unitsToString(_info.sequence.start(), _info.sequence.speed)));

        print(QString("End = %1").
            arg(djvTime::unitsToString(_info.sequence.end(), _info.sequence.speed)));

        print(QString("Duration = %1").
            arg(djvTime::unitsToString(
                _info.sequence.frames.count(),
                _info.sequence.speed)));

        print(QString("Speed = %1").
            arg(djvSpeed::speedToFloat(_info.sequence.speed)));

        const QStringList keys = _info.tags.keys();

        for (int i = 0; i < keys.count(); ++i)
        {
            print(QString("Tag %1 = %2").
                arg(keys[i]).
                arg(_info.tags[keys[i]]));
        }

        printSeparator();
    }
    else
    {
        print(name);
    }
}

void djvInfoApplication::printDirectory(const djvFileInfo & in, bool label)
    throw (djvError)
{
    //DJV_DEBUG("djvInfoApplication::printDirectory");
    //DJV_DEBUG_PRINT("in = " << in);

    // Read the directory contents.

    if (! QDir(in.path()).exists() && ! _recurse)
    {
        throw djvError(QString("Cannot open directory: %1").arg(in));
    }

    djvFileInfoList items = djvFileInfoUtil::list(in, _sequence);

    // Process the items.

    djvFileInfoUtil::filter(items, djvFileInfoUtil::FILTER_DIRECTORY);
    //djvFileInfoUtil::compressSequence(items, _sequence);

    // Print the items.

    if (label)
    {
        print(QString("%1:").arg(in));

        if (_verbose)
        {
            printSeparator();
        }
    }

    for (int i = 0; i < items.count(); ++i)
    {
        try
        {
            printItem(items[i], _filePath, _info);
        }
        catch (const djvError &)
        {}
    }

    if (label)
    {
        printSeparator();
    }

    // Recurse.

    if (_recurse)
    {
        djvFileInfoList list = djvFileInfoUtil::list(in, _sequence);

        djvFileInfoUtil::filter(
            list,
            djvFileInfoUtil::FILTER_FILE | djvFileInfoUtil::FILTER_HIDDEN);

        for (int i = 0; i < list.count(); ++i)
        {
            printDirectory(list[i], label);
        }
    }
}

//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------

int main(int argc, char ** argv)
{
    int r = 1;

    try
    {
        r = djvInfoApplication(argc, argv).run();
    }
    catch (const djvError & error)
    {
        djvErrorUtil::print(error);
    }

    return r;
}
