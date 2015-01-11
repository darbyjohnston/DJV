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

//! \file djvChoiceDialog.cpp

#include <djvChoiceDialog.h>

#include <QDialogButtonBox>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvChoiceDialog::P
//------------------------------------------------------------------------------

struct djvChoiceDialog::P
{
    P(const QString & label, const QStringList & choices, int choice) :
        choices     (choices),
        choice      (choice),
        buttonLayout(0),
        label       (label),
        labelWidget (0)
    {}
    
    QStringList             choices;
    int                     choice;
    QVector<QRadioButton *> buttons;
    QVBoxLayout *           buttonLayout;
    QString                 label;
    QLabel *                labelWidget;
};

//------------------------------------------------------------------------------
// djvChoiceDialog
//------------------------------------------------------------------------------

djvChoiceDialog::djvChoiceDialog(
    const QString &     label,
    const QStringList & choices,
    int                 choice,
    QWidget *           parent) :
    QDialog(parent),
    _p(new P(label, choices, choice))
{
    _p->labelWidget = new QLabel(label);
    
    QDialogButtonBox * buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok |
        QDialogButtonBox::Cancel);

    QVBoxLayout * layout = new QVBoxLayout(this);
    QVBoxLayout * vLayout = new QVBoxLayout;
    vLayout->setMargin(20);
    vLayout->addWidget(_p->labelWidget);
    _p->buttonLayout = new QVBoxLayout;
    _p->buttonLayout->setSpacing(0);
    vLayout->addLayout(_p->buttonLayout);
    layout->addLayout(vLayout);
    layout->addWidget(buttonBox);
    
    setWindowTitle(tr("Choice Dialog"));

    widgetUpdate();
    
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

djvChoiceDialog::~djvChoiceDialog()
{
    delete _p;
}

const QStringList & djvChoiceDialog::choices() const
{
    return _p->choices;
}

void djvChoiceDialog::setChoices(const QStringList & choices)
{
    _p->choices = choices;
    
    widgetUpdate();
}

int djvChoiceDialog::choice() const
{
    return _p->choice;
}

void djvChoiceDialog::setChoice(int choice)
{
    if (choice == _p->choice)
        return;
    
    _p->choice = choice;
    
    choiceUpdate();
}

const QString & djvChoiceDialog::label() const
{
    return _p->label;
}

void djvChoiceDialog::setLabel(const QString & label)
{
    _p->label = label;
    
    widgetUpdate();
}

void djvChoiceDialog::buttonCallback()
{
    for (int i = 0; i < _p->buttons.count(); ++i)
    {
        if (_p->buttons[i]->isChecked())
        {
            setChoice(i);
            
            break;
        }
    }
}

void djvChoiceDialog::choiceUpdate()
{
    for (int i = 0; i < _p->buttons.count(); ++i)
    {
        _p->buttons[i]->setChecked(_p->choice == i);
    }
}

void djvChoiceDialog::widgetUpdate()
{
    _p->labelWidget->setText(_p->label);
    
    for (int i = 0; i < _p->buttons.count(); ++i)
    {
        delete _p->buttons[i];
    }
    
    _p->buttons.clear();
    
    for (int i = 0; i < _p->choices.count(); ++i)
    {
        QRadioButton * button = new QRadioButton(_p->choices[i]);
        
        button->setChecked(_p->choice == i);
        
        connect(button, SIGNAL(toggled(bool)), SLOT(buttonCallback()));
        
        _p->buttons += button;
        _p->buttonLayout->addWidget(button);
    }
}
