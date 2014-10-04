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

//! \file djvOpenGlPrefsWidget.cpp

#include <djvOpenGlPrefsWidget.h>

#include <djvOpenGlPrefs.h>
#include <djvPrefsGroupBox.h>
#include <djvStyle.h>

#include <djvSignalBlocker.h>

#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvOpenGlPrefsWidget::P
//------------------------------------------------------------------------------

struct djvOpenGlPrefsWidget::P
{
    P() :
       filterMinWidget(0),
       filterMagWidget(0)
    {}

    QComboBox * filterMinWidget;
    QComboBox * filterMagWidget;
};

//------------------------------------------------------------------------------
// djvOpenGlPrefsWidget
//------------------------------------------------------------------------------

djvOpenGlPrefsWidget::djvOpenGlPrefsWidget(QWidget * parent) :
    djvAbstractPrefsWidget("OpenGL", parent),
    _p(new P)
{
    // Create the filter widgets.

    _p->filterMinWidget = new QComboBox;
    _p->filterMinWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->filterMinWidget->addItems(djvOpenGlImageFilter::filterLabels());

    _p->filterMagWidget = new QComboBox;
    _p->filterMagWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->filterMagWidget->addItems(djvOpenGlImageFilter::filterLabels());

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setSpacing(djvStyle::global()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        "Render Filter",
        "Set the render filter quality. The filters \"Nearest\" and "
        "\"Linear\" are generally the fastest. The other filters can provide "
        "higher quality but are generally slower. The minify filter is "
        "used when zooming out, the magnify filter is used when zooming in.");
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow("Minify:", _p->filterMinWidget);
    formLayout->addRow("Magnify:", _p->filterMagWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.

    widgetUpdate();

    // Setup the callbacks.

    connect(
        _p->filterMinWidget,
        SIGNAL(activated(int)),
        SLOT(filterMinCallback(int)));

    connect(
        _p->filterMagWidget,
        SIGNAL(activated(int)),
        SLOT(filterMagCallback(int)));
}

djvOpenGlPrefsWidget::~djvOpenGlPrefsWidget()
{
    delete _p;
}

void djvOpenGlPrefsWidget::resetPreferences()
{
    djvOpenGlPrefs::global()->setFilter(djvOpenGlImageFilter::filterDefault());

    widgetUpdate();
}

void djvOpenGlPrefsWidget::filterMinCallback(int in)
{
    djvOpenGlPrefs::global()->setFilter(
        djvOpenGlImageFilter(
        static_cast<djvOpenGlImageFilter::FILTER>(in),
        djvOpenGlPrefs::global()->filter().mag));
}

void djvOpenGlPrefsWidget::filterMagCallback(int in)
{
    djvOpenGlPrefs::global()->setFilter(
        djvOpenGlImageFilter(
        djvOpenGlPrefs::global()->filter().min,
        static_cast<djvOpenGlImageFilter::FILTER>(in)));
}

void djvOpenGlPrefsWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->filterMinWidget <<
        _p->filterMagWidget);

    _p->filterMinWidget->setCurrentIndex(djvOpenGlPrefs::global()->filter().min);
    _p->filterMagWidget->setCurrentIndex(djvOpenGlPrefs::global()->filter().mag);
}
