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

#include <QApplication>
#include <QClipboard>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        struct AboutDialog::Private
        {
            Private(UIContext * context) :
                context(context)
            {}

            UIContext * context = nullptr;
            QPixmap pixmap;
            QLabel * label = nullptr;
            QDialogButtonBox * buttonBox = nullptr;
        };

        AboutDialog::AboutDialog(const QString & text, UIContext * context) :
            _p(new Private(context))
        {
            _p->pixmap = QPixmap(":/djv/UI/Durant.png");
            
            // Create the widgets.
            _p->label = new QLabel;
            _p->label->setWordWrap(true);
            auto scrollArea = new QScrollArea;
            scrollArea->setWidgetResizable(true);
            scrollArea->setWidget(_p->label);
            auto copyButton = new QPushButton(qApp->translate("djv::UI::AboutDialog", "Copy"));
            _p->buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
            _p->buttonBox->addButton(copyButton, QDialogButtonBox::ActionRole);

            // Layout the widgets.
            auto layout = new QVBoxLayout(this);
            auto hLayout = new QHBoxLayout;
            hLayout->addStretch(1);
            hLayout->addWidget(scrollArea, 4);
            hLayout->addStretch(1);
            layout->addLayout(hLayout);
            layout->addWidget(_p->buttonBox);

            // Initialize.
            setWindowTitle(qApp->translate("djv::UI::AboutDialog", "About"));
            setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint);
            _p->label->setText(text);
            resize(800, 600);

            // Setup callbacks.
            connect(copyButton, SIGNAL(clicked()), SLOT(copyCallback()));
            connect(_p->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
        }

        void AboutDialog::showEvent(QShowEvent *)
        {
            _p->buttonBox->button(QDialogButtonBox::Close)->setFocus(Qt::PopupFocusReason);
        }

        void AboutDialog::paintEvent(QPaintEvent * event)
        {
            QDialog::paintEvent(event);
            QPainter painter(this);
            painter.drawPixmap(width() / 2 - _p->pixmap.width() / 2, height() / 2 - _p->pixmap.height() / 2, _p->pixmap);
        }

        void AboutDialog::copyCallback()
        {
            QApplication::clipboard()->setText(_p->label->text());
        }

    } // namespace UI
} // namespace djv
