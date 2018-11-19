//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <djvAV/IO.h>

#include <djvAV/AVContext.h>

#include <djvCore/CoreContext.h>
#include <djvCore/Debug.h>
#include <djvCore/DebugLog.h>
#include <djvCore/Error.h>
#include <djvCore/FileInfo.h>

#include <QCoreApplication>
#include <QDir>
#include <QMap>
#include <QPointer>

#include <algorithm>

namespace djv
{
    namespace AV
    {
        IOInfo::IOInfo()
        {
            layers.push_back(PixelDataInfo());
        }

        IOInfo::IOInfo(const PixelDataInfo & info)
        {
            layers.push_back(info);
        }

        bool IOInfo::operator == (const IOInfo & other) const
        {
            return
                layers == other.layers &&
                audio == other.audio &&
                tags == other.tags &&
                sequence == other.sequence;
        }

        bool IOInfo::operator != (const IOInfo & other) const
        {
            return !(*this == other);
        }

        ImageIOInfo::ImageIOInfo()
        {}

        ImageIOInfo::ImageIOInfo(
            qint64               frame,
            size_t               layer,
            PixelDataInfo::PROXY proxy) :
            frame(frame),
            layer(layer),
            proxy(proxy)
        {}

        bool ImageIOInfo::operator == (const ImageIOInfo & other) const
        {
            return
                frame == other.frame &&
                layer == other.layer &&
                proxy == other.proxy;
        }

        bool ImageIOInfo::operator != (const ImageIOInfo & other) const
        {
            return !(*this == other);
        }

        AudioIOInfo::AudioIOInfo()
        {}

        AudioIOInfo::AudioIOInfo(uint64_t samplesOffset, uint64_t samplesSize) :
            samplesOffset(samplesOffset),
            samplesSize(samplesSize)
        {}

        bool AudioIOInfo::operator == (const AudioIOInfo & other) const
        {
            return
                samplesOffset == other.samplesOffset &&
                samplesSize == other.samplesSize;
        }

        bool AudioIOInfo::operator != (const AudioIOInfo & other) const
        {
            return !(*this == other);
        }

        struct Load::Private
        {
            QPointer<Core::CoreContext> context;
        };

        Load::Load(const Core::FileInfo & fileInfo, const QPointer<Core::CoreContext> & context) :
            _fileInfo(fileInfo),
            _p(new Private)
        {
            _p->context = context;
        }

        Load::~Load()
        {}

        void Load::read(Image &, const ImageIOInfo &)
        {}

        void Load::read(AudioData &, const AudioIOInfo &)
        {}

        const QPointer<Core::CoreContext> & Load::context() const
        {
            return _p->context;
        }

        struct Save::Private
        {
            QPointer<Core::CoreContext> context;
        };

        Save::Save(const Core::FileInfo & fileInfo, const IOInfo & ioInfo, const QPointer<Core::CoreContext> & context) :
            _fileInfo(fileInfo),
            _ioInfo(ioInfo),
            _p(new Private)
        {
            _p->context = context;
        }

        Save::~Save()
        {}

        void Save::write(const Image &, const ImageIOInfo &)
        {}

        void Save::close()
        {}

        const QPointer<Core::CoreContext> & Save::context() const
        {
            return _p->context;
        }

        IOPlugin::IOPlugin(const QPointer<Core::CoreContext> & context) :
            Core::Plugin(context)
        {}

        IOPlugin::~IOPlugin()
        {}

        QStringList IOPlugin::extensions() const
        {
            return QStringList();
        }

        bool IOPlugin::isSequence() const
        {
            return true;
        }

        QStringList IOPlugin::option(const QString &) const
        {
            return QStringList();
        }

        bool IOPlugin::setOption(const QString &, QStringList &)
        {
            return false;
        }

        QStringList IOPlugin::options() const
        {
            return QStringList();
        }

        void IOPlugin::commandLine(QStringList &)
        {}

        QString IOPlugin::commandLineHelp() const
        {
            return QString();
        }

        std::unique_ptr<Load> IOPlugin::createLoad(const Core::FileInfo &) const
        {
            return nullptr;
        }

        std::unique_ptr<Save> IOPlugin::createSave(const Core::FileInfo &, const IOInfo &) const
        {
            return nullptr;
        }

        const QStringList & IOPlugin::errorLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::IO", "Unrecognized file") <<
                qApp->translate("djv::AV::IO", "Unsupported file") <<
                qApp->translate("djv::AV::IO", "Cannot open file") <<
                qApp->translate("djv::AV::IO", "Error reading file") <<
                qApp->translate("djv::AV::IO", "Error writing file");
            DJV_ASSERT(ERROR_COUNT == data.count());
            return data;
        }

        struct IOFactory::Private
        {
            // This map is used to lookup an I/O plugin by it's name.
            QMap<QString, IOPlugin *> nameMap;

            // This map is used to lookup an I/O plugin for a given file
            // extension.
            QMap<QString, IOPlugin *> extensionMap;
        };

        IOFactory::IOFactory(
            const QPointer<Core::CoreContext> & context,
            const QStringList & searchPath,
            QObject * parent) :
            Core::PluginFactory(context, searchPath, "djvIOEntry", "djv", "Plugin", parent),
            _p(new Private)
        {
            //DJV_DEBUG("IOFactory::IOFactory");
            const QList<Core::Plugin *> & plugins = this->plugins();
            Q_FOREACH(Core::Plugin * plugin, plugins)
            {
                if (auto ioPlugin = dynamic_cast<IOPlugin *>(plugin))
                {
                    _addPlugin(ioPlugin);
                }
            }
        }

