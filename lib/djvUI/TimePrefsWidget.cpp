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

#include <djvUI/TimePrefsWidget.h>

#include <djvUI/UIContext.h>
#include <djvUI/TimePrefs.h>
#include <djvUI/PrefsGroupBox.h>
#include <djvUI/Style.h>

#include <djvCore/Debug.h>
#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        struct TimePrefsWidget::Private
        {
            QComboBox * timeUnitsWidget = nullptr;
            QComboBox * speedWidget = nullptr;
        };

        TimePrefsWidget::TimePrefsWidget(UIContext * context, QWidget * parent) :
            AbstractPrefsWidget(
                qApp->translate("djv::UI::TimePrefsWidget", "Time"), context, parent),
            _p(new Private)
        {
            // Create the widgets.
            _p->timeUnitsWidget = new QComboBox;
            _p->timeUnitsWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->timeUnitsWidget->addItems(djvTime::unitsLabels());

            _p->speedWidget = new QComboBox;
            _p->speedWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->speedWidget->addItems(djvSpeed::fpsLabels());

            // Layout the widgets.
            QVBoxLayout * layout = new QVBoxLayout(this);
            layout->setSpacing(context->style()->sizeMetric().largeSpacing);

            PrefsGroupBox * prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::TimePrefsWidget", "Time"), context);
            QFormLayout * formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("vTimePrefsWidget", "Time units:"),
                _p->timeUnitsWidget);
            QHBoxLayout * hLayout = new QHBoxLayout;
            hLayout->addWidget(_p->speedWidget);
            hLayout->addWidget(
                new QLabel(qApp->translate("djv::UI::TimePrefsWidget", "(frames per second)")));
            formLayout->addRow(
                qApp->translate("djv::UI::TimePrefsWidget", "Default speed:"), hLayout);
            layout->addWidget(prefsGroupBox);

            layout->addStretch();

            // Initialize.
            widgetUpdate();

            // Setup the callbacks.
            connect(
                _p->timeUnitsWidget,
                SIGNAL(activated(int)),
                SLOT(timeUnitsCallback(int)));
            connect(
                _p->speedWidget,
                SIGNAL(activated(int)),
                SLOT(speedCallback(int)));
        }

        TimePrefsWidget::~TimePrefsWidget()
        {}

        void TimePrefsWidget::resetPreferences()
        {
            context()->timePrefs()->setTimeUnits(djvTime::unitsDefault());
            context()->timePrefs()->setSpeed(djvSpeed::speedDefault());
            widgetUpdate();
        }

        void TimePrefsWidget::timeUnitsCallback(int index)
        {
            context()->timePrefs()->setTimeUnits(static_cast<djvTime::UNITS>(index));
        }

        void TimePrefsWidget::speedCallback(int index)
        {
            context()->timePrefs()->setSpeed(static_cast<djvSpeed::FPS>(index));
        }

        void TimePrefsWidget::widgetUpdate()
        {
            djvSignalBlocker signalBlocker(QObjectList() <<
                _p->timeUnitsWidget <<
                _p->speedWidget);
            _p->timeUnitsWidget->setCurrentIndex(djvTime::units());
            _p->speedWidget->setCurrentIndex(djvSpeed::speed());
        }

    } // namespace UI
} // namespace djv
