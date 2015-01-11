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

//! \file djvPrefsGroupBox.cpp

#include <djvPrefsGroupBox.h>

#include <djvStyle.h>

#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>

//------------------------------------------------------------------------------
// djvPrefsGroupBox::P
//------------------------------------------------------------------------------

struct djvPrefsGroupBox::P
{
    P(const QString & text) :
        text  (text),
        label (0),
        layout(0)
    {}
    
    QString       text;

    QGroupBox *   groupBox;
    QLabel *      label;
    QVBoxLayout * layout;
};

//------------------------------------------------------------------------------
// djvPrefsGroupBox
//------------------------------------------------------------------------------

djvPrefsGroupBox::djvPrefsGroupBox(
    const QString & title,
    const QString & text,
    QWidget *       parent) :
    QWidget(parent),
    _p(new P(text))
{
    const QFont font = this->font();
    
    _p->groupBox = new QGroupBox(title);
    _p->groupBox->setStyleSheet("QGroupBox { font-weight: bold; }");

    _p->label = new QLabel(text);
    _p->label->setWordWrap(true);
    if (0 == text.length())
        _p->label->hide();
    
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(_p->groupBox);
    
    _p->layout = new QVBoxLayout(_p->groupBox);
    _p->layout->setSpacing(djvStyle::global()->sizeMetric().largeSpacing);
    _p->layout->addWidget(_p->label);
}

djvPrefsGroupBox::~djvPrefsGroupBox()
{
    delete _p;
}

const QString & djvPrefsGroupBox::text() const
{
    return _p->text;
}

void djvPrefsGroupBox::setText(const QString & text)
{
    _p->label->setText(text);
    _p->label->setVisible(text.length() > 0);
}
    
QFormLayout * djvPrefsGroupBox::createLayout()
{
    QFormLayout * formLayout = new QFormLayout;
    formLayout->setSpacing(djvStyle::global()->sizeMetric().spacing);
    
    _p->layout->addLayout(formLayout);
    
    return formLayout;
}

