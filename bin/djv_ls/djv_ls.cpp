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

//! \file djv_ls.cpp

#include <djv_ls.h>

#include <djvAssert.h>
#include <djvDebugLog.h>
#include <djvError.h>
#include <djvErrorUtil.h>
#include <djvMath.h>
#include <djvMemory.h>
#include <djvSystem.h>
#include <djvTime.h>
#include <djvUser.h>

#include <QDir>

//------------------------------------------------------------------------------
// djvLsApplication
//------------------------------------------------------------------------------

djvLsApplication::djvLsApplication(int argc, char ** argv) throw (djvError) :
    djvCoreApplication("djv_ls", argc, argv),
    _info         (true),
    _filePath     (false),
    _sequence     (djvSequence::COMPRESS_RANGE),
    _recurse      (false),
    _hidden       (false),
    _columns      (djvSystem::terminalWidth()),
    _sort         (djvFileInfoUtil::SORT_NAME),
    _reverseSort  (false),
    _doNotSortDirs(false)
{
    //DJV_DEBUG("djvLsApplication::djvLsApplication");
    
    loadTranslator("djv_ls");

    // Parse the command line.

    try
    {
        commandLine(_commandLineArgs);
    }
    catch (const QString & error)
    {
        printError(
            djvCoreApplication::errorLabels()[ERROR_COMMAND_LINE].arg(error));
        
        setExitValue(djvApplicationEnum::EXIT_ERROR);
    }

    if (hasDebugLog())
    {
        Q_FOREACH(const QString & message, djvDebugLog::global()->messages())
        {
            printMessage(message);
        }
        
        connect(
            djvDebugLog::global(),
            SIGNAL(message(const QString &)),
            SLOT(debugLogCallback(const QString &)));
    }

    if (exitValue() != djvApplicationEnum::EXIT_DEFAULT)
        return;
    
    // Convert the command line inputs:
    //
    // * Match wildcards
    // * Expand sequences

    djvFileInfoList list;

    for (int i = 0; i < _input.count(); ++i)
    {
        // Parse the input.
        
        djvFileInfo fileInfo =
            djvFileInfoUtil::commandLine(_input[i], _sequence);

        //DJV_DEBUG_PRINT("input = " << fileInfo);
        
        DJV_LOG("djv_ls", QString("Input = \"%1\"").arg(fileInfo));
        
        // Expand the sequence.

        QStringList tmp = djvFileInfoUtil::expandSequence(fileInfo);

        for (int j = 0; j < tmp.count(); ++j)
        {
            fileInfo = djvFileInfo(tmp[j], false);

            if (! fileInfo.stat())
            {
                printError(
                    djvError(errorLabels()[ERROR_OPEN].
                    arg(QDir::toNativeSeparators(fileInfo))));
                
                setExitValue(djvApplicationEnum::EXIT_ERROR);
                
                continue;
            }

            if (_sequence && fileInfo.isSequenceValid())
            {
                fileInfo.setType(djvFileInfo::SEQUENCE);
            }

            list += fileInfo;
        }
    }

    //DJV_DEBUG_PRINT("list = " << list);
    
    DJV_LOG("djv_ls", QString("Input count = %1").arg(list.count()));

    // Process the inputs.

    process(list);

    //DJV_DEBUG_PRINT("process = " << list);

    DJV_LOG("djv_ls", QString("Processed count = %1").arg(list.count()));

    // If there are no inputs list the current directory.

    if (! list.count() && exitValue() != djvApplicationEnum::EXIT_ERROR)
    {
        list += djvFileInfo(".");
    }

    // Print the items.

    for (int i = 0; i < list.count(); ++i)
    {
        if (djvFileInfo::DIRECTORY == list[i].type())
        {
            if (! printDirectory(
                list[i],
                ((list.count() > 1) || _recurse) && ! _filePath))
            {
                printError(
                    djvError(errorLabels()[ERROR_OPEN].
                    arg(QDir::toNativeSeparators(list[i]))));
                
                setExitValue(djvApplicationEnum::EXIT_ERROR);
            }
        }
        else
        {
            printItem(list[i], true, _info);
        }
    }
}

