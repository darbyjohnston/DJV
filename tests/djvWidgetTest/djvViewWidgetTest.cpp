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

//! \file djvViewWidgetTest.cpp

#include <djvViewWidgetTest.h>

#include <djvViewMiscWidget.h>

#include <djvIconLibrary.h>
#include <djvGuiContext.h>
#include <djvTimePrefs.h>
#include <djvToolButton.h>

#include <djvRangeUtil.h>
#include <djvSignalBlocker.h>
#include <djvTime.h>

#include <QApplication>
#include <QComboBox>
#include <QGridLayout>

djvViewWidget::djvViewWidget(djvGuiContext * context) :
    _frame          (0),
    _inOutEnabled   (true),
    _inPoint        (100),
    _outPoint       (150),
    _frameList      (djvSequence(1, 250).frames),
    _cachedFrames   (djvRangeUtil::frames(djvFrameRangeList() <<
                        djvFrameRange(  1,  10) <<
                        djvFrameRange(110, 180) <<
                        djvFrameRange(250, 250))),
    _timeUnitsWidget(0),
    _frameWidget    (0),
    _frameSlider    (0),
    _frameDisplay   (0),
    _speedWidget    (0),
    _speedDisplay   (0),
    _context        (context)
{
    _timeUnitsWidget = new QComboBox;
    _timeUnitsWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _timeUnitsWidget->addItems(djvTime::unitsLabels());
    
    _frameWidget = new djvViewFrameWidget(context);
    
    _frameSlider = new djvViewFrameSlider(context);

    djvToolButton * markInPointButton = new djvToolButton(
        context->iconLibrary()->icon("djvInPointMarkIcon.png"));
    djvToolButton * markOutPointButton = new djvToolButton(
        context->iconLibrary()->icon("djvOutPointMarkIcon.png"));
    djvToolButton * resetInPointButton = new djvToolButton(
        context->iconLibrary()->icon("djvInPointResetIcon.png"));
    djvToolButton * resetOutPointButton = new djvToolButton(
        context->iconLibrary()->icon("djvOutPointResetIcon.png"));

    _frameDisplay = new djvViewFrameDisplay(context);
    
    _speedWidget = new djvViewSpeedWidget(context);
    
    _speedDisplay = new djvViewSpeedDisplay;
    
    QGridLayout * layout = new QGridLayout(this);
    layout->setColumnStretch(3, 1);
    layout->setRowStretch(3, 1);
    QGridLayout * gLayout = new QGridLayout;
    gLayout->addWidget(_frameWidget,  0, 0);
    gLayout->addWidget(_frameDisplay, 0, 1);
    gLayout->addWidget(_speedWidget,  1, 0);
    gLayout->addWidget(_speedDisplay, 1, 1);
    layout->addLayout(gLayout, 0, 0, 2, 1);
    layout->addWidget(_frameSlider,  0, 1, 1, 5);
    layout->addWidget(resetInPointButton, 1, 1);
    layout->addWidget(markInPointButton, 1, 2);
    layout->addWidget(markOutPointButton, 1, 4);
    layout->addWidget(resetOutPointButton, 1, 5);
    layout->addWidget(_timeUnitsWidget, 2, 0);

    setWindowTitle("djvViewWidget");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    widgetUpdate();
    frameUpdate();
    
    connect(
        _timeUnitsWidget,
        SIGNAL(activated(int)),
        SLOT(timeUnitsCallback(int)));
    
    connect(
        _frameWidget,
        SIGNAL(frameChanged(qint64)),
        SLOT(frameCallback(qint64)));
    
    connect(
        _frameSlider,
        SIGNAL(frameChanged(qint64)),
        SLOT(frameCallback(qint64)));

    connect(
        markInPointButton,
        SIGNAL(clicked()),
        _frameSlider,
        SLOT(markInPoint()));

    connect(
        markOutPointButton,
        SIGNAL(clicked()),
        _frameSlider,
        SLOT(markOutPoint()));

    connect(
        resetInPointButton,
        SIGNAL(clicked()),
        _frameSlider,
        SLOT(resetInPoint()));

    connect(
        resetOutPointButton,
        SIGNAL(clicked()),
        _frameSlider,
        SLOT(resetOutPoint()));

    connect(
        _speedWidget,
        SIGNAL(speedChanged(const djvSpeed &)),
        SLOT(speedCallback(const djvSpeed &)));
}

void djvViewWidget::timeUnitsCallback(int index)
{
    _context->timePrefs()->setTimeUnits(static_cast<djvTime::UNITS>(index));
}

void djvViewWidget::frameCallback(qint64 frame)
{
    if (frame != _frame)
    {
        _frame = frame;
        
        frameUpdate();
    }
}

void djvViewWidget::speedCallback(const djvSpeed & speed)
{
    if (speed != _speed)
    {
        _speed = speed;
        
        widgetUpdate();
    }
}

void djvViewWidget::frameUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _frameWidget <<
        _frameSlider);
    
    _frameWidget->setFrame(_frame);

    _frameSlider->setFrame(_frame);

    _frameDisplay->setFrame(_frame);
}

void djvViewWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _timeUnitsWidget <<
        _frameWidget <<
        _frameSlider);
    
    _timeUnitsWidget->setCurrentIndex(_context->timePrefs()->timeUnits());
    
    _frameWidget->setFrameList(_frameList);
    _frameWidget->setSpeed(_speed);
    
    _frameSlider->setFrameList(_frameList);
    _frameSlider->setSpeed(_speed);
    _frameSlider->setInOutEnabled(_inOutEnabled);
    _frameSlider->setInOutPoints(_inPoint, _outPoint);
    _frameSlider->setCachedFrames(_cachedFrames);
    
    _frameDisplay->setSpeed(_speed);
    
    _speedWidget->setSpeed(_speed);
    
    _speedDisplay->setSpeed(djvSpeed::speedToFloat(_speed));
    _speedDisplay->setDroppedFrames(true);
}

djvViewWidgetTest::djvViewWidgetTest(djvGuiContext * context) :
    djvAbstractWidgetTest(context)
{}

QString djvViewWidgetTest::name()
{
    return "djvViewWidgetTest";
}

void djvViewWidgetTest::run(const QStringList & args)
{
    (new djvViewWidget(context()))->show();
}
