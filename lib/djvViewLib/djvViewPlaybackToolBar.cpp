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

//! \file djvViewPlaybackToolBar.cpp

#include <djvViewPlaybackToolBar.h>

#include <djvViewMiscWidget.h>
#include <djvViewPlaybackActions.h>
#include <djvViewPlaybackWidgets.h>
#include <djvToolButton.h>

#include <djvIconLibrary.h>

#include <QHBoxLayout>

//------------------------------------------------------------------------------
// djvViewPlaybackToolBar::P
//------------------------------------------------------------------------------

struct djvViewPlaybackToolBar::P
{
    P() :
        layout             (static_cast<djvViewUtil::LAYOUT>(0)),
        widget             (0),
        widgetLayout       (0),
        playbackButtons    (0),
        loopWidget         (0),
        speedWidget        (0),
        realSpeedDisplay   (0),
        everyFrameButton   (0),
        frameWidget        (0),
        frameSlider        (0),
        startWidget        (0),
        endWidget          (0),
        frameButtons       (0),
        durationDisplay    (0),
        inOutEnabledButton (0),
        markInPointButton  (0),
        markOutPointButton (0),
        resetInPointButton (0),
        resetOutPointButton(0)
    {}

    djvViewUtil::LAYOUT      layout;
    QWidget *                widget;
    QHBoxLayout *            widgetLayout;
    djvViewPlaybackButtons * playbackButtons;
    djvViewLoopWidget *      loopWidget;
    djvViewSpeedWidget *     speedWidget;
    djvViewSpeedDisplay *    realSpeedDisplay;
    djvToolButton *          everyFrameButton;
    djvViewFrameWidget *     frameWidget;
    djvViewFrameSlider *     frameSlider;
    djvViewFrameWidget *     startWidget;
    djvViewFrameWidget *     endWidget;
    djvViewFrameButtons *    frameButtons;
    djvViewFrameDisplay *    durationDisplay;
    djvToolButton *          inOutEnabledButton;
    djvToolButton *          markInPointButton;
    djvToolButton *          markOutPointButton;
    djvToolButton *          resetInPointButton;
    djvToolButton *          resetOutPointButton;
};

//------------------------------------------------------------------------------
// djvViewPlaybackToolBar
//------------------------------------------------------------------------------