void djvLsApplication::commandLine(QStringList & in) throw (QString)
{
    //DJV_DEBUG("djvLsApplication::commandLine");
    //DJV_DEBUG_PRINT("in = " << in);

    djvCoreApplication::commandLine(in);

    if (exitValue() != djvApplicationEnum::EXIT_DEFAULT)
        return;

    QStringList args;
    QString     arg;

    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            // Parse the options.

            if (tr("-x_info") == arg || tr("-xi") == arg)
            {
                _info = false;
            }
            else if (tr("-file_path") == arg || tr("-fp") == arg)
            {
                _filePath = true;
            }
            else if (tr("-seq") == arg || tr("-q") == arg)
            {
                in >> _sequence;
            }
            else if (tr("-recurse") == arg || tr("-r") == arg)
            {
                _recurse = true;
            }
            else if (tr("-hidden") == arg)
            {
                _hidden = true;
            }
            else if (tr("-columns") == arg || tr("-c") == arg)
            {
                in >> _columns;
            }

            // Parse the sorting options.

            else if (tr("-sort") == arg || tr("-s") == arg)
            {
                in >> _sort;
            }
            else if (tr("-reverse_sort") == arg || tr("-rs") == arg)
            {
                _reverseSort = true;
            }
            else if (tr("-x_sort_dirs") == arg || tr("-xsd") == arg)
            {
                _doNotSortDirs = false;
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
        throw QString(arg);
    }
}
    
const QStringList & djvLsApplication::errorLabels()
{
    static const QStringList data = QStringList() <<
        tr("Cannot open: \"%1\"");

    DJV_ASSERT(ERROR_COUNT == data.count());
    
    return data;
}

QString djvLsApplication::commandLineHelp() const
{
    static const QString label = tr(
"djv_ls\n"
"\n"
"    This application provides a command line tool for listing directories "
"with file sequences.\n"
"\n"
"    Example output:\n"
"\n"
"    el_cerrito_bart.1k.tiff   File 2.23MB darby rw Mon Jun 12 21:21:55 2006\n"
"    richmond_train.2k.tiff    File 8.86MB darby rw Mon Jun 12 21:21:58 2006\n"
"    fishpond.1-749.png       Seq 293.17MB darby rw Thu Aug 17 16:47:43 2006\n"
"\n"
"    Key:\n"
"\n"
"    (name)                        (type) (size) (user) (permissions) (time)\n"
"\n"
"    File types:                   Permissions:\n"
"\n"
"    * File - Regular file         * r - Readable\n"
"    * Seq  - File sequence        * w - Writable\n"
"    * Dir  - Directory            * x - Executable\n"
"\n"
"Usage\n"
"\n"
"    djv_ls [file|directory]... [option]...\n"
"\n"
"Options\n"
"\n"
"    -x_info, -xi\n"
"        Don't show information, only file names.\n"
"    -file_path, -fp\n"
"        Show file path names.\n"
"    -seq, -q (value)\n"
"        Set file sequencing. Options = %1. Default = %2.\n"
"    -recurse, -r\n"
"        Descend into sub-directories.\n"
"    -hidden\n"
"        Show hidden files.\n"
"    -columns, -c (value)\n"
"        Set the number of columns for formatting output. A value of zero "
"disables formatting.\n"
"\n"
"Sorting Options\n"
"\n"
"    -sort, -s (value)\n"
"        Set the sorting. Options = %3. Default = %4.\n"
"    -reverse_sort, -rs\n"
"        Reverse the sorting order.\n"
"    -x_sort_dirs, -xsd\n"
"        Don't sort directories first.\n"
"%5"
"\n"
"Examples\n"
"\n"
"    List the current directory:\n"
"    > djv_ls\n"
"\n"
"    List specific directories:\n"
"    > djv_ls ~/movies ~/pictures\n"
"\n"
"    Sort by time with the most recent first:\n"
"    > djv_ls -sort time -reverse_sort\n");

    return QString(label).
        arg(djvSequence::compressLabels().join(", ")).
        arg(djvStringUtil::label(_sequence).join(", ")).
        arg(djvFileInfoUtil::sortLabels().join(", ")).
        arg(djvStringUtil::label(_sort).join(", ")).
        arg(djvCoreApplication::commandLineHelp());
}

