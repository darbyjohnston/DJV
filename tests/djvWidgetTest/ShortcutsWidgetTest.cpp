//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvWidgetTest/ShortcutsWidgetTest.h>

#include <djvUI/ShortcutsWidget.h>

#include <djvCore/System.h>

#include <QHBoxLayout>

namespace djv
{
    namespace WidgetTest
    {
        ShortcutsWidgetTest::ShortcutsWidgetTest(const QPointer<UI::UIContext> & context) :
            AbstractWidgetTest(context)
        {}

        QString ShortcutsWidgetTest::name()
        {
            return "ShortcutsWidgetTest";
        }

        void ShortcutsWidgetTest::run(const QStringList & args)
        {
            QWidget * window = new QWidget;

            UI::ShortcutsWidget * widget = new UI::ShortcutsWidget(context());

            QHBoxLayout * layout = new QHBoxLayout(window);
            layout->addWidget(widget);

            widget->setShortcuts(QVector<UI::Shortcut>() <<
                UI::Shortcut("A", QKeySequence("A")) <<
                UI::Shortcut("B", QKeySequence("Ctrl+B")) <<
                UI::Shortcut("C", QKeySequence("Alt+C")));

            connect(
                widget,
                SIGNAL(shortcutsChanged(const QVector<djv::UI::Shortcut> &)),
                SLOT(callback(const QVector<djv::UI::Shortcut> &)));

            window->show();
        }

        void ShortcutsWidgetTest::callback(const QVector<UI::Shortcut> & shortcuts)
        {
            for (int i = 0; i < shortcuts.count(); ++i)
            {
                Core::System::print(QString("%1: %2").
                    arg(shortcuts[i].name).
                    arg(shortcuts[i].value.toString()));
            }
        }

    } // namespace WidgetTest
} // namespace djv
