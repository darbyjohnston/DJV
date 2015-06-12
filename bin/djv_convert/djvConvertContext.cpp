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

//! \file djvConvertContext.cpp

#include <djvConvertContext.h>

#include <djvDebugLog.h>
#include <djvFileInfoUtil.h>

#include <QCoreApplication>

//------------------------------------------------------------------------------
// djvConvertOptions
//------------------------------------------------------------------------------

djvConvertOptions::djvConvertOptions() :
    scale   (1.0),
    channel (static_cast<djvOpenGlImageOptions::CHANNEL>(0)),
    sequence(djvSequence::COMPRESS_RANGE)
{}

//------------------------------------------------------------------------------
// djvConvertInput
//------------------------------------------------------------------------------

djvConvertInput::djvConvertInput() :
    layer      (0),
    proxy      (static_cast<djvPixelDataInfo::PROXY>(0)),
    slateFrames(0),
    timeout    (0)
{}

//------------------------------------------------------------------------------
// djvConvertOutput
//------------------------------------------------------------------------------

djvConvertOutput::djvConvertOutput() :
    tagsAuto(true)
{}

//------------------------------------------------------------------------------
// djvConvertContext
//------------------------------------------------------------------------------

djvConvertContext::djvConvertContext(QObject * parent) :
    djvImageContext(parent)
{
    //DJV_DEBUG("djvConvertContext::djvConvertContext");
    
    // Load translators.
    
    loadTranslator("djv_convert");
}

djvConvertContext::~djvConvertContext()
{
    //DJV_DEBUG("djvConvertContext::~djvConvertContext");
}

const djvConvertOptions & djvConvertContext::options() const
{
    return _options;
}

const djvConvertInput & djvConvertContext::input() const
{
    return _input;
}

const djvConvertOutput & djvConvertContext::output() const
{
    return _output;
}

