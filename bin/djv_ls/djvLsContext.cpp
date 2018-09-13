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

#include <djvLsContext.h>

#include <djvCore/System.h>

#include <QCoreApplication>

using namespace djv;

djvLsContext::djvLsContext(QObject * parent) :
    Graphics::GraphicsContext(parent),
    _fileInfo     (true),
    _filePath     (false),
    _sequence     (djvSequence::COMPRESS_RANGE),
    _recurse      (false),
    _hidden       (false),
    _columns      (djvSystem::terminalWidth()),
    _sort         (djvFileInfoUtil::SORT_NAME),
    _reverseSort  (false),
    _sortDirsFirst(true)
{
    //DJV_DEBUG("djvLsContext::djvLsContext");
    
    // Load translators.
    loadTranslator("djv_ls");
}

djvLsContext::~djvLsContext()
{
    //DJV_DEBUG("djvLsContext::~djvLsContext");
}

const QStringList & djvLsContext::input() const
{
    return _input;
}

bool djvLsContext::hasFileInfo() const
{
    return _fileInfo;
}

bool djvLsContext::hasFilePath() const
{
    return _filePath;
}

djvSequence::COMPRESS djvLsContext::sequence() const
{
    return _sequence;
}

bool djvLsContext::hasRecurse() const
{
    return _recurse;
}

bool djvLsContext::hasHidden() const
{
    return _hidden;
}

const QStringList & djvLsContext::glob() const
{
    return _glob;
}

int djvLsContext::columns() const
{
    return _columns;
}

djvFileInfoUtil::SORT djvLsContext::sort() const
{
    return _sort;
}

bool djvLsContext::hasReverseSort() const
{
    return _reverseSort;
}

bool djvLsContext::hasSortDirsFirst() const
{
    return _sortDirsFirst;
}

bool djvLsContext::commandLineParse(QStringList & in) throw (QString)
{
    //DJV_DEBUG("djvLsContext::commandLineParse");
    //DJV_DEBUG_PRINT("in = " << in);

    if (!Graphics::GraphicsContext::commandLineParse(in))
        return false;

    QStringList args;
    QString     arg;
    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            // Parse the options.
            if (
                qApp->translate("djvLsContext", "-x_info") == arg ||
                qApp->translate("djvLsContext", "-xi") == arg)
            {
                _fileInfo = false;
            }
            else if (
                qApp->translate("djvLsContext", "-file_path") == arg ||
                qApp->translate("djvLsContext", "-fp") == arg)
            {
                _filePath = true;
            }
            else if (
                qApp->translate("djvLsContext", "-seq") == arg ||
                qApp->translate("djvLsContext", "-q") == arg)
            {
                in >> _sequence;
            }
            else if (
                qApp->translate("djvLsContext", "-recurse") == arg ||
                qApp->translate("djvLsContext", "-r") == arg)
            {
                _recurse = true;
            }
            else if (
                qApp->translate("djvLsContext", "-hidden") == arg)
            {
                _hidden = true;
            }
            else if (
                qApp->translate("djvLsContext", "-columns") == arg ||
                qApp->translate("djvLsContext", "-c") == arg)
            {
                in >> _columns;
            }

            // Parse the sorting options.
            else if (
                qApp->translate("djvLsContext", "-sort") == arg ||
                qApp->translate("djvLsContext", "-s") == arg)
            {
                in >> _sort;
            }
            else if (
                qApp->translate("djvLsContext", "-reverse_sort") == arg ||
                qApp->translate("djvLsContext", "-rs") == arg)
            {
                _reverseSort = true;
            }
            else if (
                qApp->translate("djvLsContext", "-x_sort_dirs") == arg ||
                qApp->translate("djvLsContext", "-xsd") == arg)
            {
                _sortDirsFirst = false;
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
    
    return true;
}

QString djvLsContext::commandLineHelp() const
{
    static const QString label = qApp->translate("djvLsContext",
"djv_ls\n"
"\n"
"    The djv_ls application is a command line tool for listing directories "
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
"    File - Regular file           r - Readable\n"
"    Seq  - File sequence          w - Writable\n"
"    Dir  - Directory              x - Executable\n"
"\n"
"Usage\n"
"\n"
"    djv_ls [file|directory]... [option]...\n"
"\n"
"    file      - One or more files or image sequences\n"
"    directory - One or more directories\n"
"    option    - Additional options (see below)\n"
"\n"
"    If no files or directories are given then the current directory will be "
"used.\n"
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
"\n"
"    > djv_ls\n"
"\n"
"    List specific directories:\n"
"\n"
"    > djv_ls ~/movies ~/pictures\n"
"\n"
"    Sort by time with the most recent first:\n"
"\n"
"    > djv_ls -sort time -reverse_sort\n");
    return QString(label).
        arg(djvSequence::compressLabels().join(", ")).
        arg(djvStringUtil::label(_sequence).join(", ")).
        arg(djvFileInfoUtil::sortLabels().join(", ")).
        arg(djvStringUtil::label(_sort).join(", ")).
        arg(Graphics::GraphicsContext::commandLineHelp());
}
