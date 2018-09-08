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

#include <djvPPMPlugin.h>

#include <djvPPMLoad.h>
#include <djvPPMSave.h>

#include <djvAssert.h>
#include <djvError.h>
#include <djvFileIOUtil.h>
#include <djvMath.h>

#include <QCoreApplication>

//------------------------------------------------------------------------------
// djvPPMPlugin
//------------------------------------------------------------------------------

djvPPMPlugin::djvPPMPlugin(djvCoreContext * context) :
    djvImageIO(context)
{}

QString djvPPMPlugin::pluginName() const
{
    return djvPPM::staticName;
}

QStringList djvPPMPlugin::extensions() const
{
    return QStringList() <<
        ".ppm" <<
        ".pnm" <<
        ".pgm" <<
        ".pbm";
}

QStringList djvPPMPlugin::option(const QString & in) const
{
    QStringList out;
    if (0 == in.compare(options()[djvPPM::TYPE_OPTION], Qt::CaseInsensitive))
    {
        out << _options.type;
    }
    else if (0 == in.compare(options()[djvPPM::DATA_OPTION], Qt::CaseInsensitive))
    {
        out << _options.data;
    }
    return out;
}

bool djvPPMPlugin::setOption(const QString & in, QStringList & data)
{
    try
    {
        if (0 == in.compare(options()[djvPPM::TYPE_OPTION], Qt::CaseInsensitive))
        {
            djvPPM::TYPE type = static_cast<djvPPM::TYPE>(0);
            data >> type;
            if (type != _options.type)
            {
                _options.type = type;
                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(options()[djvPPM::DATA_OPTION], Qt::CaseInsensitive))
        {
            djvPPM::DATA tmp = static_cast<djvPPM::DATA>(0);
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

QStringList djvPPMPlugin::options() const
{
    return djvPPM::optionsLabels();
}

void djvPPMPlugin::commandLine(QStringList & in) throw (QString)
{
    QStringList tmp;
    QString     arg;
    try
    {
        while (! in.isEmpty())
        {
            in >> arg;
            if (qApp->translate("djvPPMPlugin", "-ppm_type") == arg)
            {
                in >> _options.type;
            }
            else if (qApp->translate("djvPPMPlugin", "-ppm_data") == arg)
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

QString djvPPMPlugin::commandLineHelp() const
{
    return qApp->translate("djvPPMPlugin",
"\n"
"PPM Options\n"
"\n"
"    -ppm_type (value)\n"
"        Set the file type used when saving PPM images. Options = %1. "
"Default = %2.\n"
"    -ppm_data (value)\n"
"        Set the data type used when saving PPM images. Options = %3. "
"Default = %4.\n").
    arg(djvPPM::typeLabels().join(", ")).
    arg(djvStringUtil::label(_options.type).join(", ")).
    arg(djvPPM::dataLabels().join(", ")).
    arg(djvStringUtil::label(_options.data).join(", "));
}

djvImageLoad * djvPPMPlugin::createLoad() const
{
    return new djvPPMLoad(context());
}

djvImageSave * djvPPMPlugin::createSave() const
{
    return new djvPPMSave(_options, context());
}