bool djvConvertContext::commandLineParse(QStringList & in) throw (QString)
{
    //DJV_DEBUG("djvConvertContext::commandLineParse");
    //DJV_DEBUG_PRINT("in = " << in);

    if (! djvImageContext::commandLineParse(in))
        return false;

    if (in.isEmpty())
    {
        print("\n" + commandLineHelp());
        
        return false;
    }

    QStringList args;
    QString     arg;

    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            // Parse the options.

            if (
                qApp->translate("djvConvertContext", "-mirror_h") == arg)
            {
                _options.mirror.x = true;
            }
            else if (
                qApp->translate("djvConvertContext", "-mirror_v") == arg)
            {
                _options.mirror.y = true;
            }
            else if (
                qApp->translate("djvConvertContext", "-scale") == arg)
            {
                in >> _options.scale.x;
                _options.scale.y = _options.scale.x;
            }
            else if (
                qApp->translate("djvConvertContext", "-scale_separate") == arg)
            {
                in >> _options.scale;
            }
            else if (
                qApp->translate("djvConvertContext", "-resize") == arg)
            {
                in >> _options.size;
            }
            else if (
                qApp->translate("djvConvertContext", "-width") == arg)
            {
                in >> _options.size.x;
            }
            else if (
                qApp->translate("djvConvertContext", "-height") == arg)
            {
                in >> _options.size.y;
            }
            else if (
                qApp->translate("djvConvertContext", "-crop") == arg)
            {
                in >> _options.crop;
            }
            else if (
                qApp->translate("djvConvertContext", "-crop_percent") == arg)
            {
                in >> _options.cropPercent;
            }
            else if (
                qApp->translate("djvConvertContext", "-channel") == arg)
            {
                in >> _options.channel;
            }
            else if (
                qApp->translate("djvConvertContext", "-seq") == arg ||
                qApp->translate("djvConvertContext", "-q") == arg)
            {
                in >> _options.sequence;
            }

            // Parse the input options.

            else if (
                qApp->translate("djvConvertContext", "-layer") == arg)
            {
                in >> _input.layer;
            }
            else if (
                qApp->translate("djvConvertContext", "-proxy") == arg)
            {
                in >> _input.proxy;
            }
            else if (
                qApp->translate("djvConvertContext", "-time") == arg)
            {
                in >> _input.start;
                in >> _input.end;
            }
            else if (
                qApp->translate("djvConvertContext", "-slate") == arg)
            {
                QString tmp;
                in >> tmp;
                _input.slate = tmp;
                in >> _input.slateFrames;
            }
            else if (
                qApp->translate("djvConvertContext", "-timeout") == arg)
            {
                in >> _input.timeout;
            }

            // Parse the output options.

            else if (
                qApp->translate("djvConvertContext", "-pixel") == arg)
            {
                djvPixel::PIXEL value = static_cast<djvPixel::PIXEL>(0);
                in >> value;
                _output.pixel.reset(new djvPixel::PIXEL(value));
            }
            else if (
                qApp->translate("djvConvertContext", "-speed") == arg)
            {
                djvSpeed::FPS value = static_cast<djvSpeed::FPS>(0);
                in >> value;
                _output.speed.reset(new djvSpeed::FPS(value));
            }
            else if (
                qApp->translate("djvConvertContext", "-tag") == arg)
            {
                QString name, value;
                in >> name;
                in >> value;
                _output.tags[name] = value;
            }
            else if (
                qApp->translate("djvConvertContext", "-tags_auto") == arg)
            {
                in >> _output.tagsAuto;
            }

            // Parse the arguments.

            else
            {
                args += arg;
            }
        }
    }
    catch (const QString &)
    {
        throw QString(arg);
    }

    //DJV_DEBUG_PRINT("args = " << args);
    
    DJV_LOG(debugLog(), "djv_convert",
        QString("Args = %1").arg(djvStringUtil::addQuotes(args).join(", ")));

    // Parse the input file.

    if (! args.count())
    {
        throw QString(qApp->translate("djvConvertContext", "input"));
    }

    _input.file = djvFileInfoUtil::parse(args.first(), _options.sequence);
    
    DJV_LOG(debugLog(), "djv_convert",
        QString("Input = \"%1\"").arg(_input.file));

    args.pop_front();

    // Parse the output file.

    if (! args.count())
    {
        throw QString(qApp->translate("djvConvertContext", "output"));
    }

    _output.file = djvFileInfoUtil::parse(args.first(), _options.sequence);

    DJV_LOG(debugLog(), "djv_convert",
        QString("Output = \"%1\"").arg(_output.file));

    args.pop_front();

    if (! args.isEmpty())
    {
        throw QString(args[0]);
    }
    
    return true;
}

