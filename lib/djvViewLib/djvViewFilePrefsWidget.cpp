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

#include <djvViewContext.h>
#include <djvViewFilePrefs.h>
#include <djvViewMiscWidget.h>

#include <djvPrefs.h>
#include <djvPrefsGroupBox.h>

#include <djvFileInfoUtil.h>
#include <djvListUtil.h>
#include <djvSignalBlocker.h>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvViewFilePrefsWidgetPrivate
//------------------------------------------------------------------------------

struct djvViewFilePrefsWidgetPrivate
{
    djvViewFilePrefsWidgetPrivate() :
        autoSequenceWidget(0),
        proxyWidget       (0),
        u8ConversionWidget(0),
        cacheWidget       (0),
        cacheSizeWidget   (0),
        preloadWidget     (0),
        displayCacheWidget(0)
    {}
    
    QCheckBox *              autoSequenceWidget;
    QComboBox *              proxyWidget;
    QCheckBox *              u8ConversionWidget;
    QCheckBox *              cacheWidget;
    djvViewCacheSizeWidget * cacheSizeWidget;
    QCheckBox *              preloadWidget;
    QCheckBox *              displayCacheWidget;
};

//------------------------------------------------------------------------------
// djvViewFilePrefsWidget
//------------------------------------------------------------------------------

djvViewFilePrefsWidget::djvViewFilePrefsWidget(djvViewContext * context) :
    djvViewAbstractPrefsWidget(
        qApp->translate("djvViewFilePrefsWidget", "Files"), context),
    _p(new djvViewFilePrefsWidgetPrivate)
{
    // Create the options widgets.

    _p->autoSequenceWidget = new QCheckBox(
        qApp->translate("djvViewFilePrefsWidget", "Automatically detect sequences when opening files"));

    // Create the proxy scale widgets.

    _p->proxyWidget = new QComboBox;
    _p->proxyWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->proxyWidget->addItems(djvPixelDataInfo::proxyLabels());

    // Create the convert to 8-bits widgets.

    _p->u8ConversionWidget = new QCheckBox(
        qApp->translate("djvViewFilePrefsWidget", "Enable 8-bit conversion"));

    // Create the file cache widgets.

    _p->cacheWidget = new QCheckBox(
        qApp->translate("djvViewFilePrefsWidget", "Enable the memory cache"));

    _p->cacheSizeWidget = new djvViewCacheSizeWidget(context);

    _p->preloadWidget = new QCheckBox(
        qApp->translate("djvViewFilePrefsWidget", "Pre-load cache frames"));

    _p->displayCacheWidget = new QCheckBox(
        qApp->translate("djvViewFilePrefsWidget", "Display cached frames in timeline"));

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvViewFilePrefsWidget", "Files"), context);
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->autoSequenceWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvViewFilePrefsWidget", "Proxy Scale"),
        qApp->translate("djvViewFilePrefsWidget",
        "Set proxy scaling to reduce the resolution when loading images. This "
        "allows more images to be stored in the memory cache at the expense of "
        "image quality. Proxy scaling can also improve playback speed since the "
        "images are smaller."),
        context);
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(
        qApp->translate("djvViewFilePrefsWidget", "Proxy scale:"),
        _p->proxyWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvViewFilePrefsWidget", "8-bit Conversion"),
        qApp->translate("djvViewFilePrefsWidget",
        "Set 8-bit conversion to allow more images to be stored in the memory "
        "cache at the expense of image quality."),
        context);
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->u8ConversionWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvViewFilePrefsWidget", "Memory Cache"),
        qApp->translate("djvViewFilePrefsWidget",
        "The memory cache allows the application to store images in memory "
        "which can improve playback performance. When the memory cache is "
        "disabled the images are streamed directly from disk."),
        context);
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->cacheWidget);
    formLayout->addRow(
        qApp->translate("djvViewFilePrefsWidget", "Cache size (gigabytes):"),
        _p->cacheSizeWidget);
    formLayout->addRow(_p->preloadWidget);
    formLayout->addRow(_p->displayCacheWidget);
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
        _p->preloadWidget,
        SIGNAL(toggled(bool)),
        SLOT(preloadCallback(bool)));

    connect(
        _p->displayCacheWidget,
        SIGNAL(toggled(bool)),
        SLOT(displayCacheCallback(bool)));
}

djvViewFilePrefsWidget::~djvViewFilePrefsWidget()
{
    delete _p;
}

void djvViewFilePrefsWidget::resetPreferences()
{
    context()->filePrefs()->setAutoSequence(
        djvViewFilePrefs::autoSequenceDefault());
    context()->filePrefs()->setProxy(
        djvViewFilePrefs::proxyDefault());
    context()->filePrefs()->setU8Conversion(
        djvViewFilePrefs::u8ConversionDefault());
    context()->filePrefs()->setCache(
        djvViewFilePrefs::cacheDefault());
    context()->filePrefs()->setCacheSize(
        djvViewFilePrefs::cacheSizeDefault());
    context()->filePrefs()->setPreload(
        djvViewFilePrefs::preloadDefault());
    context()->filePrefs()->setDisplayCache(
        djvViewFilePrefs::displayCacheDefault());

    widgetUpdate();
}

void djvViewFilePrefsWidget::autoSequenceCallback(bool in)
{
    context()->filePrefs()->setAutoSequence(in);
}

void djvViewFilePrefsWidget::proxyCallback(int in)
{
    context()->filePrefs()->setProxy(static_cast<djvPixelDataInfo::PROXY>(in));
}

void djvViewFilePrefsWidget::u8ConversionCallback(bool in)
{
    context()->filePrefs()->setU8Conversion(in);
}

void djvViewFilePrefsWidget::cacheCallback(bool in)
{
    context()->filePrefs()->setCache(in);
}

void djvViewFilePrefsWidget::cacheSizeCallback(double in)
{
    context()->filePrefs()->setCacheSize(in);
}

void djvViewFilePrefsWidget::preloadCallback(bool in)
{
    context()->filePrefs()->setPreload(in);
}

void djvViewFilePrefsWidget::displayCacheCallback(bool in)
{
    context()->filePrefs()->setDisplayCache(in);
}

void djvViewFilePrefsWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->autoSequenceWidget <<
        _p->proxyWidget <<
        _p->u8ConversionWidget <<
        _p->cacheWidget <<
        _p->cacheSizeWidget <<
        _p->preloadWidget <<
        _p->displayCacheWidget);
    
    _p->autoSequenceWidget->setChecked(
        context()->filePrefs()->hasAutoSequence());
    
    _p->proxyWidget->setCurrentIndex(
        context()->filePrefs()->proxy());
    
    _p->u8ConversionWidget->setChecked(
        context()->filePrefs()->hasU8Conversion());
    
    _p->cacheWidget->setChecked(
        context()->filePrefs()->hasCache());
    
    _p->cacheSizeWidget->setCacheSize(
        context()->filePrefs()->cacheSize());

    _p->preloadWidget->setChecked(
        context()->filePrefs()->hasPreload());

    _p->displayCacheWidget->setChecked(
        context()->filePrefs()->hasDisplayCache());
}
