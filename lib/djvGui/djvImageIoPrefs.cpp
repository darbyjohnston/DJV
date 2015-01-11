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

//! \file djvImageIoPrefs.cpp

#include <djvImageIoPrefs.h>

#include <djvPrefs.h>

#include <djvImageIo.h>

#include <QApplication>

//------------------------------------------------------------------------------
// djvImageIoPrefs
//------------------------------------------------------------------------------

namespace
{

void _load(djvImageIo * plugin)
{
    //DJV_DEBUG("_load");
    //DJV_DEBUG_PRINT("plugin = " << plugin->pluginName());
    
    djvPrefs prefs(plugin->pluginName(), djvPrefs::SYSTEM);

    const QStringList & options = plugin->options();

    for (int i = 0; i < options.count(); ++i)
    {
        QStringList tmp;

        if (prefs.get(options[i], tmp))
        {
            //DJV_DEBUG_PRINT("tmp = " << tmp);

            plugin->setOption(options[i], tmp);
        }
    }
}

void _save(const djvImageIo * plugin)
{
    //DJV_DEBUG("_save");
    //DJV_DEBUG_PRINT("plugin = " << plugin->pluginName());
    
    djvPrefs prefs(plugin->pluginName(), djvPrefs::SYSTEM);

    const QStringList & options = plugin->options();

    for (int i = 0; i < options.count(); ++i)
    {
        prefs.set(options[i], plugin->option(options[i]));
    }
}

} // namespace

djvImageIoPrefs::djvImageIoPrefs(QObject * parent) :
    QObject(parent)
{
    //DJV_DEBUG("djvImageIoPrefs::djvImageIoPrefs");

    const QList<djvPlugin *> & plugins = djvImageIoFactory::global()->plugins();

    for (int i = 0; i < plugins.count(); ++i)
    {
        if (djvImageIo * plugin = dynamic_cast<djvImageIo *>(plugins[i]))
        {
            _load(plugin);
        }
    }
}

djvImageIoPrefs::~djvImageIoPrefs()
{
    //DJV_DEBUG("djvImageIoPrefs::~djvImageIoPrefs");

    const QList<djvPlugin *> & plugins = djvImageIoFactory::global()->plugins();
    
    for (int i = 0; i < plugins.count(); ++i)
    {
        if (djvImageIo * plugin = dynamic_cast<djvImageIo *>(plugins[i]))
        {
            _save(plugin);
        }
    }
}

djvImageIoPrefs * djvImageIoPrefs::global()
{
    static djvImageIoPrefs * global = 0;
    
    if (! global)
    {
        global = new djvImageIoPrefs(qApp);
    }
    
    return global;
}