djvViewPlaybackToolBar::djvViewPlaybackToolBar(
    djvViewAbstractActions * actions,
    QWidget *                parent) :
    djvViewAbstractToolBar(actions, parent),
    _p(new P)
{
    // Create the playback widgets.

    _p->playbackButtons = new djvViewPlaybackButtons(
        actions->group(djvViewPlaybackActions::PLAYBACK_GROUP));

    _p->loopWidget = new djvViewLoopWidget(
        actions->group(djvViewPlaybackActions::LOOP_GROUP));

    // Create the speed widgets.

    _p->speedWidget = new djvViewSpeedWidget;
    _p->speedWidget->setToolTip(tr("Playback speed"));

    _p->realSpeedDisplay = new djvViewSpeedDisplay;
    _p->realSpeedDisplay->setToolTip(tr("Real playback speed"));

    _p->everyFrameButton = new djvToolButton;
    _p->everyFrameButton->setDefaultAction(
        actions->action(djvViewPlaybackActions::EVERY_FRAME));
    _p->everyFrameButton->setIconSize(QSize(20, 20));
    _p->everyFrameButton->setToolTip(tr("Playback every frame"));

    // Create the frame widgets.

    _p->frameWidget = new djvViewFrameWidget;
    _p->frameWidget->setToolTip(tr("Current frame"));

    _p->frameSlider = new djvViewFrameSlider;
    _p->frameSlider->setToolTip(tr("Frame slider"));

    _p->startWidget = new djvViewFrameWidget;
    _p->startWidget->setToolTip(tr("Start frame / in point"));

    _p->frameButtons = new djvViewFrameButtons(
        actions->group(djvViewPlaybackActions::FRAME_GROUP));

    _p->endWidget = new djvViewFrameWidget;
    _p->endWidget->setToolTip(tr("End frame / out point"));

    _p->durationDisplay = new djvViewFrameDisplay;
    _p->durationDisplay->setToolTip(tr("Playback duration"));

    // Create the in/out point widgets.

    _p->inOutEnabledButton = new djvToolButton;
    _p->inOutEnabledButton->setDefaultAction(
        actions->group(djvViewPlaybackActions::IN_OUT_GROUP)->actions()[
            djvViewUtil::IN_OUT_ENABLE]);
    _p->inOutEnabledButton->setIconSize(QSize(20, 20));

    _p->markInPointButton = new djvToolButton;
    _p->markInPointButton->setDefaultAction(
        actions->group(djvViewPlaybackActions::IN_OUT_GROUP)->actions()[
            djvViewUtil::MARK_IN]);
    _p->markInPointButton->setIconSize(QSize(20, 20));

    _p->markOutPointButton = new djvToolButton;
    _p->markOutPointButton->setDefaultAction(
        actions->group(djvViewPlaybackActions::IN_OUT_GROUP)->actions()[
            djvViewUtil::MARK_OUT]);
    _p->markOutPointButton->setIconSize(QSize(20, 20));

    _p->resetInPointButton = new djvToolButton;
    _p->resetInPointButton->setDefaultAction(
        actions->group(djvViewPlaybackActions::IN_OUT_GROUP)->actions()[
            djvViewUtil::RESET_IN]);
    _p->resetInPointButton->setIconSize(QSize(20, 20));

    _p->resetOutPointButton = new djvToolButton;
    _p->resetOutPointButton->setDefaultAction(
        actions->group(djvViewPlaybackActions::IN_OUT_GROUP)->actions()[
            djvViewUtil::RESET_OUT]);
    _p->resetOutPointButton->setIconSize(QSize(20, 20));

    // Layout the widgets.

    _p->widget = new QWidget;

    addWidget(_p->widget);

    // Initialize.

    setAllowedAreas(Qt::BottomToolBarArea);
    setFloatable(false);
    setMovable(false);
    setIconSize(djvIconLibrary::global()->defaultSize());

    layoutUpdate();

    // Setup callbacks.

    connect(
        _p->playbackButtons,
        SIGNAL(shuttlePressed(bool)),
        SIGNAL(playbackShuttlePressed(bool)));

    connect(
        _p->playbackButtons,
        SIGNAL(shuttleChanged(int)),
        SIGNAL(playbackShuttleValue(int)));

    connect(
        _p->speedWidget,
        SIGNAL(speedChanged(const djvSpeed &)),
        SIGNAL(speedChanged(const djvSpeed &)));

    connect(
        _p->frameWidget,
        SIGNAL(frameChanged(qint64)),
        SIGNAL(frameChanged(qint64)));

    connect(
        _p->frameSlider,
        SIGNAL(pressed(bool)),
        SIGNAL(frameSliderPressed(bool)));

    connect(
        _p->frameSlider,
        SIGNAL(frameChanged(qint64)),
        SIGNAL(frameSliderChanged(qint64)));

    connect(
        _p->frameSlider,
        SIGNAL(inPointChanged(qint64)),
        SIGNAL(inPointChanged(qint64)));

    connect(
        _p->frameSlider,
        SIGNAL(outPointChanged(qint64)),
        SIGNAL(outPointChanged(qint64)));

    connect(
        _p->startWidget,
        SIGNAL(frameChanged(qint64)),
        _p->frameSlider,
        SLOT(setInPoint(qint64)));

    connect(
        _p->endWidget,
        SIGNAL(frameChanged(qint64)),
        _p->frameSlider,
        SLOT(setOutPoint(qint64)));

    connect(
        _p->frameButtons,
        SIGNAL(shuttlePressed(bool)),
        SIGNAL(frameShuttlePressed(bool)));

    connect(
        _p->frameButtons,
        SIGNAL(shuttleChanged(int)),
        SIGNAL(frameShuttleValue(int)));

    connect(
        _p->frameButtons,
        SIGNAL(pressed()),
        SIGNAL(frameButtonsPressed()));

    connect(
        _p->frameButtons,
        SIGNAL(released()),
        SIGNAL(frameButtonsReleased()));
}

djvViewPlaybackToolBar::~djvViewPlaybackToolBar()
{
    delete _p;
}

void djvViewPlaybackToolBar::setSpeed(const djvSpeed & speed)
{
    _p->speedWidget->setSpeed(speed);

    _p->frameWidget->setSpeed(speed);

    _p->frameSlider->setSpeed(speed);

    _p->startWidget->setSpeed(speed);

    _p->endWidget->setSpeed(speed);

    _p->durationDisplay->setSpeed(speed);
}

