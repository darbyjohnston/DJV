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

//! \file djv_convert.cpp

#include <djv_convert.h>

#include <djvColor.h>
#include <djvError.h>
#include <djvErrorUtil.h>
#include <djvImageIo.h>
#include <djvSequenceUtil.h>
#include <djvSystem.h>
#include <djvTime.h>
#include <djvTimer.h>
#include <djvUser.h>

//------------------------------------------------------------------------------
// djvConvertOptions
//------------------------------------------------------------------------------

djvConvertOptions::djvConvertOptions() :
    scale   (1.0),
    channel (static_cast<djvOpenGlImageOptions::CHANNEL>(0)),
    sequence(true)
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
// djvConvertApplication
//------------------------------------------------------------------------------

djvConvertApplication::djvConvertApplication(int argc, char ** argv)
    throw (djvError) :
    djvImageApplication("djv_convert", argc, argv)
{
    //DJV_DEBUG("djvConvertApplication::djvConvertApplication");

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

    // Convert the images.

    if (! work())
    {
        setExitValue(EXIT_VALUE_ERROR);
    }
}

void djvConvertApplication::commandLine(QStringList & in) throw (djvError)
{
    //DJV_DEBUG("djvConvertApplication::commandLine");
    //DJV_DEBUG_PRINT("in = " << in);

    djvImageApplication::commandLine(in);

    if (exitValue() != EXIT_VALUE_DEFAULT)
        return;

    if (in.isEmpty())
    {
        setExitValue(EXIT_VALUE_HELP);
        
        return;
    }

    QStringList args;
    QString     arg;

    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            // Parse the options.

            if ("-mirror_h" == arg)
            {
                _options.mirror.x = true;
            }
            else if ("-mirror_v" == arg)
            {
                _options.mirror.y = true;
            }
            else if ("-scale" == arg)
            {
                in >> _options.scale.x;
                _options.scale.y = _options.scale.x;
            }
            else if ("-scale_xy" == arg)
            {
                in >> _options.scale;
            }
            else if ("-resize" == arg)
            {
                in >> _options.size;
            }
            else if ("-width" == arg)
            {
                in >> _options.size.x;
            }
            else if ("-height" == arg)
            {
                in >> _options.size.y;
            }
            else if ("-crop" == arg)
            {
                in >> _options.crop;
            }
            else if ("-crop_percent" == arg)
            {
                in >> _options.cropPercent;
            }
            else if ("-channel" == arg)
            {
                in >> _options.channel;
            }
            else if ("-seq" == arg)
            {
                in >> _options.sequence;
            }

            // Parse the input options.

            else if ("-layer" == arg)
            {
                in >> _input.layer;
            }
            else if ("-proxy" == arg)
            {
                in >> _input.proxy;
            }
            else if ("-time" == arg)
            {
                in >> _input.start;
                in >> _input.end;
            }
            else if ("-slate" == arg)
            {
                QString tmp;
                in >> tmp;
                _input.slate = tmp;
                in >> _input.slateFrames;
            }
            else if ("-timeout" == arg)
            {
                in >> _input.timeout;
            }

            // Parse the output options.

            else if ("-pixel" == arg)
            {
                djvPixel::PIXEL value = static_cast<djvPixel::PIXEL>(0);
                in >> value;
                _output.pixel.reset(new djvPixel::PIXEL(value));
            }
            else if ("-speed" == arg)
            {
                djvSpeed::FPS value = static_cast<djvSpeed::FPS>(0);
                in >> value;
                _output.speed.reset(new djvSpeed::FPS(value));
            }
            else if ("-tag" == arg)
            {
                QString name, value;
                in >> name;
                in >> value;
                _output.tags[name] = value;
            }
            else if ("-tags_auto" == arg)
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
        throw djvError(arg);
    }

    //DJV_DEBUG_PRINT("arg list = " << args);

    // Parse the input file.

    if (! args.count())
    {
        throw djvError("Input");
    }

    _input.file = args.first();

    args.pop_front();

    if (_options.sequence && _input.file.isSequenceValid())
    {
        _input.file.setType(djvFileInfo::SEQUENCE);
    }

    // Parse the output file.

    if (! args.count())
    {
        throw djvError("Output");
    }

    _output.file = args.first();

    args.pop_front();

    if (_options.sequence && _output.file.isSequenceValid())
    {
        _output.file.setType(djvFileInfo::SEQUENCE);
    }

    if (! args.isEmpty())
    {
        throw djvError(args[0]);
    }
}

