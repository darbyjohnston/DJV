//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <djvWidgetTest/ViewWidgetTest.h>

#include <djvViewLib/MiscWidget.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/TimePrefs.h>
#include <djvUI/ToolButton.h>
#include <djvUI/UIContext.h>

#include <djvCore/RangeUtil.h>
#include <djvCore/SignalBlocker.h>
#include <djvCore/Time.h>

#include <QApplication>
#include <QComboBox>
#include <QGridLayout>

namespace djv
{
    namespace WidgetTest
    {
        ViewWidget::ViewWidget(const QPointer<UI::UIContext> & context) :
            _frame(0),
            _inOutEnabled(true),
            _inPoint(100),
            _outPoint(150),
            _frameList(Core::Sequence(1, 250).frames),
            _cachedFrames(Core::RangeUtil::frames(Core::FrameRangeList() <<
                Core::FrameRange(1, 10) <<
                Core::FrameRange(110, 180) <<
                Core::FrameRange(250, 250))),
            _context(context),
            _timeUnitsWidget(0),
            _frameWidget(0),
            _frameSlider(0),
            _frameDisplay(0),
            _speedWidget(0),
            _speedDisplay(0)
        {
            _timeUnitsWidget = new QComboBox;
            _timeUnitsWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _timeUnitsWidget->addItems(Core::Time::unitsLabels());

            _frameWidget = new ViewLib::FrameWidget(context);

            _frameSlider = new ViewLib::FrameSlider(context);
            
            UI::ToolButton * markInPointButton = new UI::ToolButton(context);
            markInPointButton->setIcon(context->iconLibrary()->icon("djvInPointMarkIcon"));
            UI::ToolButton * markOutPointButton = new UI::ToolButton(context);
            markOutPointButton->setIcon(context->iconLibrary()->icon("djvOutPointMarkIcon"));
            UI::ToolButton * resetInPointButton = new UI::ToolButton(context);
            resetInPointButton->setIcon(context->iconLibrary()->icon("djvInPointResetIcon"));
            UI::ToolButton * resetOutPointButton = new UI::ToolButton(context);
            resetOutPointButton->setIcon(context->iconLibrary()->icon("djvOutPointResetIcon"));

            _frameDisplay = new ViewLib::FrameDisplay(context);

            _speedWidget = new ViewLib::SpeedWidget(context);

            _speedDisplay = new ViewLib::SpeedDisplay(context);

            QGridLayout * layout = new QGridLayout(this);
            layout->setColumnStretch(3, 1);
            layout->setRowStretch(3, 1);
            QGridLayout * gLayout = new QGridLayout;
            gLayout->addWidget(_frameWidget, 0, 0);
            gLayout->addWidget(_frameDisplay, 0, 1);
            gLayout->addWidget(_speedWidget, 1, 0);
            gLayout->addWidget(_speedDisplay, 1, 1);
            layout->addLayout(gLayout, 0, 0, 2, 1);
            layout->addWidget(_frameSlider, 0, 1, 1, 5);
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
                SIGNAL(speedChanged(const djv::Core::Speed &)),
                SLOT(speedCallback(const djv::Core::Speed &)));
        }

        void ViewWidget::timeUnitsCallback(int index)
        {
            _context->timePrefs()->setUnits(static_cast<Core::Time::UNITS>(index));
        }

        void ViewWidget::frameCallback(qint64 frame)
        {
            if (frame != _frame)
            {
                _frame = frame;
                frameUpdate();
            }
        }

        void ViewWidget::speedCallback(const Core::Speed & speed)
        {
            if (speed != _speed)
            {
                _speed = speed;
                widgetUpdate();
            }
        }

        void ViewWidget::frameUpdate()
        {
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _frameWidget <<
                _frameSlider);
            _frameWidget->setFrame(_frame);
            _frameSlider->setFrame(_frame);
            _frameDisplay->setFrame(_frame);
        }

        void ViewWidget::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _timeUnitsWidget <<
                _frameWidget <<
                _frameSlider);
            _timeUnitsWidget->setCurrentIndex(_context->timePrefs()->units());
            _frameWidget->setFrameList(_frameList);
            _frameWidget->setSpeed(_speed);
            _frameSlider->setFrameList(_frameList);
            _frameSlider->setSpeed(_speed);
            _frameSlider->setInOutEnabled(_inOutEnabled);
            _frameSlider->setInOutPoints(_inPoint, _outPoint);
            _frameSlider->setCachedFrames(_cachedFrames);
            _frameDisplay->setSpeed(_speed);
            _speedWidget->setSpeed(_speed);
            _speedDisplay->setSpeed(Core::Speed::speedToFloat(_speed));
            _speedDisplay->setDroppedFrames(true);
        }

        ViewWidgetTest::ViewWidgetTest(const QPointer<UI::UIContext> & context) :
            AbstractWidgetTest(context)
        {}

        QString ViewWidgetTest::name()
        {
            return "ViewWidgetTest";
        }

        void ViewWidgetTest::run(const QStringList & args)
        {
            (new ViewWidget(context()))->show();
        }

    } // namespace WidgetTest
} // namespace djv
