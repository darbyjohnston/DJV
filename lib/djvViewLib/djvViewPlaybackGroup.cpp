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

//! \file djvViewPlaybackGroup.cpp

#include <djvViewPlaybackGroup.h>

#include <djvViewFileCache.h>
#include <djvViewMainWindow.h>
#include <djvViewPlaybackActions.h>
#include <djvViewPlaybackMenu.h>
#include <djvViewPlaybackPrefs.h>
#include <djvViewPlaybackToolBar.h>
#include <djvViewShortcutPrefs.h>
#include <djvToolButton.h>

#include <djvListUtil.h>
#include <djvSignalBlocker.h>
#include <djvTimer.h>

#include <QAction>
#include <QActionGroup>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QToolBar>

namespace
{

qint64 sequenceEnd(const djvSequence & sequence)
{
    return sequence.frames.count() ? sequence.frames.count() - 1 : 0;
}

} // namespace

//------------------------------------------------------------------------------
// djvViewPlaybackGroup::P
//------------------------------------------------------------------------------

struct djvViewPlaybackGroup::P
{
    P() :
        playback        (djvViewUtil::STOP),
        playbackPrev    (djvViewUtil::STOP),
        loop            (djvViewUtil::LOOP_REPEAT),
        realSpeed       (0.0),
        droppedFrames   (false),
        droppedFramesTmp(false),
        everyFrame      (djvViewPlaybackPrefs::global()->hasEveryFrame()),
        frame           (0),
        frameTmp        (0),
        shuttle         (false),
        shuttleSpeed    (0.0),
        inOutEnabled    (true),
        inPoint         (0),
        outPoint        (0),
        timer           (0),
        idlePause       (false),
        idleInit        (true),
        idleFrame       (0),
        speedCounter    (0),
        layout          (djvViewPlaybackPrefs::global()->layout()),
        actions         (0),
        menu            (0),
        toolBar         (0)
    {}
    
    djvSequence              sequence;
    djvViewUtil::PLAYBACK    playback;
    djvViewUtil::PLAYBACK    playbackPrev;
    djvViewUtil::LOOP        loop;
    djvSpeed                 speed;
    double                   realSpeed;
    bool                     droppedFrames;
    bool                     droppedFramesTmp;
    bool                     everyFrame;
    qint64                   frame;
    qint64                   frameTmp;
    bool                     shuttle;
    double                   shuttleSpeed;
    bool                     inOutEnabled;
    qint64                   inPoint;
    qint64                   outPoint;
    int                      timer;
    bool                     idlePause;
    bool                     idleInit;
    djvTimer                 idleTimer;
    quint64                  idleFrame;
    djvTimer                 speedTimer;
    quint64                  speedCounter;
    djvViewUtil::LAYOUT      layout;
    djvViewPlaybackActions * actions;
    djvViewPlaybackMenu *    menu;
    djvViewPlaybackToolBar * toolBar;
};

//------------------------------------------------------------------------------
// djvViewPlaybackGroup
//------------------------------------------------------------------------------

