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

//! \file djvLibquicktimePlugin.cpp

#include <djvLibquicktimePlugin.h>

#include <djvLibquicktimeLoad.h>
#include <djvLibquicktimeSave.h>
#include <djvLibquicktimeWidget.h>

#include <djvAssert.h>
#include <djvCoreApplication.h>
#include <djvDebug.h>
#include <djvStringUtil.h>
#include <djvSystem.h>

extern "C"
{

DJV_PLUGIN_EXPORT djvPlugin * djvImageIo()
{
    return new djvLibquicktimePlugin;
}

void logCallback(lqt_log_level_t, const char *, const char *, void *)
{}

}

//------------------------------------------------------------------------------
// djvLibquicktimePlugin::Options
//------------------------------------------------------------------------------

djvLibquicktimePlugin::Options::Options() :
    codec("mjpa")
{}

//------------------------------------------------------------------------------
// djvLibquicktimePlugin
//------------------------------------------------------------------------------

const QString djvLibquicktimePlugin::staticName = "libquicktime";

QStringList djvLibquicktimePlugin::codecLabels()
{
    QStringList out;

    lqt_codec_info_t ** info = lqt_query_registry(0, 1, 1, 0);

    if (info)
    {
        for (int i = 0; info[i]; ++i)
        {
            out += info[i]->name;
        }

        lqt_destroy_codec_info(info);
    }

    return out;
}

QStringList djvLibquicktimePlugin::codecTextLabels()
{
    QStringList out;

    lqt_codec_info_t ** info = lqt_query_registry(0, 1, 1, 0);

    if (info)
    {
        for (int i = 0; info[i]; ++i)
        {
            out += info[i]->long_name;
        }

        lqt_destroy_codec_info(info);
    }

    return out;
}

const QStringList & djvLibquicktimePlugin::optionsLabels()
{
    static const QStringList data = QStringList() <<
        "Save Codec";

    DJV_ASSERT(data.count() == OPTIONS_COUNT);

    return data;
}

void djvLibquicktimePlugin::initPlugin() throw (djvError)
{
    //DJV_DEBUG("djvLibquicktimePlugin::initPlugin");

    static const QString envVar = "LIBQUICKTIME_PLUGIN_DIR";

    const QString env = djvSystem::env(envVar);
    
    if (env.isEmpty())
    {
        const QString pluginPath =
            qApp->applicationDirPath() + "/../lib/libquicktime";

        //DJV_DEBUG_PRINT("plugin path = " << pluginPath);

        djvSystem::setEnv(envVar, pluginPath);
    }

    lqt_set_log_callback(logCallback, 0);

    //DJV_DEBUG_PRINT("codec = " << codecLabels());
}

djvPlugin * djvLibquicktimePlugin::copyPlugin() const
{
    djvLibquicktimePlugin * plugin = new djvLibquicktimePlugin;
    
    plugin->_options = _options;
    
    return plugin;
}

QString djvLibquicktimePlugin::pluginName() const
{
    return staticName;
}

QStringList djvLibquicktimePlugin::extensions() const
{
    return QStringList() <<
       ".qt"  <<
       ".mov" <<
       ".avi" <<
       ".mp4";
}

bool djvLibquicktimePlugin::isSeq() const
{
    return false;
}

QStringList djvLibquicktimePlugin::option(const QString & in) const
{
    const QStringList & list = options();

    QStringList out;

    if (in.compare(list[CODEC], Qt::CaseInsensitive))
    {
        out << _options.codec;
    }

    return out;
}

bool djvLibquicktimePlugin::setOption(const QString & in, QStringList & data)
{
    const QStringList & list = options();

    try
    {
        if (in.compare(list[CODEC], Qt::CaseInsensitive))
        {
            QString codec;
            
            data >> codec;
            
            if (codec != _options.codec)
            {
                _options.codec = codec;
                
                Q_EMIT optionChanged(in);
            }
        }
    }
    catch (QString)
    {
        return false;
    }

    return true;
}

QStringList djvLibquicktimePlugin::options() const
{
    return optionsLabels();
}

void djvLibquicktimePlugin::commandLine(QStringList & in) throw (QString)
{
    QStringList tmp;
    QString     arg;

    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            if ("-libquicktime_codec" == arg)
            {
                in >> _options.codec;
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

QString djvLibquicktimePlugin::commandLineHelp() const
{
    return QString(
"\n"
"Libquicktime Options\n"
"\n"
"    -libquicktime_codec (value)\n"
"        Set the codec used when saving Libquicktime movies. Options = %1. "
"Default = %2.\n").
    arg(codecLabels().join(", ")).
    arg(djvStringUtil::label(_options.codec).join(", "));
}

djvImageLoad * djvLibquicktimePlugin::createLoad() const
{
    return new djvLibquicktimeLoad(_options);
}

djvImageSave * djvLibquicktimePlugin::createSave() const
{
    return new djvLibquicktimeSave(_options);
}

djvAbstractPrefsWidget * djvLibquicktimePlugin::createWidget()
{
    return new djvLibquicktimeWidget(this);
}

