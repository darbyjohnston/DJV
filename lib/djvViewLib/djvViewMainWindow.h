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

//! \file djvViewMainWindow.h

#ifndef DJV_VIEW_MAIN_WINDOW_H
#define DJV_VIEW_MAIN_WINDOW_H

#include <djvViewUtil.h>

#include <djvImage.h>

#include <QMainWindow>

class  djvViewContext;
class  djvViewImageView;
struct djvViewMainWindowPrivate;

class djvImageIoInfo;
class djvOpenGlImageOptions;
class djvSpeed;

//! \addtogroup djvViewWindow
//@{

//------------------------------------------------------------------------------
//! \class djvViewMainWindow
//!
//! This class provides a main window.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:

    //! Constructor.

    explicit djvViewMainWindow(
        const djvViewMainWindow * copy,
        djvViewContext *          context);

    //! Destructor.

    virtual ~djvViewMainWindow();

    //! Get the image I/O information.

    const djvImageIoInfo & imageIoInfo() const;
    
    //! Get the view widget.
    
    djvViewImageView * viewWidget() const;
    
    //! Get the list of main windows.
    
    static QVector<djvViewMainWindow *> mainWindowList();

    //! Create a new main window.
    //!
    //! \todo Can we remove this function?
    
    static djvViewMainWindow * createWindow(djvViewContext *);

public Q_SLOTS:

    //! Open a file.

    void fileOpen(const djvFileInfo &, bool init = true);

    //! Set the file layer to open.

    void setFileLayer(int);

    //! Set the file proxy scale.

    void setFileProxy(djvPixelDataInfo::PROXY);

    //! Set whether the file cache is enabled.

    void setFileCache(bool);

    //! Fit the window to the image.

    void fitWindow(bool move = true);

    //! Set the playback.

    void setPlayback(djvViewUtil::PLAYBACK);

    //! Set the playback frame.

    void setPlaybackFrame(qint64);

    //! Set the playback speed.

    void setPlaybackSpeed(const djvSpeed &);

Q_SIGNALS:

    //! This signal is emitted when the image is changed.

    void imageChanged();

protected:

    virtual void showEvent(QShowEvent *);
    virtual void closeEvent(QCloseEvent *);
    virtual void keyPressEvent(QKeyEvent *);
    
private Q_SLOTS:

    void windowResizeCallback();
    void enableUpdatesCallback();
    void reloadFrameCallback();
    void saveCallback(const djvFileInfo &);
    void saveFrameCallback(const djvFileInfo &);
    void loadFrameStoreCallback();
    void pickCallback(const djvVector2i &);
    void mouseWheelCallback(djvViewUtil::MOUSE_WHEEL);
    void mouseWheelValueCallback(int);
    
    void fileUpdate();
    void fileCacheUpdate();
    void imageUpdate();
    void controlsUpdate();
    void viewOverlayUpdate();
    void viewPickUpdate();
    void playbackUpdate();
    
private:

    //! Get the current image.

    const djvImage * image() const;

    //! Get the image drawing options.

    djvOpenGlImageOptions imageOptions() const;

    DJV_PRIVATE_COPY(djvViewMainWindow);
    
    djvViewMainWindowPrivate * _p;
};

//@} // djvViewWindow

#endif // DJV_VIEW_MAIN_WINDOW_H