djvViewPlaybackGroup::djvViewPlaybackGroup(
    djvViewMainWindow *    mainWindow,
    djvViewPlaybackGroup * copy) :
    djvViewAbstractGroup(mainWindow),
    _p(new P)
{
    // Create the actions.

    _p->actions = new djvViewPlaybackActions(this);

    // Create the menus.

    _p->menu = new djvViewPlaybackMenu(_p->actions, mainWindow->menuBar());

    mainWindow->menuBar()->addMenu(_p->menu);
    
    // Create the widgets.

    _p->toolBar = new djvViewPlaybackToolBar(_p->actions);

    mainWindow->addToolBar(Qt::BottomToolBarArea, _p->toolBar);

    // Initialize.

    if (copy)
    {
        _p->sequence     = copy->_p->sequence;
        _p->playback     = copy->_p->playback;
        _p->playbackPrev = copy->_p->playbackPrev;
        _p->loop         = copy->_p->loop;
        _p->speed        = copy->_p->speed;
        _p->everyFrame   = copy->_p->everyFrame;
        _p->frame        = copy->_p->frame;
        _p->inOutEnabled = copy->_p->inOutEnabled;
        _p->inPoint      = copy->_p->inPoint;
        _p->outPoint     = copy->_p->outPoint;
        _p->layout       = copy->_p->layout;
    }

    playbackUpdate();
    timeUpdate();
    speedUpdate();
    frameUpdate();
    layoutUpdate();

    // Setup the action callbacks.

    connect(
        _p->actions->action(djvViewPlaybackActions::PLAYBACK_TOGGLE),
        SIGNAL(triggered()),
        SLOT(togglePlayback()));

    connect(
        _p->actions->action(djvViewPlaybackActions::EVERY_FRAME),
        SIGNAL(toggled(bool)),
        SLOT(setEveryFrame(bool)));

    // Setup the action group callbacks.

    connect(
        _p->actions->group(djvViewPlaybackActions::PLAYBACK_GROUP),
        SIGNAL(triggered(QAction *)),
        SLOT(playbackCallback(QAction *)));

    connect(
        _p->actions->group(djvViewPlaybackActions::LOOP_GROUP),
        SIGNAL(triggered(QAction *)),
        SLOT(loopCallback(QAction *)));

    connect(
        _p->actions->group(djvViewPlaybackActions::FRAME_GROUP),
        SIGNAL(triggered(QAction *)),
        SLOT(frameCallback(QAction *)));

    connect(
        _p->actions->group(djvViewPlaybackActions::IN_OUT_GROUP),
        SIGNAL(triggered(QAction *)),
        SLOT(inOutCallback(QAction *)));

    connect(
        _p->actions->group(djvViewPlaybackActions::LAYOUT_GROUP),
        SIGNAL(triggered(QAction *)),
        SLOT(layoutCallback(QAction *)));

    // Setup the tool bar callbacks.

    connect(
        _p->toolBar,
        SIGNAL(playbackShuttlePressed(bool)),
        SLOT(playbackShuttleCallback(bool)));
    
    connect(
        _p->toolBar,
        SIGNAL(playbackShuttleValue(int)),
        SLOT(playbackShuttleValueCallback(int)));

    connect(
        _p->toolBar,
        SIGNAL(speedChanged(const djvSpeed &)),
        SLOT(setSpeed(const djvSpeed &)));
    
    connect(
        _p->toolBar,
        SIGNAL(frameChanged(qint64)),
        SLOT(frameStopCallback(qint64)));
    
    connect(
        _p->toolBar,
        SIGNAL(frameShuttlePressed(bool)),
        SLOT(frameShuttleCallback(bool)));

    connect(
        _p->toolBar,
        SIGNAL(frameShuttleValue(int)),
        SLOT(frameShuttleValueCallback(int)));

    connect(
        _p->toolBar,
        SIGNAL(frameSliderPressed(bool)),
        SLOT(framePressedCallback(bool)));

    connect(
        _p->toolBar,
        SIGNAL(frameSliderChanged(qint64)),
        SLOT(frameSliderCallback(qint64)));

    connect(
        _p->toolBar,
        SIGNAL(frameButtonsPressed()),
        SLOT(framePressedCallback()));

    connect(
        _p->toolBar,
        SIGNAL(frameButtonsReleased()),
        SLOT(frameReleasedCallback()));

    connect(
        _p->toolBar,
        SIGNAL(inPointChanged(qint64)),
        SLOT(setInPoint(qint64)));

    connect(
        _p->toolBar,
        SIGNAL(outPointChanged(qint64)),
        SLOT(setOutPoint(qint64)));

    // Setup the preferences callbacks.

    connect(
        djvViewPlaybackPrefs::global(),
        SIGNAL(everyFrameChanged(bool)),
        SLOT(setEveryFrame(bool)));
    
    connect(
        djvViewPlaybackPrefs::global(),
        SIGNAL(layoutChanged(djvViewUtil::LAYOUT)),
        SLOT(setLayout(djvViewUtil::LAYOUT)));

    // Setup other callbacks.

    connect(
        djvViewFileCache::global(),
        SIGNAL(cacheChanged()),
        SLOT(cacheCallback()));
}

