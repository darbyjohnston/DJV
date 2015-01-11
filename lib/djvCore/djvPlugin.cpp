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

//! \file djvPlugin.cpp

#include <djvPlugin.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvDebugLog.h>
#include <djvError.h>
#include <djvErrorUtil.h>
#include <djvFileInfoUtil.h>

#include <QDir>
#include <QMap>
#include <QPair>
#include <QStringList>

#if defined(DJV_WINDOWS)

#define WIN32_LEAN_AND_MEAN

#define NOMINMAX

#include <windows.h>

#endif // DJV_WINDOWS

#include <algorithm>
#if ! defined(DJV_WINDOWS)
#include <dlfcn.h>
#endif

//----------------------------------------------7-------------------------------
// djvPlugin
//------------------------------------------------------------------------------

djvPlugin::~djvPlugin()
{}

//------------------------------------------------------------------------------
// djvPluginFactory::P
//------------------------------------------------------------------------------

namespace
{

class Handle
{
public:

    Handle() :
        _p(0)
    {}

    ~Handle()
    {
#if defined(DJV_WINDOWS)

        if (_p)
        {
            ::FreeLibrary(_p);
        }

#else // DJV_WINDOWS

        if (_p)
        {
            ::dlclose(_p);
        }

#endif // DJV_WINDOWS
    }

    void open(const QString & in) throw (QString)
    {
#if defined(DJV_WINDOWS)

        _p = ::LoadLibrary(in.toLatin1().data());

#else // DJV_WINDOWS

        _p = ::dlopen(in.toLatin1().data(), RTLD_LAZY);

#endif // DJV_WINDOWS

        if (! _p)
        {
            QString error;

#if defined(DJV_WINDOWS)

            error = djvErrorUtil::lastError();

#else // DJV_WINDOWS

            error = ::dlerror();

#endif // DJV_WINDOWS

            throw error;
        }
    }

    void * fnc(const QString & in) const
    {
#if defined(DJV_WINDOWS)

        return (void *)::GetProcAddress(_p, in.toLatin1().data());

#else // DJV_WINDOWS

        return ::dlsym(_p, in.toLatin1().data());

#endif // DJV_WINDOWS
    }

private:

#if defined(DJV_WINDOWS)

    HINSTANCE _p;

#else // DJV_WINDOWS

    void * _p;

#endif // DJV_WINDOWS
};

} // namespace

struct djvPluginFactory::P
{
    typedef QPair<djvPlugin *, Handle *> Pair;
    
    QString             pluginPrefix;
    QString             pluginEntry;
    QMap<QString, Pair> plugins;
};

//------------------------------------------------------------------------------
// djvPluginFactory
//------------------------------------------------------------------------------

