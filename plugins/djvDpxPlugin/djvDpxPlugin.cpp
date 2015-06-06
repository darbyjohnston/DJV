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

//! \file djvDpxPlugin.cpp

#include <djvDpxPlugin.h>

#include <djvDpxLoad.h>
#include <djvDpxSave.h>

#include <djvAssert.h>
#include <djvError.h>

#include <QCoreApplication>

extern "C"
{

DJV_PLUGIN_EXPORT djvPlugin * djvImageIoEntry(djvCoreContext * context)
{
    return new djvDpxPlugin(context);
}

} // extern "C"

//------------------------------------------------------------------------------
// djvDpxPlugin
//------------------------------------------------------------------------------

djvDpxPlugin::djvDpxPlugin(djvCoreContext * context) :
    djvImageIo(context)
{}

QString djvDpxPlugin::pluginName() const
{
    return djvDpx::staticName;
}

QStringList djvDpxPlugin::extensions() const
{
    return QStringList() << ".dpx";
}

QStringList djvDpxPlugin::option(const QString & in) const
{
    QStringList out;

    if (0 == in.compare(options()[djvDpx::INPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
    {
        out << _options.inputColorProfile;
    }
    else if (0 == in.compare(options()[djvDpx::INPUT_FILM_PRINT_OPTION], Qt::CaseInsensitive))
    {
        out << _options.inputFilmPrint;
    }
    else if (0 == in.compare(options()[djvDpx::OUTPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
    {
        out << _options.outputColorProfile;
    }
    else if (0 == in.compare(options()[djvDpx::OUTPUT_FILM_PRINT_OPTION], Qt::CaseInsensitive))
    {
        out << _options.outputFilmPrint;
    }
    else if (0 == in.compare(options()[djvDpx::VERSION_OPTION], Qt::CaseInsensitive))
    {
        out << _options.version;
    }
    else if (0 == in.compare(options()[djvDpx::TYPE_OPTION], Qt::CaseInsensitive))
    {
        out << _options.type;
    }
    else if (0 == in.compare(options()[djvDpx::ENDIAN_OPTION], Qt::CaseInsensitive))
    {
        out << _options.endian;
    }

    return out;
}

bool djvDpxPlugin::setOption(const QString & in, QStringList & data)
{
    //DJV_DEBUG("djvDpxPlugin::setOption");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("data = " << *data);
    
    try
    {
        if (0 == in.compare(options()[djvDpx::INPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
        {
            djvCineon::COLOR_PROFILE colorProfile =
                static_cast<djvCineon::COLOR_PROFILE>(0);
            
            data >> colorProfile;
            
            if (colorProfile != _options.inputColorProfile)
            {
                _options.inputColorProfile = colorProfile;
            
                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(options()[djvDpx::INPUT_FILM_PRINT_OPTION], Qt::CaseInsensitive))
        {
            djvCineon::FilmPrintToLinear filmPrint;
            
            data >> filmPrint;
            
            if (filmPrint != _options.inputFilmPrint)
            {
                _options.inputFilmPrint = filmPrint;
                
                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(options()[djvDpx::OUTPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
        {
            djvCineon::COLOR_PROFILE colorProfile =
                static_cast<djvCineon::COLOR_PROFILE>(0);
            
            data >> colorProfile;
            
            if (colorProfile != _options.outputColorProfile)
            {
                _options.outputColorProfile = colorProfile;
            
                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(options()[djvDpx::OUTPUT_FILM_PRINT_OPTION], Qt::CaseInsensitive))
        {
            djvCineon::LinearToFilmPrint filmPrint;
            
            data >> filmPrint;
            
            if (filmPrint != _options.outputFilmPrint)
            {
                _options.outputFilmPrint = filmPrint;
            
                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(options()[djvDpx::VERSION_OPTION], Qt::CaseInsensitive))
        {
            djvDpx::VERSION version = static_cast<djvDpx::VERSION>(0);
            
            data >> version;
            
            if (version != _options.version)
            {
                _options.version = version;
            
                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(options()[djvDpx::TYPE_OPTION], Qt::CaseInsensitive))
        {
            djvDpx::TYPE type = static_cast<djvDpx::TYPE>(0);
            
            data >> type;
            
            if (type != _options.type)
            {
                _options.type = type;
            
                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(options()[djvDpx::ENDIAN_OPTION], Qt::CaseInsensitive))
        {
            djvDpx::ENDIAN endian = static_cast<djvDpx::ENDIAN>(0);
            
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

QStringList djvDpxPlugin::options() const
{
    return djvDpx::optionsLabels();
}

void djvDpxPlugin::commandLine(QStringList & in) throw (QString)
{
    QStringList tmp;
    QString     arg;

    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            if (
                qApp->translate("djvDpxPlugin", "-dpx_input_color_profile") == arg)
            {
                in >> _options.inputColorProfile;
            }
            else if (
                qApp->translate("djvDpxPlugin", "-dpx_input_film_print") == arg)
            {
                in >> _options.inputFilmPrint;
            }
            else if (
                qApp->translate("djvDpxPlugin", "-dpx_output_color_profile") == arg)
            {
                in >> _options.outputColorProfile;
            }
            else if (
                qApp->translate("djvDpxPlugin", "-dpx_output_film_print") == arg)
            {
                in >> _options.outputFilmPrint;
            }
            else if (
                qApp->translate("djvDpxPlugin", "-dpx_version") == arg)
            {
                in >> _options.version;
            }
            else if (
                qApp->translate("djvDpxPlugin", "-dpx_type") == arg)
            {
                in >> _options.type;
            }
            else if (
                qApp->translate("djvDpxPlugin", "-dpx_endian") == arg)
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

QString djvDpxPlugin::commandLineHelp() const
{
    return qApp->translate("djvDpxPlugin",
"\n"
"DPX Options\n"
"\n"
"    -dpx_input_color_profile (value)\n"
"        Set the color profile used when loading DPX images. Options = %1. "
"Default = %2.\n"
"    -dpx_input_film_print (black) (white) (gamma) (soft clip)\n"
"        Set the film print values used when loading DPX images. Default = "
"%3.\n"
"    -dpx_output_color_profile (value)\n"
"        Set the color profile used when saving DPX images. Options = %4. "
"Default = %5.\n"
"    -dpx_output_film_print (black) (white) (gamma) (soft clip)\n"
"        Set the film print values used when saving DPX images. Default = "
"%6.\n"
"    -dpx_version (value)\n"
"        Set the file version used when saving DPX images. Options = %7. Default = "
"%8.\n"
"    -dpx_type (value)\n"
"        Set the pixel type used when saving DPX images. Options = %9. "
"Default = %10.\n"
"    -dpx_endian (value)\n"
"        Set the endian used when saving DPX images. Setting the endian to "
"\"Auto\" will use the endian of the current hardware. Options = %11. Default = "
"%12.\n").
    arg(djvCineon::colorProfileLabels().join(", ")).
    arg(djvStringUtil::label(_options.inputColorProfile).join(", ")).
    arg(djvStringUtil::label(_options.inputFilmPrint).join(", ")).
    arg(djvCineon::colorProfileLabels().join(", ")).
    arg(djvStringUtil::label(_options.outputColorProfile).join(", ")).
    arg(djvStringUtil::label(_options.outputFilmPrint).join(", ")).
    arg(djvDpx::versionLabels().join(", ")).
    arg(djvStringUtil::label(_options.version).join(", ")).
    arg(djvDpx::typeLabels().join(", ")).
    arg(djvStringUtil::label(_options.type).join(", ")).
    arg(djvDpx::endianLabels().join(", ")).
    arg(djvStringUtil::label(_options.endian).join(", "));
}

djvImageLoad * djvDpxPlugin::createLoad() const
{
    return new djvDpxLoad(_options, imageContext());
}

djvImageSave * djvDpxPlugin::createSave() const
{
    return new djvDpxSave(_options, imageContext());
}
