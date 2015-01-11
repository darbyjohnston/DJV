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

//! \file djvCineonPlugin.cpp

#include <djvCineonPlugin.h>

#include <djvCineonLoad.h>
#include <djvCineonSave.h>
#include <djvCineonWidget.h>

#include <djvAssert.h>
#include <djvError.h>

extern "C"
{

DJV_PLUGIN_EXPORT djvPlugin * djvImageIo()
{
    return new djvCineonPlugin;
}

} // extern "C"

//------------------------------------------------------------------------------
// djvCineonPlugin::Options
//------------------------------------------------------------------------------

djvCineonPlugin::Options::Options() :
    inputColorProfile (djvCineon::COLOR_PROFILE_AUTO),
    outputColorProfile(djvCineon::COLOR_PROFILE_FILM_PRINT)
{}

//------------------------------------------------------------------------------
// djvCineonPlugin
//------------------------------------------------------------------------------

const QStringList & djvCineonPlugin::optionsLabels()
{
    static const QStringList data = QStringList() <<
        "Input Color Profile" <<
        "Input Film Print" <<
        "Output Color Profile" <<
        "Output Film Print";

    DJV_ASSERT(data.count() == OPTIONS_COUNT);

    return data;
}

djvCineonPlugin::~djvCineonPlugin()
{}

djvPlugin * djvCineonPlugin::copyPlugin() const
{
    djvCineonPlugin * out = new djvCineonPlugin;
    
    out->_options = _options;
    
    return out;
}

QString djvCineonPlugin::pluginName() const
{
    return djvCineon::staticName;
}

QStringList djvCineonPlugin::extensions() const
{
    return QStringList() << ".cin";
}

QStringList djvCineonPlugin::option(const QString & in) const
{
    QStringList out;

    if (0 == in.compare(options()[INPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
    {
        out << _options.inputColorProfile;
    }
    else if (0 == in.compare(options()[INPUT_FILM_PRINT_OPTION], Qt::CaseInsensitive))
    {
        out << _options.inputFilmPrint;
    }
    else if (0 == in.compare(options()[OUTPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
    {
        out << _options.outputColorProfile;
    }
    else if (0 == in.compare(options()[OUTPUT_FILM_PRINT_OPTION], Qt::CaseInsensitive))
    {
        out << _options.outputFilmPrint;
    }

    return out;
}

bool djvCineonPlugin::setOption(const QString & in, QStringList & data)
{
    //DJV_DEBUG("djvCineonPlugin::setOption");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("data = " << data);

    try
    {
        if (0 == in.compare(options()[INPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
        {
            djvCineon::COLOR_PROFILE colorProfile =
                static_cast<djvCineon::COLOR_PROFILE>(0);
            
            data >> colorProfile;
            
            if (colorProfile != _options.inputColorProfile)
            {
                //DJV_DEBUG_PRINT("color profile = " << colorProfile);
                
                _options.inputColorProfile = colorProfile;
            
                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(options()[INPUT_FILM_PRINT_OPTION], Qt::CaseInsensitive))
        {
            djvCineon::FilmPrintToLinear filmPrint;
            
            data >> filmPrint;
            
            if (filmPrint != _options.inputFilmPrint)
            {
                _options.inputFilmPrint = filmPrint;
                
                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(options()[OUTPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
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
        else if (0 == in.compare(options()[OUTPUT_FILM_PRINT_OPTION], Qt::CaseInsensitive))
        {
            djvCineon::LinearToFilmPrint filmPrint;
            
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

QStringList djvCineonPlugin::options() const
{
    return optionsLabels();
}

void djvCineonPlugin::commandLine(QStringList & in) throw (QString)
{
    QStringList tmp;
    QString     arg;

    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            if ("-cineon_input_color_profile" == arg)
            {
                in >> _options.inputColorProfile;
            }
            else if ("-cineon_input_film_print" == arg)
            {
                in >> _options.inputFilmPrint;
            }
            else if ("-cineon_output_color_profile" == arg)
            {
                in >> _options.outputColorProfile;
            }
            else if ("-cineon_output_film_print" == arg)
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

QString djvCineonPlugin::commandLineHelp() const
{
    return QString(
"\n"
"Cineon Options\n"
"\n"
"    -cineon_input_color_profile (value)\n"
"        Set the color profile used when loading Cineon images. Options = %1. "
"Default = %2.\n"
"    -cineon_input_film_print (black) (white) (gamma) (soft clip)\n"
"        Set the film print values used when loading Cineon images. Default = "
"%3.\n"
"    -cineon_output_color_profile (value)\n"
"        Set the color profile used when saving Cineon images. Options = %4. "
"Default = %5.\n"
"    -cineon_output_film_print (black) (white) (gamma) (soft clip)\n"
"        Set the film print values used when saving Cineon images. Default = "
"%6.\n").
    arg(djvCineon::colorProfileLabels().join(", ")).
    arg(djvStringUtil::label(_options.inputColorProfile).join(", ")).
    arg(djvStringUtil::label(_options.inputFilmPrint).join(", ")).
    arg(djvCineon::colorProfileLabels().join(", ")).
    arg(djvStringUtil::label(_options.outputColorProfile).join(", ")).
    arg(djvStringUtil::label(_options.outputFilmPrint).join(", "));
}

djvImageLoad * djvCineonPlugin::createLoad() const
{
    return new djvCineonLoad(_options);
}

djvImageSave * djvCineonPlugin::createSave() const
{
    return new djvCineonSave(_options);
}

djvAbstractPrefsWidget * djvCineonPlugin::createWidget()
{
    return new djvCineonWidget(this);
}
