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

#include <djvViewLib/PlaybackGroup.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/FileCache.h>
#include <djvViewLib/MainWindow.h>
#include <djvViewLib/PlaybackActions.h>
#include <djvViewLib/PlaybackMenu.h>
#include <djvViewLib/PlaybackPrefs.h>
#include <djvViewLib/PlaybackToolBar.h>
#include <djvViewLib/ShortcutPrefs.h>

#include <djvUI/ToolButton.h>

#include <djvCore/ListUtil.h>
#include <djvCore/SignalBlocker.h>
#include <djvCore/Timer.h>

#include <QAction>
#include <QActionGroup>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QToolBar>

namespace djv
{
    namespace ViewLib
    {
        namespace
        {
            qint64 sequenceEnd(const Core::Sequence & sequence)
            {
                return sequence.frames.count() ? sequence.frames.count() - 1 : 0;
            }

        } // namespace

        struct PlaybackGroup::Private
        {
            Private(Context * context) :
                everyFrame(context->playbackPrefs()->hasEveryFrame()),
                layout(context->playbackPrefs()->layout())
            {}

            Core::Sequence    sequence;
            Util::PLAYBACK    playback = Util::STOP;
            Util::PLAYBACK    playbackPrev = Util::STOP;
            Util::LOOP        loop = Util::LOOP_REPEAT;
            Core::Speed       speed;
            float             realSpeed = 0.f;
            bool              droppedFrames = false;
            bool              droppedFramesTmp = false;
            bool              everyFrame = false;
            qint64            frame = 0;
            qint64            frameTmp = 0;
            bool              shuttle = false;
            float             shuttleSpeed = 0.f;
            bool              inOutEnabled = true;
            qint64            inPoint = 0;
            qint64            outPoint = 0;
            int               timer = 0;
            bool              idlePause = false;
            bool              idleInit = true;
            Core::Timer       idleTimer;
            quint64           idleFrame = 0;
            Core::Timer       speedTimer;
            quint64           speedCounter = 0;
            Util::LAYOUT      layout = static_cast<Util::LAYOUT>(0);
            PlaybackActions * actions = nullptr;
            PlaybackMenu *    menu = nullptr;
            PlaybackToolBar * toolBar = nullptr;
        };

        //------------------------------------------------------------------------------
        // PlaybackGroup
        //------------------------------------------------------------------------------

        PlaybackGroup::PlaybackGroup(
            PlaybackGroup * copy,
            MainWindow *    mainWindow,
            Context *       context) :
            AbstractGroup(mainWindow, context),
            _p(new Private(context))
        {
            // Create the actions.
            _p->actions = new PlaybackActions(context, this);

            // Create the menus.
            _p->menu = new PlaybackMenu(_p->actions, mainWindow->menuBar());
            mainWindow->menuBar()->addMenu(_p->menu);

            // Create the widgets.
            _p->toolBar = new PlaybackToolBar(_p->actions, context);
            mainWindow->addToolBar(Qt::BottomToolBarArea, _p->toolBar);

            // Initialize.
            if (copy)
            {
                _p->sequence = copy->_p->sequence;
                _p->playback = copy->_p->playback;
                _p->playbackPrev = copy->_p->playbackPrev;
                _p->loop = copy->_p->loop;
                _p->speed = copy->_p->speed;
                _p->everyFrame = copy->_p->everyFrame;
                _p->frame = copy->_p->frame;
                _p->inOutEnabled = copy->_p->inOutEnabled;
                _p->inPoint = copy->_p->inPoint;
                _p->outPoint = copy->_p->outPoint;
                _p->layout = copy->_p->layout;
            }
            playbackUpdate();
            timeUpdate();
            speedUpdate();
            frameUpdate();
            layoutUpdate();

            // Setup the action callbacks.
            connect(
                _p->actions->action(PlaybackActions::PLAYBACK_TOGGLE),
                SIGNAL(triggered()),
                SLOT(togglePlayback()));
            connect(
                _p->actions->action(PlaybackActions::EVERY_FRAME),
                SIGNAL(toggled(bool)),
                SLOT(setEveryFrame(bool)));

            // Setup the action group callbacks.
            connect(
                _p->actions->group(PlaybackActions::PLAYBACK_GROUP),
                SIGNAL(triggered(QAction *)),
                SLOT(playbackCallback(QAction *)));
            connect(
                _p->actions->group(PlaybackActions::LOOP_GROUP),
                SIGNAL(triggered(QAction *)),
                SLOT(loopCallback(QAction *)));
            connect(
                _p->actions->group(PlaybackActions::FRAME_GROUP),
                SIGNAL(triggered(QAction *)),
                SLOT(frameCallback(QAction *)));
            connect(
                _p->actions->group(PlaybackActions::IN_OUT_GROUP),
                SIGNAL(triggered(QAction *)),
                SLOT(inOutCallback(QAction *)));
            connect(
                _p->actions->group(PlaybackActions::LAYOUT_GROUP),
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
                SIGNAL(speedChanged(const djv::Core::Speed &)),
                SLOT(setSpeed(const djv::Core::Speed &)));
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
                context->playbackPrefs(),
                SIGNAL(everyFrameChanged(bool)),
                SLOT(setEveryFrame(bool)));
            connect(
                context->playbackPrefs(),
                SIGNAL(layoutChanged(djv::ViewLib::Util::LAYOUT)),
                SLOT(setLayout(djv::ViewLib::Util::LAYOUT)));

