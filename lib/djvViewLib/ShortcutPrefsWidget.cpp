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

#include <djvViewLib/ShortcutPrefsWidget.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/ShortcutPrefs.h>

#include <djvUI/PrefsGroupBox.h>
#include <djvUI/ShortcutsWidget.h>

#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QFormLayout>
#include <QVBoxLayout>

namespace djv
{
    namespace ViewLib
    {
        struct ShortcutPrefsWidget::Private
        {
            Private() :
                shortcutsWidget(0)
            {}

            UI::ShortcutsWidget * shortcutsWidget;
        };

        ShortcutPrefsWidget::ShortcutPrefsWidget(Context * context) :
            AbstractPrefsWidget(qApp->translate("djv::ViewLib::ShortcutPrefsWidget", "Shortcuts"), context),
            _p(new Private)
        {
            // Create the widgets.
            _p->shortcutsWidget = new UI::ShortcutsWidget(context);

            // Layout the widgets.
            QVBoxLayout * layout = new QVBoxLayout(this);

            UI::PrefsGroupBox * prefsGroupBox = new UI::PrefsGroupBox(
                qApp->translate("djv::ViewLib::ShortcutPrefsWidget", "Keyboard Shortcuts"), context);
            QFormLayout * formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(_p->shortcutsWidget);
            layout->addWidget(prefsGroupBox);

            layout->addStretch();

            // Initialize.
            widgetUpdate();

            // Setup the callbacks.
            connect(
                _p->shortcutsWidget,
                SIGNAL(shortcutsChanged(const QVector<djvShortcut> &)),
                SLOT(shortcutsCallback(const QVector<djvShortcut> &)));
        }

        ShortcutPrefsWidget::~ShortcutPrefsWidget()
        {}

        void ShortcutPrefsWidget::resetPreferences()
        {
            context()->shortcutPrefs()->setShortcuts(ShortcutPrefs::shortcutsDefault());
            widgetUpdate();
        }

        void ShortcutPrefsWidget::shortcutsCallback(const QVector<UI::Shortcut> & in)
        {
            context()->shortcutPrefs()->setShortcuts(in);
        }

        void ShortcutPrefsWidget::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _p->shortcutsWidget);
            _p->shortcutsWidget->setShortcuts(context()->shortcutPrefs()->shortcuts());
        }

    } // namespace ViewLib
} // namespace djv