djvViewPlaybackGroup::~djvViewPlaybackGroup()
{
    delete _p;
}

const djvSequence & djvViewPlaybackGroup::sequence() const
{
    return _p->sequence;
}

djvViewUtil::PLAYBACK djvViewPlaybackGroup::playback() const
{
    return _p->playback;
}

djvViewUtil::LOOP djvViewPlaybackGroup::loop() const
{
    return _p->loop;
}

const djvSpeed & djvViewPlaybackGroup::speed() const
{
    return _p->speed;
}

double djvViewPlaybackGroup::realSpeed() const
{
    return _p->realSpeed;
}

bool djvViewPlaybackGroup::hasDroppedFrames() const
{
    return _p->droppedFrames;
}

bool djvViewPlaybackGroup::hasEveryFrame() const
{
    return _p->everyFrame;
}

qint64 djvViewPlaybackGroup::frame() const
{
    return _p->frame;
}

bool djvViewPlaybackGroup::isInOutEnabled() const
{
    return _p->inOutEnabled;
}

qint64 djvViewPlaybackGroup::inPoint() const
{
    return _p->inPoint;
}

qint64 djvViewPlaybackGroup::outPoint() const
{
    return _p->outPoint;
}

djvViewUtil::LAYOUT djvViewPlaybackGroup::layout() const
{
    return _p->layout;
}

QToolBar * djvViewPlaybackGroup::toolBar() const
{
    return _p->toolBar;
}

void djvViewPlaybackGroup::setSequence(const djvSequence & sequence)
{
    if (sequence == _p->sequence)
        return;

    //DJV_DEBUG("djvViewPlaybackGroup::setSequence");
    //DJV_DEBUG_PRINT("sequence = " << sequence);

    _p->sequence      = sequence;
    _p->speed         = sequence.speed;
    _p->realSpeed     = 0.0;
    _p->droppedFrames = false;

    _p->inPoint  = 0;
    _p->outPoint = sequenceEnd(_p->sequence);

    timeUpdate();
    speedUpdate();

    Q_EMIT sequenceChanged(_p->sequence);
    Q_EMIT speedChanged(_p->speed);
    Q_EMIT realSpeedChanged(_p->realSpeed);
    Q_EMIT droppedFramesChanged(_p->droppedFrames);
}

void djvViewPlaybackGroup::setPlayback(djvViewUtil::PLAYBACK playback)
{
    if (playback == _p->playback)
        return;

    //DJV_DEBUG("djvViewPlaybackGroup::setPlayback");
    //DJV_DEBUG_PRINT("playback = " << playback);

    _p->playbackPrev = _p->playback;

    _p->playback = playback;

    playbackUpdate();

    Q_EMIT playbackChanged(_p->playback);
}

void djvViewPlaybackGroup::togglePlayback()
{
    switch (_p->playback)
    {
        case djvViewUtil::FORWARD:
        case djvViewUtil::REVERSE: setPlayback(djvViewUtil::STOP); break;
            
        default: setPlayback(_p->playbackPrev); break;
    }
}

void djvViewPlaybackGroup::setLoop(djvViewUtil::LOOP loop)
{
    if (loop == _p->loop)
        return;

    _p->loop = loop;

    //DJV_DEBUG("djvViewPlaybackGroup::setLoop");
    //DJV_DEBUG_PRINT("loop = " << loop);

    playbackUpdate();

    Q_EMIT loopChanged(_p->loop);
}