            // Setup other callbacks.
            connect(
                context->fileCache(),
                SIGNAL(cacheChanged()),
                SLOT(cacheCallback()));
        }

        PlaybackGroup::~PlaybackGroup()
        {}

        const Core::Sequence & PlaybackGroup::sequence() const
        {
            return _p->sequence;
        }

        Util::PLAYBACK PlaybackGroup::playback() const
        {
            return _p->playback;
        }

        Util::LOOP PlaybackGroup::loop() const
        {
            return _p->loop;
        }

        const Core::Speed & PlaybackGroup::speed() const
        {
            return _p->speed;
        }

        float PlaybackGroup::realSpeed() const
        {
            return _p->realSpeed;
        }

        bool PlaybackGroup::hasDroppedFrames() const
        {
            return _p->droppedFrames;
        }

        bool PlaybackGroup::hasEveryFrame() const
        {
            return _p->everyFrame;
        }

        qint64 PlaybackGroup::frame() const
        {
            return _p->frame;
        }

        bool PlaybackGroup::isInOutEnabled() const
        {
            return _p->inOutEnabled;
        }

        qint64 PlaybackGroup::inPoint() const
        {
            return _p->inPoint;
        }

        qint64 PlaybackGroup::outPoint() const
        {
            return _p->outPoint;
        }

        Util::LAYOUT PlaybackGroup::layout() const
        {
            return _p->layout;
        }

        QToolBar * PlaybackGroup::toolBar() const
        {
            return _p->toolBar;
        }

        void PlaybackGroup::setSequence(const Core::Sequence & sequence)
        {
            if (sequence == _p->sequence)
                return;
            //DJV_DEBUG("PlaybackGroup::setSequence");
            //DJV_DEBUG_PRINT("sequence = " << sequence);
            _p->sequence = sequence;
            _p->speed = sequence.speed;
            _p->realSpeed = 0.f;
            _p->droppedFrames = false;
            _p->inPoint = 0;
            _p->outPoint = sequenceEnd(_p->sequence);
            timeUpdate();
            speedUpdate();
            Q_EMIT sequenceChanged(_p->sequence);
            Q_EMIT speedChanged(_p->speed);
            Q_EMIT realSpeedChanged(_p->realSpeed);
            Q_EMIT droppedFramesChanged(_p->droppedFrames);
        }

        void PlaybackGroup::setPlayback(Util::PLAYBACK playback)
        {
            if (playback == _p->playback)
                return;
            //DJV_DEBUG("PlaybackGroup::setPlayback");
            //DJV_DEBUG_PRINT("playback = " << playback);
            _p->playbackPrev = _p->playback;
            _p->playback = playback;
            playbackUpdate();
            Q_EMIT playbackChanged(_p->playback);
        }

        void PlaybackGroup::togglePlayback()
        {
            switch (_p->playback)
            {
            case Util::FORWARD:
            case Util::REVERSE: setPlayback(Util::STOP); break;
            default: setPlayback(_p->playbackPrev); break;
            }
        }

