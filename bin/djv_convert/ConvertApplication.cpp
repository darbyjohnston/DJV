//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <djv_convert/ConvertApplication.h>

#include <djv_convert/ConvertContext.h>

#include <djvAV/IO.h>

#include <djvCore/Sequence.h>
#include <djvCore/Time.h>
#include <djvCore/Timer.h>
#include <djvCore/VectorUtil.h>

#include <QDir>
#include <QTimer>

namespace djv
{
    namespace convert
    {
        Application::Application(int & argc, char ** argv) :
            QGuiApplication(argc, argv)
        {
            //DJV_DEBUG("Application::Application");

            setOrganizationName("djv.sourceforge.net");
            setApplicationName("djv_convert");

            // Create the context.
            _context.reset(new Context(argc, argv));

            // Parse the command line.
            if (!_context->commandLine(argc, argv))
            {
                QTimer::singleShot(0, this, SLOT(commandLineExit()));
            }
            else
            {
                QTimer::singleShot(0, this, SLOT(work()));
            }
        }

        Application::~Application()
        {}

        const QStringList & Application::errorLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::convert::Application", "Cannot open input: \"%1\"") <<
                qApp->translate("djv::convert::Application", "Cannot open output: \"%1\"") <<
                qApp->translate("djv::convert::Application", "Cannot open slate: \"%1\"") <<
                qApp->translate("djv::convert::Application", "Cannot read input: \"%1\"") <<
                qApp->translate("djv::convert::Application", "Cannot write output: \"%1\"");
            DJV_ASSERT(ERROR_COUNT == data.count());
            return data;
        }

        void Application::commandLineExit()
        {
            exit(1);
        }

        void Application::work()
        {
            //DJV_DEBUG("Application::work");

            Core::Timer timer;
            timer.start();

            const Options & options = _context->options();
            const Input &   input = _context->input();
            const Output &  output = _context->output();

            //DJV_DEBUG_PRINT("input = " << input.file);
            //DJV_DEBUG_PRINT("output = " << output.file);

            std::unique_ptr<AV::OpenGLImage> openGLImage(new AV::OpenGLImage);

            AV::OpenGLImageOptions imageOptions;
            imageOptions.xform.mirror = options.mirror;
            imageOptions.xform.scale = options.scale;
            imageOptions.channel = options.channel;

            // Open the input file.
            std::unique_ptr<AV::Load> load;
            AV::IOInfo loadInfo;
            Core::Error error;
            while (!load)
            {
                try
                {
                    load = _context->ioFactory()->load(input.file, loadInfo);
                }
                catch (const Core::Error & in)
                {
                    error = in;
                }
                if (!load && input.timeout)
                {
                    _context->print(qApp->translate("djv::convert::Application", "Timeout..."));

                    Core::Time::sleep(1);
                }
                else
                {
                    break;
                }
            }
            if (!load)
            {
                error.add(
                    errorLabels()[ERROR_OPEN_INPUT].
                    arg(QDir::toNativeSeparators(input.file)));
                _context->printError(error);
                exit(1);
                return;
            }

            const size_t layer = Core::Math::clamp(input.layer, size_t(0), loadInfo.layers.size() - 1);
            qint64 start = 0;
            qint64 end =
                loadInfo.sequence.frames.count() ?
                (static_cast<qint64>(loadInfo.sequence.frames.count()) - 1) :
                0;
            if (!input.start.isEmpty())
            {
                start = Core::Sequence::findClosest(
                    Core::Time::stringToFrame(input.start, loadInfo.sequence.speed),
                    loadInfo.sequence.frames);
            }
            if (!input.end.isEmpty())
            {
                end = Core::Sequence::findClosest(
                    Core::Time::stringToFrame(input.end, loadInfo.sequence.speed),
                    loadInfo.sequence.frames);
            }
            //DJV_DEBUG_PRINT("start = " << start);
            //DJV_DEBUG_PRINT("end = " << end);

            loadInfo.sequence.frames =
                loadInfo.sequence.frames.mid(start, end - start + 1);
            _context->print(qApp->translate("djv::convert::Application", "%1 %2").
                arg(QDir::toNativeSeparators(input.file)).
                arg(labelImage(loadInfo.layers[0], loadInfo.sequence)));

            // Open the output file.
            std::unique_ptr<AV::Save> save;
            AV::IOInfo saveInfo(loadInfo.layers[layer]);
            glm::ivec2 scaleSize = loadInfo.layers[0].size;
            glm::ivec2 size = options.size;
            if (Core::VectorUtil::isSizeValid(size))
            {
                scaleSize = size;
            }
            else if (size.x)
            {
                scaleSize = glm::ivec2(
                    size.x,
                    Core::Math::ceil(size.x / Core::VectorUtil::aspect(loadInfo.layers[0].size)));
            }
            else if (size.y)
            {
                scaleSize = glm::ivec2(
                    Core::Math::ceil(size.x * Core::VectorUtil::aspect(loadInfo.layers[0].size)),
                    size.y);
            }
            else if (
                !Core::Math::fuzzyCompare(imageOptions.xform.scale.x, 1.f) &&
                !Core::Math::fuzzyCompare(imageOptions.xform.scale.y, 1.f))
            {
                scaleSize = Core::VectorUtil::ceil(
                    glm::vec2(loadInfo.layers[0].size) * imageOptions.xform.scale);
            }
            glm::vec2 position(0.f, 0.f);
            if (options.crop.isValid())
            {
                position = -options.crop.position;

                saveInfo.layers[0].size = options.crop.size;
            }
            else if (options.cropPercent.isValid())
            {
                position = -Core::VectorUtil::ceil(glm::vec2(scaleSize) *
                    (options.cropPercent.position / 100.f));

                saveInfo.layers[0].size = Core::VectorUtil::ceil(glm::vec2(scaleSize) *
                    (options.cropPercent.size / 100.f));
            }
            else
            {
                saveInfo.layers[0].size = scaleSize;
            }
            if (output.pixel.data())
            {
                saveInfo.layers[0].pixel = *output.pixel;
            }
            saveInfo.tags = output.tags;
            saveInfo.sequence = Core::Sequence(
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
                save = _context->ioFactory()->save(output.file, saveInfo);
            }
            catch (Core::Error error)
            {
                error.add(
                    errorLabels()[ERROR_OPEN_OUTPUT].
                    arg(QDir::toNativeSeparators(output.file)));
                _context->printError(error);
                exit(1);
                return;
            }
            _context->print(qApp->translate("djv::convert::Application", "%1 %2").
                arg(QDir::toNativeSeparators(output.file)).
                arg(labelImage(saveInfo.layers[0], saveInfo.sequence)));

            // Add the slate.
            AV::Image slate;
            if (!input.slate.name().isEmpty())
            {
                try
                {
                    _context->print(qApp->translate("djv::convert::Application", "Slating..."));
                    AV::IOInfo info;
                    auto load = _context->ioFactory()->load(input.slate, info);
                    AV::Image image;
                    load->read(image);
                    slate.set(saveInfo.layers[0]);
                    AV::OpenGLImageOptions imageOptions;
                    imageOptions.xform.position = position;
                    imageOptions.xform.scale = glm::vec2(scaleSize) / glm::vec2(info.layers[0].size);
                    imageOptions.colorProfile = image.colorProfile;
                    openGLImage->copy(image, slate, imageOptions);
                }
                catch (Core::Error error)
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
                        AV::ImageIOInfo(saveInfo.sequence.frames.first()));
                    saveInfo.sequence.frames.pop_front();
                }
                catch (Core::Error error)
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
            Core::Timer progressTimer;
            progressTimer.start();
            for (qint64 i = 0; i < length; ++i)
            {
                Core::Timer frameTimer;
                frameTimer.start();

                // Load the current image.
                AV::Image image;
                int timeout = input.timeout;
                while (!image.isValid())
                {
                    try
                    {
                        load->read(
                            image,
                            AV::ImageIOInfo(
                                loadInfo.sequence.frames.count() ?
                                loadInfo.sequence.frames[i] :
                                -1,
                                layer,
                                input.proxy));
                    }
                    catch (const Core::Error & in)
                    {
                        error = in;
                    }
                    if (!image.isValid() && timeout > 0)
                    {
                        //print("Timeout...");
                        --timeout;
                        Core::Time::sleep(1);
                    }
                    else
                    {
                        break;
                    }
                }
                if (!image.isValid())
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

                // Process the tags.
                AV::Tags tags = output.tags;
                tags.add(image.tags);
                if (output.tagsAuto)
                {
                    tags[AV::Tags::tagLabels()[AV::Tags::CREATOR]] =
                        Core::User::current();
                    tags[AV::Tags::tagLabels()[AV::Tags::TIME]] =
                        Core::Time::timeToString(Core::Time::current());
                    tags[AV::Tags::tagLabels()[AV::Tags::TIMECODE]] =
                        Core::Time::timecodeToString(
                            Core::Time::frameToTimecode(
                                saveInfo.sequence.frames.count() ?
                                saveInfo.sequence.frames[i] :
                                0,
                                saveInfo.sequence.speed));
                }

                // Convert.
                AV::Image * p = &image;
                AV::Image tmp;
                imageOptions.xform.position = position;
                imageOptions.xform.scale = glm::vec2(scaleSize) / glm::vec2(loadInfo.layers[0].size);
                imageOptions.colorProfile = image.colorProfile;
                if (p->info() != static_cast<AV::PixelDataInfo>(saveInfo.layers[0]) ||
                    imageOptions != AV::OpenGLImageOptions())
                {
                    tmp.set(saveInfo.layers[0]);
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
                        AV::ImageIOInfo(
                            saveInfo.sequence.frames.count() ?
                            saveInfo.sequence.frames[i] :
                            -1));
                }
                catch (Core::Error error)
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
                    _context->print(qApp->translate("djv::convert::Application",
                        "[%1%] Estimated = %2 (%3 Frames/Second)").
                        arg(static_cast<int>(
                            i / static_cast<float>(length) * 100.f), 3).
                        arg(Core::Time::labelTime(estimate)).
                        arg(i / timer.seconds(), 0, 'f', 2));
                    progressTimer.start();
                }
            }

            if (length > 1)
            {
                _context->print(qApp->translate("djv::convert::Application", "[100%] "), false);
            }

            try
            {
                save->close();
            }
            catch (Core::Error error)
            {
                error.add(
                    errorLabels()[ERROR_WRITE_OUTPUT].
                    arg(QDir::toNativeSeparators(output.file)));
                _context->printError(error);
                exit(1);
                return;
            }

            _context->print(QString(qApp->translate("djv::convert::Application", "Elapsed = %1")).
                arg(Core::Time::labelTime(timer.seconds())));

            exit(0);
        }

        QString Application::labelImage(
            const AV::PixelDataInfo & in,
            const Core::Sequence & sequence) const
        {
            QStringList pixelLabel;
            pixelLabel << in.pixel;
            return qApp->translate("djv::convert::Application", "%1x%2:%3 %4 %5@%6").
                arg(in.size.x).
                arg(in.size.y).
                arg(Core::VectorUtil::aspect(in.size), 0, 'f', 2).
                arg(pixelLabel.join(", ")).
                arg(Core::Time::frameToString(sequence.frames.count(), sequence.speed)).
                arg(Core::Speed::speedToFloat(sequence.speed));
        }

    } // namespace convert
} // namespace djv
