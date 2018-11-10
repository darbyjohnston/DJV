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

#include <djvAV/DPXPlugin.h>

#include <djvAV/DPXLoad.h>
#include <djvAV/DPXSave.h>

#include <djvCore/Assert.h>
#include <djvCore/CoreContext.h>
#include <djvCore/Error.h>

#include <QCoreApplication>

namespace djv
{
    namespace AV
    {
        DPXPlugin::DPXPlugin(const QPointer<Core::CoreContext> & context) :
            IOPlugin(context)
        {}

        QString DPXPlugin::pluginName() const
        {
            return DPX::staticName;
        }

        QStringList DPXPlugin::extensions() const
        {
            return QStringList() << ".dpx";
        }

        QStringList DPXPlugin::option(const QString & in) const
        {
            QStringList out;
            if (0 == in.compare(options()[DPX::INPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
            {
                out << _options.inputColorProfile;
            }
            else if (0 == in.compare(options()[DPX::INPUT_FILM_PRINT_OPTION], Qt::CaseInsensitive))
            {
                out << _options.inputFilmPrint;
            }
            else if (0 == in.compare(options()[DPX::OUTPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
            {
                out << _options.outputColorProfile;
            }
            else if (0 == in.compare(options()[DPX::OUTPUT_FILM_PRINT_OPTION], Qt::CaseInsensitive))
            {
                out << _options.outputFilmPrint;
            }
            else if (0 == in.compare(options()[DPX::VERSION_OPTION], Qt::CaseInsensitive))
            {
                out << _options.version;
            }
            else if (0 == in.compare(options()[DPX::TYPE_OPTION], Qt::CaseInsensitive))
            {
                out << _options.type;
            }
            else if (0 == in.compare(options()[DPX::ENDIAN_OPTION], Qt::CaseInsensitive))
            {
                out << _options.endian;
            }

            return out;
        }

        bool DPXPlugin::setOption(const QString & in, QStringList & data)
        {
            //DJV_DEBUG("DPXPlugin::setOption");
            //DJV_DEBUG_PRINT("in = " << in);
            //DJV_DEBUG_PRINT("data = " << *data);
            try
            {
                if (0 == in.compare(options()[DPX::INPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
                {
                    Cineon::COLOR_PROFILE colorProfile = static_cast<Cineon::COLOR_PROFILE>(0);
                    data >> colorProfile;
                    if (colorProfile != _options.inputColorProfile)
                    {
                        _options.inputColorProfile = colorProfile;
                        Q_EMIT optionChanged(in);
                    }
                }
                else if (0 == in.compare(options()[DPX::INPUT_FILM_PRINT_OPTION], Qt::CaseInsensitive))
                {
                    Cineon::FilmPrintToLinear filmPrint;
                    data >> filmPrint;
                    if (filmPrint != _options.inputFilmPrint)
                    {
                        _options.inputFilmPrint = filmPrint;
                        Q_EMIT optionChanged(in);
                    }
                }
                else if (0 == in.compare(options()[DPX::OUTPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
                {
                    Cineon::COLOR_PROFILE colorProfile = static_cast<Cineon::COLOR_PROFILE>(0);
                    data >> colorProfile;
                    if (colorProfile != _options.outputColorProfile)
                    {
                        _options.outputColorProfile = colorProfile;
                        Q_EMIT optionChanged(in);
                    }
                }
                else if (0 == in.compare(options()[DPX::OUTPUT_FILM_PRINT_OPTION], Qt::CaseInsensitive))
                {
                    Cineon::LinearToFilmPrint filmPrint;
                    data >> filmPrint;
                    if (filmPrint != _options.outputFilmPrint)
                    {
                        _options.outputFilmPrint = filmPrint;
                        Q_EMIT optionChanged(in);
                    }
                }
                else if (0 == in.compare(options()[DPX::VERSION_OPTION], Qt::CaseInsensitive))
                {
                    DPX::VERSION version = static_cast<DPX::VERSION>(0);
                    data >> version;
                    if (version != _options.version)
                    {
                        _options.version = version;
                        Q_EMIT optionChanged(in);
                    }
                }
                else if (0 == in.compare(options()[DPX::TYPE_OPTION], Qt::CaseInsensitive))
                {
                    DPX::TYPE type = static_cast<DPX::TYPE>(0);
                    data >> type;
                    if (type != _options.type)
                    {
                        _options.type = type;
                        Q_EMIT optionChanged(in);
                    }
                }
                else if (0 == in.compare(options()[DPX::ENDIAN_OPTION], Qt::CaseInsensitive))
                {
                    DPX::ENDIAN endian = static_cast<DPX::ENDIAN>(0);
                    data >> endian;
                    if (endian != _options.endian)
                    {
                        _options.endian = endian;
                        Q_EMIT optionChanged(in);
                    }
                }
            }
            catch (const QString &)
            {
                return false;
            }
            return true;
        }

        QStringList DPXPlugin::options() const
        {
            return DPX::optionsLabels();
        }

        void DPXPlugin::commandLine(QStringList & in)
        {
            QStringList tmp;
            QString     arg;
            try
            {
                while (!in.isEmpty())
                {
                    in >> arg;
                    if (
                        qApp->translate("djv::AV::DPXPlugin", "-dpx_input_color_profile") == arg)
                    {
                        in >> _options.inputColorProfile;
                    }
                    else if (
                        qApp->translate("djv::AV::DPXPlugin", "-dpx_input_film_print") == arg)
                    {
                        in >> _options.inputFilmPrint;
                    }
                    else if (
                        qApp->translate("djv::AV::DPXPlugin", "-dpx_output_color_profile") == arg)
                    {
                        in >> _options.outputColorProfile;
                    }
                    else if (
                        qApp->translate("djv::AV::DPXPlugin", "-dpx_output_film_print") == arg)
                    {
                        in >> _options.outputFilmPrint;
                    }
                    else if (
                        qApp->translate("djv::AV::DPXPlugin", "-dpx_version") == arg)
                    {
                        in >> _options.version;
                    }
                    else if (
                        qApp->translate("djv::AV::DPXPlugin", "-dpx_type") == arg)
                    {
                        in >> _options.type;
                    }
                    else if (
                        qApp->translate("djv::AV::DPXPlugin", "-dpx_endian") == arg)
                    {
                        in >> _options.endian;
                    }
                    else
                    {
                        tmp << arg;
                    }
                }
            }
            catch (const QString &)
            {
                throw arg;
            }
            in = tmp;
        }

        QString DPXPlugin::commandLineHelp() const
        {
            QStringList inputColorProfileLabel;
            inputColorProfileLabel << _options.inputColorProfile;
            QStringList inputFilmPrintLabel;
            inputFilmPrintLabel << _options.inputFilmPrint;
            QStringList outputColorProfileLabel;
            outputColorProfileLabel << _options.outputColorProfile;
            QStringList outputFilmPrintLabel;
            outputFilmPrintLabel << _options.outputFilmPrint;
            QStringList versionLabel;
            versionLabel << _options.version;
            QStringList typeLabel;
            typeLabel << _options.type;
            QStringList endianLabel;
            endianLabel << _options.endian;
            return qApp->translate("djv::AV::DPXPlugin",
                "\n"
                "DPX Options\n"
                "\n"
                "    -dpx_input_color_profile (value)\n"
                "        Set the color profile used when loading DPX images: %1. "
                "Default = %2.\n"
                "    -dpx_input_film_print (black) (white) (gamma) (soft clip)\n"
                "        Set the film print values used when loading DPX images. Default = "
                "%3.\n"
                "    -dpx_output_color_profile (value)\n"
                "        Set the color profile used when saving DPX images: %4. "
                "Default = %5.\n"
                "    -dpx_output_film_print (black) (white) (gamma) (soft clip)\n"
                "        Set the film print values used when saving DPX images. Default = "
                "%6.\n"
                "    -dpx_version (value)\n"
                "        Set the file version used when saving DPX images: %7. Default = "
                "%8.\n"
                "    -dpx_type (value)\n"
                "        Set the pixel type used when saving DPX images: %9. "
                "Default = %10.\n"
                "    -dpx_endian (value)\n"
                "        Set the endian used when saving DPX images: %11. Default = "
                "%12.\n").
                arg(Cineon::colorProfileLabels().join(", ")).
                arg(inputColorProfileLabel.join(", ")).
                arg(inputFilmPrintLabel.join(", ")).
                arg(Cineon::colorProfileLabels().join(", ")).
                arg(outputColorProfileLabel.join(", ")).
                arg(outputFilmPrintLabel.join(", ")).
                arg(DPX::versionLabels().join(", ")).
                arg(versionLabel.join(", ")).
                arg(DPX::typeLabels().join(", ")).
                arg(typeLabel.join(", ")).
                arg(DPX::endianLabels().join(", ")).
                arg(endianLabel.join(", "));
        }

        std::unique_ptr<Load> DPXPlugin::createLoad(const Core::FileInfo & fileInfo) const
        {
            return std::unique_ptr<Load>(new DPXLoad(fileInfo, _options, context()));
        }

        std::unique_ptr<Save> DPXPlugin::createSave(const Core::FileInfo & fileInfo, const IOInfo & ioInfo) const
        {
            return std::unique_ptr<Save>(new DPXSave(fileInfo, ioInfo, _options, context()));
        }

    } // namespace AV
} // namespace djv
