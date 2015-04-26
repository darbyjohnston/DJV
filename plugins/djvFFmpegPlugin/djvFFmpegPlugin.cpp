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

//! \file djvFFmpegPlugin.cpp

#include <djvFFmpegPlugin.h>

#include <djvFFmpegLoad.h>
#include <djvFFmpegSave.h>
#include <djvFFmpegWidget.h>

#include <djvAssert.h>
#include <djvError.h>
#include <djvFileIoUtil.h>
#include <djvMath.h>

#include <QApplication>

extern "C"
{

DJV_PLUGIN_EXPORT djvPlugin * djvImageIo()
{
    return new djvFFmpegPlugin;
}

} // extern "C"

//------------------------------------------------------------------------------
// djvFFmpegPlugin::Options
//------------------------------------------------------------------------------

djvFFmpegPlugin::Options::Options()
{}

//------------------------------------------------------------------------------
// djvFFmpegPlugin
//------------------------------------------------------------------------------

const QString djvFFmpegPlugin::staticName = "FFmpeg";

const QStringList & djvFFmpegPlugin::optionsLabels()
{
    static const QStringList data = QStringList();

    DJV_ASSERT(data.count() == OPTIONS_COUNT);

    return data;
}

void djvFFmpegPlugin::initPlugin() throw (djvError)
{
    //DJV_DEBUG("djvFFmpegPlugin::initPlugin");
    
    av_register_all();
}

djvPlugin * djvFFmpegPlugin::copyPlugin() const
{
    djvFFmpegPlugin * plugin = new djvFFmpegPlugin;
    
    plugin->_options = _options;
    
    return plugin;
}

QString djvFFmpegPlugin::pluginName() const
{
    return staticName;
}

QStringList djvFFmpegPlugin::extensions() const
{
    return QStringList() <<
        ".mov" <<
        ".avi" <<
        ".mp4" <<
        ".mpg" <<
        ".mpeg";
}

QStringList djvFFmpegPlugin::option(const QString & in) const
{
    QStringList out;

    return out;
}

bool djvFFmpegPlugin::setOption(const QString & in, QStringList & data)
{
    try
    {
    }
    catch (const QString &)
    {
        return false;
    }

    return true;
}

QStringList djvFFmpegPlugin::options() const
{
    return optionsLabels();
}

void djvFFmpegPlugin::commandLine(QStringList & in) throw (QString)
{
    QStringList tmp;
    QString     arg;

    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            if (0)
            {
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

QString djvFFmpegPlugin::commandLineHelp() const
{
    return qApp->translate("djvFFmpegPlugin",
"\n"
"FFmpeg Options\n"
"\n");
}

djvImageLoad * djvFFmpegPlugin::createLoad() const
{
    return new djvFFmpegLoad;
}

djvImageSave * djvFFmpegPlugin::createSave() const
{
    return new djvFFmpegSave(_options);
}

djvAbstractPrefsWidget * djvFFmpegPlugin::createWidget()
{
    return new djvFFmpegWidget(this);
}
