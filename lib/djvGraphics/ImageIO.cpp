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

#include <djvGraphics/ImageIO.h>

#include <djvGraphics/GraphicsContext.h>

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
    namespace Graphics
    {
        ImageIOInfo::ImageIOInfo() :
            _info(1)
        {}

        ImageIOInfo::ImageIOInfo(const PixelDataInfo & in) :
            PixelDataInfo(in),
            _info(1)
        {}

        void ImageIOInfo::addLayer(const PixelDataInfo & in)
        {
            _info += in;
        }

        int ImageIOInfo::layerCount() const
        {
            return _info.count();
        }

        void ImageIOInfo::setLayerCount(int count)
        {
            _info.resize(count);
        }

        void ImageIOInfo::clearLayers()
        {
            _info.resize(1);
        }

        PixelDataInfo & ImageIOInfo::operator [] (int index)
        {
            return 0 == index ? static_cast<PixelDataInfo &>(*this) : _info[index];
        }

        const PixelDataInfo & ImageIOInfo::operator [] (int index) const
        {
            return 0 == index ? static_cast<const PixelDataInfo &>(*this) : _info[index];
        }

        ImageIOFrameInfo::ImageIOFrameInfo()
        {}

        ImageIOFrameInfo::ImageIOFrameInfo(
            qint64               frame,
            int                  layer,
            PixelDataInfo::PROXY proxy) :
            frame(frame),
            layer(layer),
            proxy(proxy)
        {}

        ImageLoad::ImageLoad(Core::CoreContext * context) :
            _context(context)
        {}

        ImageLoad::~ImageLoad()
        {}

        void ImageLoad::close()
        {}

        Core::CoreContext * ImageLoad::context() const
        {
            return _context;
        }

        ImageSave::ImageSave(Core::CoreContext * context) :
            _context(context)
        {}

        ImageSave::~ImageSave()
        {}

        void ImageSave::close()
        {}

        Core::CoreContext * ImageSave::context() const
        {
            return _context;
        }

        ImageIO::ImageIO(Core::CoreContext * context) :
            Core::Plugin(context)
        {}

        ImageIO::~ImageIO()
        {}

        QStringList ImageIO::extensions() const
        {
            return QStringList();
        }

        bool ImageIO::isSequence() const
        {
            return true;
        }

        QStringList ImageIO::option(const QString &) const
        {
            return QStringList();
        }

        bool ImageIO::setOption(const QString &, QStringList &)
        {
            return false;
        }

        QStringList ImageIO::options() const
        {
            return QStringList();
        }

        void ImageIO::commandLine(QStringList &)
        {}

        QString ImageIO::commandLineHelp() const
        {
            return QString();
        }

        ImageLoad * ImageIO::createLoad() const
        {
            return 0;
        }

        ImageSave * ImageIO::createSave() const
        {
            return 0;
        }

        const QStringList & ImageIO::errorLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::ImageIO", "Unrecognized file") <<
                qApp->translate("djv::Graphics::ImageIO", "Unsupported file") <<
                qApp->translate("djv::Graphics::ImageIO", "Cannot open file") <<
                qApp->translate("djv::Graphics::ImageIO", "Error reading file") <<
                qApp->translate("djv::Graphics::ImageIO", "Error writing file");
            DJV_ASSERT(ERROR_COUNT == data.count());
            return data;
        }

        struct ImageIOFactory::Private
        {
            // This map is used to lookup an image I/O plugin by it's name.
            QMap<QString, ImageIO *> nameMap;

            // This map is used to lookup an image I/O plugin for a given file
            // extension.
            QMap<QString, ImageIO *> extensionMap;
        };

        ImageIOFactory::ImageIOFactory(
            Core::CoreContext *  context,
            const QStringList &  searchPath,
            QObject *            parent) :
            Core::PluginFactory(context, searchPath, "djvImageIOEntry", "djv", "Plugin", parent),
            _p(new Private)
        {
            //DJV_DEBUG("ImageIOFactory::ImageIOFactory");
            const QList<Core::Plugin *> & plugins = this->plugins();
            Q_FOREACH(Core::Plugin * plugin, plugins)
            {
                if (ImageIO * imageIO = dynamic_cast<ImageIO *>(plugin))
                {
                    _addPlugin(imageIO);
                }
            }
        }

        ImageIOFactory::~ImageIOFactory()
        {
            //DJV_DEBUG("ImageIOFactory::~ImageIOFactory");
        }

        QStringList ImageIOFactory::option(
            const QString & name,
            const QString & option) const
        {
            const QString nameLower = name.toLower();
            if (_p->nameMap.contains(nameLower))
            {
                //DJV_DEBUG("ImageIOFactory::option");
                //DJV_DEBUG_PRINT("name   = " << name);
                //DJV_DEBUG_PRINT("option = " << option);
                ImageIO * imageIO = _p->nameMap[nameLower];
                return imageIO->option(option);
            }
            return QStringList();
        }

        bool ImageIOFactory::setOption(
            const QString & name,
            const QString & option,
            QStringList &   data)
        {
            const QString nameLower = name.toLower();
            if (_p->nameMap.contains(nameLower))
            {
                //DJV_DEBUG("djvImageIOFactory::setOption");
                //DJV_DEBUG_PRINT("name   = " << name);
                //DJV_DEBUG_PRINT("option = " << option);
                //DJV_DEBUG_PRINT("data   = " << data);
                ImageIO * imageIO = _p->nameMap[nameLower];
                return imageIO->setOption(option, data);
            }
            return false;
        }

        ImageLoad * ImageIOFactory::load(
            const Core::FileInfo & fileInfo,
            ImageIOInfo &          imageIOInfo) const
        {
            //DJV_DEBUG("ImageIOFactory::load");
            //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
            //DJV_LOG("ImageIOFactory", QString("Loading: \"%1\"...").arg(fileInfo));
            const QString extensionLower = fileInfo.extension().toLower();
            if (_p->extensionMap.contains(extensionLower))
            {
                ImageIO * imageIO = _p->extensionMap[extensionLower];
                //DJV_LOG("ImageIOFactory", QString("Using plugin: \"%1\"").arg(imageIO->pluginName()));
                if (ImageLoad * imageLoad = imageIO->createLoad())
                {
                    imageLoad->open(fileInfo, imageIOInfo);
                    //QStringList tmp;
                    //tmp << imageIOInfo.size;
                    //DJV_LOG("ImageIOFactory", QString("Size: %1").arg(tmp.join(", ")));
                    //tmp.clear();
                    //tmp << imageIOInfo.pixel;
                    //DJV_LOG("ImageIOFactory", QString("Pixel: %1").arg(tmp.join(", ")));
                    return imageLoad;
                }
            }
            throw Core::Error(
                "ImageIOFactory",
                qApp->translate("ImageIOFactory", "Unrecognized image: %1").
                arg(QDir::toNativeSeparators(fileInfo)));
            return 0;
        }

        ImageSave * ImageIOFactory::save(
            const Core::FileInfo & fileInfo,
            const ImageIOInfo & imageIOInfo) const
        {
            //DJV_DEBUG("ImageIOFactory::save");
            //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
            //DJV_DEBUG_PRINT("imageIOInfo = " << imageIOInfp);
            //DJV_LOG("ImageIOFactory", QString("Saving: \"%1\"").arg(fileInfo));
            //QStringList tmp;
            //tmp << imageIOInfo.size;
            //DJV_LOG("ImageIOFactory", QString("Size: %1").arg(tmp.join(", ")));
            //tmp.clear();
            //tmp << imageIOInfo.pixel;
            //DJV_LOG("ImageIOFactory", QString("Pixel: %1").arg(tmp.join(", ")));
            const QString extensionLower = fileInfo.extension().toLower();
            if (_p->extensionMap.contains(extensionLower))
            {
                ImageIO * imageIO = _p->extensionMap[extensionLower];
                //DJV_LOG("ImageIOFactory", QString("Using plugin: \"%1\"").arg(imageIO->pluginName()));
                if (ImageSave * imageSave = imageIO->createSave())
                {
                    imageSave->open(fileInfo, imageIOInfo);
                    return imageSave;
                }
            }
            throw Core::Error(
                "djv::Graphics::ImageIOFactory",
                qApp->translate("djv::Graphics::ImageIOFactory", "Unrecognized image: %1").
                arg(QDir::toNativeSeparators(fileInfo)));
            return 0;
        }

        const QStringList & ImageIOFactory::errorLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::ImageIOFactory", "Unrecognized file: \"%1\"");
            DJV_ASSERT(ERROR_COUNT == data.count());
            return data;
        }

        void ImageIOFactory::addPlugin(Core::Plugin * plugin)
        {
            Core::PluginFactory::addPlugin(plugin);
            if (ImageIO * imageIO = dynamic_cast<ImageIO *>(plugin))
            {
                _addPlugin(imageIO);
            }
        }

        void ImageIOFactory::pluginOptionCallback(const QString &)
        {
            Q_EMIT optionChanged();
        }

        void ImageIOFactory::_addPlugin(ImageIO * plugin)
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

            // This callback listens to option changes in the image I/O
            // plugins.
            connect(
                plugin,
                SIGNAL(optionChanged(const QString &)),
                SLOT(pluginOptionCallback(const QString &)));
        }

    } // namespace Graphics

    bool operator == (const Graphics::ImageIOInfo & a, const Graphics::ImageIOInfo & b)
    {
        if (a.layerCount() != b.layerCount())
            return false;
        for (int i = 0; i < a.layerCount(); ++i)
            if (a[i] != b[i])
                return false;
        return
            operator == (
                static_cast<const Graphics::PixelDataInfo &>(a),
                static_cast<const Graphics::PixelDataInfo &>(b)) &&
            a.tags == b.tags &&
            a.sequence == b.sequence;
    }

    bool operator != (const Graphics::ImageIOInfo & a, const Graphics::ImageIOInfo & b)
    {
        return !(a == b);
    }

    bool operator == (const Graphics::ImageIOFrameInfo & a, const Graphics::ImageIOFrameInfo & b)
    {
        return
            a.frame == b.frame &&
            a.layer == b.layer &&
            a.proxy == b.proxy;
    }

    bool operator != (const Graphics::ImageIOFrameInfo & a, const Graphics::ImageIOFrameInfo & b)
    {
        return !(a == b);
    }

    Core::Debug & operator << (Core::Debug & debug, const Graphics::ImageIOInfo & in)
    {
        return operator << (debug, static_cast<const Graphics::PixelDataInfo &>(in));
    }

    Core::Debug & operator << (Core::Debug & debug, const Graphics::ImageIOFrameInfo & in)
    {
        return debug << in.frame;
    }

} // namespace djv
