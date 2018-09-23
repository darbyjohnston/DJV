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

#include <djvUI/HelpPrefsWidget.h>

#include <djvUI/UIContext.h>
#include <djvUI/HelpPrefs.h>
#include <djvUI/PrefsGroupBox.h>

#include <djvCore/Debug.h>
#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QCheckBox>
#include <QFormLayout>
#include <QLabel>
#include <QPointer>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        struct HelpPrefsWidget::Private
        {
            QPointer<QCheckBox> toolTipsWidget;
            QPointer<QVBoxLayout> layout;
        };

        HelpPrefsWidget::HelpPrefsWidget(UIContext * context, QWidget * parent) :
            AbstractPrefsWidget(
                qApp->translate("djv::UI::HelpPrefsWidget", "Help"), context, parent),
            _p(new Private)
        {
            // Create the widgets.
            _p->toolTipsWidget = new QCheckBox(
                qApp->translate("djv::UI::HelpPrefsWidget", "Enable tool tips"));

            // Layout the widgets.
            _p->layout = new QVBoxLayout(this);

            PrefsGroupBox * prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::HelpPrefsWidget", "Tool Tips"), context);
            QFormLayout * formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(_p->toolTipsWidget);
            _p->layout->addWidget(prefsGroupBox);

            _p->layout->addStretch();

            // Initialize.
            widgetUpdate();

            // Setup the callbacks.
            connect(
                _p->toolTipsWidget,
                SIGNAL(toggled(bool)),
                SLOT(toolTipsCallback(bool)));
        }

        HelpPrefsWidget::~HelpPrefsWidget()
        {}
        
        void HelpPrefsWidget::resetPreferences()
        {
            context()->helpPrefs()->setToolTips(HelpPrefs::toolTipsDefault());

            widgetUpdate();
        }

        void HelpPrefsWidget::toolTipsCallback(bool toolTips)
        {
            context()->helpPrefs()->setToolTips(toolTips);
        }

        void HelpPrefsWidget::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _p->toolTipsWidget);
            _p->toolTipsWidget->setChecked(context()->helpPrefs()->hasToolTips());
        }

    } // namespace UI
} // namespace djv
