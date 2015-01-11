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

//! \file djvViewPlaybackPrefsWidget.cpp

#include <djvViewPlaybackPrefsWidget.h>

#include <djvViewPlaybackPrefs.h>

#include <djvPrefsGroupBox.h>

#include <djvSignalBlocker.h>

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvViewPlaybackPrefsWidget::P
//------------------------------------------------------------------------------

struct djvViewPlaybackPrefsWidget::P
{
    P() :
        autoStartWidget (0),
        loopWidget      (0),
        everyFrameWidget(0),
        layoutWidget    (0)
    {}
    
    QCheckBox * autoStartWidget;
    QComboBox * loopWidget;
    QCheckBox * everyFrameWidget;
    QComboBox * layoutWidget;
};

//------------------------------------------------------------------------------
// djvViewPlaybackPrefsWidget
//------------------------------------------------------------------------------

djvViewPlaybackPrefsWidget::djvViewPlaybackPrefsWidget() :
    djvViewAbstractPrefsWidget(tr("Playback")),
    _p(new P)
{
    // Create the widgets.

    _p->autoStartWidget = new QCheckBox(tr("Start playback when opening files"));

    _p->loopWidget = new QComboBox;
    _p->loopWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->loopWidget->addItems(djvViewUtil::loopLabels());

    _p->everyFrameWidget = new QCheckBox(tr("Play every frame"));

    _p->layoutWidget = new QComboBox;
    _p->layoutWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->layoutWidget->addItems(djvViewUtil::layoutLabels());

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(tr("General"));
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->autoStartWidget);
    formLayout->addRow(tr("Loop mode:"), _p->loopWidget);
    formLayout->addRow(_p->everyFrameWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(tr("Layout"));
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(tr("Playback controls:"), _p->layoutWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.
    
    widgetUpdate();

    // Setup the callbacks.

    connect(
        _p->autoStartWidget,
        SIGNAL(toggled(bool)),
        SLOT(autoStartCallback(bool)));

    connect(
        _p->loopWidget,
        SIGNAL(activated(int)),
        SLOT(loopCallback(int)));

    connect(
        _p->everyFrameWidget,
        SIGNAL(toggled(bool)),
        SLOT(everyFrameCallback(bool)));

    connect(
        _p->layoutWidget,
        SIGNAL(activated(int)),
        SLOT(layoutCallback(int)));
}

djvViewPlaybackPrefsWidget::~djvViewPlaybackPrefsWidget()
{
    delete _p;
}

void djvViewPlaybackPrefsWidget::resetPreferences()
{
    djvViewPlaybackPrefs::global()->setAutoStart(
        djvViewPlaybackPrefs::autoStartDefault());
    djvViewPlaybackPrefs::global()->setLoop(
        djvViewPlaybackPrefs::loopDefault());
    djvViewPlaybackPrefs::global()->setEveryFrame(
        djvViewPlaybackPrefs::everyFrameDefault());
    djvViewPlaybackPrefs::global()->setLayout(
        djvViewPlaybackPrefs::layoutDefault());
    
    widgetUpdate();
}

void djvViewPlaybackPrefsWidget::autoStartCallback(bool in)
{
    djvViewPlaybackPrefs::global()->setAutoStart(in);
}

void djvViewPlaybackPrefsWidget::loopCallback(int in)
{
    djvViewPlaybackPrefs::global()->setLoop(
        static_cast<djvViewUtil::LOOP>(in));
}

void djvViewPlaybackPrefsWidget::everyFrameCallback(bool in)
{
    djvViewPlaybackPrefs::global()->setEveryFrame(in);
}

void djvViewPlaybackPrefsWidget::layoutCallback(int in)
{
    djvViewPlaybackPrefs::global()->setLayout(
        static_cast<djvViewUtil::LAYOUT>(in));
}

void djvViewPlaybackPrefsWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->autoStartWidget <<
        _p->loopWidget <<
        _p->everyFrameWidget <<
        _p->layoutWidget);
    
    _p->autoStartWidget->setChecked(
        djvViewPlaybackPrefs::global()->hasAutoStart());
    
    _p->loopWidget->setCurrentIndex(
        djvViewPlaybackPrefs::global()->loop());
    
    _p->everyFrameWidget->setChecked(
        djvViewPlaybackPrefs::global()->hasEveryFrame());
    
    _p->layoutWidget->setCurrentIndex(
        djvViewPlaybackPrefs::global()->layout());
}

