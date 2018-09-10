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

#include <djvUI/MessagesDialog.h>

#include <djvUI/UIContext.h>
#include <djvUI/Prefs.h>
#include <djvUI/Style.h>

#include <djvCore/Debug.h>
#include <djvCore/Error.h>
#include <djvCore/ErrorUtil.h>

#include <QApplication>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvMessagesDialog::Private
//------------------------------------------------------------------------------

struct djvMessagesDialog::Private
{
    Private(djvUIContext * context) :
        context(context)
    {}

    QStringList        list;
    QTextEdit *        widget       = nullptr;
    bool               show         = true;
    QCheckBox *        showCheckBox = nullptr;
    QDialogButtonBox * buttonBox    = nullptr;
    djvUIContext *    context      = nullptr;
};

//------------------------------------------------------------------------------
// djvMessagesDialog
//------------------------------------------------------------------------------

djvMessagesDialog::djvMessagesDialog(djvUIContext * context) :
    _p(new Private(context))
{
    //DJV_DEBUG("djvMessagesDialog::djvMessagesDialog");
    
    // Create the widgets.
    _p->widget = new QTextEdit;
    _p->widget->setLineWrapMode(QTextEdit::NoWrap);
    _p->widget->setReadOnly(true);
    
    _p->showCheckBox = new QCheckBox(
        qApp->translate("djvMessagesDialog", "Show"));
    
    QPushButton * clearButton = new QPushButton(
        qApp->translate("djvMessagesDialog", "Clear"));
    
    _p->buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    _p->buttonBox->addButton(_p->showCheckBox, QDialogButtonBox::ActionRole);
    _p->buttonBox->addButton(clearButton, QDialogButtonBox::ActionRole);
    
    // Layout the widgets.
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->addWidget(_p->widget);
    layout->addWidget(_p->buttonBox);

    // Preferences.
    djvPrefs prefs("djvMessagesDialog", djvPrefs::SYSTEM);
    prefs.get("show", _p->show);

    // Initialize.
    setWindowTitle(
        qApp->translate("djvMessagesDialog", "Messages Dialog"));
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    
    resize(400, 200);
    
    updateWidget();

    // Setup the callbacks.
    connect(_p->showCheckBox, SIGNAL(toggled(bool)), SLOT(showCallback(bool)));
    connect(clearButton, SIGNAL(clicked()), SLOT(clearCallback()));
    connect(_p->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(
        context->style(),
        SIGNAL(fontsChanged()),
        SLOT(updateWidget()));
}

djvMessagesDialog::~djvMessagesDialog()
{
    //DJV_DEBUG("djvMessagesDialog::~djvMessagesDialog");
    djvPrefs prefs("djvMessagesDialog", djvPrefs::SYSTEM);
    prefs.set("show", _p->show);
}

void djvMessagesDialog::message(const QString & in)
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

void djvMessagesDialog::message(const djvError & in)
{
    message(djvErrorUtil::format(in).join("\n"));
}

void djvMessagesDialog::showEvent(QShowEvent *)
{
    _p->buttonBox->button(QDialogButtonBox::Close)->setFocus(
        Qt::PopupFocusReason);
}

void djvMessagesDialog::clearCallback()
{
    _p->list.clear();
    updateWidget();
}

void djvMessagesDialog::showCallback(bool value)
{
    _p->show = value;
}

void djvMessagesDialog::updateWidget()
{
    //DJV_DEBUG("djvMessagesDialog::updateWidget");
    _p->widget->setFont(_p->context->style()->fonts().fixed);
    const QString text = _p->list.join("\n");
    //DJV_DEBUG_PRINT("text = " << text);
    _p->widget->setText(text);
    //DJV_DEBUG_PRINT("show = " << _p->show);
    _p->showCheckBox->setChecked(_p->show);
}
