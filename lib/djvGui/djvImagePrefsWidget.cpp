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

#include <djvImagePrefsWidget.h>

#include <djvGuiContext.h>
#include <djvImagePrefs.h>
#include <djvPrefsGroupBox.h>
#include <djvStyle.h>

#include <djvSignalBlocker.h>

#include <QApplication>
#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvImagePrefsWidgetPrivate
//------------------------------------------------------------------------------

struct djvImagePrefsWidgetPrivate
{
    djvImagePrefsWidgetPrivate() :
       filterMinWidget(0),
       filterMagWidget(0)
    {}

    QComboBox * filterMinWidget;
    QComboBox * filterMagWidget;
};

//------------------------------------------------------------------------------
// djvImagePrefsWidget
//------------------------------------------------------------------------------

djvImagePrefsWidget::djvImagePrefsWidget(djvGuiContext * context, QWidget * parent) :
    djvAbstractPrefsWidget(qApp->translate("djvImagePrefsWidget", "Images"), context, parent),
    _p(new djvImagePrefsWidgetPrivate)
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
    layout->setSpacing(context->style()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvImagePrefsWidget", "Scaling"),
        qApp->translate("djvImagePrefsWidget",
        "Set the image scaling quality. The filters \"Nearest\" and "
        "\"Linear\" are generally the fastest. The other filters can provide "
        "higher quality but are generally slower."),
        context);
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(
        qApp->translate("djvImagePrefsWidget", "Scale down:"),
        _p->filterMinWidget);
    formLayout->addRow(
        qApp->translate("djvImagePrefsWidget", "Scale up:"),
        _p->filterMagWidget);
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

djvImagePrefsWidget::~djvImagePrefsWidget()
{
    delete _p;
}

void djvImagePrefsWidget::resetPreferences()
{
    //DJV_DEBUG("djvImagePrefsWidget::resetPreferences");
    context()->imagePrefs()->setFilter(djvOpenGlImageFilter::filterDefault());
    widgetUpdate();
}

void djvImagePrefsWidget::filterMinCallback(int in)
{
    context()->imagePrefs()->setFilter(
        djvOpenGlImageFilter(
        static_cast<djvOpenGlImageFilter::FILTER>(in),
        context()->imagePrefs()->filter().mag));
    widgetUpdate();
}

void djvImagePrefsWidget::filterMagCallback(int in)
{
    context()->imagePrefs()->setFilter(
        djvOpenGlImageFilter(
        context()->imagePrefs()->filter().min,
        static_cast<djvOpenGlImageFilter::FILTER>(in)));
    widgetUpdate();
}

void djvImagePrefsWidget::widgetUpdate()
{
    //DJV_DEBUG("djvImagePrefsWidget::widgetUpdate");
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->filterMinWidget <<
        _p->filterMagWidget);
    //DJV_DEBUG_PRINT("filter = " << context()->imagePrefs()->filter());
    _p->filterMinWidget->setCurrentIndex(context()->imagePrefs()->filter().min);
    _p->filterMagWidget->setCurrentIndex(context()->imagePrefs()->filter().mag);
}