namespace
{

const QString commandLineHelpLabel =
"\n"
" djv_convert\n"
"\n"
"     This application provides a command line tool for processing images and "
"movies.\n"
"\n"
" Usage\n"
"\n"
"     djv_convert (input) (output) [option]...\n"
"\n"
" Conversion Options\n"
"\n"
"     -mirror_h\n"
"     -mirror_v\n"
"         Mirror the image horizontally or vertically.\n"
"     -scale (value)\n"
"     -scale_xy (x) (y)\n"
"         Scale the image.\n"
"     -resize (width) (height)\n"
"         Resize the image.\n"
"     -width (value)\n"
"     -height (value)\n"
"         Resize the width or height of the image maintaining the aspect"
"ratio.\n"
"     -crop (x) (y) (width) (height)\n"
"         Crop the image.\n"
"     -crop_percent (x) (y) (width) (height)\n"
"         Crop the image as a percentage.\n"
"     -channel (value)\n"
"         Show only specific image channels. Options = %1. Default = %2.\n"
"     -seq (value)\n"
"         Set whether file sequencing is enabled. Options = %3. Default = %4.\n"
"\n"
" Input Options\n"
"\n"
"     -layer (value)\n"
"         Set the input layer.\n"
"     -proxy (value)\n"
"         Set the proxy scale. Options = %5. Default = %6.\n"
"     -time (start) (end)\n"
"         Set the start and end time.\n"
"     -slate (input) (frames)\n"
"         Set the slate.\n"
"     -timeout (value)\n"
"         Set the maximum number of seconds to wait for each input frame."
" Default = %7.\n"
"\n"
" Output Options\n"
"\n"
"     -pixel (value)\n"
"         Convert the pixel type. Options = %8.\n"
"     -speed (value)\n"
"         Set the speed. Options = %9.\n"
"     -tag (name) (value)\n"
"         Set an image tag.\n"
"     -tags_auto (value)\n"
"         Automatically generate image tags (e.g., timecode). Options = %10."
" Default = %11.\n"
"%12"
"\n"
" Examples\n"
"\n"
"     Convert an image:\n"
"     > djv_convert input.sgi output.tga\n"
"\n"
"     Convert an image sequence. Note that only the first frame of the output"
" is specified:\n"
"     > djv_convert input.1-100.sgi output.1.tga\n"
"\n"
"     Create an RLE compressed image sequence:\n"
"     > djv_convert input.1-100.sgi output.1.tga -targa_compression rle\n"
"\n"
"     Convert an image sequence to a movie:\n"
"     > djv_convert input.0001-0100.dpx output.mov\n"
"\n"
"     Convert a movie to an image sequence:\n"
"     > djv_convert input.mov output.1.tga\n"
"\n"
"     Convert the pixel type:\n"
"     > djv_convert input.sgi output.sgi -pixel \"rgb u16\"\n"
"\n"
"     Scale an image by half:\n"
"     > djv_convert input.tga output.tga -scale 0.5 0.5\n"
"\n"
"     Resize an image:\n"
"     > djv_convert input.tga output.tga -resize 2048 1556\n"
"\n"
"     Convert a Cineon file to a linear format using the default settings:\n"
"     > djv_convert input.cin output.tga\n"
"\n"
"     Convert a Cineon file to a linear format using custom print settings"
" (black point, white point, gamma, and soft clip):\n"
"     > djv_convert input.cin output.tga -cineon_input_film_print 95 685 2.2 10\n";

} // namespace

QString djvConvertApplication::commandLineHelp() const
{
    return QString(commandLineHelpLabel).
        arg(djvOpenGlImageOptions::channelLabels().join(", ")).
        arg(djvStringUtil::label(_options.channel).join(", ")).
        arg(djvStringUtil::boolLabels().join(", ")).
        arg(djvStringUtil::label(_options.sequence).join(", ")).
        arg(djvPixelDataInfo::proxyLabels().join(", ")).
        arg(djvStringUtil::label(_input.proxy).join(", ")).
        arg(_input.timeout).
        arg(djvPixel::pixelLabels().join(", ")).
        arg(djvSpeed::fpsLabels().join(", ")).
        arg(djvStringUtil::boolLabels().join(", ")).
        arg(djvStringUtil::label(_output.tagsAuto).join(", ")).
        arg(djvImageApplication::commandLineHelp());
}

namespace
{

QString labelImage(const djvPixelDataInfo & in, const djvSequence & sequence)
{
    return QString("%1x%2:%3 %4 %5@%6").
        arg(in.size.x).
        arg(in.size.y).
        arg(djvVectorUtil::aspect(in.size), 0, 'f', 2).
        arg(djvStringUtil::label(in.pixel).join(", ")).
        arg(djvTime::unitsToString(sequence.frames.count(), sequence.speed)).
        arg(djvSpeed::speedToFloat(sequence.speed));
}

} // namespace

