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

#include <djvGraphics/JPEGPlugin.h>

#include <djvGraphics/JPEGLoad.h>
#include <djvGraphics/JPEGSave.h>

#include <djvCore/Assert.h>
#include <djvCore/Error.h>

#include <QCoreApplication>

//------------------------------------------------------------------------------
// djvJPEGPlugin
//------------------------------------------------------------------------------

djvJPEGPlugin::djvJPEGPlugin(djvCoreContext * context) :
    djvImageIO(context)
{}

QString djvJPEGPlugin::pluginName() const
{
    return djvJPEG::staticName;
}

QStringList djvJPEGPlugin::extensions() const
{
    return QStringList() <<
        ".jpeg" <<
        ".jpg" <<
        ".jfif";
}

QStringList djvJPEGPlugin::option(const QString & in) const
{
    QStringList out;
    if (0 == in.compare(options()[djvJPEG::QUALITY_OPTION], Qt::CaseInsensitive))
    {
        out << _options.quality;
    }
    return out;
}

bool djvJPEGPlugin::setOption(const QString & in, QStringList & data)
{
    try
    {
        if (0 == in.compare(options()[djvJPEG::QUALITY_OPTION], Qt::CaseInsensitive))
        {
            int quality = 0;
            data >> quality;
            if (quality != _options.quality)
            {
                _options.quality = quality;
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

QStringList djvJPEGPlugin::options() const
{
    return djvJPEG::optionsLabels();
}

void djvJPEGPlugin::commandLine(QStringList & in) throw (QString)
{
    QStringList tmp;
    QString     arg;
    try
    {
        while (! in.isEmpty())
        {
            in >> arg;
            if (qApp->translate("djvJPEGPlugin", "-jpeg_quality") == arg)
            {
                in >> _options.quality;
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

QString djvJPEGPlugin::commandLineHelp() const
{
    return qApp->translate("djvJPEGPlugin",
"\n"
"JPEG Options\n"
"\n"
"    -jpeg_quality (value)\n"
"        Set the quality used when saving JPEG images. Default = %1.\n").
    arg(djvStringUtil::label(_options.quality).join(", "));
}

djvImageLoad * djvJPEGPlugin::createLoad() const
{
    return new djvJPEGLoad(context());
}

djvImageSave * djvJPEGPlugin::createSave() const
{
    return new djvJPEGSave(_options, context());
}