void djvViewPlaybackGroup::setSpeed(const djvSpeed & in)
{
    if (in == _p->speed)
        return;

    //DJV_DEBUG("djvViewPlaybackGroup::speed");
    //DJV_DEBUG_PRINT("in = " << in);

    _p->speed         = in;
    _p->realSpeed     = 0.0;
    _p->droppedFrames = false;

    playbackUpdate();
    speedUpdate();

    Q_EMIT speedChanged(_p->speed);
    Q_EMIT realSpeedChanged(_p->realSpeed);
    Q_EMIT droppedFramesChanged(_p->droppedFrames);
}

void djvViewPlaybackGroup::setEveryFrame(bool in)
{
    if (in == _p->everyFrame)
        return;

    _p->everyFrame = in;

    speedUpdate();

    Q_EMIT everyFrameChanged(_p->everyFrame);
}

void djvViewPlaybackGroup::setFrame(qint64 in, bool inOutEnabled)
{
    const qint64 frameStart = inOutEnabled ? this->frameStart() : 0;
    const qint64 frameEnd   = inOutEnabled ? this->frameEnd() : sequenceEnd(_p->sequence);

    switch (_p->loop)
    {
        case djvViewUtil::LOOP_ONCE:

            if (in <= frameStart || in >= frameEnd)
            {
                setPlayback(djvViewUtil::STOP);
            }

            in = djvMath::clamp(in, frameStart, frameEnd);

            break;

        case djvViewUtil::LOOP_REPEAT:

            in = djvMath::wrap(in, frameStart, frameEnd);

            break;

        case djvViewUtil::LOOP_PING_PONG:

            if (_p->playback != djvViewUtil::STOP)
            {
                if (in <= frameStart)
                {
                    setPlayback(djvViewUtil::FORWARD);
                }
                else if (in >= frameEnd)
                {
                    setPlayback(djvViewUtil::REVERSE);
                }
            }

            in = djvMath::clamp(in, frameStart, frameEnd);

            break;

        default: break;
    }

    if (in == _p->frame)
        return;

    //DJV_DEBUG("djvViewPlaybackGroup::setFrame");
    //DJV_DEBUG_PRINT("in = " << in);

    _p->frame = in;

    frameUpdate();

    Q_EMIT frameChanged(_p->frame);
}

void djvViewPlaybackGroup::setInOutEnabled(bool in)
{
    if (in == _p->inOutEnabled)
        return;

    _p->inOutEnabled = in;

    timeUpdate();
    layoutUpdate();

    Q_EMIT inOutEnabledChanged(_p->inOutEnabled);
}

void djvViewPlaybackGroup::setInPoint(qint64 in)
{
    if (in == _p->inPoint)
        return;

    _p->inPoint = in;

    timeUpdate();

    Q_EMIT inPointChanged(_p->inPoint);
}

void djvViewPlaybackGroup::setOutPoint(qint64 in)
{
    if (in == _p->outPoint)
        return;

    _p->outPoint = in;

    timeUpdate();

    Q_EMIT outPointChanged(_p->outPoint);
}

void djvViewPlaybackGroup::setLayout(djvViewUtil::LAYOUT in)
{
    if (in == _p->layout)
        return;

    //DJV_DEBUG("djvViewPlaybackGroup::setLayout");
    //DJV_DEBUG_PRINT("in = " << in);

    _p->layout = in;

    layoutUpdate();

    Q_EMIT layoutChanged(_p->layout);
}

