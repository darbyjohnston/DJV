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

//! \file djvViewPlaybackGroup.h

#ifndef DJV_VIEW_PLAYBACK_GROUP_H
#define DJV_VIEW_PLAYBACK_GROUP_H

#include <djvViewAbstractGroup.h>
#include <djvViewUtil.h>

struct djvViewPlaybackGroupPrivate;

class djvSequence;
class djvSpeed;

class QAction;

//! \addtogroup djvViewPlayback
//@{

//------------------------------------------------------------------------------
//! \class djvViewPlaybackGroup
//!
//! This class provides the playback group. The playback group encapsulates all
//! of the functionality relating to movie playback such as the current frame,
//! whether the movie is being played or stopped, etc.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewPlaybackGroup : public djvViewAbstractGroup
{
    Q_OBJECT
    
public:

    //! Constructor.

    djvViewPlaybackGroup(
        djvViewPlaybackGroup * copy,
        djvViewMainWindow *    mainWindow,
        djvViewContext *       context);

    //! Destructor.

    virtual ~djvViewPlaybackGroup();

    //! Get the sequence.

    const djvSequence & sequence() const;

    //! Get the playback.

    djvViewUtil::PLAYBACK playback() const;

    //! Get the loop mode.

    djvViewUtil::LOOP loop() const;

    //! Get the speed.

    const djvSpeed & speed() const;

    //! Get the real playback speed.

    double realSpeed() const;

    //! Get whether frames were dropped.

    bool hasDroppedFrames() const;

    //! Get whether every frame should be played back.

    bool hasEveryFrame() const;

    //! Get the current frame.

    qint64 frame() const;

    //! Get whether in/out points are enabled.

    bool isInOutEnabled() const;

    //! Get the in point.

    qint64 inPoint() const;

    //! Get the out point.

    qint64 outPoint() const;

    //! Get the layout.

    djvViewUtil::LAYOUT layout() const;

    virtual QToolBar * toolBar() const;

public Q_SLOTS:

    //! Set the sequence.

    void setSequence(const djvSequence &);

    //! Set the playback.

    void setPlayback(djvViewUtil::PLAYBACK);

    //! Toggle the platback.
    
    void togglePlayback();

    //! Set the loop mode.

    void setLoop(djvViewUtil::LOOP);

    //! Set the speed.

    void setSpeed(const djvSpeed &);

    //! Set whether every frame is played back.

    void setEveryFrame(bool);

    //! Set the frame.

    void setFrame(qint64, bool inOutPoints = true);

    //! Set whether the in/out points are enabled.

    void setInOutEnabled(bool);

    //! Set the in point.

    void setInPoint(qint64);

    //! Set the out point.

    void setOutPoint(qint64);

    //! Set the layout.

    void setLayout(djvViewUtil::LAYOUT);

Q_SIGNALS:

    //! This signal is emitted when the sequence is changed.

    void sequenceChanged(const djvSequence &);

    //! This signal is emitted when the playback is changed.

    void playbackChanged(djvViewUtil::PLAYBACK);

    //! This signal is emitted when the loop mode is changed.

    void loopChanged(djvViewUtil::LOOP);

    //! This signal is emitted when the speed is changed.

    void speedChanged(const djvSpeed &);

    //! This signal is emitted when the real speed is changed.

    void realSpeedChanged(double);
    
    //! This signal is emitted when dropped frames are changed.

    void droppedFramesChanged(bool);

    //! This signal is emitted when every frame playback is changed.

    void everyFrameChanged(bool);

    //! This signal is emitted when the frame is changed.

    void frameChanged(qint64);

    //! This signal is emitted when the in/out points are enabled or disabled.

    void inOutEnabledChanged(bool);

    //! This signal is emitted when the in point is changed.

    void inPointChanged(qint64);

    //! This signal is emitted when the out point is changed.

    void outPointChanged(qint64);

    //! This signal is emitted when the layout is changed.

    void layoutChanged(djvViewUtil::LAYOUT);

protected:

    virtual void timerEvent(QTimerEvent *);

private Q_SLOTS:

    void playbackCallback(QAction *);
    void playbackShuttleCallback(bool);
    void playbackShuttleValueCallback(int);
    void loopCallback(QAction *);
    void frameCallback(QAction *);
    void frameCallback(qint64);
    void frameStopCallback(qint64);
    void frameSliderCallback(qint64);
    void frameShuttleValueCallback(int);
    void frameShuttleCallback(bool);
    void framePressedCallback(bool);
    void framePressedCallback();
    void frameReleasedCallback();
    void inOutCallback(QAction *);
    void layoutCallback(QAction *);
    void cacheCallback();

private:

    qint64 frameStart() const;
    qint64 frameEnd() const;

    void playbackUpdate();
    void timeUpdate();
    void frameUpdate();
    void speedUpdate();
    void layoutUpdate();

    DJV_PRIVATE_COPY(djvViewPlaybackGroup);
    
    djvViewPlaybackGroupPrivate * _p;
};

//@} // djvViewPlayback

#endif // DJV_VIEW_PLAYBACK_GROUP_H