        IOFactory::~IOFactory()
        {
            //DJV_DEBUG("IOFactory::~IOFactory");
        }

        QStringList IOFactory::option(const QString & name,  const QString & option) const
        {
            const QString nameLower = name.toLower();
            if (_p->nameMap.contains(nameLower))
            {
                //DJV_DEBUG("IOFactory::option");
                //DJV_DEBUG_PRINT("name   = " << name);
                //DJV_DEBUG_PRINT("option = " << option);
                auto ioPlugin = _p->nameMap[nameLower];
                return ioPlugin->option(option);
            }
            return QStringList();
        }

        bool IOFactory::setOption(const QString & name, const QString & option, QStringList & data)
        {
            const QString nameLower = name.toLower();
            if (_p->nameMap.contains(nameLower))
            {
                //DJV_DEBUG("djvIOFactory::setOption");
                //DJV_DEBUG_PRINT("name   = " << name);
                //DJV_DEBUG_PRINT("option = " << option);
                //DJV_DEBUG_PRINT("data   = " << data);
                auto ioPlugin = _p->nameMap[nameLower];
                return ioPlugin->setOption(option, data);
            }
            return false;
        }

        std::unique_ptr<Load> IOFactory::load(const Core::FileInfo & fileInfo, IOInfo & ioInfo) const
        {
            //DJV_DEBUG("IOFactory::load");
            //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
            //DJV_LOG("IOFactory", QString("Loading: \"%1\"...").arg(fileInfo));
            const QString extensionLower = fileInfo.extension().toLower();
            if (_p->extensionMap.contains(extensionLower))
            {
                auto ioPlugin = _p->extensionMap[extensionLower];
                //DJV_LOG("IOFactory", QString("Using plugin: \"%1\"").arg(ioPlugin->pluginName()));
                if (auto load = ioPlugin->createLoad(fileInfo))
                {
                    ioInfo = load->ioInfo();
                    return load;
                }
            }
            throw Core::Error(
                "IOFactory",
                qApp->translate("djv::AV::IOFactory", "Unrecognized file: %1").
                arg(QDir::toNativeSeparators(fileInfo)));
            return nullptr;
        }

        std::unique_ptr<Save> IOFactory::save(const Core::FileInfo & fileInfo, const IOInfo & ioInfo) const
        {
            //DJV_DEBUG("IOFactory::save");
            //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
            //DJV_DEBUG_PRINT("ioInfo = " << ioInfp);
            //DJV_LOG("IOFactory", QString("Saving: \"%1\"").arg(fileInfo));
            //QStringList tmp;
            //tmp << ioInfo.size;
            //DJV_LOG("IOFactory", QString("Size: %1").arg(tmp.join(", ")));
            //tmp.clear();
            //tmp << ioInfo.pixel;
            //DJV_LOG("IOFactory", QString("Pixel: %1").arg(tmp.join(", ")));
            const QString extensionLower = fileInfo.extension().toLower();
            if (_p->extensionMap.contains(extensionLower))
            {
                auto ioPlugin = _p->extensionMap[extensionLower];
                //DJV_LOG("IOFactory", QString("Using plugin: \"%1\"").arg(ioPlugin->pluginName()));
                if (auto save = ioPlugin->createSave(fileInfo, ioInfo))
                {
                    return save;
                }
            }
            throw Core::Error(
                "djv::AV::IOFactory",
                qApp->translate("djv::AV::IOFactory", "Unrecognized file: %1").
                arg(QDir::toNativeSeparators(fileInfo)));
            return nullptr;
        }

        const QStringList & IOFactory::errorLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::IOFactory", "Unrecognized file: \"%1\"");
            DJV_ASSERT(ERROR_COUNT == data.count());
            return data;
        }

        void IOFactory::addPlugin(Core::Plugin * plugin)
        {
            Core::PluginFactory::addPlugin(plugin);
            if (auto ioPlugin = dynamic_cast<IOPlugin *>(plugin))
            {
                _addPlugin(ioPlugin);
            }
        }

        void IOFactory::pluginOptionCallback(const QString &)
        {
            Q_EMIT optionChanged();
        }

        void IOFactory::_addPlugin(IOPlugin * plugin)
        {
            // Register file sequence extensions.
            if (plugin->isSequence())
            {
                Q_FOREACH(const QString & extension, plugin->extensions())
                {
                    Core::FileInfo::sequenceExtensions.insert(extension.toLower());
                    Core::FileInfo::sequenceExtensions.insert(extension.toUpper());
                }
            }

            // Setup internal maps.
            _p->nameMap[plugin->pluginName().toLower()] = plugin;
            Q_FOREACH(const QString & extension, plugin->extensions())
            {
                _p->extensionMap[extension.toLower()] = plugin;
            }

            // This callback listens to option changes in the I/O plugins.
            connect(
                plugin,
                SIGNAL(optionChanged(const QString &)),
                SLOT(pluginOptionCallback(const QString &)));
        }

    } // namespace AV

    Core::Debug & operator << (Core::Debug & debug, const AV::IOInfo & in)
    {
        debug << "layers=" << static_cast<int>(in.layers.size());
        return debug;
    }

    Core::Debug & operator << (Core::Debug & debug, const AV::ImageIOInfo & in)
    {
        return debug << in.frame;
    }

} // namespace djv