void djvViewPlaybackGroup::timerEvent(QTimerEvent *)
{
    if (_p->idlePause)
        return;

    //DJV_DEBUG("djvViewPlaybackGroup::timerEvent");
    //DJV_DEBUG_PRINT("playback = " << _p->playback);
    //DJV_DEBUG_PRINT("loop = " << _p->loop);
    //DJV_DEBUG_PRINT("every frame = " << _p->everyFrame);
    //DJV_DEBUG_PRINT("frame = " << _p->frame);

    if (_p->idleInit)
    {
        //DJV_DEBUG_PRINT("init");

        _p->idleTimer.start();

        _p->idleFrame = 0;
        _p->speedTimer = _p->idleTimer;
        _p->speedCounter = 0;
        _p->droppedFrames = false;
        _p->droppedFramesTmp = false;
        _p->idleInit = false;
    }
    else
    {
        _p->idleTimer.check();
    }

    const double speed =
        _p->shuttle ? _p->shuttleSpeed : djvSpeed::speedToFloat(_p->speed);

    //DJV_DEBUG_PRINT("speed = " << speed);

    const quint64 absoluteFrame = quint64(_p->idleTimer.seconds() * speed);

    int inc = static_cast<int>(absoluteFrame - _p->idleFrame);

    //DJV_DEBUG_PRINT("absolute frame = " << static_cast<int>(absoluteFrame));
    //DJV_DEBUG_PRINT("inc = " << inc);

    if (! inc)
        return;

    // Calculate current frame.

    _p->idleFrame = absoluteFrame;

    if (djvViewUtil::REVERSE == _p->playback)
    {
        inc = -inc;
    }

    const bool everyFrame = _p->everyFrame && ! _p->shuttle;

    if (everyFrame)
    {
        inc = inc > 0 ? 1 : (inc < 0 ? -1 : 0);
    }
    else
    {
        _p->droppedFramesTmp |= inc > 1 || inc < -1;
    }

    _p->speedCounter += djvMath::abs(inc);

    // Calculate real playback speed.

    _p->speedTimer.check();

    if (_p->speedTimer.seconds() >= 1.0)
    {
        _p->realSpeed = _p->speedCounter / _p->speedTimer.seconds();
        _p->speedTimer.start();

        _p->droppedFrames = _p->droppedFramesTmp;
        _p->droppedFramesTmp = false;
        _p->speedCounter = 0;

        _p->toolBar->setRealSpeed(_p->realSpeed);
        _p->toolBar->setDroppedFrames(everyFrame ? false : _p->droppedFrames);

        Q_EMIT realSpeedChanged(_p->realSpeed);
        Q_EMIT droppedFramesChanged(_p->droppedFrames);
    }

    setFrame(_p->frame + inc, _p->inOutEnabled);
}

void djvViewPlaybackGroup::playbackCallback(QAction * action)
{
    //DJV_DEBUG("djvViewPlaybackGroup::playbackCallback");

    const int data = action->data().toInt();

    //DJV_DEBUG_PRINT("data = " << data);

    setPlayback(static_cast<djvViewUtil::PLAYBACK>(data));
}

void djvViewPlaybackGroup::playbackShuttleCallback(bool in)
{
    //DJV_DEBUG("djvViewPlaybackGroup::playbackShuttleCallback");
    //DJV_DEBUG_PRINT("in = " << in);

    if (in)
    {
        setPlayback(djvViewUtil::STOP);

        _p->shuttle = true;
        _p->shuttleSpeed = 0.0;
        _p->idleInit = true;

        if (_p->timer)
            killTimer(_p->timer);
        _p->timer = startTimer(0);
    }
    else
    {
        _p->shuttle = false;
        _p->droppedFrames = false;

        if (_p->timer)
        {
            killTimer(_p->timer);
            _p->timer = 0;
        }

        _p->toolBar->setSpeed(_p->speed);
    }
}

void djvViewPlaybackGroup::playbackShuttleValueCallback(int in)
{
    //DJV_DEBUG("djvViewPlaybackGroup::playbackShuttleValueCallback");
    //DJV_DEBUG_PRINT("in = " << in);

    _p->shuttleSpeed =
        djvMath::pow(static_cast<double>(djvMath::abs(in)), 1.5) *
        (in >= 0 ? 1.0 : -1.0);

    _p->idleFrame = quint64(_p->idleTimer.seconds() * _p->shuttleSpeed);
}

void djvViewPlaybackGroup::loopCallback(QAction * action)
{
    setLoop(static_cast<djvViewUtil::LOOP>(action->data().toInt()));
}

