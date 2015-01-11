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

//! \file djvViewShortcutPrefsWidget.cpp

#include <djvViewShortcutPrefsWidget.h>

#include <djvViewShortcutPrefs.h>

#include <djvPrefsGroupBox.h>
#include <djvShortcutsWidget.h>

#include <djvSignalBlocker.h>

#include <QFormLayout>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvViewShortcutPrefsWidget::P
//------------------------------------------------------------------------------

struct djvViewShortcutPrefsWidget::P
{
    P() :
        shortcutsWidget(0)
    {}
    
    djvShortcutsWidget * shortcutsWidget;
};

//------------------------------------------------------------------------------
// djvViewShortcutPrefsWidget
//------------------------------------------------------------------------------

djvViewShortcutPrefsWidget::djvViewShortcutPrefsWidget() :
    djvViewAbstractPrefsWidget(tr("Shortcuts")),
    _p(new P)
{
    // Create the widgets.

    _p->shortcutsWidget = new djvShortcutsWidget;
    
    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(tr("Keyboard Shortcuts"));
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->shortcutsWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.

    widgetUpdate();

    // Setup the callbacks.

    connect(
        _p->shortcutsWidget,
        SIGNAL(shortcutsChanged(const QVector<djvShortcut> &)),
        SLOT(shortcutsCallback(const QVector<djvShortcut> &)));
}

djvViewShortcutPrefsWidget::~djvViewShortcutPrefsWidget()
{
    delete _p;
}

void djvViewShortcutPrefsWidget::resetPreferences()
{
    djvViewShortcutPrefs::global()->setShortcuts(
        djvViewShortcutPrefs::shortcutsDefault());
    
    widgetUpdate();
}

void djvViewShortcutPrefsWidget::shortcutsCallback(const QVector<djvShortcut> & in)
{
    djvViewShortcutPrefs::global()->setShortcuts(in);
}

void djvViewShortcutPrefsWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->shortcutsWidget);
    
    _p->shortcutsWidget->setShortcuts(
        djvViewShortcutPrefs::global()->shortcuts());
}

