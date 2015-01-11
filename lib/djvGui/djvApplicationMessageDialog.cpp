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

//! \file djvApplicationMessageDialog.cpp

#include <djvApplicationMessageDialog.h>

#include <djvPrefs.h>
#include <djvStyle.h>

#include <djvDebug.h>
#include <djvError.h>
#include <djvErrorUtil.h>

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvApplicationMessageDialog::P
//------------------------------------------------------------------------------

struct djvApplicationMessageDialog::P
{
    P() :
        widget      (0),
        show        (true),
        showCheckBox(0),
        buttonBox   (0)
    {}

    QStringList        list;
    QTextEdit *        widget;
    bool               show;
    QCheckBox *        showCheckBox;
    QDialogButtonBox * buttonBox;
};

//------------------------------------------------------------------------------
// djvApplicationMessageDialog
//------------------------------------------------------------------------------

djvApplicationMessageDialog::djvApplicationMessageDialog() :
    _p(new P)
{
    // Create the widgets.
    
    _p->widget = new QTextEdit;
    _p->widget->setLineWrapMode(QTextEdit::NoWrap);
    _p->widget->setReadOnly(true);
    
    _p->showCheckBox = new QCheckBox(tr("Show"));
    
    QPushButton * clearButton = new QPushButton(tr("Clear"));
    
    _p->buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    _p->buttonBox->addButton(_p->showCheckBox, QDialogButtonBox::ActionRole);
    _p->buttonBox->addButton(clearButton, QDialogButtonBox::ActionRole);
    
    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->addWidget(_p->widget);
    layout->addWidget(_p->buttonBox);

    // Preferences.

    djvPrefs prefs("djvApplicationMessageDialog", djvPrefs::SYSTEM);
    prefs.get("show", _p->show);

    // Initialize.
    
    setWindowTitle(tr("Messages Dialog"));
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    
    resize(400, 200);
    
    updateWidget();

    // Setup the callbacks.
    
    connect(_p->showCheckBox, SIGNAL(toggled(bool)), SLOT(showCallback(bool)));
    
    connect(clearButton, SIGNAL(clicked()), SLOT(clearCallback()));
    
    connect(_p->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    
    connect(
        djvStyle::global(),
        SIGNAL(fontsChanged()),
        SLOT(updateWidget()));
}

djvApplicationMessageDialog::~djvApplicationMessageDialog()
{
    djvPrefs prefs("djvApplicationMessageDialog", djvPrefs::SYSTEM);
    prefs.set("show", _p->show);
    
    delete _p;
}

void djvApplicationMessageDialog::message(const QString & in)
{
    _p->list.push_front(in);

    if (_p->list.count() > 100)
    {
        _p->list.pop_back();
    }

    updateWidget();

    if (_p->show)
    {
        show();
    }
}

void djvApplicationMessageDialog::message(const djvError & in)
{
    message(djvErrorUtil::format(in).join("\n"));
}

djvApplicationMessageDialog * djvApplicationMessageDialog::global()
{
    static djvApplicationMessageDialog * data = 0;
    
    if (! data)
    {
        data = new djvApplicationMessageDialog;
    }
    
    return data;
}

void djvApplicationMessageDialog::showEvent(QShowEvent *)
{
    _p->buttonBox->button(QDialogButtonBox::Close)->setFocus(
        Qt::PopupFocusReason);
}

void djvApplicationMessageDialog::clearCallback()
{
    _p->list.clear();

    updateWidget();
}

void djvApplicationMessageDialog::showCallback(bool value)
{
    _p->show = value;
}

void djvApplicationMessageDialog::updateWidget()
{
    //DJV_DEBUG("ApplicationMessageDialog::updateWidget");
    
    _p->widget->setFont(djvStyle::global()->fonts().fixed);
    
    const QString text = _p->list.join("\n");
    
    //DJV_DEBUG_PRINT("text = " << text);
    
    _p->widget->setText(text);
    
    _p->showCheckBox->setChecked(_p->show);
}