void djvViewPlaybackGroup::frameCallback(QAction * action)
{
    switch (static_cast<djvViewUtil::FRAME>(action->data().toInt()))
    {
        case djvViewUtil::FRAME_START:     frameCallback(_p->inPoint);     break;
        case djvViewUtil::FRAME_START_ABS: frameSliderCallback(0);         break;
        case djvViewUtil::FRAME_PREV:      frameCallback(_p->frame - 1);   break;
        case djvViewUtil::FRAME_PREV_10:   frameCallback(_p->frame - 10);  break;
        case djvViewUtil::FRAME_PREV_100:  frameCallback(_p->frame - 100); break;
        case djvViewUtil::FRAME_NEXT:      frameCallback(_p->frame + 1);   break;
        case djvViewUtil::FRAME_NEXT_10:   frameCallback(_p->frame + 10);  break;
        case djvViewUtil::FRAME_NEXT_100:  frameCallback(_p->frame + 100); break;
        case djvViewUtil::FRAME_END:       frameCallback(_p->outPoint);    break;
        case djvViewUtil::FRAME_END_ABS:   frameSliderCallback(sequenceEnd(_p->sequence)); break;
        
        default: break;
    }
}

void djvViewPlaybackGroup::frameCallback(qint64 in)
{
    setFrame(in, _p->inOutEnabled);
}

void djvViewPlaybackGroup::frameStopCallback(qint64 in)
{
    setPlayback(djvViewUtil::STOP);

    frameCallback(in);
}

void djvViewPlaybackGroup::frameSliderCallback(qint64 in)
{
    setFrame(in, false);
}

void djvViewPlaybackGroup::frameShuttleCallback(bool in)
{
    _p->frameTmp = _p->frame;
}

void djvViewPlaybackGroup::frameShuttleValueCallback(int in)
{
    setFrame(_p->frameTmp + in, _p->inOutEnabled);
}

void djvViewPlaybackGroup::framePressedCallback(bool value)
{
    //DJV_DEBUG("djvViewPlaybackGroup::framePressedCallback");

    _p->idlePause = value;
    _p->idleInit  = ! value;
}

void djvViewPlaybackGroup::framePressedCallback()
{
    framePressedCallback(true);
}

void djvViewPlaybackGroup::frameReleasedCallback()
{
    framePressedCallback(false);
}

void djvViewPlaybackGroup::inOutCallback(QAction * action)
{
    switch (static_cast<djvViewUtil::IN_OUT>(action->data().toInt()))
    {
        case djvViewUtil::IN_OUT_ENABLE:
        
            setInOutEnabled(action->isChecked());
            
            break;
        
        case djvViewUtil::MARK_IN:   _p->toolBar->markInPoint();   break;
        case djvViewUtil::MARK_OUT:  _p->toolBar->markOutPoint();  break;
        case djvViewUtil::RESET_IN:  _p->toolBar->resetInPoint();  break;
        case djvViewUtil::RESET_OUT: _p->toolBar->resetOutPoint(); break;
        
        default: break;
    }

    timeUpdate();
    layoutUpdate();
}

void djvViewPlaybackGroup::layoutCallback(QAction * action)
{
    setLayout(static_cast<djvViewUtil::LAYOUT>(action->data().toInt()));
}

void djvViewPlaybackGroup::cacheCallback()
{
    frameUpdate();
}

qint64 djvViewPlaybackGroup::frameStart() const
{
    return djvMath::max(static_cast<qint64>(0), _p->inPoint);
}

qint64 djvViewPlaybackGroup::frameEnd() const
{
    return djvMath::min(sequenceEnd(_p->sequence), _p->outPoint);
}

