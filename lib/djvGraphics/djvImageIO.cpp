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

#include <djvImageIO.h>

#include <djvGraphicsContext.h>

#include <djvDebug.h>
#include <djvDebugLog.h>
#include <djvError.h>
#include <djvFileInfo.h>

#include <QCoreApplication>
#include <QDir>
#include <QMap>
#include <QPointer>

#include <algorithm>

//------------------------------------------------------------------------------
// djvImageIOInfo
//------------------------------------------------------------------------------

djvImageIOInfo::djvImageIOInfo() :
    _info(1)
{}

djvImageIOInfo::djvImageIOInfo(const djvPixelDataInfo & in) :
    djvPixelDataInfo(in),
    _info(1)
{}

void djvImageIOInfo::addLayer(const djvPixelDataInfo & in)
{
    _info += in;
}

int djvImageIOInfo::layerCount() const
{
    return _info.count();
}

void djvImageIOInfo::setLayerCount(int count)
{
    _info.resize(count);
}

void djvImageIOInfo::clearLayers()
{
    _info.resize(1);
}

djvPixelDataInfo & djvImageIOInfo::operator [] (int index)
{
    return 0 == index ? static_cast<djvPixelDataInfo &>(*this) : _info[index];
}

const djvPixelDataInfo & djvImageIOInfo::operator [] (int index) const
{
    return 0 == index ? static_cast<const djvPixelDataInfo &>(*this) : _info[index];
}

//------------------------------------------------------------------------------
// djvImageIOFrameInfo
//------------------------------------------------------------------------------

djvImageIOFrameInfo::djvImageIOFrameInfo()
{}

djvImageIOFrameInfo::djvImageIOFrameInfo(
    qint64                  frame,
    int                     layer,
    djvPixelDataInfo::PROXY proxy) :
    frame     (frame),
    layer     (layer),
    proxy     (proxy)
{}

//------------------------------------------------------------------------------
// djvImageLoad
//------------------------------------------------------------------------------

djvImageLoad::djvImageLoad(djvCoreContext * context) :
    _context(context)
{}

djvImageLoad::~djvImageLoad()
{}

void djvImageLoad::close() throw (djvError)
{}

djvCoreContext * djvImageLoad::context() const
{
    return _context;
}

//------------------------------------------------------------------------------
// djvImageSave
//------------------------------------------------------------------------------

djvImageSave::djvImageSave(djvCoreContext * context) :
    _context(context)
{}

djvImageSave::~djvImageSave()
{}

void djvImageSave::close() throw (djvError)
{}

djvCoreContext * djvImageSave::context() const
{
    return _context;
}

//------------------------------------------------------------------------------
// djvImageIO
//------------------------------------------------------------------------------

djvImageIO::djvImageIO(djvCoreContext * context) :
    djvPlugin(context)
{}

djvImageIO::~djvImageIO()
{}

QStringList djvImageIO::extensions() const
{
    return QStringList();
}

bool djvImageIO::isSequence() const
{
    return true;
}

QStringList djvImageIO::option(const QString &) const
{
    return QStringList();
}

bool djvImageIO::setOption(const QString &, QStringList &)
{
    return false;
}

QStringList djvImageIO::options() const
{
    return QStringList();
}

void djvImageIO::commandLine(QStringList &) throw (QString)
{}

QString djvImageIO::commandLineHelp() const
{
    return QString();
}

djvImageLoad * djvImageIO::createLoad() const
{
    return 0;
}
    
djvImageSave * djvImageIO::createSave() const
{
    return 0;
}

const QStringList & djvImageIO::errorLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvImageIO", "Unrecognized file") <<
        qApp->translate("djvImageIO", "Unsupported file") <<
        qApp->translate("djvImageIO", "Cannot open file") <<
        qApp->translate("djvImageIO", "Error reading file") <<
        qApp->translate("djvImageIO", "Error writing file");    
    DJV_ASSERT(ERROR_COUNT == data.count());
    return data;
}

//------------------------------------------------------------------------------
// djvImageIOFactory::Private
//------------------------------------------------------------------------------

struct djvImageIOFactory::Private
{
    // This map is used to lookup an image I/O plugin by it's name.
    QMap<QString, djvImageIO *> nameMap;

    // This map is used to lookup an image I/O plugin for a given file
    // extension.
    QMap<QString, djvImageIO *> extensionMap;
};

//------------------------------------------------------------------------------
// djvImageIOFactory
//------------------------------------------------------------------------------

djvImageIOFactory::djvImageIOFactory(
    djvCoreContext *     context,
    const QStringList &  searchPath,
    QObject *            parent) :
    djvPluginFactory(context, searchPath, "djvImageIOEntry", "djv", "Plugin", parent),
    _p(new Private)
{
    //DJV_DEBUG("djvImageIOFactory::djvImageIOFactory");
    
    const QList<djvPlugin *> & plugins = this->plugins();
    Q_FOREACH(djvPlugin * plugin, plugins)
    {
        if (djvImageIO * imageIO = dynamic_cast<djvImageIO *>(plugin))
        {
            _addPlugin(imageIO);
        }
    }
}

djvImageIOFactory::~djvImageIOFactory()
{
    //DJV_DEBUG("djvImageIOFactory::~djvImageIOFactory");
}

QStringList djvImageIOFactory::option(
    const QString & name,
    const QString & option) const
{
    const QString nameLower = name.toLower();
    if (_p->nameMap.contains(nameLower))
    {
        //DJV_DEBUG("djvImageIOFactory::option");
        //DJV_DEBUG_PRINT("name   = " << name);
        //DJV_DEBUG_PRINT("option = " << option);
        djvImageIO * imageIO = _p->nameMap[nameLower];
        return imageIO->option(option);
    }
    return QStringList();
}