djvPluginFactory::djvPluginFactory(
    const QStringList & searchPath,
    const QString &     pluginEntry,
    const QString &     pluginPrefix,
    const QString &     pluginSuffix,
    QObject *           parent) :
    QObject(parent),
    _p(new P)
{
    //DJV_DEBUG("djvPluginFactory::djvPluginFactory");
    //DJV_DEBUG_PRINT("search path = " << searchPath);
    //DJV_DEBUG_PRINT("plugin entry = " << pluginEntry);
    //DJV_DEBUG_PRINT("plugin prefix = " << pluginPrefix);
    //DJV_DEBUG_PRINT("plugin suffix = " << pluginSuffix);

    //! \todo Hard-coded OS specific shared library file extensions.

    QStringList glob;

#if defined(DJV_WINDOWS)

    glob += pluginPrefix + "*" + pluginSuffix + ".dll";

#else // DJV_WINDOWS

    glob += "lib" + pluginPrefix + "*" + pluginSuffix + ".so";

#endif // DJV_WINDOWS

    //DJV_DEBUG_PRINT("glob = " << glob);
    
    // Find plugins.

    DJV_LOG("djvPluginFactory", "Searching for plugins...");
    DJV_LOG("djvPluginFactory",
        QString("Plugin search pattern: \"%1\"").arg(glob.join(", ")));

    djvFileInfoList fileInfoList;

    Q_FOREACH(const QString & path, searchPath)
    {
        //DJV_DEBUG_PRINT("searching = " << path);

        DJV_LOG("djvPluginFactory",
            QString("Checking search path: \"%1\"").arg(path));

        djvFileInfoList tmp = djvFileInfoUtil::list(
            path,
            djvSequence::COMPRESS_OFF);
        
        djvFileInfoUtil::filter(
            tmp,
            djvFileInfoUtil::FILTER_NONE,
            QString(),
            glob);

        //DJV_DEBUG_PRINT("tmp = " << tmp.count());
        
        Q_FOREACH(const djvFileInfo & fileInfo, tmp)
        {
            //DJV_DEBUG_PRINT("found = " << fileInfo);

            DJV_LOG("djvPluginFactory",
                QString("Found plugin: \"%1\"").arg(fileInfo));

            fileInfoList += fileInfo;
        }
    }

    // Load plugins.

    //DJV_DEBUG_PRINT("fileInfoList = " << fileInfoList.count());
    
    Q_FOREACH(const djvFileInfo & fileInfo, fileInfoList)
    {
        // Open.

        //DJV_DEBUG_PRINT("loading = " << fileInfo);

        DJV_LOG("djvPluginFactory",
            QString("Loading plugin: \"%1\"...").arg(fileInfo));

        QScopedPointer<Handle> handle(new Handle);

        try
        {
            handle->open(fileInfo);
        }
        catch (const QString & error)
        {
            DJV_LOG(
                "djvPluginFactory",         
                errorLabels()[ERROR_OPEN].
                arg(QDir::toNativeSeparators(fileInfo)).
                arg(error));

            continue;
        }

        djvPluginEntry * entry = (djvPluginEntry *)handle->fnc(pluginEntry);

        if (! entry)
        {
            DJV_LOG("djvPluginFactory", "No plugin entry point");
            
            continue;
        }

        QScopedPointer<djvPlugin> plugin;

        try
        {
            plugin.reset(entry());
        }
        catch (const djvError & error)
        {
            DJV_LOG("djvPluginFactory", djvErrorUtil::format(error).join("\n"));

            plugin.reset();
        }

        if (! plugin.data())
        {
            DJV_LOG(
                "djvPluginFactory",
                errorLabels()[ERROR_LOAD].
                arg(QDir::toNativeSeparators(fileInfo)));

            continue;
        }

        //DJV_DEBUG_PRINT("name = " << plugin->pluginName());

        DJV_LOG("djvPluginFactory",
            QString("Plugin name: \"%1\"").arg(plugin->pluginName()));

        // Check for duplicates.

        if (_p->plugins.contains(plugin->pluginName()))
        {
            //DJV_DEBUG_PRINT("duplicate");
            
            DJV_LOG("djvPluginFactory", "Duplicate plugin, discarding");

            plugin.reset();

            continue;
        }

        // Initialize.

        try
        {
            plugin->initPlugin();
        }
        catch (const djvError & error)
        {
            DJV_LOG("djvPluginFactory", djvErrorUtil::format(error).join("\n"));

            plugin->releasePlugin();
            plugin.reset();

            continue;
        }

        // Add.

        djvPlugin * p = plugin.take();
        Handle    * h = handle.take();

        _p->plugins[p->pluginName()] = P::Pair(p, h);
    }
    
    DJV_LOG("djvPluginFactory",
        QString("Plugins loaded: %1").arg(_p->plugins.count()));
}

djvPluginFactory::~djvPluginFactory()
{
    //DJV_DEBUG("djvPluginFactory::~djvPluginFactory");

    Q_FOREACH(P::Pair pair, _p->plugins)
    {
        pair.first->releasePlugin();

        delete pair.first;
        delete pair.second;
    }

    delete _p;
}

QList<djvPlugin *> djvPluginFactory::plugins() const
{
    QList<djvPlugin *> list;
    
    Q_FOREACH(P::Pair pair, _p->plugins)
    {
        list += pair.first;
    }
    
    return list;
}
    
djvPlugin * djvPluginFactory::plugin(const QString & name) const
{
    return _p->plugins.contains(name) ? _p->plugins[name].first : 0;
}

QStringList djvPluginFactory::names() const
{
    QStringList out;

    Q_FOREACH(QString name, _p->plugins.keys())
    {
        out += name;
    }

    return out;
}

const QStringList & djvPluginFactory::errorLabels()
{
    static const QStringList data = QStringList() <<
        tr("Cannot open plugin \"%1\": %2") <<
        tr("Cannot load plugin \"%1\"");
    
    DJV_ASSERT(ERROR_COUNT == data.count());
    
    return data;
}

