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

#include <djvUI/TimePrefsWidget.h>

#include <djvUI/IntEdit.h>
#include <djvUI/IntObject.h>
#include <djvUI/TimePrefs.h>
#include <djvUI/PrefsGroupBox.h>
#include <djvUI/UIContext.h>

#include <djvCore/Debug.h>
#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QPointer>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        struct TimePrefsWidget::Private
        {
            QPointer<QComboBox> unitsWidget;
            QPointer<QComboBox> speedWidget;
            QPointer<QVBoxLayout> layout;
        };

        TimePrefsWidget::TimePrefsWidget(const QPointer<UIContext> & context, QWidget * parent) :
            AbstractPrefsWidget(
                qApp->translate("djv::UI::TimePrefsWidget", "Time"), context, parent),
            _p(new Private)
        {
            // Create the widgets.
            _p->unitsWidget = new QComboBox;
            _p->unitsWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->unitsWidget->addItems(Core::Time::unitsLabels());

            _p->speedWidget = new QComboBox;
            _p->speedWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->speedWidget->addItems(Core::Speed::fpsLabels());
            
            // Layout the widgets.
            _p->layout = new QVBoxLayout(this);

            auto prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::TimePrefsWidget", "Time Units"), context);
            auto formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(_p->unitsWidget);
            _p->layout->addWidget(prefsGroupBox);

            prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::TimePrefsWidget", "Default Speed"), context);
            formLayout = prefsGroupBox->createLayout();
            auto hLayout = new QHBoxLayout;
            hLayout->addWidget(_p->speedWidget);
            hLayout->addWidget(
                new QLabel(qApp->translate("djv::UI::TimePrefsWidget", "(frames per second)")));
            formLayout->addRow(hLayout);
            _p->layout->addWidget(prefsGroupBox);

            _p->layout->addStretch();

            // Initialize.
            widgetUpdate();

            // Setup the callbacks.
            connect(
                _p->unitsWidget,
                SIGNAL(activated(int)),
                SLOT(unitsCallback(int)));
            connect(
                _p->speedWidget,
                SIGNAL(activated(int)),
                SLOT(speedCallback(int)));
            connect(
                context->timePrefs(),
                SIGNAL(prefChanged()),
                SLOT(widgetUpdate()));
        }

        TimePrefsWidget::~TimePrefsWidget()
        {}
        
        void TimePrefsWidget::resetPreferences()
        {
            context()->timePrefs()->setUnits(Core::Time::unitsDefault());
            context()->timePrefs()->setSpeed(Core::Speed::speedDefault());
        }

        void TimePrefsWidget::unitsCallback(int index)
        {
            context()->timePrefs()->setUnits(static_cast<Core::Time::UNITS>(index));
        }

        void TimePrefsWidget::speedCallback(int index)
        {
            context()->timePrefs()->setSpeed(static_cast<Core::Speed::FPS>(index));
        }

        void TimePrefsWidget::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _p->unitsWidget <<
                _p->speedWidget);
            _p->unitsWidget->setCurrentIndex(Core::Time::units());
            _p->speedWidget->setCurrentIndex(Core::Speed::speed());
        }

    } // namespace UI
} // namespace djv