bool djvImageIOFactory::setOption(
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
        djvImageIO * imageIO = _p->nameMap[nameLower];
        return imageIO->setOption(option, data);
    }
    return false;
}

djvImageLoad * djvImageIOFactory::load(
    const djvFileInfo & fileInfo,
    djvImageIOInfo &    imageIOInfo) const throw (djvError)
{
    //DJV_DEBUG("djvImageIOFactory::load");
    //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
    //DJV_LOG("djvImageIOFactory", QString("Loading: \"%1\"...").arg(fileInfo));
    const QString extensionLower = fileInfo.extension().toLower();
    if (_p->extensionMap.contains(extensionLower))
    {
        djvImageIO * imageIO = _p->extensionMap[extensionLower];
        //DJV_LOG("djvImageIOFactory", QString("Using plugin: \"%1\"").arg(imageIO->pluginName()));
        if (djvImageLoad * imageLoad = imageIO->createLoad())
        {
            imageLoad->open(fileInfo, imageIOInfo);
            //QStringList tmp;
            //tmp << imageIOInfo.size;
            //DJV_LOG("djvImageIOFactory", QString("Size: %1").arg(tmp.join(", ")));
            //tmp.clear();
            //tmp << imageIOInfo.pixel;
            //DJV_LOG("djvImageIOFactory", QString("Pixel: %1").arg(tmp.join(", ")));
            return imageLoad;
        }
    }
    throw djvError(
        "djvImageIOFactory",
        qApp->translate("djvImageIOFactory", "Unrecognized image: %1").
        arg(QDir::toNativeSeparators(fileInfo)));
    return 0;
}
    
djvImageSave * djvImageIOFactory::save(
    const djvFileInfo &    fileInfo,
    const djvImageIOInfo & imageIOInfo) const throw (djvError)
{
    //DJV_DEBUG("djvImageIOFactory::save");
    //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
    //DJV_DEBUG_PRINT("imageIOInfo = " << imageIOInfp);
    //DJV_LOG("djvImageIOFactory", QString("Saving: \"%1\"").arg(fileInfo));
    //QStringList tmp;
    //tmp << imageIOInfo.size;
    //DJV_LOG("djvImageIOFactory", QString("Size: %1").arg(tmp.join(", ")));
    //tmp.clear();
    //tmp << imageIOInfo.pixel;
    //DJV_LOG("djvImageIOFactory", QString("Pixel: %1").arg(tmp.join(", ")));
    const QString extensionLower = fileInfo.extension().toLower();
    if (_p->extensionMap.contains(extensionLower))
    {
        djvImageIO * imageIO = _p->extensionMap[extensionLower];
        //DJV_LOG("djvImageIOFactory",
        //    QString("Using plugin: \"%1\"").arg(imageIO->pluginName()));
        if (djvImageSave * imageSave = imageIO->createSave())
        {
            imageSave->open(fileInfo, imageIOInfo);
            return imageSave;
        }
    }
    throw djvError(
        "djvImageIOFactory",
        qApp->translate("djvImageIOFactory", "Unrecognized image: %1").
        arg(QDir::toNativeSeparators(fileInfo)));
    return 0;
}

const QStringList & djvImageIOFactory::errorLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvImageIOFactory", "Unrecognized file: \"%1\"");    
    DJV_ASSERT(ERROR_COUNT == data.count());
    return data;
}

void djvImageIOFactory::addPlugin(djvPlugin * plugin)
{
    djvPluginFactory::addPlugin(plugin);
    if (djvImageIO * imageIO = dynamic_cast<djvImageIO *>(plugin))
    {
        _addPlugin(imageIO);
    }
}

void djvImageIOFactory::pluginOptionCallback(const QString &)
{
    Q_EMIT optionChanged();
}

void djvImageIOFactory::_addPlugin(djvImageIO * plugin)
{
    // Register file sequence extensions.
    if (plugin->isSequence())
    {
        Q_FOREACH(const QString & extension, plugin->extensions())
        {
            djvFileInfo::sequenceExtensions.insert(extension.toLower());
            djvFileInfo::sequenceExtensions.insert(extension.toUpper());
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

bool operator == (const djvImageIOInfo & a, const djvImageIOInfo & b)
{
    if (a.layerCount() != b.layerCount())
        return false;
    for (int i = 0; i < a.layerCount(); ++i)
        if (a[i] != b[i])
            return false;
    return
        operator == (
            static_cast<const djvPixelDataInfo &>(a),
            static_cast<const djvPixelDataInfo &>(b)) &&
        a.tags     == b.tags &&
        a.sequence == b.sequence;
}

bool operator != (const djvImageIOInfo & a, const djvImageIOInfo & b)
{
    return ! (a == b);
}

bool operator == (const djvImageIOFrameInfo & a, const djvImageIOFrameInfo & b)
{
    return
        a.frame == b.frame &&
        a.layer == b.layer &&
        a.proxy == b.proxy;
}

bool operator != (const djvImageIOFrameInfo & a, const djvImageIOFrameInfo & b)
{
    return ! (a == b);
}

djvDebug & operator << (djvDebug & debug, const djvImageIOInfo & in)
{
    return operator << (debug, static_cast<const djvPixelDataInfo &>(in));
}

djvDebug & operator << (djvDebug & debug, const djvImageIOFrameInfo & in)
{
    return debug << in.frame;
}
