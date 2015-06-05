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

//! \file djvPpmPlugin.cpp

#include <djvPpmPlugin.h>

#include <djvPpmLoad.h>
#include <djvPpmSave.h>

#include <djvAssert.h>
#include <djvError.h>
#include <djvFileIoUtil.h>
#include <djvMath.h>

#include <QApplication>

extern "C"
{

DJV_PLUGIN_EXPORT djvPlugin * djvImageIoEntry(djvCoreContext * context)
{
    return new djvPpmPlugin(context);
}

} // extern "C"

//------------------------------------------------------------------------------
// djvPpmPlugin
//------------------------------------------------------------------------------

djvPpmPlugin::djvPpmPlugin(djvCoreContext * context) :
    djvImageIo(context)
{}

djvPlugin * djvPpmPlugin::copyPlugin() const
{
    djvPpmPlugin * plugin = new djvPpmPlugin(context());
    
    plugin->_options = _options;
    
    return plugin;
}

QString djvPpmPlugin::pluginName() const
{
    return djvPpm::staticName;
}

QStringList djvPpmPlugin::extensions() const
{
    return QStringList() <<
        ".ppm" <<
        ".pnm" <<
        ".pgm" <<
        ".pbm";
}

QStringList djvPpmPlugin::option(const QString & in) const
{
    QStringList out;

    if (0 == in.compare(options()[djvPpm::TYPE_OPTION], Qt::CaseInsensitive))
    {
        out << _options.type;
    }
    else if (0 == in.compare(options()[djvPpm::DATA_OPTION], Qt::CaseInsensitive))
    {
        out << _options.data;
    }

    return out;
}

bool djvPpmPlugin::setOption(const QString & in, QStringList & data)
{
    try
    {
        if (0 == in.compare(options()[djvPpm::TYPE_OPTION], Qt::CaseInsensitive))
        {
            djvPpm::TYPE type = static_cast<djvPpm::TYPE>(0);
            
            data >> type;
            
            if (type != _options.type)
            {
                _options.type = type;
            
                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(options()[djvPpm::DATA_OPTION], Qt::CaseInsensitive))
        {
            djvPpm::DATA tmp = static_cast<djvPpm::DATA>(0);
            
            data >> tmp;
            
            if (tmp != _options.data)
            {
                _options.data = tmp;

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

QStringList djvPpmPlugin::options() const
{
    return djvPpm::optionsLabels();
}

void djvPpmPlugin::commandLine(QStringList & in) throw (QString)
{
    QStringList tmp;
    QString     arg;

    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            if (qApp->translate("djvPpmPlugin", "-ppm_type") == arg)
            {
                in >> _options.type;
            }
            else if (qApp->translate("djvPpmPlugin", "-ppm_data") == arg)
            {
                in >> _options.data;
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

QString djvPpmPlugin::commandLineHelp() const
{
    return qApp->translate("djvPpmPlugin",
"\n"
"PPM Options\n"
"\n"
"    -ppm_type (value)\n"
"        Set the file type used when saving PPM images. Options = %1. "
"Default = %2.\n"
"    -ppm_data (value)\n"
"        Set the data type used when saving PPM images. Options = %3. "
"Default = %4.\n").
    arg(djvPpm::typeLabels().join(", ")).
    arg(djvStringUtil::label(_options.type).join(", ")).
    arg(djvPpm::dataLabels().join(", ")).
    arg(djvStringUtil::label(_options.data).join(", "));
}

djvImageLoad * djvPpmPlugin::createLoad() const
{
    return new djvPpmLoad;
}

djvImageSave * djvPpmPlugin::createSave() const
{
    return new djvPpmSave(_options);
}
