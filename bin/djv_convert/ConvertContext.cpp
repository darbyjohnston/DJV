//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djv_convert/ConvertContext.h>

#include <djvCore/DebugLog.h>
#include <djvCore/FileInfoUtil.h>

#include <QCoreApplication>

namespace djv
{
    namespace convert
    {
        Options::Options() :
            scale(1.0),
            channel(static_cast<Graphics::OpenGLImageOptions::CHANNEL>(0))
        {}

        Input::Input() :
            layer(0),
            proxy(static_cast<Graphics::PixelDataInfo::PROXY>(0)),
            slateFrames(0),
            timeout(0)
        {}

        Output::Output() :
            tagsAuto(true)
        {}

        Context::Context(int & argc, char ** argv, QObject * parent) :
            Graphics::GraphicsContext(argc, argv, parent)
        {
            //DJV_DEBUG("Context::Context");

            // Load translators.
            loadTranslator("djv_convert");
        }

        Context::~Context()
        {
            //DJV_DEBUG("Context::~Context");
        }

        const Options & Context::options() const
        {
            return _options;
        }

        const Input & Context::input() const
        {
            return _input;
        }

        const Output & Context::output() const
        {
            return _output;
        }

        bool Context::commandLineParse(QStringList & in)
        {
            //DJV_DEBUG("Context::commandLineParse");
            //DJV_DEBUG_PRINT("in = " << in);

            if (!Graphics::GraphicsContext::commandLineParse(in))
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
                while (!in.isEmpty())
                {
                    in >> arg;

                    // Parse the options.
                    if (qApp->translate("djv::convert::Context", "-mirror_h") == arg)
                    {
                        _options.mirror.x = true;
                    }
                    else if (qApp->translate("djv::convert::Context", "-mirror_v") == arg)
                    {
                        _options.mirror.y = true;
                    }
                    else if (qApp->translate("djv::convert::Context", "-scale") == arg)
                    {
                        in >> _options.scale.x;
                        _options.scale.y = _options.scale.x;
                    }
                    else if (qApp->translate("djv::convert::Context", "-scale_separate") == arg)
                    {
                        in >> _options.scale;
                    }
                    else if (qApp->translate("djv::convert::Context", "-resize") == arg)
                    {
                        in >> _options.size;
                    }
                    else if (qApp->translate("djv::convert::Context", "-width") == arg)
                    {
                        in >> _options.size.x;
                    }
                    else if (qApp->translate("djv::convert::Context", "-height") == arg)
                    {
                        in >> _options.size.y;
                    }
                    else if (qApp->translate("djv::convert::Context", "-crop") == arg)
                    {
                        in >> _options.crop;
                    }
                    else if (qApp->translate("djv::convert::Context", "-crop_percent") == arg)
                    {
                        in >> _options.cropPercent;
                    }
                    else if (qApp->translate("djv::convert::Context", "-channel") == arg)
                    {
                        in >> _options.channel;
                    }

                    // Parse the input options.
                    else if (qApp->translate("djv::convert::Context", "-layer") == arg)
                    {
                        in >> _input.layer;
                    }
                    else if (qApp->translate("djv::convert::Context", "-proxy") == arg)
                    {
                        in >> _input.proxy;
                    }
                    else if (qApp->translate("djv::convert::Context", "-time") == arg)
                    {
                        in >> _input.start;
                        in >> _input.end;
                    }
                    else if (qApp->translate("djv::convert::Context", "-slate") == arg)
                    {
                        QString tmp;
                        in >> tmp;
                        _input.slate = tmp;
                        in >> _input.slateFrames;
                    }
                    else if (qApp->translate("djv::convert::Context", "-timeout") == arg)
                    {
                        in >> _input.timeout;
                    }

                    // Parse the output options.
                    else if (qApp->translate("djv::convert::Context", "-pixel") == arg)
                    {
                        Graphics::Pixel::PIXEL value = static_cast<Graphics::Pixel::PIXEL>(0);
                        in >> value;
                        _output.pixel.reset(new Graphics::Pixel::PIXEL(value));
                    }
                    else if (qApp->translate("djv::convert::Context", "-speed") == arg)
                    {
                        Core::Speed::FPS value = static_cast<Core::Speed::FPS>(0);
                        in >> value;
                        _output.speed.reset(new Core::Speed::FPS(value));
                    }
                    else if (qApp->translate("djv::convert::Context", "-tag") == arg)
                    {
                        QString name, value;
                        in >> name;
                        in >> value;
                        _output.tags[name] = value;
                    }
                    else if (qApp->translate("djv::convert::Context", "-tags_auto") == arg)
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
                QString("Args = %1").arg(Core::StringUtil::addQuotes(args).join(", ")));

            // Parse the input file.
            if (!args.count())
            {
                throw QString(qApp->translate("djv::convert::Context", "input"));
            }
            _input.file = Core::FileInfoUtil::parse(args.first(), Core::Sequence::compress());
            DJV_LOG(debugLog(), "djv_convert", QString("Input = \"%1\"").arg(_input.file));
            args.pop_front();

            // Parse the output file.
            if (!args.count())
            {
                throw QString(qApp->translate("djv::convert::Context", "output"));
            }
            _output.file = Core::FileInfoUtil::parse(args.first(), Core::Sequence::compress());
            DJV_LOG(debugLog(), "djv_convert", QString("Output = \"%1\"").arg(_output.file));
            args.pop_front();
            if (!args.isEmpty())
            {
                throw QString(args[0]);
            }

            return true;
        }

