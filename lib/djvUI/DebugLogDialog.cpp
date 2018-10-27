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

#include <djvUI/DebugLogDialog.h>

#include <djvUI/StylePrefs.h>
#include <djvUI/UIContext.h>

#include <djvCore/DebugLog.h>

#include <QApplication>
#include <QClipboard>
#include <QDialogButtonBox>
#include <QPointer>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        struct DebugLogDialog::Private
        {
            Private(const QPointer<UIContext> & context) :
                context(context)
            {}

            QPointer<QTextEdit> widget;
            QPointer<QDialogButtonBox> buttonBox;
            QPointer<UIContext> context;
        };

        DebugLogDialog::DebugLogDialog(const QPointer<UIContext> & context) :
            _p(new Private(context))
        {
            // Create the widgets.
            _p->widget = new QTextEdit;
            _p->widget->setReadOnly(true);
            _p->widget->setLineWrapMode(QTextEdit::NoWrap);
            _p->widget->document()->setMaximumBlockCount(10000);

            auto copyButton = new QPushButton(
                qApp->translate("djv::UI::DebugLogDialog", "Copy"));
            auto clearButton = new QPushButton(
                qApp->translate("djv::UI::DebugLogDialog", "Clear"));

            _p->buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
            _p->buttonBox->addButton(copyButton, QDialogButtonBox::ActionRole);
            _p->buttonBox->addButton(clearButton, QDialogButtonBox::ActionRole);

            // Layout the widgets.
            auto layout = new QVBoxLayout(this);
            layout->addWidget(_p->widget);
            layout->addWidget(_p->buttonBox);

            // Initialize.
            setWindowTitle(qApp->translate("djv::UI::DebugLogDialog", "Debugging Log"));
            setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint);
            resize(800, 600);

            Q_FOREACH(const QString & message, context->debugLog()->messages())
            {
                _p->widget->append(message);
            }

            styleUpdate();

            // Setup the callbacks.
            connect(copyButton, SIGNAL(clicked()), SLOT(copyCallback()));
            connect(clearButton, SIGNAL(clicked()), SLOT(clearCallback()));
            connect(_p->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
            connect(
                context->debugLog(),
                SIGNAL(message(const QString &)),
                SLOT(messageCallback(const QString &)));
        }

        DebugLogDialog::~DebugLogDialog()
        {}
        
        void DebugLogDialog::showEvent(QShowEvent *)
        {
            _p->buttonBox->button(QDialogButtonBox::Close)->setFocus(
                Qt::PopupFocusReason);
        }

        bool DebugLogDialog::event(QEvent * event)
        {
            if (QEvent::StyleChange == event->type())
            {
                styleUpdate();
            }
            return QDialog::event(event);
        }

        void DebugLogDialog::messageCallback(const QString & message)
        {
            _p->widget->append(message);
        }

        void DebugLogDialog::copyCallback()
        {
            QApplication::clipboard()->setText(_p->widget->toPlainText());
        }

        void DebugLogDialog::clearCallback()
        {
            _p->widget->clear();
        }

        void DebugLogDialog::styleUpdate()
        {
            _p->widget->setFont(_p->context->stylePrefs()->fonts().fixed);
        }

    } // namespace UI
} // namespace djv
