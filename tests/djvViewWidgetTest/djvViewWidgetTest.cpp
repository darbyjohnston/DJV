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

#include <djvApplication.h>
#include <djvIconLibrary.h>
#include <djvMiscPrefs.h>
#include <djvToolButton.h>

#include <djvRangeUtil.h>
#include <djvSignalBlocker.h>
#include <djvTime.h>

#include <QComboBox>
#include <QFormLayout>

djvViewWidgetTest::djvViewWidgetTest() :
    _frame          (0),
    _inOutEnabled   (true),
    _inPoint        (100),
    _outPoint       (150),
    _frameList      (djvSequence(1, 250).frames),
    _cachedFrames   (djvRangeUtil::frames(djvFrameRangeList() <<
                        djvFrameRange(1, 10) <<
                        djvFrameRange(110, 180) <<
                        djvFrameRange(250, 250))),
    _timeUnitsWidget(0),
    _frameWidget    (0),
    _frameSlider    (0),
    _frameDisplay   (0),
    _speedWidget    (0),
    _speedDisplay   (0)
{
    _timeUnitsWidget = new QComboBox;
    _timeUnitsWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _timeUnitsWidget->addItems(djvTime::unitsLabels());
    
    _frameWidget = new djvViewFrameWidget;
    
    _frameSlider = new djvViewFrameSlider;

    djvToolButton * markInPointButton = new djvToolButton(
        djvIconLibrary::global()->icon("djvInPointMarkIcon.png"));
    djvToolButton * markOutPointButton = new djvToolButton(
        djvIconLibrary::global()->icon("djvOutPointMarkIcon.png"));
    djvToolButton * resetInPointButton = new djvToolButton(
        djvIconLibrary::global()->icon("djvInPointResetIcon.png"));
    djvToolButton * resetOutPointButton = new djvToolButton(
        djvIconLibrary::global()->icon("djvOutPointResetIcon.png"));

    _frameDisplay = new djvViewFrameDisplay;
    
    _speedWidget = new djvViewSpeedWidget;
    
    _speedDisplay = new djvViewSpeedDisplay;
    
    QFormLayout * layout = new QFormLayout(this);
    layout->addRow("Time units:", _timeUnitsWidget);
    layout->addRow("djvViewFrameWidget:", _frameWidget);
    QHBoxLayout * hLayout = new QHBoxLayout;
    hLayout->setMargin(0);
    hLayout->setSpacing(0);
    hLayout->addWidget(resetInPointButton);
    hLayout->addWidget(markInPointButton);
    hLayout->addWidget(_frameSlider);
    hLayout->addWidget(markOutPointButton);
    hLayout->addWidget(resetOutPointButton);
    layout->addRow("djvViewFrameSlider:", hLayout);
    layout->addRow("djvViewFrameDisplay:", _frameDisplay);
    layout->addRow("djvViewSpeedWidget:", _speedWidget);
    layout->addRow("djvViewSpeedDisplayWidget:", _speedDisplay);

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

void djvViewWidgetTest::timeUnitsCallback(int index)
{
    djvMiscPrefs::global()->setTimeUnits(static_cast<djvTime::UNITS>(index));
}

void djvViewWidgetTest::frameCallback(qint64 frame)
{
    if (frame != _frame)
    {
        _frame = frame;
        
        frameUpdate();
    }
}

void djvViewWidgetTest::speedCallback(const djvSpeed & speed)
{
    if (speed != _speed)
    {
        _speed = speed;
        
        widgetUpdate();
    }
}

void djvViewWidgetTest::frameUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _frameWidget <<
        _frameSlider);
    
    _frameWidget->setFrame(_frame);

    _frameSlider->setFrame(_frame);

    _frameDisplay->setFrame(_frame);
}

void djvViewWidgetTest::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _timeUnitsWidget <<
        _frameWidget <<
        _frameSlider);
    
    _timeUnitsWidget->setCurrentIndex(djvMiscPrefs::global()->timeUnits());
    
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

int main(int argc, char ** argv)
{
    djvApplication app("djvViewWidgetTest", argc, argv);
    
    (new djvViewWidgetTest)->show();
    
    return app.exec();
}