        QString Context::commandLineHelp() const
        {
            static const QString label = qApp->translate("djv::convert::Context",
                "djv_convert\n"
                "\n"
                "    Command-line tool for image and movie processing. Common "
                "uses include resizing images and converting sequences of images "
                "to a movie.\n"
                "\n"
                "    Example of converting an image sequence to a movie:\n"
                "    > djv_convert input.1-100.tga output.mp4\n"
                "    See below for more examples.\n"
                "\n"
                "    Note that djv_convert requires OpenGL in order to run.\n"
                "\n"
                "Usage\n"
                "\n"
                "    djv_convert (input) (output) [option]...\n"
                "\n"
                "    input  - Input image, image sequence, or movie\n"
                "    output - Output image, image sequence, or movie\n"
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
                "        Scale the image width and height separately using floating point "
                "values (1.0 = 100%).\n"
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
                "        Crop the image using floating point values (1.0 = 100%).\n"
                "    -channel (value)\n"
                "        Show only specific image channels: %1. Default = %2.\n"
                "\n"
                "Input Options\n"
                "\n"
                "    -layer (value)\n"
                "        Set the input layer.\n"
                "    -proxy (value)\n"
                "        Set the proxy scale: %3. Default = %4.\n"
                "    -time (start) (end)\n"
                "        Set the start and end time.\n"
                "    -slate (input) (frames)\n"
                "        Set the slate.\n"
                "    -timeout (value)\n"
                "        Set the maximum number of seconds to wait for each input frame. "
                "Default = %5.\n"
                "\n"
                "Output Options\n"
                "\n"
                "    -pixel (value)\n"
                "        Convert the pixel type: %6.\n"
                "    -speed (value)\n"
                "        Set the speed: %7.\n"
                "    -tag (name) (value)\n"
                "        Set an image tag.\n"
                "    -tags_auto (value)\n"
                "        Automatically generate image tags (e.g., timecode): %8. "
                "Default = %9.\n"
                "%10"
                "\n"
                "Examples\n"
                "\n"
                "    Convert an image:\n"
                "    > djv_convert input.sgi output.tga\n"
                "\n"
                "    Convert an image sequence:\n"
                "    > djv_convert input.1-100.sgi output.1.tga\n"
                "    Note that only the first frame of the output sequence needs to be specified.\n"
                "\n"
                "    Create an RLE compressed image sequence:\n"
                "    > djv_convert input.1-100.sgi output.1.tga -targa_compression RLE\n"
                "\n"
                "    Convert an image sequence to a movie:\n"
                "    > djv_convert input.0001-0100.dpx output.m4v\n"
                "\n"
                "    Convert a movie to an image sequence:\n"
                "    > djv_convert input.m4v output.1.tga\n"
                "\n"
                "    Convert the pixel type:\n"
                "    > djv_convert input.sgi output.sgi -pixel \"RGB U16\"\n"
                "    Note the use of the quotes around the pixel type option.\n"
                "\n"
                "    Scale an image by half:\n"
                "    > djv_convert input.tga output.tga -scale 0.5\n"
                "\n"
                "    Resize an image:\n"
                "    > djv_convert input.tga output.tga -resize 2048 1556\n"
                "\n"
                "    Convert a Cineon file to a linear format using the default settings:\n"
                "    > djv_convert input.cin output.tga\n"
                "\n"
                "    Convert a Cineon file to a linear format using custom film print settings "
                "(black point, white point, gamma, and soft clip):\n"
                "    > djv_convert input.cin output.tga -cineon_input_film_print 95 685 2.2 2\n");
            QStringList channelLabel;
            channelLabel << _options.channel;
            QStringList proxyLabel;
            proxyLabel << _input.proxy;
            QStringList tagsAutoLabel;
            tagsAutoLabel << _output.tagsAuto;
            return QString(label).
                arg(Graphics::OpenGLImageOptions::channelLabels().join(", ")).
                arg(channelLabel.join(", ")).
                arg(Graphics::PixelDataInfo::proxyLabels().join(", ")).
                arg(proxyLabel.join(", ")).
                arg(_input.timeout).
                arg(Graphics::Pixel::pixelLabels().join(", ")).
                arg(Core::Speed::fpsLabels().join(", ")).
                arg(Core::StringUtil::boolLabels().join(", ")).
                arg(tagsAutoLabel.join(", ")).
                arg(Graphics::GraphicsContext::commandLineHelp());
        }

    } // namespace convert
} // namespace djv
