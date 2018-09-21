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

#include <djvCore/Plugin.h>

#include <djvCore/Assert.h>
#include <djvCore/CoreContext.h>
#include <djvCore/Debug.h>
#include <djvCore/DebugLog.h>
#include <djvCore/Error.h>
#include <djvCore/ErrorUtil.h>
#include <djvCore/FileInfoUtil.h>

#include <QCoreApplication>
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

namespace djv
{
    namespace Core
    {
        Plugin::Plugin(CoreContext * context) :
            _context(context)
        {}

        Plugin::~Plugin()
        {}

        CoreContext * Plugin::context() const
        {
            return _context;
        }

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

                void open(const QString & in)
                {
#if defined(DJV_WINDOWS)
                    _p = ::LoadLibrary(in.toLatin1().data());
#else // DJV_WINDOWS
                    _p = ::dlopen(in.toLatin1().data(), RTLD_LAZY);
#endif // DJV_WINDOWS
                    if (!_p)
                    {
                        QString error;
#if defined(DJV_WINDOWS)
                        error = ErrorUtil::lastError();
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

        struct PluginFactory::Private
        {
            Private(CoreContext * context) :
                context(context)
            {}

            QString pluginPrefix;
            QString pluginEntry;
            typedef QPair<Plugin *, Handle *> Pair;
            QMap<QString, Pair> plugins;
            CoreContext * context = nullptr;
        };

        PluginFactory::PluginFactory(
            CoreContext *    context,
            const QStringList & searchPath,
            const QString &     pluginEntry,
            const QString &     pluginPrefix,
            const QString &     pluginSuffix,
            QObject *           parent) :
            QObject(parent),
            _p(new Private(context))
        {
            //DJV_DEBUG("PluginFactory::PluginFactory");
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
            DJV_LOG(context->debugLog(), "djv::Core::PluginFactory", "Searching for plugins...");
            DJV_LOG(context->debugLog(), "djv::Core::PluginFactory",
                QString("Plugin search pattern: \"%1\"").arg(glob.join(", ")));
            FileInfoList fileInfoList;
            Q_FOREACH(const QString & path, searchPath)
            {
                //DJV_DEBUG_PRINT("searching = " << path);
                DJV_LOG(context->debugLog(), "djv::Core::PluginFactory",
                    QString("Checking search path: \"%1\"").arg(path));
                FileInfoList tmp = FileInfoUtil::list(
                    path,
                    Sequence::COMPRESS_OFF);
                FileInfoUtil::filter(
                    tmp,
                    FileInfoUtil::FILTER_NONE,
                    QString(),
                    glob);
                //DJV_DEBUG_PRINT("tmp = " << tmp.count());
                Q_FOREACH(const FileInfo & fileInfo, tmp)
                {
                    //DJV_DEBUG_PRINT("found = " << fileInfo);
                    DJV_LOG(context->debugLog(), "djv::Core::PluginFactory",
                        QString("Found plugin: \"%1\"").arg(fileInfo));
                    fileInfoList += fileInfo;
                }
            }

            // Load plugins.
            //DJV_DEBUG_PRINT("fileInfoList = " << fileInfoList.count());
            Q_FOREACH(const FileInfo & fileInfo, fileInfoList)
            {
                // Open.
                //DJV_DEBUG_PRINT("loading = " << fileInfo);
                DJV_LOG(context->debugLog(), "djv::Core::PluginFactory",
                    QString("Loading plugin: \"%1\"...").arg(fileInfo));
                QScopedPointer<Handle> handle(new Handle);
                try
                {
                    handle->open(fileInfo);
                }
                catch (const QString & error)
                {
                    DJV_LOG(context->debugLog(),
                        "djv::Core::PluginFactory",
                        errorLabels()[ERROR_OPEN].
                        arg(QDir::toNativeSeparators(fileInfo)).
                        arg(error));
                    continue;
                }
                djvCorePluginEntry * entry = (djvCorePluginEntry *)handle->fnc(pluginEntry);
                if (!entry)
                {
                    DJV_LOG(context->debugLog(), "djv::Core::PluginFactory",
                        "No plugin entry point");
                    continue;
                }
                QScopedPointer<Plugin> plugin;
                try
                {
                    plugin.reset(entry(_p->context));
                }
                catch (const Error & error)
                {
                    DJV_LOG(context->debugLog(), "djv::Core::PluginFactory",
                        ErrorUtil::format(error).join("\n"));

                    plugin.reset();
                }
                if (!plugin.data())
                {
                    DJV_LOG(context->debugLog(), "djv::Core::PluginFactory",
                        errorLabels()[ERROR_LOAD].
                        arg(QDir::toNativeSeparators(fileInfo)));
                    continue;
                }
                //DJV_DEBUG_PRINT("name = " << plugin->pluginName());
                DJV_LOG(context->debugLog(), "djv::Core::PluginFactory",
                    QString("Plugin name: \"%1\"").arg(plugin->pluginName()));

                // Check for duplicates.
                if (_p->plugins.contains(plugin->pluginName()))
                {
                    //DJV_DEBUG_PRINT("duplicate");
                    DJV_LOG(context->debugLog(), "djv::Core::PluginFactory",
                        "Duplicate plugin, discarding");
                    plugin.reset();
                    continue;
                }

                // Initialize.
                try
                {
                    plugin->initPlugin();
                }
                catch (const Error & error)
                {
                    DJV_LOG(context->debugLog(), "djv::Core::PluginFactory",
                        ErrorUtil::format(error).join("\n"));
                    plugin->releasePlugin();
                    plugin.reset();
                    continue;
                }

                // Add.
                Plugin * p = plugin.take();
                Handle * h = handle.take();
                _p->plugins[p->pluginName()] = Private::Pair(p, h);
            }

            DJV_LOG(context->debugLog(), "djv::Core::PluginFactory",
                QString("Plugins loaded: %1").arg(_p->plugins.count()));
        }

        PluginFactory::~PluginFactory()
        {
            //DJV_DEBUG("PluginFactory::~PluginFactory");
            Q_FOREACH(Private::Pair pair, _p->plugins)
            {
                pair.first->releasePlugin();
                delete pair.first;
                delete pair.second;
            }
        }

        QList<Plugin *> PluginFactory::plugins() const
        {
            QList<Plugin *> list;
            Q_FOREACH(Private::Pair pair, _p->plugins)
            {
                list += pair.first;
            }
            return list;
        }

        Plugin * PluginFactory::plugin(const QString & name) const
        {
            return _p->plugins.contains(name) ? _p->plugins[name].first : 0;
        }

        QStringList PluginFactory::names() const
        {
            QStringList out;
            Q_FOREACH(QString name, _p->plugins.keys())
            {
                out += name;
            }
            return out;
        }

        void PluginFactory::addPlugin(Plugin * in)
        {
            //DJV_DEBUG("PluginFactory::addPlugin");
            try
            {
                in->initPlugin();
                _p->plugins[in->pluginName()] = Private::Pair(in, nullptr);
            }
            catch (const Error & error)
            {
                DJV_LOG(_p->context->debugLog(), "djv::Core::PluginFactory",
                    ErrorUtil::format(error).join("\n"));
                in->releasePlugin();
            }
        }

        const QStringList & PluginFactory::errorLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Core::PluginFactory", "Cannot open plugin \"%1\": %2") <<
                qApp->translate("djv::Core::PluginFactory", "Cannot load plugin \"%1\"");
            DJV_ASSERT(ERROR_COUNT == data.count());
            return data;
        }

    } // namespace Core
} // namespace djv
