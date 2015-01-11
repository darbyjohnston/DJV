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

//! \file djvImageIo.cpp

#include <djvImageIo.h>

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
// djvImageIoInfo
//------------------------------------------------------------------------------

djvImageIoInfo::djvImageIoInfo() :
    _info(1)
{}

djvImageIoInfo::djvImageIoInfo(const djvPixelDataInfo & in) :
    djvPixelDataInfo(in),
    _info(1)
{}

void djvImageIoInfo::addLayer(const djvPixelDataInfo & in)
{
    _info += in;
}

int djvImageIoInfo::layerCount() const
{
    return _info.count();
}

void djvImageIoInfo::setLayerCount(int count)
{
    _info.resize(count);
}

void djvImageIoInfo::clearLayers()
{
    _info.resize(1);
}

djvPixelDataInfo & djvImageIoInfo::operator [] (int index)
{
    return 0 == index ? static_cast<djvPixelDataInfo &>(*this) : _info[index];
}

const djvPixelDataInfo & djvImageIoInfo::operator [] (int index) const
{
    return 0 == index ? static_cast<const djvPixelDataInfo &>(*this) : _info[index];
}

//------------------------------------------------------------------------------
// djvImageIoFrameInfo
//------------------------------------------------------------------------------

djvImageIoFrameInfo::djvImageIoFrameInfo(
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

djvImageLoad::~djvImageLoad()
{}

void djvImageLoad::close() throw (djvError)
{}

//------------------------------------------------------------------------------
// djvImageSave
//------------------------------------------------------------------------------

djvImageSave::~djvImageSave()
{}

void djvImageSave::close() throw (djvError)
{}

//------------------------------------------------------------------------------
// djvImageIo
//------------------------------------------------------------------------------

djvImageIo::djvImageIo()
{}

djvImageIo::~djvImageIo()
{}

QStringList djvImageIo::extensions() const
{
    return QStringList();
}

bool djvImageIo::isSequence() const
{
    return true;
}

QStringList djvImageIo::option(const QString &) const
{
    return QStringList();
}

bool djvImageIo::setOption(const QString &, QStringList &)
{
    return false;
}

QStringList djvImageIo::options() const
{
    return QStringList();
}

void djvImageIo::commandLine(QStringList &) throw (QString)
{}

QString djvImageIo::commandLineHelp() const
{
    return QString();
}

djvImageLoad * djvImageIo::createLoad() const
{
    return 0;
}
    
djvImageSave * djvImageIo::createSave() const
{
    return 0;
}

djvAbstractPrefsWidget * djvImageIo::createWidget()
{
    return 0;
}

const QStringList & djvImageIo::errorLabels()
{
    static const QStringList data = QStringList() <<
        tr("Unrecognized file: \"%1\"") <<
        tr("Unsupported file: \"%1\"") <<
        tr("Cannot open file: \"%1\"") <<
        tr("Error reading file: \"%1\"") <<
        tr("Error writing file: \"%1\"");
    
    DJV_ASSERT(ERROR_COUNT == data.count());
    
    return data;
}

//------------------------------------------------------------------------------
// djvImageIoFactory::P
//------------------------------------------------------------------------------

struct djvImageIoFactory::P
{
    // This map is used to lookup an image I/O plugin by it's name.

    QMap<QString, djvImageIo *> nameMap;

    // This map is used to lookup an image I/O plugin for a given file
    // extension.

    QMap<QString, djvImageIo *> extensionMap;
};

//------------------------------------------------------------------------------
// djvImageIoFactory
//------------------------------------------------------------------------------

djvImageIoFactory::djvImageIoFactory(const QStringList & searchPath, QObject * parent) :
    djvPluginFactory(searchPath, "djvImageIo", "djv", "Plugin", parent),
    _p(new P)
{
    //DJV_DEBUG("djvImageIoFactory::djvImageIoFactory");

    // Setup maps.

    const QList<djvPlugin *> & plugins = this->plugins();

    Q_FOREACH(djvPlugin * plugin, plugins)
    {
        if (djvImageIo * imageIo = dynamic_cast<djvImageIo *>(plugin))
        {
            //DJV_DEBUG_PRINT("imageIo = " << imageIo->pluginName());
            //DJV_DEBUG_PRINT("extensions = " << imageIo->extensions());

            _p->nameMap[plugin->pluginName().toLower()] = imageIo;

            Q_FOREACH(const QString & extension, imageIo->extensions())
            {
                _p->extensionMap[extension.toLower()] = imageIo;
            }
        }
    }

    // Setup callbacks for image I/O plugins.

    for (int i = 0; i < plugins.count(); ++i)
    {
        if (djvImageIo * plugin = dynamic_cast<djvImageIo *>(plugins[i]))
        {
            // This callback listens to option changes in the image I/O
            // plugins.

            connect(
                plugin,
                SIGNAL(optionChanged(const QString &)),
                SLOT(pluginOptionCallback(const QString &)));
        }
    }
}

djvImageIoFactory::~djvImageIoFactory()
{
    //DJV_DEBUG("djvImageIoFactory::~djvImageIoFactory");

    delete _p;
}

QStringList djvImageIoFactory::option(
    const QString & name,
    const QString & option) const
{
    const QString nameLower = name.toLower();
    
    if (_p->nameMap.contains(nameLower))
    {
        //DJV_DEBUG("djvImageIoFactory::option");
        //DJV_DEBUG_PRINT("name   = " << name);
        //DJV_DEBUG_PRINT("option = " << option);

        djvImageIo * imageIo = _p->nameMap[nameLower];

        return imageIo->option(option);
    }

    return QStringList();
}

bool djvImageIoFactory::setOption(
    const QString & name,
    const QString & option,
    QStringList &   data)
{
    const QString nameLower = name.toLower();
    
    if (_p->nameMap.contains(nameLower))
    {
        //DJV_DEBUG("djvImageIoFactory::setOption");
        //DJV_DEBUG_PRINT("name   = " << name);
        //DJV_DEBUG_PRINT("option = " << option);
        //DJV_DEBUG_PRINT("data   = " << data);

        djvImageIo * imageIo = _p->nameMap[nameLower];

        return imageIo->setOption(option, data);
    }

    return false;
}

djvImageLoad * djvImageIoFactory::load(
    const djvFileInfo & fileInfo,
    djvImageIoInfo &    imageIoInfo) const throw (djvError)
{
    //DJV_DEBUG("djvImageIoFactory::load");
    //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
    
    //DJV_LOG("djvImageIoFactory", QString("Loading: \"%1\"...").arg(fileInfo));

    const QString extensionLower = fileInfo.extension().toLower();

    if (_p->extensionMap.contains(extensionLower))
    {
        djvImageIo * imageIo = _p->extensionMap[extensionLower];
        
        //DJV_LOG("djvImageIoFactory",
        //    QString("Using plugin: \"%1\"").arg(imageIo->pluginName()));

        if (djvImageLoad * imageLoad = imageIo->createLoad())
        {
            imageLoad->open(fileInfo, imageIoInfo);

            //QStringList tmp;
            //tmp << imageIoInfo.size;
            //DJV_LOG("djvImageIoFactory", QString("Size: %1").arg(tmp.join(", ")));
            //tmp.clear();
            //tmp << imageIoInfo.pixel;
            //DJV_LOG("djvImageIoFactory", QString("Pixel: %1").arg(tmp.join(", ")));

            return imageLoad;
        }
    }

    throw djvError(
        "djvImageIoFactory",
        errorLabels()[ERROR_UNRECOGNIZED].
        arg(QDir::toNativeSeparators(fileInfo)));

    return 0;
}
    
djvImageSave * djvImageIoFactory::save(
    const djvFileInfo &    fileInfo,
    const djvImageIoInfo & imageIoInfo) const throw (djvError)
{
    //DJV_DEBUG("djvImageIoFactory::save");
    //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
    //DJV_DEBUG_PRINT("imageIoInfo = " << imageIoInfp);

    //DJV_LOG("djvImageIoFactory", QString("Saving: \"%1\"").arg(fileInfo));
    //QStringList tmp;
    //tmp << imageIoInfo.size;
    //DJV_LOG("djvImageIoFactory", QString("Size: %1").arg(tmp.join(", ")));
    //tmp.clear();
    //tmp << imageIoInfo.pixel;
    //DJV_LOG("djvImageIoFactory", QString("Pixel: %1").arg(tmp.join(", ")));

    const QString extensionLower = fileInfo.extension().toLower();

    if (_p->extensionMap.contains(extensionLower))
    {
        djvImageIo * imageIo = _p->extensionMap[extensionLower];
        
        //DJV_LOG("djvImageIoFactory",
        //    QString("Using plugin: \"%1\"").arg(imageIo->pluginName()));

        if (djvImageSave * imageSave = imageIo->createSave())
        {
            imageSave->open(fileInfo, imageIoInfo);

            return imageSave;
        }
    }

    throw djvError(
        "djvImageIoFactory",
        errorLabels()[ERROR_UNRECOGNIZED].
        arg(QDir::toNativeSeparators(fileInfo)));
    
    return 0;
}

djvImageIoFactory * djvImageIoFactory::global()
{
    static QPointer<djvImageIoFactory> factory;

    if (! factory.data())
    {
        //! \todo Parenting the image I/O factory to the application is
        //! causing issues when the application exits on Windows.

        //factory = new djvImageIoFactory(djvSystem::searchPath(), qApp);
        factory = new djvImageIoFactory;
    }

    return factory.data();
}

const QStringList & djvImageIoFactory::errorLabels()
{
    static const QStringList data = QStringList() <<
        tr("Unrecognized file: \"%1\"");
    
    DJV_ASSERT(ERROR_COUNT == data.count());
    
    return data;
}

void djvImageIoFactory::pluginOptionCallback(const QString &)
{
    Q_EMIT optionChanged();
}

//------------------------------------------------------------------------------

bool operator == (const djvImageIoInfo & a, const djvImageIoInfo & b)
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

bool operator != (const djvImageIoInfo & a, const djvImageIoInfo & b)
{
    return ! (a == b);
}

bool operator == (const djvImageIoFrameInfo & a, const djvImageIoFrameInfo & b)
{
    return
        a.frame == b.frame &&
        a.layer == b.layer &&
        a.proxy == b.proxy;
}

bool operator != (const djvImageIoFrameInfo & a, const djvImageIoFrameInfo & b)
{
    return ! (a == b);
}

djvDebug & operator << (djvDebug & debug, const djvImageIoInfo & in)
{
    return operator << (debug, static_cast<const djvPixelDataInfo &>(in));
}

djvDebug & operator << (djvDebug & debug, const djvImageIoFrameInfo & in)
{
    return debug << in.frame;
}