void djvLsApplication::debugLogCallback(const QString & in)
{
    printMessage(in);
}

void djvLsApplication::process(djvFileInfoList & in)
{
    //DJV_DEBUG("djvLsApplication::process");
    //DJV_DEBUG_PRINT("in = " << in);

    // Compress files into sequences.

    djvFileInfoUtil::compressSequence(in, _sequence);

    //DJV_DEBUG_PRINT("compress = " << in);

    // Remove hidden files.

    if (! _hidden)
    {
        djvFileInfoUtil::filter(in, djvFileInfoUtil::FILTER_HIDDEN);

        //DJV_DEBUG_PRINT("hidden = " << in);
    }

    // Sort.

    djvFileInfoUtil::sort(in, _sort, _reverseSort);

    //DJV_DEBUG_PRINT("sort = " << in);

    if (! _doNotSortDirs)
    {
        djvFileInfoUtil::sortDirsFirst(in);

        //DJV_DEBUG_PRINT("sort directories = " << in);
    }
}

void djvLsApplication::printItem(const djvFileInfo & in, bool path, bool info)
{
    //DJV_DEBUG("djvLsApplication::printItem");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("path = " << path);
    //DJV_DEBUG_PRINT("info = " << info);
    //DJV_DEBUG_PRINT("columns = " << _columns);

    QString name = in.fileName(-1, path);

    if (info)
    {
        const QString infoString =
#if defined(DJV_WINDOWS)
            tr("%1 %2 %3 %4").
#else
            tr("%1 %2 %3 %4 %5").
#endif
            arg(djvFileInfo::typeLabels()[in.type()], 4).
            arg(djvMemory::sizeLabel(in.size())).
#if ! defined(DJV_WINDOWS)
            arg(djvUser::uidToString(in.user())).
#endif // DJV_WINDOWS
            arg(djvFileInfo::permissionsLabel(in.permissions()), 3).
            arg(djvTime::timeToString(in.time()));

        // Elide the name if there isn't enough space.

        if (_columns)
        {
            const int length = djvMath::max(_columns - infoString.length() - 2, 0);

            if (name.length() > length)
            {
                name = name.mid(0, length);

                int i = name.length();
                int j = 3;

                while (i-- > 0 && j-- > 0)
                {
                    name[i] = '.';
                }
            }
        }

        print(
            tr("%1 %2").
                arg(QDir::toNativeSeparators(name)).
                arg(infoString, _columns - name.length() - 2));
    }
    else
    {
        print(name);
    }
}

bool djvLsApplication::printDirectory(const djvFileInfo & in, bool label)
{
    //DJV_DEBUG("djvLsApplication::printDirectory");
    //DJV_DEBUG_PRINT("in = " << in);

    // Read the directory contents.

    if (! QDir(in.path()).exists())
        return false;

    djvFileInfoList items = djvFileInfoUtil::list(in, _sequence);

    // Process the items.

    process(items);

    // Print the items.

    if (label)
    {
        print(tr("%1:").arg(QDir::toNativeSeparators(in)));
    }

    for (int i = 0; i < items.count(); ++i)
    {
        printItem(items[i], _filePath, _info);
    }

    if (label)
    {
        printSeparator();
    }

    // Recurse.

    bool r = true;

    if (_recurse)
    {
        djvFileInfoList items = djvFileInfoUtil::list(in, _sequence);

        djvFileInfoUtil::filter(
            items,
            djvFileInfoUtil::FILTER_FILES |
                (! _hidden ? djvFileInfoUtil::FILTER_HIDDEN : 0));

        for (int i = 0; i < items.count(); ++i)
        {
            r &= printDirectory(items[i], label);
        }
    }

    return r;
}

//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------

int main(int argc, char ** argv)
{
    int r = 1;

    try
    {
        r = djvLsApplication(argc, argv).run();
    }
    catch (const djvError & error)
    {
        djvErrorUtil::print(error);
    }

    return r;
}