void djvViewPlaybackToolBar::setDefaultSpeed(const djvSpeed & speed)
{
    _p->speedWidget->setDefaultSpeed(speed);
}

void djvViewPlaybackToolBar::setRealSpeed(double speed)
{
    _p->realSpeedDisplay->setSpeed(speed);
}

void djvViewPlaybackToolBar::setDroppedFrames(bool in)
{
    _p->realSpeedDisplay->setDroppedFrames(in);
}

void djvViewPlaybackToolBar::setFrameList(const djvFrameList & in)
{
    _p->frameWidget->setFrameList(in);

    _p->frameSlider->setFrameList(in);

    _p->startWidget->setFrameList(in);

    _p->endWidget->setFrameList(in);
}

void djvViewPlaybackToolBar::setFrame(qint64 in)
{
    _p->frameWidget->setFrame(in);

    _p->frameSlider->setFrame(in);
}

void djvViewPlaybackToolBar::setCachedFrames(const djvFrameList & in)
{
    _p->frameSlider->setCachedFrames(in);
}

void djvViewPlaybackToolBar::setStart(qint64 in)
{
    _p->startWidget->setFrame(in);
}

void djvViewPlaybackToolBar::setEnd(qint64 in)
{
    _p->endWidget->setFrame(in);
}

void djvViewPlaybackToolBar::setDuration(qint64 in, bool inOutEnabled)
{
    _p->durationDisplay->setFrame(in);

    _p->durationDisplay->setInOutEnabled(inOutEnabled);
}

void djvViewPlaybackToolBar::setInOutEnabled(bool in)
{
    _p->frameSlider->setInOutEnabled(in);
}

void djvViewPlaybackToolBar::setInPoint(qint64 in)
{
    _p->frameSlider->setInPoint(in);
}

void djvViewPlaybackToolBar::setOutPoint(qint64 in)
{
    _p->frameSlider->setOutPoint(in);
}

void djvViewPlaybackToolBar::markInPoint()
{
    _p->frameSlider->markInPoint();
}

void djvViewPlaybackToolBar::markOutPoint()
{
    _p->frameSlider->markOutPoint();
}

void djvViewPlaybackToolBar::resetInPoint()
{
    _p->frameSlider->resetInPoint();
}

void djvViewPlaybackToolBar::resetOutPoint()
{
    _p->frameSlider->resetOutPoint();
}

void djvViewPlaybackToolBar::setLayout(djvViewUtil::LAYOUT layout)
{
    if (layout == _p->layout)
        return;

    _p->layout = layout;

    layoutUpdate();
}

