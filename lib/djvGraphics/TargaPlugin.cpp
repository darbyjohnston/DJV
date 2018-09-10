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

#include <djvGraphics/TargaPlugin.h>

#include <djvGraphics/TargaLoad.h>
#include <djvGraphics/TargaSave.h>

#include <djvCore/Assert.h>
#include <djvCore/Error.h>

#include <QCoreApplication>

//------------------------------------------------------------------------------
// djvTargaPlugin
//------------------------------------------------------------------------------

djvTargaPlugin::djvTargaPlugin(djvCoreContext * context) :
    djvImageIO(context)
{}

QString djvTargaPlugin::pluginName() const
{
    return djvTarga::staticName;
}

QStringList djvTargaPlugin::extensions() const
{
    return QStringList() << ".tga";
}

QStringList djvTargaPlugin::option(const QString & in) const
{
    QStringList out;
    if (0 == in.compare(options()[djvTarga::COMPRESSION_OPTION], Qt::CaseInsensitive))
    {
        out << _options.compression;
    }
    return out;
}

bool djvTargaPlugin::setOption(const QString & in, QStringList & data)
{
    try
    {
        if (0 == in.compare(options()[djvTarga::COMPRESSION_OPTION], Qt::CaseInsensitive))
        {
            djvTarga::COMPRESSION compression = static_cast<djvTarga::COMPRESSION>(0);
            data >> compression;
            if (compression != _options.compression)
            {
                _options.compression = compression;
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

QStringList djvTargaPlugin::options() const
{
    return djvTarga::optionsLabels();
}

void djvTargaPlugin::commandLine(QStringList & in) throw (QString)
{
    QStringList tmp;
    QString     arg;
    try
    {
        while (! in.isEmpty())
        {
            in >> arg;
            if (qApp->translate("djvTargaPlugin", "-targa_compression") == arg)
            {
                in >> _options.compression;
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

QString djvTargaPlugin::commandLineHelp() const
{
    return qApp->translate("djvTargaPlugin",
"\n"
"Targa Options\n"
"\n"
"    -targa_compression (value)\n"
"        Set the file compression used when saving Targa images. Options = "
"%1. Default = %2.\n").
    arg(djvTarga::compressionLabels().join(", ")).
    arg(djvStringUtil::label(_options.compression).join(", "));
}
    
djvImageLoad * djvTargaPlugin::createLoad() const
{
    return new djvTargaLoad(context());
}

djvImageSave * djvTargaPlugin::createSave() const
{
    return new djvTargaSave(_options, context());
}
