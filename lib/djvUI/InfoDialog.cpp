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
//------------------------------------------------------------------------------s

#include <djvUI/InfoDialog.h>

#include <djvUI/UIContext.h>

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
        struct InfoDialog::Private
        {
            Private(UIContext * context) :
                context(context)
            {}

            QPointer<QTextEdit> widget;
            QPointer<QDialogButtonBox> buttonBox;
            UIContext * context = nullptr;
        };

        InfoDialog::InfoDialog(const QString & text, UIContext * context) :
            _p(new Private(context))
        {
            //DJV_DEBUG("InfoDialog::InfoDialog");

            // Create the widgets.
            _p->widget = new QTextEdit;
            _p->widget->setReadOnly(true);
            QPushButton * copyButton = new QPushButton(qApp->translate("djv::UI::InfoDialog", "Copy"));
            _p->buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
            _p->buttonBox->addButton(copyButton, QDialogButtonBox::ActionRole);

            // Layout the widgets.
            QVBoxLayout * layout = new QVBoxLayout(this);
            layout->addWidget(_p->widget);
            layout->addWidget(_p->buttonBox);

            // Initialize.
            setWindowTitle(qApp->translate("djv::UI::InfoDialog", "Information"));
            setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint);
            _p->widget->setText(text);
            resize(800, 600);

            // Setup callbacks.
            connect(copyButton, SIGNAL(clicked()), SLOT(copyCallback()));
            connect(_p->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
        }

        InfoDialog::~InfoDialog()
        {}
        
        void InfoDialog::showEvent(QShowEvent *)
        {
            _p->buttonBox->button(QDialogButtonBox::Close)->setFocus(
                Qt::PopupFocusReason);
        }

        void InfoDialog::copyCallback()
        {
            QApplication::clipboard()->setText(_p->widget->toPlainText());
        }

    } // namespace UI
} // namespace djv
