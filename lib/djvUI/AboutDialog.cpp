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

#include <djvUI/AboutDialog.h>

#include <djvUI/UIContext.h>

#include <djvCore/Math.h>

#include <QApplication>
#include <QClipboard>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QPainter>
#include <QPixmap>
#include <QPointer>
#include <QPushButton>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        struct AboutDialog::Private
        {
            Private(const QPointer<UIContext> & context) :
                context(context)
            {}

            QPointer<UIContext> context;
            QString text;
            int textHeight = 0;
            QPixmap pixmap;
            QPointer<QDialogButtonBox> buttonBox;
            int scrollTimer = 0;
            int scrollPos = 0;
        };

        AboutDialog::AboutDialog(const QString & text, const QPointer<UIContext> & context) :
            _p(new Private(context))
        {
            _p->text = text;
            _p->pixmap = QPixmap(":/djv/UI/Durant.jpg");
            
            // Create the widgets.
            auto copyButton = new QPushButton(qApp->translate("djv::UI::AboutDialog", "Copy"));
            _p->buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
            _p->buttonBox->addButton(copyButton, QDialogButtonBox::ActionRole);

            // Layout the widgets.
            auto layout = new QVBoxLayout(this);
            layout->addStretch(1);
            layout->addWidget(_p->buttonBox);

            // Initialize.
            setWindowTitle(qApp->translate("djv::UI::AboutDialog", "About"));
            setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint);
            resize(800, 600);

            // Setup callbacks.
            connect(copyButton, SIGNAL(clicked()), SLOT(copyCallback()));
            connect(_p->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
        }

        AboutDialog::~AboutDialog()
        {}
        
        void AboutDialog::timerEvent(QTimerEvent *)
        {
            _p->scrollPos += 1;
            if (_p->scrollPos > _p->textHeight)
            {
                _p->scrollPos = -40;
            }
            update();
        }
        
        void AboutDialog::showEvent(QShowEvent *)
        {
            _p->buttonBox->button(QDialogButtonBox::Close)->setFocus(Qt::PopupFocusReason);

            _p->scrollPos = -40;
            _p->scrollTimer = startTimer(50);
        }
        
        void AboutDialog::hideEvent(QHideEvent *)
        {
            killTimer(_p->scrollTimer);
            _p->scrollTimer = 0;
        }

        void AboutDialog::paintEvent(QPaintEvent * event)
        {
            QDialog::paintEvent(event);
            QPainter painter(this);
            painter.drawPixmap(width() / 2 - _p->pixmap.width() / 2, height() / 2 - _p->pixmap.height() / 2, _p->pixmap);
            QFont font = painter.font();
            int pixelSize = font.pixelSize() * 1;
            font.setPixelSize(pixelSize);
            painter.setFont(font);
            QRect rect = this->rect();
            painter.fillRect(rect, QColor(0, 0, 0, 127));
            painter.setPen(palette().color(QPalette::Foreground));
            rect = rect.adjusted(pixelSize * 5, pixelSize * 5, pixelSize * -5, pixelSize * -5);
            painter.setClipRect(rect);
            rect = rect.adjusted(0, -Core::Math::max(0, _p->scrollPos), 0, 0);
            painter.drawText(rect, Qt::TextWordWrap, _p->text);
            _p->textHeight = painter.boundingRect(rect, Qt::TextWordWrap, _p->text).height();
        }

        void AboutDialog::copyCallback()
        {
            QApplication::clipboard()->setText(_p->text);
        }

    } // namespace UI
} // namespace djv
