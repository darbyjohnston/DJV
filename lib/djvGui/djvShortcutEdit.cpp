//------------------------------------------------------------------------------
// Copyright (c) 2004-2014 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditi3ns, and the following disclaimer.
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

//! \file djvShortcutEdit.cpp

#include <djvShortcutEdit.h>

#include <djvIconLibrary.h>
#include <djvShortcut.h>
#include <djvSignalBlocker.h>
#include <djvToolButton.h>

#include <QHBoxLayout>
#include <QKeySequence>
#include <QKeySequenceEdit>

//------------------------------------------------------------------------------
// djvShortcutEdit::P
//------------------------------------------------------------------------------

struct djvShortcutEdit::P
{
    P() :
        widget     (0),
        clearButton(0)
    {}

    djvShortcut        shortcut;
    QKeySequenceEdit * widget;
    djvToolButton *    clearButton;
};

//------------------------------------------------------------------------------
// djvShortcutEdit
//------------------------------------------------------------------------------

djvShortcutEdit::djvShortcutEdit(QWidget * parent) :
    QWidget(parent),
    _p(new P)
{
    // Create the widgets.

    _p->widget = new QKeySequenceEdit;

    _p->clearButton = new djvToolButton;
    _p->clearButton->setIconSize(QSize(16, 16));
    _p->clearButton->setIcon(
        djvIconLibrary::global()->icon("djvResetIcon.png"));
    _p->clearButton->setToolTip("Clear the keyboard shortcut");

    // Layout the widgets.

    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(_p->widget, 1);
    layout->addWidget(_p->clearButton);

    // Initialize.

    setFocusProxy(_p->widget);

    widgetUpdate();

    // Setup the callbacks.

    connect(
        _p->widget,
        SIGNAL(keySequenceChanged(const QKeySequence &)),
        SLOT(widgetCallback(const QKeySequence &)));

    connect(
        _p->clearButton,
        SIGNAL(clicked()),
        _p->widget,
        SLOT(clear()));
}

djvShortcutEdit::~djvShortcutEdit()
{
    delete _p;
}

const djvShortcut & djvShortcutEdit::shortcut() const
{
    return _p->shortcut;
}

void djvShortcutEdit::setShortcut(const djvShortcut & shortcut)
{
    if (shortcut == _p->shortcut)
        return;

    _p->shortcut = shortcut;

    widgetUpdate();

    Q_EMIT shortcutChanged(_p->shortcut);
}

void djvShortcutEdit::widgetCallback(const QKeySequence & key)
{
    setShortcut(djvShortcut(_p->shortcut.name, key));
}

void djvShortcutEdit::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->widget <<
        _p->clearButton);

    _p->widget->setKeySequence(_p->shortcut.value);
}
