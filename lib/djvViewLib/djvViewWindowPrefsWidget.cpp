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

//! \file djvViewWindowPrefsWidget.cpp

#include <djvViewWindowPrefsWidget.h>

#include <djvViewWindowPrefs.h>

#include <djvPrefsGroupBox.h>
#include <djvVector2iEditWidget.h>

#include <djvSignalBlocker.h>

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
        autoFitWidget           (0),
        viewMaxWidget           (0),
        viewMaxUserWidget       (0),
        fullScreenControlsWidget(0),
        toolBarButtonGroup      (0)
    {}
    
    QCheckBox *             autoFitWidget;
    QComboBox *             viewMaxWidget;
    djvVector2iEditWidget * viewMaxUserWidget;
    QCheckBox *             fullScreenControlsWidget;
    QButtonGroup *          toolBarButtonGroup;
};

//------------------------------------------------------------------------------
// djvViewWindowPrefsWidget
//------------------------------------------------------------------------------

djvViewWindowPrefsWidget::djvViewWindowPrefsWidget() :
    djvViewAbstractPrefsWidget(tr("Windows")),
    _p(new P)
{
    // Create the size widgets.

    _p->autoFitWidget = new QCheckBox(
        tr("Automatically fit the window to the image"));

    _p->viewMaxWidget = new QComboBox;
    _p->viewMaxWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->viewMaxWidget->addItems(djvViewUtil::viewMaxLabels());

    _p->viewMaxUserWidget = new djvVector2iEditWidget;
    _p->viewMaxUserWidget->setRange(djvVector2i(100, 100), djvVector2i(8192, 8192));
    _p->viewMaxUserWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    // Create the full screen widgets.

    _p->fullScreenControlsWidget = new QCheckBox(
        tr("Set whether the controls are visible in full screen mode"));

    // Create the tool bar widgets.

    _p->toolBarButtonGroup = new QButtonGroup(this);
    _p->toolBarButtonGroup->setExclusive(false);
    
    for (int i = 0; i < djvViewUtil::toolBarLabels().count(); ++i)
    {
        QCheckBox * checkBox = new QCheckBox(djvViewUtil::toolBarLabels()[i]);
        
        _p->toolBarButtonGroup->addButton(checkBox, i);
    }

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(tr("Size"));
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->autoFitWidget);
    formLayout->addRow(tr("Maximum view size:"), _p->viewMaxWidget);
    formLayout->addRow("", _p->viewMaxUserWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(tr("Full Screen"));
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->fullScreenControlsWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(tr("Tool Bars"));
    formLayout = prefsGroupBox->createLayout();
    for (int i = 0; i < _p->toolBarButtonGroup->buttons().count(); ++i)
        formLayout->addRow(_p->toolBarButtonGroup->button(i));
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.
    
    widgetUpdate();

    // Setup the callbacks.

    connect(
        _p->autoFitWidget,
        SIGNAL(toggled(bool)),
        SLOT(autoFitCallback(bool)));
    
    connect(
        _p->viewMaxWidget,
        SIGNAL(activated(int)),
        SLOT(viewMaxCallback(int)));

    connect(
        _p->viewMaxUserWidget,
        SIGNAL(valueChanged(const djvVector2i &)),
        SLOT(viewMaxUserCallback(const djvVector2i &)));
    
    connect(
        _p->fullScreenControlsWidget,
        SIGNAL(toggled(bool)),
        SLOT(fullScreenControlsCallback(bool)));
    
    connect(
        _p->toolBarButtonGroup,
        SIGNAL(buttonClicked(int)),
        SLOT(toolBarCallback(int)));
}

djvViewWindowPrefsWidget::~djvViewWindowPrefsWidget()
{
    delete _p;
}

void djvViewWindowPrefsWidget::resetPreferences()
{
    djvViewWindowPrefs::global()->setAutoFit(
        djvViewWindowPrefs::autoFitDefault());
    djvViewWindowPrefs::global()->setViewMax(
        djvViewWindowPrefs::viewMaxDefault());
    djvViewWindowPrefs::global()->setViewMaxUser(
        djvViewWindowPrefs::viewMaxUserDefault());
    djvViewWindowPrefs::global()->setFullScreenControls(
        djvViewWindowPrefs::fullScreenControlsDefault());
    djvViewWindowPrefs::global()->setToolBar(
        djvViewWindowPrefs::toolBarDefault());
    
    widgetUpdate();
}

void djvViewWindowPrefsWidget::autoFitCallback(bool in)
{
    djvViewWindowPrefs::global()->setAutoFit(in);
}

void djvViewWindowPrefsWidget::viewMaxCallback(int in)
{
    djvViewWindowPrefs::global()->setViewMax(static_cast<djvViewUtil::VIEW_MAX>(in));
    
    widgetUpdate();
}

void djvViewWindowPrefsWidget::viewMaxUserCallback(const djvVector2i & in)
{
    djvViewWindowPrefs::global()->setViewMaxUser(in);
}

void djvViewWindowPrefsWidget::fullScreenControlsCallback(bool in)
{
    djvViewWindowPrefs::global()->setFullScreenControls(in);
}

void djvViewWindowPrefsWidget::toolBarCallback(int id)
{
    QVector<bool> visible = djvViewWindowPrefs::global()->toolBar();
    
    visible[id] = _p->toolBarButtonGroup->button(id)->isChecked();

    djvViewWindowPrefs::global()->setToolBar(visible);
}

void djvViewWindowPrefsWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->autoFitWidget <<
        _p->viewMaxWidget <<
        _p->viewMaxUserWidget <<
        _p->fullScreenControlsWidget <<
        _p->toolBarButtonGroup);

    _p->autoFitWidget->setChecked(
        djvViewWindowPrefs::global()->hasAutoFit());
    
    _p->viewMaxWidget->setCurrentIndex(
        djvViewWindowPrefs::global()->viewMax());
    
    _p->viewMaxUserWidget->setValue(
        djvViewWindowPrefs::global()->viewMaxUser());
    _p->viewMaxUserWidget->setVisible(
        djvViewUtil::VIEW_MAX_USER == djvViewWindowPrefs::global()->viewMax());
    
    _p->fullScreenControlsWidget->setChecked(
        djvViewWindowPrefs::global()->hasFullScreenControls());

    const QVector<bool> & visible = djvViewWindowPrefs::global()->toolBar();
    
    for (int i = 0; i < visible.count(); ++i)
    {
        _p->toolBarButtonGroup->button(i)->setChecked(visible[i]);
    }
}

