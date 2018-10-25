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

#include <djv_info/InfoContext.h>

#include <djvCore/System.h>

#include <QCoreApplication>

namespace djv
{
    namespace info
    {
        Context::Context(int & argc, char ** argv, QObject * parent) :
            Graphics::GraphicsContext(argc, argv, parent),
            _columns(Core::System::terminalWidth())
        {
            //DJV_DEBUG("Context::Context");

            // Load translators.
            loadTranslator("djv_info");
        }

        Context::~Context()
        {
            //DJV_DEBUG("Context::~Context");
        }

        const QStringList & Context::input() const
        {
            return _input;
        }

        bool Context::hasInfo() const
        {
            return _info;
        }

        bool Context::hasVerbose() const
        {
            return _verbose;
        }

        bool Context::hasFilePath() const
        {
            return _filePath;
        }

        bool Context::hasRecurse() const
        {
            return _recurse;
        }

        int Context::columns() const
        {
            return _columns;
        }

        bool Context::commandLineParse(QStringList & in)
        {
            //DJV_DEBUG("Context::commandLineParse");
            //DJV_DEBUG_PRINT("in = " << in);

            if (!Graphics::GraphicsContext::commandLineParse(in))
                return false;

            QString arg;
            try
            {
                while (!in.isEmpty())
                {
                    in >> arg;

                    // Parse the options.
                    if (
                        qApp->translate("djv::info::Context", "-x_info") == arg ||
                        qApp->translate("djv::info::Context", "-xi") == arg)
                    {
                        _info = false;
                    }
                    else if (
                        qApp->translate("djv::info::Context", "-verbose") == arg ||
                        qApp->translate("djv::info::Context", "-v") == arg)
                    {
                        _verbose = true;
                    }
                    else if (
                        qApp->translate("djv::info::Context", "-file_path") == arg ||
                        qApp->translate("djv::info::Context", "-fp") == arg)
                    {
                        _filePath = true;
                    }
                    else if (
                        qApp->translate("djv::info::Context", "-recurse") == arg ||
                        qApp->translate("djv::info::Context", "-r") == arg)
                    {
                        _recurse = true;
                    }
                    else if (
                        qApp->translate("djv::info::Context", "-columns") == arg ||
                        qApp->translate("djv::info::Context", "-c") == arg)
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

        QString Context::commandLineHelp() const
        {
            static const QString label = qApp->translate("djv::info::Context",
                "djv_info\n"
                "\n"
                "    Print file metadata.\n"
                "\n"
                "    Example output:\n"
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
                "    (name)     (width)x(height):(aspect) (format) (type) (duration)@(speed)\n"
                "    (name)                                               (duration)@(speed)\n"
                "        (layer): (name)           (width)x(height):(aspect) (format) (type)\n"
                "\n"
                "Usage\n"
                "\n"
                "    djv_info [image|directory]... [option]...\n"
                "\n"
                "    image     - One or more images, image sequences, or movies\n"
                "    directory - One or more directories\n"
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
                "    -recurse, -r\n"
                "        Recursively descend into sub-directories.\n"
                "    -columns, -c (value)\n"
                "        Set the number of columns used to format the output. "
                "Setting this value to zero disables formatting.\n"
                "%1"
                "\n"
                "Examples\n"
                "\n"
                "    Display image information:\n"
                "    > djv_info image.sgi image2.sgi\n"
                "\n"
                "    Display image sequence information:\n"
                "    > djv_info image.1-100.sgi\n"
                "\n"
                "    Display information about all images within a directory:\n"
                "    > djv_info ~/pics\n");
            return QString(label).
                arg(Graphics::GraphicsContext::commandLineHelp());
        }
    
    } // namespace info
} // namespace djv