        void PlaybackGroup::setLoop(Util::LOOP loop)
        {
            if (loop == _p->loop)
                return;
            _p->loop = loop;
            //DJV_DEBUG("PlaybackGroup::setLoop");
            //DJV_DEBUG_PRINT("loop = " << loop);
            playbackUpdate();
            Q_EMIT loopChanged(_p->loop);
        }

        void PlaybackGroup::setSpeed(const Core::Speed & in)
        {
            if (in == _p->speed)
                return;
            //DJV_DEBUG("PlaybackGroup::speed");
            //DJV_DEBUG_PRINT("in = " << in);
            _p->speed = in;
            _p->realSpeed = 0.f;
            _p->droppedFrames = false;
            playbackUpdate();
            speedUpdate();
            Q_EMIT speedChanged(_p->speed);
            Q_EMIT realSpeedChanged(_p->realSpeed);
            Q_EMIT droppedFramesChanged(_p->droppedFrames);
        }

        void PlaybackGroup::setEveryFrame(bool in)
        {
            if (in == _p->everyFrame)
                return;
            _p->everyFrame = in;
            speedUpdate();
            Q_EMIT everyFrameChanged(_p->everyFrame);
        }

        void PlaybackGroup::setFrame(qint64 in, bool inOutEnabled)
        {
            const qint64 frameStart = inOutEnabled ? this->frameStart() : 0;
            const qint64 frameEnd = inOutEnabled ? this->frameEnd() : sequenceEnd(_p->sequence);
            switch (_p->loop)
            {
            case Util::LOOP_ONCE:
                if (in <= frameStart || in >= frameEnd)
                {
                    setPlayback(Util::STOP);
                }
                in = Core::Math::clamp(in, frameStart, frameEnd);
                break;
            case Util::LOOP_REPEAT:
                in = Core::Math::wrap(in, frameStart, frameEnd);
                break;
            case Util::LOOP_PING_PONG:
                if (_p->playback != Util::STOP)
                {
                    if (in <= frameStart)
                    {
                        setPlayback(Util::FORWARD);
                    }
                    else if (in >= frameEnd)
                    {
                        setPlayback(Util::REVERSE);
                    }
                }
                in = Core::Math::clamp(in, frameStart, frameEnd);
                break;
            default: break;
            }
            if (in == _p->frame)
                return;
            //DJV_DEBUG("PlaybackGroup::setFrame");
            //DJV_DEBUG_PRINT("in = " << in);
            _p->frame = in;
            frameUpdate();
            Q_EMIT frameChanged(_p->frame);
        }

        void PlaybackGroup::setInOutEnabled(bool in)
        {
            if (in == _p->inOutEnabled)
                return;
            _p->inOutEnabled = in;
            timeUpdate();
            layoutUpdate();
            Q_EMIT inOutEnabledChanged(_p->inOutEnabled);
        }

        void PlaybackGroup::setInPoint(qint64 in)
        {
            if (in == _p->inPoint)
                return;
            _p->inPoint = in;
            timeUpdate();
            Q_EMIT inPointChanged(_p->inPoint);
        }

        void PlaybackGroup::setOutPoint(qint64 in)
        {
            if (in == _p->outPoint)
                return;
            _p->outPoint = in;
            timeUpdate();
            Q_EMIT outPointChanged(_p->outPoint);
        }

        void PlaybackGroup::setLayout(Util::LAYOUT in)
        {
            if (in == _p->layout)
                return;
            //DJV_DEBUG("PlaybackGroup::setLayout");
            //DJV_DEBUG_PRINT("in = " << in);
            _p->layout = in;
            layoutUpdate();
            Q_EMIT layoutChanged(_p->layout);
        }

