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

//! \file djvViewFilePrefsWidget.cpp

#include <djvViewFilePrefsWidget.h>

#include <djvViewFilePrefs.h>

#include <djvViewMiscWidget.h>

#include <djvPrefs.h>
#include <djvPrefsGroupBox.h>

#include <djvFileInfoUtil.h>
#include <djvListUtil.h>
#include <djvSignalBlocker.h>

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvViewFilePrefsWidget::P
//------------------------------------------------------------------------------

struct djvViewFilePrefsWidget::P
{
    P() :
        autoSequenceWidget      (0),
        proxyWidget             (0),
        u8ConversionWidget      (0),
        cacheWidget             (0),
        cacheSizeWidget         (0),
        cacheDisplayWidget      (0)
    {}
    
    QCheckBox *              autoSequenceWidget;
    QComboBox *              proxyWidget;
    QCheckBox *              u8ConversionWidget;
    QCheckBox *              cacheWidget;
    djvViewCacheSizeWidget * cacheSizeWidget;
    QCheckBox *              cacheDisplayWidget;
};

//------------------------------------------------------------------------------
// djvViewFilePrefsWidget
//------------------------------------------------------------------------------

djvViewFilePrefsWidget::djvViewFilePrefsWidget() :
    djvViewAbstractPrefsWidget(tr("Files")),
    _p(new P)
{
    // Create the options widgets.

    _p->autoSequenceWidget = new QCheckBox(
        tr("Automatically detect sequences when opening files"));

    // Create the proxy scale widgets.

    _p->proxyWidget = new QComboBox;
    _p->proxyWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->proxyWidget->addItems(djvPixelDataInfo::proxyLabels());

    // Create the convert to 8-bits widgets.

    _p->u8ConversionWidget = new QCheckBox(tr("Enable 8-bit conversion"));

    // Create the file cache widgets.

    _p->cacheWidget = new QCheckBox(tr("Enable the memory cache"));

    _p->cacheSizeWidget = new djvViewCacheSizeWidget;

    _p->cacheDisplayWidget = new QCheckBox(
        tr("Display cached frames in timeline"));

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(tr("General"));
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->autoSequenceWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        tr("Proxy Scale"),
        tr("Proxy scaling reduces the resolution when loading files to allow "
        "more images to be stored in the memory cache at the expense of image "
        "quality. Proxy scaling can also improve playback speed since the "
        "images are smaller."));
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->proxyWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        tr("Conversion"),
        tr("This option converts images to 8-bits when loading files to allow "
        "more images to be stored in the memory cache at the expense of image "
        "quality."));
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->u8ConversionWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        tr("Memory Cache"),
        tr("The memory cache allows the application to store images in memory "
        "which can improve playback performance. When the memory cache is "
        "disabled the images are streamed directly from disk."));
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->cacheWidget);
    formLayout->addRow(tr("Cache size (gigabytes):"), _p->cacheSizeWidget);
    formLayout->addRow(_p->cacheDisplayWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.
    
    widgetUpdate();

    // Setup the callbacks.

    connect(
        _p->autoSequenceWidget,
        SIGNAL(toggled(bool)),
        SLOT(autoSequenceCallback(bool)));

    connect(
        _p->proxyWidget,
        SIGNAL(activated(int)),
        SLOT(proxyCallback(int)));

    connect(
        _p->u8ConversionWidget,
        SIGNAL(toggled(bool)),
        SLOT(u8ConversionCallback(bool)));

    connect(
        _p->cacheWidget,
        SIGNAL(toggled(bool)),
        SLOT(cacheCallback(bool)));

    connect(
        _p->cacheSizeWidget,
        SIGNAL(cacheSizeChanged(double)),
        SLOT(cacheSizeCallback(double)));

    connect(
        _p->cacheDisplayWidget,
        SIGNAL(toggled(bool)),
        SLOT(cacheDisplayCallback(bool)));
}

djvViewFilePrefsWidget::~djvViewFilePrefsWidget()
{
    delete _p;
}

void djvViewFilePrefsWidget::resetPreferences()
{
    djvViewFilePrefs::global()->setAutoSequence(
        djvViewFilePrefs::autoSequenceDefault());
    djvViewFilePrefs::global()->setProxy(
        djvViewFilePrefs::proxyDefault());
    djvViewFilePrefs::global()->setU8Conversion(
        djvViewFilePrefs::u8ConversionDefault());
    djvViewFilePrefs::global()->setCacheEnabled(
        djvViewFilePrefs::cacheEnabledDefault());
    djvViewFilePrefs::global()->setCacheSize(
        djvViewFilePrefs::cacheSizeDefault());
    djvViewFilePrefs::global()->setCacheDisplay(
        djvViewFilePrefs::cacheDisplayDefault());
    
    widgetUpdate();
}

void djvViewFilePrefsWidget::autoSequenceCallback(bool in)
{
    djvViewFilePrefs::global()->setAutoSequence(in);
}

void djvViewFilePrefsWidget::proxyCallback(int in)
{
    djvViewFilePrefs::global()->setProxy(static_cast<djvPixelDataInfo::PROXY>(in));
}

void djvViewFilePrefsWidget::u8ConversionCallback(bool in)
{
    djvViewFilePrefs::global()->setU8Conversion(in);
}

void djvViewFilePrefsWidget::cacheCallback(bool in)
{
    djvViewFilePrefs::global()->setCacheEnabled(in);
}

void djvViewFilePrefsWidget::cacheSizeCallback(double in)
{
    djvViewFilePrefs::global()->setCacheSize(in);
}

void djvViewFilePrefsWidget::cacheDisplayCallback(bool in)
{
    djvViewFilePrefs::global()->setCacheDisplay(in);
}

void djvViewFilePrefsWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->autoSequenceWidget <<
        _p->proxyWidget <<
        _p->u8ConversionWidget <<
        _p->cacheWidget <<
        _p->cacheSizeWidget <<
        _p->cacheDisplayWidget);
    
    _p->autoSequenceWidget->setChecked(
        djvViewFilePrefs::global()->hasAutoSequence());
    
    _p->proxyWidget->setCurrentIndex(
        djvViewFilePrefs::global()->proxy());
    
    _p->u8ConversionWidget->setChecked(
        djvViewFilePrefs::global()->hasU8Conversion());
    
    _p->cacheWidget->setChecked(
        djvViewFilePrefs::global()->isCacheEnabled());
    
    _p->cacheSizeWidget->setCacheSize(
        djvViewFilePrefs::global()->cacheSize());
    
    _p->cacheDisplayWidget->setChecked(
        djvViewFilePrefs::global()->hasCacheDisplay());
}
