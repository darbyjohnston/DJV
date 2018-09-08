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

#pragma once

#include <djvImageTags.h>
#include <djvPixelData.h>

#include <djvPlugin.h>
#include <djvSequence.h>
#include <djvSystem.h>

#include <QMetaType>
#include <QObject>
#include <QStringList>

#include <memory>

class djvImage;

class djvCoreContext;
class djvFileInfo;

#if defined DJV_WINDOWS
#undef ERROR
#endif // DJV_WINDOWS

//! \addtogroup djvGraphicsImage
//@{

//------------------------------------------------------------------------------
//! \class djvImageIOInfo
//!
//! This class provides image I/O information.
//------------------------------------------------------------------------------

class djvImageIOInfo : public djvPixelDataInfo
{
public:
    djvImageIOInfo();
    djvImageIOInfo(const djvPixelDataInfo &);

    //! Add a layer.
    void addLayer(const djvPixelDataInfo &);

    //! Get the number of layers.
    int layerCount() const;

    //! Set the number of layers.
    void setLayerCount(int);

    //! Remove all the layers.
    void clearLayers();

    //! The image tags.
    djvImageTags tags;

    //! The frame sequence.
    djvSequence sequence;

    djvPixelDataInfo & operator [] (int);

    const djvPixelDataInfo & operator [] (int) const;

private:
    QVector<djvPixelDataInfo> _info;
};

//------------------------------------------------------------------------------
//! \class djvImageIOFrameInfo
//!
//! This struct provides image I/O frame information.
//------------------------------------------------------------------------------

struct djvImageIOFrameInfo
{
    djvImageIOFrameInfo();
    djvImageIOFrameInfo(
        qint64                  frame,
        int                     layer = 0,
        djvPixelDataInfo::PROXY proxy = djvPixelDataInfo::PROXY_NONE);

    //! The frame number.
    qint64 frame = -1;

    //! The image layer.
    int layer = 0;

    //! The proxy scale.
    djvPixelDataInfo::PROXY proxy = djvPixelDataInfo::PROXY_NONE;
};

//------------------------------------------------------------------------------
//! \class djvImageLoad
//!
//! This class provides the base functionality for image loading.
//!
//! \todo Add comments about ensuring loaders are thread safe (ie. don't use
//! OpenGL functionality, debug logging, etc.).
//------------------------------------------------------------------------------

class djvImageLoad
{
public:
    explicit djvImageLoad(djvCoreContext *);
        
    virtual ~djvImageLoad() = 0;

    //! Open an image.
    virtual void open(
        const djvFileInfo & fileInfo,
        djvImageIOInfo &    imageIOInfo)
        throw (djvError) = 0;

    //! Load an image.
    virtual void read(
        djvImage &                  image,
        const djvImageIOFrameInfo & frameInfo = djvImageIOFrameInfo())
        throw (djvError) = 0;
  
    //! Close the image.
    virtual void close() throw (djvError);
    
    //! Get the context.
    djvCoreContext * context() const;

private:
    djvCoreContext * _context = nullptr;
};

//------------------------------------------------------------------------------
//! \class djvImageSave
//!
//! This class provides the base functionality for image saving.
//------------------------------------------------------------------------------

class djvImageSave
{
public:
    explicit djvImageSave(djvCoreContext *);
    
    virtual ~djvImageSave() = 0;

    //! Open an image.
    virtual void open(
        const djvFileInfo &    fileInfo,
        const djvImageIOInfo & imageIOInfo)
        throw (djvError) = 0;

    //! Save an image.
    virtual void write(
        const djvImage &            image,
        const djvImageIOFrameInfo & frameInfo = djvImageIOFrameInfo())
        throw (djvError) = 0;

    //! Close the image.
    virtual void close() throw (djvError);
    
    //! Get the context.
    djvCoreContext * context() const;

private:
    djvCoreContext * _context = nullptr;
};

//------------------------------------------------------------------------------
//! \class djvImageIO
//!
//! This class provides the base functionality for image I/O plugins.
//------------------------------------------------------------------------------

class djvImageIO : public QObject, public djvPlugin
{
    Q_OBJECT
    
public:
    explicit djvImageIO(djvCoreContext *);
    
    virtual ~djvImageIO() = 0;
    
    //! Get the list of supported file extensions.
    virtual QStringList extensions() const;

    //! Does the plugin use file sequences?
    virtual bool isSequence() const;

    //! Get an option.
    virtual QStringList option(const QString &) const;

    //! Set an option.
    virtual bool setOption(const QString &, QStringList &);
    
    //! Get the list of options.
    virtual QStringList options() const;

    //! Parse the command line.
    virtual void commandLine(QStringList &) throw (QString);

    //! Get the command line help.
    virtual QString commandLineHelp() const;
    
    //! Get an image loader.
    virtual djvImageLoad * createLoad() const;
    
    //! Get an image saver.
    virtual djvImageSave * createSave() const;
    
    //! This enumeration provides error codes.
    enum ERROR
    {
        ERROR_UNRECOGNIZED,
        ERROR_UNSUPPORTED,
        ERROR_OPEN,
        ERROR_READ,
        ERROR_WRITE,
        
        ERROR_COUNT
    };
    
    //! Get the error code labels.
    static const QStringList & errorLabels();

Q_SIGNALS:
    //! This signal is emitted when an option is changed.
    void optionChanged(const QString &);
};

//------------------------------------------------------------------------------
//! \class djvImageIOFactory
//!
//! This class provides a factory for image I/O plugins.
//------------------------------------------------------------------------------

class djvImageIOFactory : public djvPluginFactory
{
    Q_OBJECT
    
public:

    explicit djvImageIOFactory(
        djvCoreContext *    context,
        const QStringList & searchPath = djvSystem::searchPath(),
        QObject *           parent     = nullptr);

    virtual ~djvImageIOFactory();

    //! Get a plugin option.
    QStringList option(const QString & name, const QString &) const;

    //! Set a plugin option.
    bool setOption(const QString & name, const QString &, QStringList &);
    
    //! Open an image for loading.
    djvImageLoad * load(
        const djvFileInfo & fileInfo,
        djvImageIOInfo &    imageIoInfo) const throw (djvError);
    
    //! Open an image for saving.
    djvImageSave * save(
        const djvFileInfo &    fileInfo,
        const djvImageIOInfo & imageIoInfo) const throw (djvError);

    //! This enumeration provides error codes.
    enum ERROR
    {
        ERROR_UNRECOGNIZED,
        
        ERROR_COUNT
    };
    
    //! Get the error code labels.
    static const QStringList & errorLabels();

    virtual void addPlugin(djvPlugin *);

Q_SIGNALS:
    //! This signal is emitted when a plugin option is changed.
    void optionChanged();
    
private Q_SLOTS:
    void pluginOptionCallback(const QString &);

private:
    DJV_PRIVATE_COPY(djvImageIOFactory);
    
    void _addPlugin(djvImageIO *);
    
    struct Private;
    std::unique_ptr<Private> _p;
};

Q_DECLARE_METATYPE(djvImageIOInfo)
Q_DECLARE_METATYPE(djvImageIOFrameInfo)

DJV_COMPARISON_OPERATOR(djvImageIOInfo);
DJV_COMPARISON_OPERATOR(djvImageIOFrameInfo);

DJV_DEBUG_OPERATOR(djvImageIOInfo);
DJV_DEBUG_OPERATOR(djvImageIOFrameInfo);

//@} // djvGraphicsImage

