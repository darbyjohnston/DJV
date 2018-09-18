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

#include <djvUI/SearchBox.h>

#include <djvUI/UIContext.h>
#include <djvUI/IconLibrary.h>
#include <djvUI/ToolButton.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

namespace djv
{
    namespace UI
    {
        struct SearchBox::Private
        {
            QString text;
            QLineEdit * lineEdit = nullptr;
            ToolButton * resetButton = nullptr;
        };

        SearchBox::SearchBox(UIContext * context, QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            // Create the widgets.
            _p->lineEdit = new QLineEdit;
            _p->lineEdit->setToolTip(
                qApp->translate("djv::UI::SearchBox", "Enter a search"));

            _p->resetButton = new ToolButton(context);
            _p->resetButton->setIconSize(QSize(16, 16));
            _p->resetButton->setIcon(
                context->iconLibrary()->icon("djvResetIcon.png"));
            _p->resetButton->setToolTip(
                qApp->translate("djv::UI::SearchBox", "Reset the search"));

            QLabel * label = new QLabel;
            label->setPixmap(
                context->iconLibrary()->pixmap("djvMagnifyIcon.png"));

            // Layout the widgets.
            QHBoxLayout * layout = new QHBoxLayout(this);
            layout->setMargin(0);
            layout->addWidget(label);
            layout->addWidget(_p->lineEdit);
            layout->addWidget(_p->resetButton);

            // Setup callbacks.
            connect(
                _p->lineEdit,
                SIGNAL(textChanged(const QString &)),
                SLOT(textCallback(const QString &)));
            connect(
                _p->resetButton,
                SIGNAL(clicked()),
                SLOT(resetCallback()));
        }

        SearchBox::~SearchBox()
        {}

        const QString & SearchBox::text() const
        {
            return _p->text;
        }

        void SearchBox::setText(const QString & text)
        {
            if (text == _p->text)
                return;
            _p->text = text;
            _p->lineEdit->setText(_p->text);
            Q_EMIT textChanged(_p->text);
            Q_EMIT textChanged(_p->text);
        }

        void SearchBox::textCallback(const QString & text)
        {
            setText(text);
        }

        void SearchBox::resetCallback()
        {
            setText(QString());
        }

    } // namespace UI
} // namespace djv