void djvViewPlaybackToolBar::layoutUpdate()
{
    //DJV_DEBUG("djvViewPlaybackToolBar::layoutUpdate");

    delete _p->widgetLayout;
    _p->widgetLayout = new QHBoxLayout(_p->widget);
    _p->widgetLayout->setMargin(0);

    switch (_p->layout)
    {
        case djvViewUtil::LAYOUT_DEFAULT:
        case djvViewUtil::LAYOUT_LEFT:
        {
            QVBoxLayout * leftLayout = new QVBoxLayout;
            leftLayout->setMargin(0);

            QHBoxLayout * hLayout = new QHBoxLayout;
            hLayout->setMargin(0);
            hLayout->addWidget(_p->playbackButtons);
            hLayout->addWidget(_p->frameButtons);
            leftLayout->addLayout(hLayout);

            hLayout = new QHBoxLayout;
            hLayout->setMargin(0);
            QHBoxLayout * hLayout2 = new QHBoxLayout;
            hLayout2->setMargin(0);
            hLayout2->setSpacing(0);
            hLayout2->addWidget(_p->speedWidget);
            hLayout2->addWidget(_p->realSpeedDisplay);
            hLayout2->addWidget(_p->everyFrameButton);
            hLayout->addLayout(hLayout2);
            hLayout->addStretch(1000);
            hLayout->addWidget(_p->loopWidget);
            hLayout->addWidget(_p->inOutEnabledButton);
            leftLayout->addLayout(hLayout);

            _p->widgetLayout->addLayout(leftLayout);

            QVBoxLayout * rightLayout = new QVBoxLayout;
            rightLayout->setMargin(0);

            hLayout = new QHBoxLayout;
            hLayout->setMargin(0);
            hLayout->addWidget(_p->frameSlider, 1);
            rightLayout->addLayout(hLayout);

            hLayout = new QHBoxLayout;
            hLayout->setMargin(0);
            hLayout->addWidget(_p->frameWidget);
            hLayout2 = new QHBoxLayout;
            hLayout2->setSpacing(0);
            hLayout2->setMargin(0);
            hLayout2->addWidget(_p->markInPointButton);
            hLayout2->addWidget(_p->resetInPointButton);
            hLayout2->addWidget(_p->startWidget);
            hLayout->addLayout(hLayout2);
            if (djvViewUtil::LAYOUT_DEFAULT == _p->layout)
                hLayout->addStretch(1000);
            hLayout2 = new QHBoxLayout;
            hLayout2->setSpacing(0);
            hLayout2->setMargin(0);
            hLayout2->addWidget(_p->endWidget);
            hLayout2->addWidget(_p->resetOutPointButton);
            hLayout2->addWidget(_p->markOutPointButton);
            hLayout->addLayout(hLayout2);
            hLayout->addWidget(_p->durationDisplay);
            if (djvViewUtil::LAYOUT_LEFT == _p->layout)
                hLayout->addStretch(1000);
            rightLayout->addLayout(hLayout);

            _p->widgetLayout->addLayout(rightLayout, 1);
        }
        break;

        case djvViewUtil::LAYOUT_CENTER:
        {
            QVBoxLayout * layout = new QVBoxLayout;
            layout->setMargin(0);

            layout->addWidget(_p->frameSlider);

            QHBoxLayout * hLayout = new QHBoxLayout;
            hLayout->setMargin(0);
            hLayout->setSpacing(0);
            hLayout->addStretch(1000);
            hLayout->addWidget(_p->frameWidget);
            hLayout->addWidget(_p->markInPointButton);
            hLayout->addWidget(_p->resetInPointButton);
            hLayout->addWidget(_p->startWidget);
            hLayout->addWidget(_p->playbackButtons);
            hLayout->addWidget(_p->frameButtons);
            hLayout->addWidget(_p->endWidget);
            hLayout->addWidget(_p->resetOutPointButton);
            hLayout->addWidget(_p->markOutPointButton);
            hLayout->addWidget(_p->durationDisplay);
            hLayout->addStretch(1000);
            layout->addLayout(hLayout);

            _p->widgetLayout->addLayout(layout, 1);
        }
        break;

        case djvViewUtil::LAYOUT_MINIMAL:
        {
            _p->widgetLayout->addWidget(_p->playbackButtons);
            _p->widgetLayout->addWidget(_p->frameSlider, 1);
        }
        break;

        default: break;
    }

    switch (_p->layout)
    {
        case djvViewUtil::LAYOUT_DEFAULT:
        case djvViewUtil::LAYOUT_LEFT:

            _p->speedWidget->show();
            _p->realSpeedDisplay->show();
            _p->everyFrameButton->show();
            _p->loopWidget->show();
            _p->inOutEnabledButton->show();
            _p->frameSlider->show();
            _p->frameWidget->show();
            _p->frameButtons->show();
            _p->startWidget->show();
            _p->endWidget->show();
            _p->markInPointButton->show();
            _p->resetInPointButton->show();
            _p->markOutPointButton->show();
            _p->resetOutPointButton->show();
            _p->durationDisplay->show();

            break;

        case djvViewUtil::LAYOUT_CENTER:

            _p->speedWidget->hide();
            _p->realSpeedDisplay->hide();
            _p->everyFrameButton->hide();
            _p->loopWidget->hide();
            _p->inOutEnabledButton->hide();

            _p->frameSlider->show();
            _p->frameWidget->show();
            _p->frameButtons->show();
            _p->startWidget->show();
            _p->endWidget->show();
            _p->markInPointButton->show();
            _p->resetInPointButton->show();
            _p->markOutPointButton->show();
            _p->resetOutPointButton->show();
            _p->durationDisplay->show();

            break;

        case djvViewUtil::LAYOUT_MINIMAL:

            _p->speedWidget->hide();
            _p->realSpeedDisplay->hide();
            _p->everyFrameButton->hide();
            _p->loopWidget->hide();
            _p->inOutEnabledButton->hide();
            _p->frameWidget->hide();
            _p->frameButtons->hide();

            _p->startWidget->hide();
            _p->endWidget->hide();
            _p->markInPointButton->hide();
            _p->resetInPointButton->hide();
            _p->markOutPointButton->hide();
            _p->resetOutPointButton->hide();
            _p->durationDisplay->hide();

            break;

        default: break;
    }
}