void djvViewPlaybackGroup::playbackUpdate()
{
    //DJV_DEBUG("djvViewPlaybackGroup::playbackUpdate");
    //DJV_DEBUG_PRINT("playback = " << _p->playback);
    //DJV_DEBUG_PRINT("loop = " << _p->loop);

    if (! _p->actions->group(djvViewPlaybackActions::PLAYBACK_GROUP)->actions()[
        _p->playback]->isChecked())
    {
        _p->actions->group(djvViewPlaybackActions::PLAYBACK_GROUP)->actions()[
            _p->playback]->trigger();
    }

    if (! _p->actions->group(djvViewPlaybackActions::LOOP_GROUP)->actions()[
        _p->loop]->isChecked())
    {
        _p->actions->group(djvViewPlaybackActions::LOOP_GROUP)->actions()[
            _p->loop]->trigger();
    }

    if (djvViewUtil::LOOP_ONCE == _p->loop)
    {
        switch (_p->playback)
        {
            case djvViewUtil::REVERSE:

                if (_p->frame == frameStart())
                {
                    _p->frame = frameEnd();
                }

                break;

            case djvViewUtil::FORWARD:

                if (_p->frame == frameEnd())
                {
                    _p->frame = frameStart();
                }

                break;

            default: break;
        }
    }

    if (_p->timer)
    {
        killTimer(_p->timer);

        _p->timer = 0;
    }

    switch (_p->playback)
    {
        case djvViewUtil::REVERSE:
        case djvViewUtil::FORWARD:

            _p->timer = startTimer(0);

            _p->idleInit = true;

            break;

        default: break;
    }
}

void djvViewPlaybackGroup::frameUpdate()
{
    //DJV_DEBUG("djvViewPlaybackGroup::frameUpdate");

    djvSignalBlocker signalBlocker(_p->toolBar);

    _p->toolBar->setFrame(_p->frame);

    _p->toolBar->setCachedFrames(
        djvViewFileCache::global()->frames(mainWindow()));
}

void djvViewPlaybackGroup::timeUpdate()
{
    //DJV_DEBUG("djvViewPlaybackGroup::timeUpdate");

    _p->actions->group(djvViewPlaybackActions::IN_OUT_GROUP)->actions()[
        djvViewUtil::IN_OUT_ENABLE]->setChecked(_p->inOutEnabled);

    const qint64 end = sequenceEnd(_p->sequence);

    djvSignalBlocker signalBlocker(_p->toolBar);

    _p->toolBar->setFrameList(_p->sequence.frames);
    _p->toolBar->setFrame(_p->frame);
    _p->toolBar->setStart(_p->inOutEnabled ? _p->inPoint : 0);
    _p->toolBar->setEnd(_p->inOutEnabled ? _p->outPoint : end);
    _p->toolBar->setDuration(
        _p->inOutEnabled ?
        (_p->outPoint - _p->inPoint + 1) :
        (end + 1),
        _p->inOutEnabled && (_p->inPoint != 0 || _p->outPoint != end));
    _p->toolBar->setInOutEnabled(_p->inOutEnabled);
    _p->toolBar->setInPoint(_p->inPoint);
    _p->toolBar->setOutPoint(_p->outPoint);
}

void djvViewPlaybackGroup::speedUpdate()
{
    //DJV_DEBUG("djvViewPlaybackGroup::speedUpdate");

    _p->actions->action(djvViewPlaybackActions::EVERY_FRAME)->
        setChecked(_p->everyFrame);

    djvSignalBlocker signalBlocker(_p->toolBar);

    _p->toolBar->setSpeed(_p->speed);
    _p->toolBar->setDefaultSpeed(_p->sequence.speed);
    _p->toolBar->setRealSpeed(_p->realSpeed);
    _p->toolBar->setDroppedFrames(_p->droppedFrames);
}

void djvViewPlaybackGroup::layoutUpdate()
{
    //DJV_DEBUG("djvViewPlaybackGroup::layoutUpdate");

    _p->actions->group(djvViewPlaybackActions::LAYOUT_GROUP)->
        actions()[_p->layout]->trigger();

    djvSignalBlocker signalBlocker(_p->toolBar);

    _p->toolBar->setLayout(_p->layout);
}
