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

//! \file djvDebugLogDialog.cpp

#include <djvDebugLogDialog.h>

#include <djvStyle.h>

#include <djvDebugLog.h>

#include <QApplication>
#include <QClipboard>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvDebugLogDialog::P
//------------------------------------------------------------------------------

struct djvDebugLogDialog::P
{
    P() :
        widget   (0),
        buttonBox(0)
    {}

    QTextEdit *        widget;
    QDialogButtonBox * buttonBox;
};

//------------------------------------------------------------------------------
// djvDebugLogDialog
//------------------------------------------------------------------------------

djvDebugLogDialog::djvDebugLogDialog() :
    _p(new P)
{
    // Create the widgets.
    
    _p->widget = new QTextEdit;
    _p->widget->setReadOnly(true);
    _p->widget->setLineWrapMode(QTextEdit::NoWrap);
    _p->widget->document()->setMaximumBlockCount(10000);
    
    QPushButton * copyButton = new QPushButton(tr("Copy"));
    QPushButton * clearButton = new QPushButton(tr("Clear"));
    
    _p->buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    _p->buttonBox->addButton(copyButton, QDialogButtonBox::ActionRole);
    _p->buttonBox->addButton(clearButton, QDialogButtonBox::ActionRole);
    
    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->addWidget(_p->widget);
    layout->addWidget(_p->buttonBox);

    // Initialize.
    
    setWindowTitle(tr("Debugging Log"));
    
    resize(800, 600);
    
    Q_FOREACH(const QString & message, djvDebugLog::global()->messages())
    {
        _p->widget->append(message);
    }
    
    updateWidget();

    // Setup the callbacks.
    
    connect(copyButton, SIGNAL(clicked()), SLOT(copyCallback()));
    
    connect(clearButton, SIGNAL(clicked()), SLOT(clearCallback()));
    
    connect(_p->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    
    connect(
        djvDebugLog::global(),
        SIGNAL(message(const QString &)),
        SLOT(messageCallback(const QString &)));
    
    connect(
        djvStyle::global(),
        SIGNAL(fontsChanged()),
        SLOT(updateWidget()));
}

djvDebugLogDialog::~djvDebugLogDialog()
{
    delete _p;
}

djvDebugLogDialog * djvDebugLogDialog::global()
{
    static djvDebugLogDialog * data = 0;
    
    if (! data)
    {
        data = new djvDebugLogDialog;
    }
    
    return data;
}

void djvDebugLogDialog::showEvent(QShowEvent *)
{
    _p->buttonBox->button(QDialogButtonBox::Close)->setFocus(
        Qt::PopupFocusReason);
}

void djvDebugLogDialog::messageCallback(const QString & message)
{
    _p->widget->append(message);
}

void djvDebugLogDialog::copyCallback()
{
    QApplication::clipboard()->setText(_p->widget->toPlainText());
}

void djvDebugLogDialog::clearCallback()
{
    _p->widget->clear();
}

void djvDebugLogDialog::updateWidget()
{
    _p->widget->setFont(djvStyle::global()->fonts().fixed);
}

