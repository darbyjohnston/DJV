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

#include <djvConvertApplication.h>

#include <djvConvertContext.h>

#include <djvGraphics/ImageIO.h>
#include <djvCore/Sequence.h>
#include <djvCore/SequenceUtil.h>
#include <djvCore/Time.h>
#include <djvCore/Timer.h>
#include <djvCore/VectorUtil.h>

#include <QDir>
#include <QTimer>

//------------------------------------------------------------------------------
// djvConvertApplication
//------------------------------------------------------------------------------

djvConvertApplication::djvConvertApplication(int & argc, char ** argv) :
    QGuiApplication(argc, argv)
{
    //DJV_DEBUG("djvConvertApplication::djvConvertApplication");
    
    setOrganizationName("djv.sourceforge.net");
    setApplicationName("djv_convert");
    
    // Create the context.
    _context = new djvConvertContext;

    // Parse the command line.
    if (! _context->commandLine(argc, argv))
    {
        QTimer::singleShot(0, this, SLOT(commandLineExit()));
    }
    else
    {
        QTimer::singleShot(0, this, SLOT(work()));
    }
}

djvConvertApplication::~djvConvertApplication()
{
    delete _context;
}

const QStringList & djvConvertApplication::errorLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvConvertApplication", "Cannot open input: \"%1\"") <<
        qApp->translate("djvConvertApplication", "Cannot open output: \"%1\"") <<
        qApp->translate("djvConvertApplication", "Cannot open slate: \"%1\"") <<
        qApp->translate("djvConvertApplication", "Cannot read input: \"%1\"") <<
        qApp->translate("djvConvertApplication", "Cannot write output: \"%1\"");
    DJV_ASSERT(ERROR_COUNT == data.count());
    return data;
}

void djvConvertApplication::commandLineExit()
{
    exit(1);
}

