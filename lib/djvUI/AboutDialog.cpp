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

#include <djvUI/AboutDialog.h>

#include <djvUI/UIContext.h>
#include <djvUI/Style.h>

#include <QApplication>
#include <QClipboard>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

#include "images/durant.xpm"

namespace djv
{
    namespace UI
    {
        struct AboutDialog::Private
        {
            Private(UIContext * context) :
                context(context)
            {}

            QTextEdit * widget = nullptr;
            QDialogButtonBox * buttonBox = nullptr;
            UIContext * context = nullptr;
        };

        AboutDialog::AboutDialog(const QString & text, UIContext * context) :
            _p(new Private(context))
        {
            // Create the widgets.
            QLabel * imageLabel = new QLabel;
            imageLabel->setPixmap(QPixmap(durantXpm));
            imageLabel->setAlignment(Qt::AlignCenter);

            _p->widget = new QTextEdit;
            _p->widget->setReadOnly(true);

            QPushButton * copyButton = new QPushButton(
                qApp->translate("djv::UI::AboutDialog", "Copy"));

            _p->buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
            _p->buttonBox->addButton(copyButton, QDialogButtonBox::ActionRole);

            // Layout the widgets.
            QVBoxLayout * layout = new QVBoxLayout(this);
            layout->addWidget(imageLabel);
            layout->addWidget(_p->widget);
            layout->addWidget(_p->buttonBox);

            // Initialize.
            setWindowTitle(qApp->translate("djv::UI::AboutDialog", "About Dialog"));

            _p->widget->setText(text);

            resize(500, 400);
            updateWidget();

            // Setup callbacks.
            connect(copyButton, SIGNAL(clicked()), SLOT(copyCallback()));
            connect(_p->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
            connect(
                context->style(),
                SIGNAL(fontsChanged()),
                SLOT(updateWidget()));
        }

        AboutDialog::~AboutDialog()
        {}

        void AboutDialog::showEvent(QShowEvent *)
        {
            _p->buttonBox->button(QDialogButtonBox::Close)->setFocus(Qt::PopupFocusReason);
        }

        void AboutDialog::copyCallback()
        {
            QApplication::clipboard()->setText(_p->widget->toPlainText());
        }

        void AboutDialog::updateWidget()
        {
            _p->widget->setFont(_p->context->style()->fonts().fixed);
        }

    } // namespace UI
} // namespace djv