QString djvConvertContext::commandLineHelp() const
{
    static const QString label = qApp->translate("djvConvertContext",
"djv_convert\n"
"\n"
"    The djv_convert application is a command line tool for batch processing "
"images and movies. Common uses include resizing images and converting "
"sequences of images to a QuickTime movie.\n"
"\n"
"    Here is an example of how to resize a sequence of images:\n"
"\n"
"    > djv_convert input.1-100.tga output.1.tga -resize 2048 1556\n"
"\n"
"    Note that only the first frame of the output sequence needs to be "
"specified, the remaining frames will be added automatically. See below for "
"more examples.\n"
"\n"
"    Note also that the djv_convert application uses OpenGL for image "
"processing so it needs to be run on a machine with graphics resources.\n"
"\n"
"Usage\n"
"\n"
"    djv_convert (input) (output) [option]...\n"
"\n"
"    input  - Input image, image sequence, or movie\n"
"    output - Output image, image sequence, or movie\n"
"    option - Additional options (see below)\n"
"\n"
"Conversion Options\n"
"\n"
"    -mirror_h\n"
"        Mirror the image horizontally.\n"
"    -mirror_v\n"
"        Mirror the image vertically.\n"
"    -scale (value)\n"
"        Scale the image width and height using a floating point value "
"        (1.0 = 100%).\n"
"    -scale_separate (width) (height)\n"
"        Scale the image width and height separately using a floating point "
"value (1.0 = 100%).\n"
"    -resize (width) (height)\n"
"        Resize the image width and height to the given resolution.\n"
"    -width (value)\n"
"        Resize the image width to the given resolution maintaining the aspect "
"ratio.\n"
"    -height (value)\n"
"        Resize the image height to the given resolution maintaining the "
"aspect ratio.\n"
"    -crop (x) (y) (width) (height)\n"
"        Crop the image.\n"
"    -crop_percent (x) (y) (width) (height)\n"
"        Crop the image using percentages.\n"
"    -channel (value)\n"
"        Show only specific image channels. Options = %1. Default = %2.\n"
"    -seq, -q (value)\n"
"        Set whether file sequencing is enabled. Options = %3. Default = %4.\n"
"\n"
"Input Options\n"
"\n"
"    -layer (value)\n"
"        Set the input layer.\n"
"    -proxy (value)\n"
"        Set the proxy scale. Options = %5. Default = %6.\n"
"    -time (start) (end)\n"
"        Set the start and end time.\n"
"    -slate (input) (frames)\n"
"        Set the slate.\n"
"    -timeout (value)\n"
"        Set the maximum number of seconds to wait for each input frame. "
"Default = %7.\n"
"\n"
"Output Options\n"
"\n"
"    -pixel (value)\n"
"        Convert the pixel type. Options = %8.\n"
"    -speed (value)\n"
"        Set the speed. Options = %9.\n"
"    -tag (name) (value)\n"
"        Set an image tag.\n"
"    -tags_auto (value)\n"
"        Automatically generate image tags (e.g., timecode). Options = %10. "
"Default = %11.\n"
"%12"
"\n"
"Examples\n"
"\n"
"    Convert an image:\n"
"\n"
"    > djv_convert input.sgi output.tga\n"
"\n"
"    Convert an image sequence. Note that only the first frame of the output "
"sequence needs to be specified, the remaining frames will be added "
"automatically:\n"
"\n"
"    > djv_convert input.1-100.sgi output.1.tga\n"
"\n"
"    Create an RLE compressed image sequence:\n"
"\n"
"    > djv_convert input.1-100.sgi output.1.tga -targa_compression RLE\n"
"\n"
"    Convert an image sequence to a movie:\n"
"\n"
"    > djv_convert input.0001-0100.dpx output.m4v\n"
"\n"
"    Convert a movie to an image sequence:\n"
"\n"
"    > djv_convert input.m4v output.1.tga\n"
"\n"
"    Convert the pixel type. Note the use of the quotes around the pixel type"
"option:\n"
"\n"
"    > djv_convert input.sgi output.sgi -pixel \"RGB U16\"\n"
"\n"
"    Scale an image by half:\n"
"\n"
"    > djv_convert input.tga output.tga -scale 0.5\n"
"\n"
"    Resize an image:\n"
"\n"
"    > djv_convert input.tga output.tga -resize 2048 1556\n"
"\n"
"    Convert a Cineon file to a linear format using the default settings:\n"
"\n"
"    > djv_convert input.cin output.tga\n"
"\n"
"    Convert a Cineon file to a linear format using custom film print settings "
"(black point, white point, gamma, and soft clip):\n"
"\n"
"    > djv_convert input.cin output.tga -cineon_input_film_print 95 685 2.2 2\n");

    return QString(label).
        arg(djvOpenGlImageOptions::channelLabels().join(", ")).
        arg(djvStringUtil::label(_options.channel).join(", ")).
        arg(djvSequence::compressLabels().join(", ")).
        arg(djvStringUtil::label(_options.sequence).join(", ")).
        arg(djvPixelDataInfo::proxyLabels().join(", ")).
        arg(djvStringUtil::label(_input.proxy).join(", ")).
        arg(_input.timeout).
        arg(djvPixel::pixelLabels().join(", ")).
        arg(djvSpeed::fpsLabels().join(", ")).
        arg(djvStringUtil::boolLabels().join(", ")).
        arg(djvStringUtil::label(_output.tagsAuto).join(", ")).
        arg(djvImageContext::commandLineHelp());
}
