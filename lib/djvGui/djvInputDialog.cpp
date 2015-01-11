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

//! \file djvInputDialog.cpp

#include <djvInputDialog.h>

#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvInputDialog::P
//------------------------------------------------------------------------------

struct djvInputDialog::P
{
    P(const QString & label, const QString & text) :
        label      (label),
        labelWidget(0),
        text       (text),
        textEdit   (0)
    {}
    
    QString     label;
    QLabel *    labelWidget;
    QString     text;
    QLineEdit * textEdit;
};

//------------------------------------------------------------------------------
// djvInputDialog
//------------------------------------------------------------------------------

djvInputDialog::djvInputDialog(
    const QString & label,
    const QString & text,
    QWidget *       parent) :
    QDialog(parent),
    _p(new P(label, text))
{
    _p->labelWidget = new QLabel;
    
    _p->textEdit = new QLineEdit;
    
    QDialogButtonBox * buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok |
        QDialogButtonBox::Cancel);

    QVBoxLayout * layout = new QVBoxLayout(this);
    QVBoxLayout * vLayout = new QVBoxLayout;
    vLayout->setMargin(20);
    vLayout->addWidget(_p->labelWidget);
    vLayout->addWidget(_p->textEdit);
    layout->addLayout(vLayout);
    layout->addWidget(buttonBox);
    
    setWindowTitle(tr("Input Dialog"));

    widgetUpdate();
    
    connect(_p->textEdit, SIGNAL(editingFinished()), SLOT(textCallback()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

djvInputDialog::~djvInputDialog()
{
    delete _p;
}

const QString & djvInputDialog::label() const
{
    return _p->label;
}

void djvInputDialog::setLabel(const QString & label)
{
    if (label == _p->label)
        return;
    
    _p->label = label;
    
    widgetUpdate();
}

const QString & djvInputDialog::text() const
{
    return _p->text;
}

void djvInputDialog::setText(const QString & text)
{
    if (text == _p->text)
        return;
    
    _p->text = text;
    
    widgetUpdate();
}

void djvInputDialog::textCallback()
{
    setText(_p->textEdit->text());
}

void djvInputDialog::widgetUpdate()
{
    _p->labelWidget->setText(_p->label);
    
    _p->textEdit->setText(_p->text);
}
