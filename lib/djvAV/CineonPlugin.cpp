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

#include <djvAV/CineonPlugin.h>

#include <djvAV/CineonLoad.h>
#include <djvAV/CineonSave.h>

#include <djvCore/Assert.h>
#include <djvCore/CoreContext.h>
#include <djvCore/Error.h>

#include <QCoreApplication>

namespace djv
{
    namespace AV
    {
        CineonPlugin::CineonPlugin(const QPointer<Core::CoreContext> & context) :
            ImageIO(context)
        {}

        CineonPlugin::~CineonPlugin()
        {}

        QString CineonPlugin::pluginName() const
        {
            return Cineon::staticName;
        }

        QStringList CineonPlugin::extensions() const
        {
            return QStringList() << ".cin";
        }

        QStringList CineonPlugin::option(const QString & in) const
        {
            QStringList out;
            if (0 == in.compare(options()[Cineon::INPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
            {
                out << _options.inputColorProfile;
            }
            else if (0 == in.compare(options()[Cineon::INPUT_FILM_PRINT_OPTION], Qt::CaseInsensitive))
            {
                out << _options.inputFilmPrint;
            }
            else if (0 == in.compare(options()[Cineon::OUTPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
            {
                out << _options.outputColorProfile;
            }
            else if (0 == in.compare(options()[Cineon::OUTPUT_FILM_PRINT_OPTION], Qt::CaseInsensitive))
            {
                out << _options.outputFilmPrint;
            }
            return out;
        }

        bool CineonPlugin::setOption(const QString & in, QStringList & data)
        {
            //DJV_DEBUG("CineonPlugin::setOption");
            //DJV_DEBUG_PRINT("in = " << in);
            //DJV_DEBUG_PRINT("data = " << data);
            try
            {
                if (0 == in.compare(options()[Cineon::INPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
                {
                    Cineon::COLOR_PROFILE colorProfile =
                        static_cast<Cineon::COLOR_PROFILE>(0);
                    data >> colorProfile;
                    if (colorProfile != _options.inputColorProfile)
                    {
                        //DJV_DEBUG_PRINT("color profile = " << colorProfile);
                        _options.inputColorProfile = colorProfile;
                        Q_EMIT optionChanged(in);
                    }
                }
                else if (0 == in.compare(options()[Cineon::INPUT_FILM_PRINT_OPTION], Qt::CaseInsensitive))
                {
                    Cineon::FilmPrintToLinear filmPrint;
                    data >> filmPrint;
                    if (filmPrint != _options.inputFilmPrint)
                    {
                        _options.inputFilmPrint = filmPrint;
                        Q_EMIT optionChanged(in);
                    }
                }
                else if (0 == in.compare(options()[Cineon::OUTPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
                {
                    Cineon::COLOR_PROFILE colorProfile =
                        static_cast<Cineon::COLOR_PROFILE>(0);
                    data >> colorProfile;
                    if (colorProfile != _options.outputColorProfile)
                    {
                        _options.outputColorProfile = colorProfile;
                        Q_EMIT optionChanged(in);
                    }
                }
                else if (0 == in.compare(options()[Cineon::OUTPUT_FILM_PRINT_OPTION], Qt::CaseInsensitive))
                {
                    Cineon::LinearToFilmPrint filmPrint;
                    data >> filmPrint;
                    if (filmPrint != _options.outputFilmPrint)
                    {
                        _options.outputFilmPrint = filmPrint;
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

        QStringList CineonPlugin::options() const
        {
            return Cineon::optionsLabels();
        }

        void CineonPlugin::commandLine(QStringList & in)
        {
            QStringList tmp;
            QString     arg;
            try
            {
                while (!in.isEmpty())
                {
                    in >> arg;
                    if (
                        qApp->translate("djv::AV::CineonPlugin", "-cineon_input_color_profile") == arg)
                    {
                        in >> _options.inputColorProfile;
                    }
                    else if (
                        qApp->translate("djv::AV::CineonPlugin", "-cineon_input_film_print") == arg)
                    {
                        in >> _options.inputFilmPrint;
                    }
                    else if (
                        qApp->translate("djv::AV::CineonPlugin", "-cineon_output_color_profile") == arg)
                    {
                        in >> _options.outputColorProfile;
                    }
                    else if (
                        qApp->translate("djv::AV::CineonPlugin", "-cineon_output_film_print") == arg)
                    {
                        in >> _options.outputFilmPrint;
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

        QString CineonPlugin::commandLineHelp() const
        {
            QStringList inputColorProfileLabel;
            inputColorProfileLabel << _options.inputColorProfile;
            QStringList inputFilmPrintLabel;
            inputFilmPrintLabel << _options.inputFilmPrint;
            QStringList outputColorProfileLabel;
            outputColorProfileLabel << _options.outputColorProfile;
            QStringList outputFilmPrintLabel;
            outputFilmPrintLabel << _options.outputFilmPrint;
            return qApp->translate("djv::AV::CineonPlugin",
                "\n"
                "Cineon Options\n"
                "\n"
                "    -cineon_input_color_profile (value)\n"
                "        Set the color profile used when loading Cineon images: %1. "
                "Default = %2.\n"
                "    -cineon_input_film_print (black) (white) (gamma) (soft clip)\n"
                "        Set the film print values used when loading Cineon images. Default = "
                "%3.\n"
                "    -cineon_output_color_profile (value)\n"
                "        Set the color profile used when saving Cineon images: %4. "
                "Default = %5.\n"
                "    -cineon_output_film_print (black) (white) (gamma) (soft clip)\n"
                "        Set the film print values used when saving Cineon images. Default = "
                "%6.\n").
                arg(Cineon::colorProfileLabels().join(", ")).
                arg(inputColorProfileLabel.join(", ")).
                arg(inputFilmPrintLabel.join(", ")).
                arg(Cineon::colorProfileLabels().join(", ")).
                arg(outputColorProfileLabel.join(", ")).
                arg(outputFilmPrintLabel.join(", "));
        }

        std::unique_ptr<ImageLoad> CineonPlugin::createLoad(const Core::FileInfo & fileInfo) const
        {
            return std::unique_ptr<ImageLoad>(new CineonLoad(fileInfo, _options, context()));
        }

        std::unique_ptr<ImageSave> CineonPlugin::createSave(const Core::FileInfo & fileInfo, const ImageIOInfo & imageIOInfo) const
        {
            return std::unique_ptr<ImageSave>(new CineonSave(fileInfo, imageIOInfo, _options, context()));
        }

    } // namespace AV
} // namespace djv