bool djvConvertApplication::work()
{
    //DJV_DEBUG("djvConvertApplication::work");
    //DJV_DEBUG_PRINT("input = " << _input.file);
    //DJV_DEBUG_PRINT("output = " << _output.file);

    djvTimer timer;
    timer.start();

    djvOpenGlImageOptions options;
    options.xform.mirror = _options.mirror;
    options.xform.scale  = _options.scale;
    options.channel      = _options.channel;

    // Open the input file.

    QScopedPointer<djvImageLoad> load;
    djvImageIoInfo               loadInfo;

    djvError error;

    while (! load.data())
    {
        try
        {
            load.reset(djvImageIoFactory::global()->load(_input.file, loadInfo));
        }
        catch (const djvError & in)
        {
            error = in;
        }

        if (! load.data() && _input.timeout)
        {
            print("Timeout...");
            
            djvTime::sleep(1);
        }
        else
        {
            break;
        }
    }

    if (! load.data())
    {
        printError(error);
        
        return false;
    }

    _input.layer = djvMath::clamp(_input.layer, 0, loadInfo.layerCount() - 1);

    qint64 start = 0;
    qint64 end =
        loadInfo.sequence.frames.count() ?
        (static_cast<qint64>(loadInfo.sequence.frames.count()) - 1) :
        0;
    
    if (! _input.start.isEmpty())
        start = djvSequenceUtil::findClosest(
            djvTime::stringToUnits(_input.start, loadInfo.sequence.speed),
            loadInfo.sequence.frames);

    if (! _input.end.isEmpty())
        end = djvSequenceUtil::findClosest(
            djvTime::stringToUnits(_input.end, loadInfo.sequence.speed),
            loadInfo.sequence.frames);

    //DJV_DEBUG_PRINT("start = " << start);
    //DJV_DEBUG_PRINT("end = " << end);

    loadInfo.sequence.frames =
        loadInfo.sequence.frames.mid(start, end - start + 1);

    print(QString("%1 %2").
        arg(_input.file).
        arg(labelImage(loadInfo, loadInfo.sequence)));

    // Open the output file.

    QScopedPointer<djvImageSave> save;

    djvImageIoInfo saveInfo(loadInfo[_input.layer]);
    
    djvVector2i scaleSize = loadInfo.size;

    if (djvVectorUtil::isSizeValid(_options.size))
    {
        scaleSize = _options.size;
    }
    else if (_options.size.x)
    {
        scaleSize = djvVector2i(
            _options.size.x,
            djvMath::ceil<int>(_options.size.x / djvVectorUtil::aspect(loadInfo.size)));
    }
    else if (_options.size.y)
    {
        scaleSize = djvVector2i(
            djvMath::ceil<int>(_options.size.x * djvVectorUtil::aspect(loadInfo.size)),
            _options.size.y);
    }
    else if (
        ! djvMath::fuzzyCompare(options.xform.scale.x, 1.0) &&
        ! djvMath::fuzzyCompare(options.xform.scale.y, 1.0))
    {
        scaleSize = djvVectorUtil::ceil<double, int>(
            djvVector2f(loadInfo.size) * options.xform.scale);
    }
    
    djvVector2f position;
    
    if (_options.crop.isValid())
    {
        position = -_options.crop.position;
        
        saveInfo.size = _options.crop.size;
    }
    else if (_options.cropPercent.isValid())
    {
        position = -djvVectorUtil::ceil<double, int>(
            djvVector2f(scaleSize) * (_options.cropPercent.position / 100.0));

        saveInfo.size = djvVectorUtil::ceil<double, int>(
            djvVector2f(scaleSize) * (_options.cropPercent.size / 100.0));
    }
    else
    {
        saveInfo.size = scaleSize;
    }
    
    if (_output.pixel.data())
    {
        saveInfo.pixel = *_output.pixel;
    }

    saveInfo.tags = _output.tags;

    saveInfo.sequence = djvSequence(
        _output.file.sequence().start(),
        _output.file.sequence().start() +
        _input.slateFrames + (
            loadInfo.sequence.frames.count() ?
            (static_cast<qint64>(loadInfo.sequence.frames.count()) - 1) :
            0),
        _output.file.sequence().pad,
        loadInfo.sequence.speed);

    if (_output.speed.data())
    {
        saveInfo.sequence.speed = *_output.speed;
    }
    
    //DJV_DEBUG_PRINT("save sequence = " << saveInfo.sequence);

    try
    {
        save.reset(
            djvImageIoFactory::global()->save(_output.file, saveInfo));
        
        if (! save.data())
            throw djvError(QString("Cannot open image \"%1\"").
                arg(_output.file));
    }
    catch (const djvError & error)
    {
        printError(error);
        
        return false;
    }

    print(QString("%1 %2").
        arg(_output.file).
        arg(labelImage(saveInfo, saveInfo.sequence)));

    // Add the slate.

    djvImage slate;

    if (! _input.slate.name().isEmpty())
    {
        try
        {
            print("Slating...");

            djvImageIoInfo info;
            
            QScopedPointer<djvImageLoad> load(
                djvImageIoFactory::global()->load(_input.slate, info));
            
            if (! load.data())
                throw djvError(QString("Cannot open slate: \"%1\"").
                    arg(_input.slate));

            djvImage image;
            
            load->read(image);

            slate.set(saveInfo);

            djvOpenGlImageOptions options;
            options.xform.position = position;
            options.xform.scale = djvVector2f(scaleSize) / djvVector2f(info.size);
            options.colorProfile = image.colorProfile;

            djvOpenGlImage::copy(image, slate, options);
        }
        catch (const djvError & error)
        {
            printError(error);
            
            return false;
        }
    }

    // Convert the images.

    _offscreenBuffer.reset(new djvOpenGlOffscreenBuffer(saveInfo));
    
    for (qint64 i = 0; i < _input.slateFrames; ++i)
    {
        try
        {
            save->write(
                slate,
                djvImageIoFrameInfo(saveInfo.sequence.frames.first()));

            saveInfo.sequence.frames.pop_front();
        }
        catch (const djvError & error)
        {
            printError(error);
            
            return false;
        }
    }

    const qint64 length = static_cast<qint64>(saveInfo.sequence.frames.count());

    double   progressAccum = 0.0;
    djvTimer progressTimer;
    progressTimer.start();

    for (qint64 i = 0; i < length; ++i)
    {
        djvTimer frameTimer;
        frameTimer.start();

        // Load the current image.

        djvImage image;

        int timeout = _input.timeout;

        while (! image.isValid())
        {
            try
            {
                load->read(
                    image,
                    djvImageIoFrameInfo(
                        loadInfo.sequence.frames.count() ?
                        loadInfo.sequence.frames[i] :
                        -1,
                        _input.layer,
                        _input.proxy));
            }
            catch (const djvError & in)
            {
                error = in;
            }

            if (! image.isValid() && timeout > 0)
            {
                //print("Timeout...");
                
                --timeout;
                
                djvTime::sleep(1);
            }
            else
            {
                break;
            }
        }

        if (! image.isValid())
        {
            printError(error);
            
            return false;
        }

        //DJV_DEBUG_PRINT("image = " << *image);

        // Process the image tags.

        djvImageTags tags = _output.tags;

        tags.add(image.tags);

        if (_output.tagsAuto)
        {
            tags[djvImageTags::tagLabels()[djvImageTags::CREATOR]] =
                djvUser::current();

            tags[djvImageTags::tagLabels()[djvImageTags::TIME]] =
                djvTime::timeToString(djvTime::current());

            tags[djvImageTags::tagLabels()[djvImageTags::TIMECODE]] =
                djvTime::timecodeToString(
                    djvTime::frameToTimecode(
                        saveInfo.sequence.frames.count() ?
                        saveInfo.sequence.frames[i] :
                        0,
                        saveInfo.sequence.speed));
        }

        // Convert.

        djvImage tmp(saveInfo);
        tmp.tags = tags;

        options.xform.position = position;
        options.xform.scale = djvVector2f(scaleSize) /
            djvVector2f(loadInfo.size);
        
        options.colorProfile = image.colorProfile;

        djvOpenGlImage::copy(
            image,
            tmp,
            options,
            &_state,
            _offscreenBuffer.data());

        // Save the image.

        //DJV_DEBUG_PRINT("output = " << tmp);

        try
        {
            save->write(
                tmp,
                djvImageIoFrameInfo(
                    saveInfo.sequence.frames.count() ?
                    saveInfo.sequence.frames[i] :
                    -1));
        }
        catch (const djvError & error)
        {
            printError(error);
            return false;
        }

        // Statistics.

        timer.check();
        frameTimer.check();

        progressAccum += frameTimer.seconds();
        progressTimer.check();

        if (length > 1 && progressTimer.seconds() > 3.0)
        {
            const double estimate =
                progressAccum /
                static_cast<double>(i + 1) * (length - (i + 1));

            print(QString("[%1%] Estimated = %2 (%3 Frames/Second)").
                arg(static_cast<int>(
                    i / static_cast<double>(length) * 100.0), 3).
                arg(djvTime::labelTime(estimate)).
                arg(i / timer.seconds(), 0, 'f', 2));

            progressTimer.start();
        }
    }

    if (length > 1)
    {
        print("[100%] ", false);
    }

    try
    {
        save->close();
    }
    catch (const djvError & error)
    {
        printError(error);
        return false;
    }

    print(QString("Elapsed = %1").
        arg(djvTime::labelTime(timer.seconds())));

    return true;
}

//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------

int main(int argc, char ** argv)
{
    int r = 1;

    try
    {
        r = djvConvertApplication(argc, argv).run();
    }
    catch (const djvError & error)
    {
        djvErrorUtil::print(error);
    }

    return r;
}
