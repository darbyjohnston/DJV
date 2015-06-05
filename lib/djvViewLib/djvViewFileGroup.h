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

//! \file djvViewFileGroup.h

#ifndef DJV_VIEW_FILE_GROUP_H
#define DJV_VIEW_FILE_GROUP_H

#include <djvViewAbstractGroup.h>

#include <djvImageIo.h>
#include <djvPixel.h>

class  djvViewFileCacheRef;
struct djvViewFileGroupPrivate;

class QAction;

//! \addtogroup djvViewFile
//@{

//------------------------------------------------------------------------------
//! \class djvViewFileGroup
//!
//! This class provides the file group. The file group encapsulates all
//! of thefunctionality relating to files such as the currently opened file,
//! options used when loading files, etc.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewFileGroup : public djvViewAbstractGroup
{
    Q_OBJECT
    
public:

    //! Constructor.

    djvViewFileGroup(
        const djvViewFileGroup * copy,
        djvViewMainWindow *      mainWindow,
        djvViewContext *         context);

    //! Destructor.

    virtual ~djvViewFileGroup();

    //! Get the currently opened file.

    const djvFileInfo & fileInfo() const;

    //! Get the layer to load.

    int layer() const;

    //! Get the proxy scale.

    djvPixelDataInfo::PROXY proxy() const;
    
    //! Get whther images are converted to 8-bits.
    
    bool hasU8Conversion() const;

    //! Get whether the cache is enabled.

    bool hasCache() const;

    //! Get whether the cache pre-load is enabled.

    bool hasPreload() const;

    //! Get whether the cache pre-load is active.

    bool isPreloadActive() const;

    //! Get the cache pre-load frame.

    qint64 preloadFrame() const;

    //! Get an image.

    const djvImage * image(qint64 frame) const;

    //! Get image I/O information.

    const djvImageIoInfo & imageIoInfo() const;

    virtual QToolBar * toolBar() const;

public Q_SLOTS:

    //! Open a file.

    void open(const djvFileInfo &);

    //! Set the layer to load.

    void setLayer(int);

    //! Set the proxy scale.

    void setProxy(djvPixelDataInfo::PROXY);

    //! Set whether images are converted to 8-bits.
    
    void setU8Conversion(bool);
    
    //! Set whether the cache is enabled.

    void setCache(bool);

    //! Set whether the cache pre-load is enabled.

    void setPreload(bool);

    //! Set whether the cache pre-load is active.

    void setPreloadActive(bool);

    //! Set the cache pre-load frame.

    void setPreloadFrame(qint64);

Q_SIGNALS:

    //! This signal is emitted when the current image is changed.

    void imageChanged();

    //! This signal is emitted to store the current frame.

    void loadFrameStore();
    
    //! This signal is emitted to reload the frame.
    
    void reloadFrame();

    //! This signal is emitted to save a sequence.

    void save(const djvFileInfo &);

    //! This signal is emitted to save a frame.

    void saveFrame(const djvFileInfo &);

protected:

    virtual void timerEvent(QTimerEvent *);

private Q_SLOTS:

    void openCallback();
    void openCallback(const djvFileInfo &);
    void recentCallback(QAction *);
    void reloadCallback();
    void reloadFrameCallback();
    void closeCallback();
    void saveCallback();
    void saveFrameCallback();
    void layerCallback(QAction *);
    void layerPrevCallback();
    void layerNextCallback();
    void proxyCallback(QAction *);
    void cacheClearCallback();
    void messagesCallback();
    void prefsCallback();
    void debugLogCallback();

    void preloadUpdate();
    void update();

private:

    void cacheDel();

    DJV_PRIVATE_COPY(djvViewFileGroup);
    
    djvViewFileGroupPrivate * _p;
};

//@} // djvViewFile

#endif // DJV_VIEW_FILE_GROUP_H

