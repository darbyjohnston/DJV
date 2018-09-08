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

#include <djvInfoContext.h>

#include <djvSystem.h>

#include <QCoreApplication>

//------------------------------------------------------------------------------
// djvInfoContext
//------------------------------------------------------------------------------

djvInfoContext::djvInfoContext(QObject * parent) :
    djvGraphicsContext(parent),
    _info    (true),
    _verbose (false),
    _filePath(false),
    _sequence(djvSequence::COMPRESS_RANGE),
    _recurse (false),
    _columns (djvSystem::terminalWidth())
{
    //DJV_DEBUG("djvInfoContext::djvInfoContext");
    
    // Load translators.
    loadTranslator("djv_info");
}

djvInfoContext::~djvInfoContext()
{
    //DJV_DEBUG("djvInfoContext::~djvInfoContext");
}

const QStringList & djvInfoContext::input() const
{
    return _input;
}

bool djvInfoContext::hasInfo() const
{
    return _info;
}

bool djvInfoContext::hasVerbose() const
{
    return _verbose;
}

bool djvInfoContext::hasFilePath() const
{
    return _filePath;
}

djvSequence::COMPRESS djvInfoContext::sequence() const
{
    return _sequence;
}

bool djvInfoContext::hasRecurse() const
{
    return _recurse;
}

int djvInfoContext::columns() const
{
    return _columns;
}

bool djvInfoContext::commandLineParse(QStringList & in) throw (QString)
{
    //DJV_DEBUG("djvInfoContext::commandLineParse");
    //DJV_DEBUG_PRINT("in = " << in);

    if (! djvGraphicsContext::commandLineParse(in))
        return false;

    QString arg;
    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            // Parse the options.
            if (
                qApp->translate("djvInfoContext", "-x_info") == arg ||
                qApp->translate("djvInfoContext", "-xi") == arg)
            {
                _info = false;
            }
            else if (
                qApp->translate("djvInfoContext", "-verbose") == arg ||
                qApp->translate("djvInfoContext", "-v") == arg)
            {
                _verbose = true;
            }
            else if (
                qApp->translate("djvInfoContext", "-file_path") == arg ||
                qApp->translate("djvInfoContext", "-fp") == arg)
            {
                _filePath = true;
            }
            else if (
                qApp->translate("djvInfoContext", "-seq") == arg ||
                qApp->translate("djvInfoContext", "-q") == arg)
            {
                in >> _sequence;
            }
            else if (
                qApp->translate("djvInfoContext", "-recurse") == arg ||
                qApp->translate("djvInfoContext", "-r") == arg)
            {
                _recurse = true;
            }
            else if (
                qApp->translate("djvInfoContext", "-columns") == arg ||
                qApp->translate("djvInfoContext", "-c") == arg)
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
        throw QString(arg);
    }

    return true;
}

QString djvInfoContext::commandLineHelp() const
{
    static const QString label = qApp->translate("djvInfoContext",
"djv_info\n"
"\n"
"    The djv_info application is a command line tool for displaying information "
"about images and movies.\n"
"\n"
"    Example output:\n"
"\n"
"    yesterdayview.mov                    640x424:1.51 RGB U8 00:02:00:01@12\n"
"    dlad.dpx                          2048x1556:1.32 RGB U10 00:00:00:01@24\n"
"    render0001-1000.exr                                      00:00:41:16@24\n"
"        0: A,B,G,R                                     720x480:1.5 RGBA F16\n"
"        1: Ambient                                      720x480:1.5 RGB F16\n"
"        2: Diffuse                                      720x480:1.5 RGB F16\n"
"        3: Specular                                     720x480:1.5 RGB F16\n"
"        4: Z                                              720x480:1.5 L F32\n"
"\n"
"    Key:\n"
"\n"
"    (name)     (width)x(height):(aspect) (format) (type) (duration)@(speed)\n"
"\n"
"    Layer key:\n"
"\n"
"    (name)                                               (duration)@(speed)\n"
"        (layer): (name)           (width)x(height):(aspect) (format) (type)\n"
"\n"
"Usage\n"
"\n"
"    djv_info [image|directory]... [option]...\n"
"\n"
"    image     - One or more images, image sequences, or movies\n"
"    directory - One or more directories\n"
"    option    - Additional options (see below)\n"
"\n"
"    If no images or directories are given then the current directory will be "
"used.\n"
"\n"
"Options\n"
"\n"
"    -x_info, -xi\n"
"        Don't show image information, only file names.\n"
"    -verbose, -v\n"
"        Show verbose image information.\n"
"    -file_path, -fp\n"
"        Show file path names.\n"
"    -seq, -q (value)\n"
"        Set file sequencing. Options = %1. Default = %2.\n"
"    -recurse, -r\n"
"        Descend into sub-directories.\n"
"    -columns, -c (value)\n"
"        Set the number of columns for formatting output. A value of zero "
"disables formatting.\n"
"%3"
"\n"
"Examples\n"
"\n"
"    Display image information:\n"
"\n"
"    > djv_info image.sgi image2.sgi\n"
"\n"
"    Display image sequence information:\n"
"\n"
"    > djv_info image.1-100.sgi\n"
"\n"
"    Display information about all images within a directory:\n"
"\n"
"    > djv_info ~/pics\n");
    return QString(label).
        arg(djvSequence::compressLabels().join(", ")).
        arg(djvStringUtil::label(_sequence).join(", ")).
        arg(djvGraphicsContext::commandLineHelp());
}
