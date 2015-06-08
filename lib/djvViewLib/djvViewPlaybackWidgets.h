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

//! \file djvViewPlaybackWidgets.h

#ifndef DJV_VIEW_PLAYBACK_WIDGETS_H
#define DJV_VIEW_PLAYBACK_WIDGETS_H

#include <djvViewUtil.h>

#include <QWidget>

class  djvViewContext;
struct djvViewFrameButtonsPrivate;
struct djvViewLoopWidgetPrivate;
struct djvViewPlaybackButtonsPrivate;

class djvGuiContext;

class QActionGroup;

//! \addtogroup djvViewPlayback
//@{

//------------------------------------------------------------------------------
//! \class djvViewPlaybackButtons
//!
//! This class provides a widget with playback control buttons.
//------------------------------------------------------------------------------

class djvViewPlaybackButtons : public QWidget
{
    Q_OBJECT

public:

    //! Constructor.

    explicit djvViewPlaybackButtons(
        QActionGroup *  actionGroup,
        djvGuiContext * context,
        QWidget *       parent      = 0);

    //! Destructor.

    virtual ~djvViewPlaybackButtons();

Q_SIGNALS:

    //! This signal is emitted when the shuttle is pressed.

    void shuttlePressed(bool);

    //! This signal is emitted when the shuttle value is changed.

    void shuttleChanged(int);

private:

    DJV_PRIVATE_COPY(djvViewPlaybackButtons);
    
    djvViewPlaybackButtonsPrivate * _p;
};

//------------------------------------------------------------------------------
//! \class djvViewLoopWidget
//!
//! This class provides a playback loop mode widget.
//------------------------------------------------------------------------------

class djvViewLoopWidget : public QWidget
{
    Q_OBJECT

public:

    //! Constructor.

    explicit djvViewLoopWidget(
        QActionGroup *   actionGroup,
        djvViewContext * context,
        QWidget *        parent      = 0);

    //! Destructor.

    virtual ~djvViewLoopWidget();

private Q_SLOTS:

    void widgetUpdate();

private:

    DJV_PRIVATE_COPY(djvViewLoopWidget);
    
    djvViewLoopWidgetPrivate * _p;
};

//------------------------------------------------------------------------------
//! \class djvViewFrameButtons
//!
//! This class provides a widget with frame control buttons.
//------------------------------------------------------------------------------

class djvViewFrameButtons : public QWidget
{
    Q_OBJECT

public:

    //! Constructor.

    explicit djvViewFrameButtons(
        QActionGroup *  actionGroup,
        djvGuiContext * context,
        QWidget *       parent      = 0);

    //! Destructor.

    virtual ~djvViewFrameButtons();

Q_SIGNALS:

    //! This signal is emitted when the shuttle is pressed.

    void shuttlePressed(bool);

    //! This signal is emitted when the shuttle value is changed.

    void shuttleChanged(int);

    //! This signal is emitted when the buttons are pressed.

    void pressed();

    //! This signal is emitted when the buttons are released.

    void released();

private:

    DJV_PRIVATE_COPY(djvViewFrameButtons);
    
    djvViewFrameButtonsPrivate * _p;
};

//@} // djvViewPlayback

#endif // DJV_VIEW_PLAYBACK_WIDGETS_H