        void PlaybackGroup::timerEvent(QTimerEvent *)
        {
            if (_p->idlePause)
                return;
            //DJV_DEBUG("PlaybackGroup::timerEvent");
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
            const float speed = _p->shuttle ? _p->shuttleSpeed : Core::Speed::speedToFloat(_p->speed);
            //DJV_DEBUG_PRINT("speed = " << speed);
            const quint64 absoluteFrame = quint64(_p->idleTimer.seconds() * speed);
            int inc = static_cast<int>(absoluteFrame - _p->idleFrame);
            //DJV_DEBUG_PRINT("absolute frame = " << static_cast<int>(absoluteFrame));
            //DJV_DEBUG_PRINT("inc = " << inc);
            if (!inc)
                return;

            // Calculate current frame.
            _p->idleFrame = absoluteFrame;
            if (Util::REVERSE == _p->playback)
            {
                inc = -inc;
            }
            const bool everyFrame = _p->everyFrame && !_p->shuttle;
            if (everyFrame)
            {
                inc = inc > 0 ? 1 : (inc < 0 ? -1 : 0);
            }
            else
            {
                _p->droppedFramesTmp |= inc > 1 || inc < -1;
            }
            _p->speedCounter += Core::Math::abs(inc);

            // Calculate real playback speed.
            _p->speedTimer.check();
            if (_p->speedTimer.seconds() >= 1.f)
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

        void PlaybackGroup::playbackCallback(QAction * action)
        {
            //DJV_DEBUG("PlaybackGroup::playbackCallback");
            const int data = action->data().toInt();
            //DJV_DEBUG_PRINT("data = " << data);
            setPlayback(static_cast<Util::PLAYBACK>(data));
        }

        void PlaybackGroup::playbackShuttleCallback(bool in)
        {
            //DJV_DEBUG("PlaybackGroup::playbackShuttleCallback");
            //DJV_DEBUG_PRINT("in = " << in);
            if (in)
            {
                setPlayback(Util::STOP);
                _p->shuttle = true;
                _p->shuttleSpeed = 0.f;
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

        void PlaybackGroup::playbackShuttleValueCallback(int in)
        {
            //DJV_DEBUG("PlaybackGroup::playbackShuttleValueCallback");
            //DJV_DEBUG_PRINT("in = " << in);
            _p->shuttleSpeed =
                Core::Math::pow(static_cast<float>(Core::Math::abs(in)), 1.5) *
                (in >= 0 ? 1.f : -1.f);
            _p->idleFrame = quint64(_p->idleTimer.seconds() * _p->shuttleSpeed);
        }

        void PlaybackGroup::loopCallback(QAction * action)
        {
            setLoop(static_cast<Util::LOOP>(action->data().toInt()));
        }

        void PlaybackGroup::frameCallback(QAction * action)
        {
            switch (static_cast<Util::FRAME>(action->data().toInt()))
            {
            case Util::FRAME_START:     frameCallback(_p->inPoint);     break;
            case Util::FRAME_START_ABS: frameSliderCallback(0);         break;
            case Util::FRAME_PREV:      frameCallback(_p->frame - 1);   break;
            case Util::FRAME_PREV_10:   frameCallback(_p->frame - 10);  break;
            case Util::FRAME_PREV_100:  frameCallback(_p->frame - 100); break;
            case Util::FRAME_NEXT:      frameCallback(_p->frame + 1);   break;
            case Util::FRAME_NEXT_10:   frameCallback(_p->frame + 10);  break;
            case Util::FRAME_NEXT_100:  frameCallback(_p->frame + 100); break;
            case Util::FRAME_END:       frameCallback(_p->outPoint);    break;
            case Util::FRAME_END_ABS:   frameSliderCallback(sequenceEnd(_p->sequence)); break;
            default: break;
            }
        }

        void PlaybackGroup::frameCallback(qint64 in)
        {
            setFrame(in, _p->inOutEnabled);
        }

        void PlaybackGroup::frameStopCallback(qint64 in)
        {
            setPlayback(Util::STOP);
            frameCallback(in);
        }

        void PlaybackGroup::frameSliderCallback(qint64 in)
        {
            setFrame(in, false);
        }

        void PlaybackGroup::frameShuttleCallback(bool in)
        {
            _p->frameTmp = _p->frame;
        }

        void PlaybackGroup::frameShuttleValueCallback(int in)
        {
            setFrame(_p->frameTmp + in, _p->inOutEnabled);
        }

        void PlaybackGroup::framePressedCallback(bool value)
        {
            //DJV_DEBUG("PlaybackGroup::framePressedCallback");
            _p->idlePause = value;
            _p->idleInit = !value;
        }

        void PlaybackGroup::framePressedCallback()
        {
            framePressedCallback(true);
        }

        void PlaybackGroup::frameReleasedCallback()
        {
            framePressedCallback(false);
        }

        void PlaybackGroup::inOutCallback(QAction * action)
        {
            switch (static_cast<Util::IN_OUT>(action->data().toInt()))
            {
            case Util::IN_OUT_ENABLE:
                setInOutEnabled(action->isChecked());
                break;
            case Util::MARK_IN:   _p->toolBar->markInPoint();   break;
            case Util::MARK_OUT:  _p->toolBar->markOutPoint();  break;
            case Util::RESET_IN:  _p->toolBar->resetInPoint();  break;
            case Util::RESET_OUT: _p->toolBar->resetOutPoint(); break;
            default: break;
            }
            timeUpdate();
            layoutUpdate();
        }

        void PlaybackGroup::layoutCallback(QAction * action)
        {
            setLayout(static_cast<Util::LAYOUT>(action->data().toInt()));
        }

        void PlaybackGroup::cacheCallback()
        {
            frameUpdate();
        }

        qint64 PlaybackGroup::frameStart() const
        {
            return Core::Math::max(static_cast<qint64>(0), _p->inPoint);
        }

        qint64 PlaybackGroup::frameEnd() const
        {
            return Core::Math::min(sequenceEnd(_p->sequence), _p->outPoint);
        }

        void PlaybackGroup::playbackUpdate()
        {
            //DJV_DEBUG("PlaybackGroup::playbackUpdate");
            //DJV_DEBUG_PRINT("playback = " << _p->playback);
            //DJV_DEBUG_PRINT("loop = " << _p->loop);

            if (!_p->actions->group(PlaybackActions::PLAYBACK_GROUP)->actions()[
                _p->playback]->isChecked())
            {
                _p->actions->group(PlaybackActions::PLAYBACK_GROUP)->actions()[
                    _p->playback]->trigger();
            }

                if (!_p->actions->group(PlaybackActions::LOOP_GROUP)->actions()[
                    _p->loop]->isChecked())
                {
                    _p->actions->group(PlaybackActions::LOOP_GROUP)->actions()[
                        _p->loop]->trigger();
                }

                    if (Util::LOOP_ONCE == _p->loop)
                    {
                        switch (_p->playback)
                        {
                        case Util::REVERSE:
                            if (_p->frame == frameStart())
                            {
                                _p->frame = frameEnd();
                            }
                            break;
                        case Util::FORWARD:
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
                    case Util::REVERSE:
                    case Util::FORWARD:
                        _p->timer = startTimer(0);
                        _p->idleInit = true;
                        break;
                    default: break;
                    }
        }

        void PlaybackGroup::frameUpdate()
        {
            //DJV_DEBUG("PlaybackGroup::frameUpdate");
            Core::SignalBlocker signalBlocker(_p->toolBar);
            _p->toolBar->setFrame(_p->frame);
            _p->toolBar->setCachedFrames(context()->fileCache()->frames(mainWindow()));
        }

        void PlaybackGroup::timeUpdate()
        {
            //DJV_DEBUG("PlaybackGroup::timeUpdate");
            _p->actions->group(PlaybackActions::IN_OUT_GROUP)->actions()[
                Util::IN_OUT_ENABLE]->setChecked(_p->inOutEnabled);
                const qint64 end = sequenceEnd(_p->sequence);
                Core::SignalBlocker signalBlocker(_p->toolBar);
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

        void PlaybackGroup::speedUpdate()
        {
            //DJV_DEBUG("PlaybackGroup::speedUpdate");
            _p->actions->action(PlaybackActions::EVERY_FRAME)->
                setChecked(_p->everyFrame);
            Core::SignalBlocker signalBlocker(_p->toolBar);
            _p->toolBar->setSpeed(_p->speed);
            _p->toolBar->setDefaultSpeed(_p->sequence.speed);
            _p->toolBar->setRealSpeed(_p->realSpeed);
            _p->toolBar->setDroppedFrames(_p->droppedFrames);
        }

        void PlaybackGroup::layoutUpdate()
        {
            //DJV_DEBUG("PlaybackGroup::layoutUpdate");
            _p->actions->group(PlaybackActions::LAYOUT_GROUP)->
                actions()[_p->layout]->trigger();
            Core::SignalBlocker signalBlocker(_p->toolBar);
            _p->toolBar->setLayout(_p->layout);
        }

    } // namespace ViewLib
} // namespace djv
