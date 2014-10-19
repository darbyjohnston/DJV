//------------------------------------------------------------------------------
// Copyright (c) 2004-2014 Darby Johnston
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

//! \file djvViewWindowPrefsWidget.cpp

#include <djvViewWindowPrefsWidget.h>

#include <djvViewWindowPrefs.h>

#include <djvPrefsGroupBox.h>

#include <djvSignalBlocker.h>
#include <djvStringUtil.h>

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvViewWindowPrefsWidget::P
//------------------------------------------------------------------------------

struct djvViewWindowPrefsWidget::P
{
    P() :
        resizeFitWidget         (0),
        resizeMaxWidget         (0),
        fullScreenControlsWidget(0),
        toolBarButtonGroup      (0)
    {}
    
    QCheckBox *    resizeFitWidget;
    QComboBox *    resizeMaxWidget;
    QCheckBox *    fullScreenControlsWidget;
    QButtonGroup * toolBarButtonGroup;
};

//------------------------------------------------------------------------------
// djvViewWindowPrefsWidget
//------------------------------------------------------------------------------

djvViewWindowPrefsWidget::djvViewWindowPrefsWidget() :
    djvViewAbstractPrefsWidget("Windows"),
    _p(new P)
{
    // Create the resize widgets.

    _p->resizeFitWidget = new QCheckBox("Fit window to image");

    _p->resizeMaxWidget = new QComboBox;
    _p->resizeMaxWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->resizeMaxWidget->addItems(djvView::windowResizeMaxLabels());

    // Create the full screen widgets.

    _p->fullScreenControlsWidget = new QCheckBox(
        "Set whether the controls are visible when going full screen");

    // Create the tool bar widgets.

    _p->toolBarButtonGroup = new QButtonGroup(this);
    _p->toolBarButtonGroup->setExclusive(false);
    
    for (int i = 0; i < djvView::toolBarLabels().count(); ++i)
    {
        QCheckBox * checkBox = new QCheckBox(djvView::toolBarLabels()[i]);
        
        _p->toolBarButtonGroup->addButton(checkBox, i);
    }

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        "Resizing",
        "Set the behavior of the window when it resizes.");
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->resizeFitWidget);
    formLayout->addRow("Maximum screen size:", _p->resizeMaxWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        "Full Screen",
        "Set general full screen options.");
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->fullScreenControlsWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        "Tool Bar",
        "Set the tool bar visibility.");
    formLayout = prefsGroupBox->createLayout();
    for (int i = 0; i < _p->toolBarButtonGroup->buttons().count(); ++i)
        formLayout->addRow(_p->toolBarButtonGroup->button(i));
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.
    
    widgetUpdate();

    // Setup the callbacks.

    connect(
        _p->resizeFitWidget,
        SIGNAL(toggled(bool)),
        SLOT(resizeFitCallback(bool)));
    
    connect(
        _p->resizeMaxWidget,
        SIGNAL(activated(int)),
        SLOT(resizeMaxCallback(int)));

    connect(
        _p->fullScreenControlsWidget,
        SIGNAL(toggled(bool)),
        SLOT(fullScreenControlsCallback(bool)));
    
    connect(
        _p->toolBarButtonGroup,
        SIGNAL(buttonToggled(int, bool)),
        SLOT(toolBarCallback(int, bool)));
}

djvViewWindowPrefsWidget::~djvViewWindowPrefsWidget()
{
    delete _p;
}

void djvViewWindowPrefsWidget::resetPreferences()
{
    djvViewWindowPrefs::global()->setResizeFit(
        djvViewWindowPrefs::resizeFitDefault());
    djvViewWindowPrefs::global()->setResizeMax(
        djvViewWindowPrefs::resizeMaxDefault());
    djvViewWindowPrefs::global()->setFullScreenControls(
        djvViewWindowPrefs::fullScreenControlsDefault());
    djvViewWindowPrefs::global()->setToolBar(
        djvViewWindowPrefs::toolBarDefault());
    
    widgetUpdate();
}

void djvViewWindowPrefsWidget::resizeFitCallback(bool in)
{
    djvViewWindowPrefs::global()->setResizeFit(in);
}

void djvViewWindowPrefsWidget::resizeMaxCallback(int in)
{
    djvViewWindowPrefs::global()->setResizeMax(
        static_cast<djvView::WINDOW_RESIZE_MAX>(in));
}

void djvViewWindowPrefsWidget::fullScreenControlsCallback(bool in)
{
    djvViewWindowPrefs::global()->setFullScreenControls(in);
}

void djvViewWindowPrefsWidget::toolBarCallback(int id, bool toggled)
{
    QVector<bool> visible = djvViewWindowPrefs::global()->toolBar();
    
    visible[id] = toggled;

    djvViewWindowPrefs::global()->setToolBar(visible);
}

void djvViewWindowPrefsWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->resizeFitWidget <<
        _p->resizeMaxWidget <<
        _p->fullScreenControlsWidget <<
        _p->toolBarButtonGroup);

    _p->resizeFitWidget->setChecked(
        djvViewWindowPrefs::global()->hasResizeFit());
    
    _p->resizeMaxWidget->setCurrentIndex(
        djvViewWindowPrefs::global()->resizeMax());
    
    _p->fullScreenControlsWidget->setChecked(
        djvViewWindowPrefs::global()->hasFullScreenControls());

    const QVector<bool> & visible = djvViewWindowPrefs::global()->toolBar();
    
    for (int i = 0; i < visible.count(); ++i)
    {
        _p->toolBarButtonGroup->button(i)->setChecked(visible[i]);
    }
}

