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

//! \file djvViewWindowGroup.h

#ifndef DJV_VIEW_WINDOW_GROUP_H
#define DJV_VIEW_WINDOW_GROUP_H

#include <djvViewAbstractGroup.h>

struct djvViewWindowGroupPrivate;

class QAction;

//! \addtogroup djvViewWindow
//@{

//------------------------------------------------------------------------------
//! \class djvViewWindowGroup
//!
//! This class provides the window group. The window group encapsulates all
//! of the functionality relating to main windows such as whether the window
//! is full screen, which tool bars are visible, etc.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewWindowGroup : public djvViewAbstractGroup
{
    Q_OBJECT
    
public:

    //! Constructor.

    djvViewWindowGroup(
        const djvViewWindowGroup * copy,
        djvViewMainWindow *        mainWindow,
        djvViewContext *           context);

    //! Destructor.

    virtual ~djvViewWindowGroup();
    
    //! Get whether full screen is enabled.
    
    bool hasFullScreen() const;

    //! Get the user interface controls visibility.

    bool hasControlsVisible() const;

    //! Get the tool bar visibility.

    const QVector<bool> & toolBarVisible() const;

    virtual QToolBar * toolBar() const;

public Q_SLOTS:

    //! Set whether full screen is enabled.

    void setFullScreen(bool);

    //! Set the user interface controls visibility.

    void setControlsVisible(bool);

    //! Set the tool bar visibility.

    void setToolBarVisible(const QVector<bool> &);

Q_SIGNALS:

    //! This signal is emitted when full screen is changed.

    void fullScreenChanged(bool);

    //! This signal is emitted when the user interface control visibility
    //! is changed.

    void controlsVisibleChanged(bool);
    
    //! This signal is emitted when the tool bar visibility is changed.
    
    void toolBarVisibleChanged(const QVector<bool> &);

private Q_SLOTS:

    void newCallback();
    void copyCallback();
    void closeCallback();
    void fitCallback();
    void toolBarVisibleCallback(QAction *);

    void update();

private:

    DJV_PRIVATE_COPY(djvViewWindowGroup);
    
    djvViewWindowGroupPrivate * _p;
};

//@} // djvViewWindow

#endif // DJV_VIEW_WINDOW_GROUP_H