void djvConvertApplication::work()
{
    //DJV_DEBUG("djvConvertApplication::work");

    djvTimer timer;
    timer.start();

    const djvConvertOptions & options = _context->options();
    const djvConvertInput &   input   = _context->input();
    const djvConvertOutput &  output  = _context->output();

    //DJV_DEBUG_PRINT("input = " << input.file);
    //DJV_DEBUG_PRINT("output = " << output.file);

    std::unique_ptr<djvOpenGLImage> openGLImage(new djvOpenGLImage);

    djvOpenGLImageOptions imageOptions;
    imageOptions.xform.mirror = options.mirror;
    imageOptions.xform.scale  = options.scale;
    imageOptions.channel      = options.channel;

    // Open the input file.
    QScopedPointer<djvImageLoad> load;
    djvImageIOInfo               loadInfo;
    djvError error;
    while (! load.data())
    {
        try
        {
            load.reset(_context->imageIOFactory()->load(input.file, loadInfo));
        }
        catch (const djvError & in)
        {
            error = in;
        }
        if (! load.data() && input.timeout)
        {
            _context->print(qApp->translate("djvConvertApplication", "Timeout..."));
            
            djvTime::sleep(1);
        }
        else
        {
            break;
        }
    }
    if (! load.data())
    {
        error.add(
            errorLabels()[ERROR_OPEN_INPUT].
            arg(QDir::toNativeSeparators(input.file)));
        _context->printError(error);
        exit(1);
        return;
    }

    const int layer = djvMath::clamp(input.layer, 0, loadInfo.layerCount() - 1);
    qint64 start = 0;
    qint64 end =
        loadInfo.sequence.frames.count() ?
        (static_cast<qint64>(loadInfo.sequence.frames.count()) - 1) :
        0;
    if (! input.start.isEmpty())
    {
        start = djvSequenceUtil::findClosest(
            djvTime::stringToFrame(input.start, loadInfo.sequence.speed),
            loadInfo.sequence.frames);
    }
    if (! input.end.isEmpty())
    {
        end = djvSequenceUtil::findClosest(
            djvTime::stringToFrame(input.end, loadInfo.sequence.speed),
            loadInfo.sequence.frames);
    }
    //DJV_DEBUG_PRINT("start = " << start);
    //DJV_DEBUG_PRINT("end = " << end);

    loadInfo.sequence.frames =
        loadInfo.sequence.frames.mid(start, end - start + 1);
    _context->print(qApp->translate("djvConvertApplication", "%1 %2").
        arg(QDir::toNativeSeparators(input.file)).
        arg(labelImage(loadInfo, loadInfo.sequence)));

    // Open the output file.
    QScopedPointer<djvImageSave> save;
    djvImageIOInfo saveInfo(loadInfo[layer]);
    glm::ivec2 scaleSize = loadInfo.size;
    glm::ivec2 size = options.size;
    if (djvVectorUtil::isSizeValid(size))
    {
        scaleSize = size;
    }
    else if (size.x)
    {
        scaleSize = glm::ivec2(
            size.x,
            djvMath::ceil(size.x / djvVectorUtil::aspect(loadInfo.size)));
    }
    else if (size.y)
    {
        scaleSize = glm::ivec2(
            djvMath::ceil(size.x * djvVectorUtil::aspect(loadInfo.size)),
            size.y);
    }
    else if (
        ! djvMath::fuzzyCompare(imageOptions.xform.scale.x, 1.f) &&
        ! djvMath::fuzzyCompare(imageOptions.xform.scale.y, 1.f))
    {
        scaleSize = djvVectorUtil::ceil(
            glm::vec2(loadInfo.size) * imageOptions.xform.scale);
    }
    glm::vec2 position(0.f, 0.f);
    if (options.crop.isValid())
    {
        position = -options.crop.position;
        
        saveInfo.size = options.crop.size;
    }
    else if (options.cropPercent.isValid())
    {
        position = -djvVectorUtil::ceil(glm::vec2(scaleSize) *
            (options.cropPercent.position / 100.f));

        saveInfo.size = djvVectorUtil::ceil(glm::vec2(scaleSize) *
            (options.cropPercent.size / 100.f));
    }
    else
    {
        saveInfo.size = scaleSize;
    }
    if (output.pixel.data())
    {
        saveInfo.pixel = *output.pixel;
    }
    saveInfo.tags = output.tags;
    saveInfo.sequence = djvSequence(
        output.file.sequence().start(),
        output.file.sequence().start() +
        input.slateFrames + (
            loadInfo.sequence.frames.count() ?
            (static_cast<qint64>(loadInfo.sequence.frames.count()) - 1) :
            0),
        output.file.sequence().pad,
        loadInfo.sequence.speed);
    if (output.speed.data())
    {
        saveInfo.sequence.speed = *output.speed;
    }
    //DJV_DEBUG_PRINT("save sequence = " << saveInfo.sequence);
    try
    {
        save.reset(_context->imageIOFactory()->save(output.file, saveInfo));
    }
    catch (djvError error)
    {
        error.add(
            errorLabels()[ERROR_OPEN_OUTPUT].
            arg(QDir::toNativeSeparators(output.file)));
        _context->printError(error);
        exit(1);
        return;
    }
    _context->print(qApp->translate("djvConvertApplication", "%1 %2").
        arg(QDir::toNativeSeparators(output.file)).
        arg(labelImage(saveInfo, saveInfo.sequence)));

    // Add the slate.
    djvImage slate;
    if (! input.slate.name().isEmpty())
    {
        try
        {
            _context->print(qApp->translate("djvConvertApplication", "Slating..."));
            djvImageIOInfo info;
            QScopedPointer<djvImageLoad> load(_context->imageIOFactory()->load(input.slate, info));
            djvImage image;
            load->read(image);
            slate.set(saveInfo);
            djvOpenGLImageOptions imageOptions;
            imageOptions.xform.position = position;
            imageOptions.xform.scale    = glm::vec2(scaleSize) / glm::vec2(info.size);
            imageOptions.colorProfile   = image.colorProfile;
            openGLImage->copy(image, slate, imageOptions);
        }
        catch (djvError error)
        {
            error.add(
                errorLabels()[ERROR_OPEN_SLATE].
                arg(QDir::toNativeSeparators(input.slate)));
            _context->printError(error);
            save->close();
            exit(1);
            return;
        }
    }

    // Convert the images.
    for (qint64 i = 0; i < input.slateFrames; ++i)
    {
        try
        {
            save->write(
                slate,
                djvImageIOFrameInfo(saveInfo.sequence.frames.first()));
            saveInfo.sequence.frames.pop_front();
        }
        catch (djvError error)
        {
            error.add(
                errorLabels()[ERROR_WRITE_OUTPUT].
                arg(QDir::toNativeSeparators(output.file)));
            _context->printError(error);
            save->close();
            exit(1);
            return;
        }
    }
    const qint64 length = static_cast<qint64>(saveInfo.sequence.frames.count());
    float    progressAccum = 0.f;
    djvTimer progressTimer;
    progressTimer.start();
    for (qint64 i = 0; i < length; ++i)
    {
        djvTimer frameTimer;
        frameTimer.start();

        // Load the current image.
        djvImage image;
        int timeout = input.timeout;
        while (! image.isValid())
        {
            try
            {
                load->read(
                    image,
                    djvImageIOFrameInfo(
                        loadInfo.sequence.frames.count() ?
                        loadInfo.sequence.frames[i] :
                        -1,
                        layer,
                        input.proxy));
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
            error.add(
                errorLabels()[ERROR_READ_INPUT].
                arg(QDir::toNativeSeparators(input.file)));
            _context->printError(error);
            save->close();
            exit(1);
            return;
        }
        //DJV_DEBUG_PRINT("image = " << *image);

        // Process the image tags.
        djvImageTags tags = output.tags;
        tags.add(image.tags);
        if (output.tagsAuto)
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
        djvImage * p = &image;
        djvImage tmp;
        imageOptions.xform.position = position;
        imageOptions.xform.scale    = glm::vec2(scaleSize) / glm::vec2(loadInfo.size);
        imageOptions.colorProfile   = image.colorProfile;
        if (p->info() != static_cast<djvPixelDataInfo>(saveInfo) ||
            imageOptions != djvOpenGLImageOptions())
        {
            tmp.set(saveInfo);
            openGLImage->copy(
                image,
                tmp,
                imageOptions);
            p = &tmp;
        }
        p->tags = tags;

        // Save the image.
        //DJV_DEBUG_PRINT("output = " << tmp);
        try
        {
            save->write(
                *p,
                djvImageIOFrameInfo(
                    saveInfo.sequence.frames.count() ?
                    saveInfo.sequence.frames[i] :
                    -1));
        }
        catch (djvError error)
        {
            error.add(
                errorLabels()[ERROR_WRITE_OUTPUT].
                arg(QDir::toNativeSeparators(output.file)));
            _context->printError(error);
            save->close();
            exit(1);
            return;
        }

        // Statistics.
        timer.check();
        frameTimer.check();
        progressAccum += frameTimer.seconds();
        progressTimer.check();
        if (length > 1 && progressTimer.seconds() > 3.f)
        {
            const float estimate =
                progressAccum /
                static_cast<float>(i + 1) * (length - (i + 1));
            _context->print(qApp->translate("djvConvertApplication",
                "[%1%] Estimated = %2 (%3 Frames/Second)").
                arg(static_cast<int>(
                    i / static_cast<float>(length) * 100.f), 3).
                arg(djvTime::labelTime(estimate)).
                arg(i / timer.seconds(), 0, 'f', 2));
            progressTimer.start();
        }
    }
    
    if (length > 1)
    {
        _context->print(qApp->translate("djvConvertApplication", "[100%] "), false);
    }
    
    try
    {
        save->close();
    }
    catch (djvError error)
    {
        error.add(
            errorLabels()[ERROR_WRITE_OUTPUT].
            arg(QDir::toNativeSeparators(output.file)));
        _context->printError(error);
        exit(1);
        return;
    }
    
    _context->print(QString(qApp->translate("djvConvertApplication", "Elapsed = %1")).
        arg(djvTime::labelTime(timer.seconds())));

    exit(0);
}

QString djvConvertApplication::labelImage(
    const djvPixelDataInfo & in,
    const djvSequence &      sequence) const
{
    return qApp->translate("djvConvertApplication", "%1x%2:%3 %4 %5@%6").
        arg(in.size.x).
        arg(in.size.y).
        arg(djvVectorUtil::aspect(in.size), 0, 'f', 2).
        arg(djvStringUtil::label(in.pixel).join(", ")).
        arg(djvTime::frameToString(sequence.frames.count(), sequence.speed)).
        arg(djvSpeed::speedToFloat(sequence.speed));
}
