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

//! \file djvStylePrefsWidget.cpp

#include <djvStylePrefsWidget.h>

#include <djvColorSwatch.h>
#include <djvIntEdit.h>
#include <djvPrefsGroupBox.h>
#include <djvStyle.h>

#include <djvSignalBlocker.h>

#include <QCheckBox>
#include <QComboBox>
#include <QFontComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvStylePrefsWidget::P
//------------------------------------------------------------------------------

struct djvStylePrefsWidget::P
{
    P() :
        colorForegroundWidget        (0),
        colorBackgroundWidget        (0),
        colorBackground2Widget       (0),
        colorButtonWidget            (0),
        colorSelectWidget            (0),
        colorSwatchTransparencyWidget(0),
        sizeValueWidget              (0),
        fontNormalWidget             (0),
        fontFixedWidget              (0)
    {}

    QComboBox *      colorWidget;
    djvColorSwatch * colorForegroundWidget;
    djvColorSwatch * colorBackgroundWidget;
    djvColorSwatch * colorBackground2Widget;
    djvColorSwatch * colorButtonWidget;
    djvColorSwatch * colorSelectWidget;
    QCheckBox *      colorSwatchTransparencyWidget;
    QComboBox *      sizeWidget;
    djvIntEdit *     sizeValueWidget;
    QFontComboBox *  fontNormalWidget;
    QFontComboBox *  fontFixedWidget;
};

//------------------------------------------------------------------------------
// djvStylePrefsWidget
//------------------------------------------------------------------------------

djvStylePrefsWidget::djvStylePrefsWidget(QWidget * parent) :
    djvAbstractPrefsWidget(tr("Style"), parent),
    _p(new P)
{
    // Create the widgets.
    
    _p->colorWidget = new QComboBox;
    _p->colorWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->colorWidget->addItems(djvStyle::global()->paletteNames());

    _p->colorForegroundWidget = new djvColorSwatch;
    _p->colorForegroundWidget->setSwatchSize(djvColorSwatch::SWATCH_SMALL);
    _p->colorForegroundWidget->setColorDialogEnabled(true);
    _p->colorForegroundWidget->setToolTip(tr("Foreground"));

    _p->colorBackgroundWidget = new djvColorSwatch;
    _p->colorBackgroundWidget->setSwatchSize(djvColorSwatch::SWATCH_SMALL);
    _p->colorBackgroundWidget->setColorDialogEnabled(true);
    _p->colorBackgroundWidget->setToolTip(tr("Background"));

    _p->colorBackground2Widget = new djvColorSwatch;
    _p->colorBackground2Widget->setSwatchSize(djvColorSwatch::SWATCH_SMALL);
    _p->colorBackground2Widget->setColorDialogEnabled(true);
    _p->colorBackground2Widget->setToolTip(tr("Background 2"));

    _p->colorButtonWidget = new djvColorSwatch;
    _p->colorButtonWidget->setSwatchSize(djvColorSwatch::SWATCH_SMALL);
    _p->colorButtonWidget->setColorDialogEnabled(true);
    _p->colorButtonWidget->setToolTip(tr("Button"));

    _p->colorSelectWidget = new djvColorSwatch;
    _p->colorSelectWidget->setSwatchSize(djvColorSwatch::SWATCH_SMALL);
    _p->colorSelectWidget->setColorDialogEnabled(true);
    _p->colorSelectWidget->setToolTip(tr("Select"));

    _p->colorSwatchTransparencyWidget =
        new QCheckBox(tr("Show transparency in color swatches"));

    _p->sizeWidget = new QComboBox;
    _p->sizeWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->sizeWidget->addItems(djvStyle::global()->sizeMetricNames());

    _p->sizeValueWidget = new djvIntEdit;
    _p->sizeValueWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    _p->fontNormalWidget = new QFontComboBox;
    _p->fontNormalWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    _p->fontFixedWidget = new QFontComboBox;
    _p->fontFixedWidget->setFontFilters(QFontComboBox::MonospacedFonts);
    _p->fontFixedWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setSpacing(djvStyle::global()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(tr("Colors"));
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    QHBoxLayout * hLayout = new QHBoxLayout;
    hLayout->addWidget(_p->colorWidget);
    hLayout->addWidget(_p->colorForegroundWidget);
    hLayout->addWidget(_p->colorBackgroundWidget);
    hLayout->addWidget(_p->colorBackground2Widget);
    hLayout->addWidget(_p->colorButtonWidget);
    hLayout->addWidget(_p->colorSelectWidget);
    formLayout->addRow(hLayout);
    formLayout->addRow(_p->colorSwatchTransparencyWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(tr("Size"));
    formLayout = prefsGroupBox->createLayout();
    hLayout = new QHBoxLayout;
    hLayout->setMargin(0);
    hLayout->addWidget(_p->sizeWidget);
    hLayout->addWidget(_p->sizeValueWidget);
    formLayout->addRow(hLayout);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(tr("Fonts"));
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(tr("Normal:"), _p->fontNormalWidget);
    formLayout->addRow(tr("Fixed:"), _p->fontFixedWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.

    widgetUpdate();

    // Setup the callbacks.

    connect(
        _p->colorWidget,
        SIGNAL(activated(int)),
        SLOT(colorCallback(int)));

    connect(
        _p->colorForegroundWidget,
        SIGNAL(colorChanged(const djvColor &)),
        SLOT(colorForegroundCallback(const djvColor &)));

    connect(
        _p->colorBackgroundWidget,
        SIGNAL(colorChanged(const djvColor &)),
        SLOT(colorBackgroundCallback(const djvColor &)));

    connect(
        _p->colorBackground2Widget,
        SIGNAL(colorChanged(const djvColor &)),
        SLOT(colorBackground2Callback(const djvColor &)));

    connect(
        _p->colorButtonWidget,
        SIGNAL(colorChanged(const djvColor &)),
        SLOT(colorButtonCallback(const djvColor &)));

    connect(
        _p->colorSelectWidget,
        SIGNAL(colorChanged(const djvColor &)),
        SLOT(colorSelectCallback(const djvColor &)));
    
    connect(
        _p->colorSwatchTransparencyWidget,
        SIGNAL(toggled(bool)),
        SLOT(colorSwatchTransparencyCallback(bool)));

    connect(
        _p->sizeWidget,
        SIGNAL(activated(int)),
        SLOT(sizeCallback(int)));

    connect(
        _p->sizeValueWidget,
        SIGNAL(valueChanged(int)),
        SLOT(sizeValueCallback(int)));
    
    connect(
        _p->fontNormalWidget,
        SIGNAL(currentFontChanged(const QFont &)),
        SLOT(fontNormalCallback(const QFont &)));
    
    connect(
        _p->fontFixedWidget,
        SIGNAL(currentFontChanged(const QFont &)),
        SLOT(fontFixedCallback(const QFont &)));
}

djvStylePrefsWidget::~djvStylePrefsWidget()
{
    delete _p;
}

void djvStylePrefsWidget::resetPreferences()
{
    djvStyle::global()->setPalettes(
        djvStyle::palettesDefault());
    djvStyle::global()->setPalettesIndex(
        djvStyle::palettesIndexDefault());
    djvStyle::global()->setColorSwatchTransparency(
        djvStyle::colorSwatchTransparencyDefault());
    djvStyle::global()->setSizeMetrics(
        djvStyle::sizeMetricsDefault());
    djvStyle::global()->setSizeMetricsIndex(
        djvStyle::sizeMetricsIndexDefault());
    djvStyle::global()->setFonts(
        djvStyle::fontsDefault());
    
    widgetUpdate();
}

void djvStylePrefsWidget::colorCallback(int index)
{
    djvStyle::global()->setPalettesIndex(index);

    widgetUpdate();
}

void djvStylePrefsWidget::colorForegroundCallback(const djvColor & color)
{
    djvStyle::Palette tmp = djvStyle::global()->palette();
    tmp.foreground = color;
    djvStyle::global()->setPalette(tmp);
}

void djvStylePrefsWidget::colorBackgroundCallback(const djvColor & color)
{
    djvStyle::Palette tmp = djvStyle::global()->palette();
    tmp.background = color;
    djvStyle::global()->setPalette(tmp);
}

void djvStylePrefsWidget::colorBackground2Callback(const djvColor & color)
{
    djvStyle::Palette tmp = djvStyle::global()->palette();
    tmp.background2 = color;
    djvStyle::global()->setPalette(tmp);
}

void djvStylePrefsWidget::colorButtonCallback(const djvColor & color)
{
    djvStyle::Palette tmp = djvStyle::global()->palette();
    tmp.button = color;
    djvStyle::global()->setPalette(tmp);
}

void djvStylePrefsWidget::colorSelectCallback(const djvColor & color)
{
    djvStyle::Palette tmp = djvStyle::global()->palette();
    tmp.select = color;
    djvStyle::global()->setPalette(tmp);
}

void djvStylePrefsWidget::colorSwatchTransparencyCallback(bool in)
{
    djvStyle::global()->setColorSwatchTransparency(in);
}

void djvStylePrefsWidget::sizeCallback(int index)
{
    djvStyle::global()->setSizeMetricsIndex(index);

    widgetUpdate();
}

void djvStylePrefsWidget::sizeValueCallback(int size)
{
    djvStyle::global()->setSizeMetric(
        djvStyle::SizeMetric(djvStyle::global()->sizeMetric().name, size));
}

void djvStylePrefsWidget::fontNormalCallback(const QFont & font)
{
    djvStyle::Fonts fonts = djvStyle::global()->fonts();
    fonts.normal = font;
    djvStyle::global()->setFonts(fonts);
}

void djvStylePrefsWidget::fontFixedCallback(const QFont & font)
{
    djvStyle::Fonts fonts = djvStyle::global()->fonts();
    fonts.fixed = font;
    djvStyle::global()->setFonts(fonts);
}

void djvStylePrefsWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->colorWidget <<
        _p->colorForegroundWidget <<
        _p->colorBackgroundWidget <<
        _p->colorBackground2Widget <<
        _p->colorButtonWidget <<
        _p->colorSelectWidget <<
        _p->colorSwatchTransparencyWidget <<
        _p->sizeWidget <<
        _p->sizeValueWidget <<
        _p->fontNormalWidget <<
        _p->fontFixedWidget);

    _p->colorWidget->setCurrentIndex(djvStyle::global()->palettesIndex());

    _p->colorForegroundWidget->setColor(djvStyle::global()->palette().foreground);
    _p->colorBackgroundWidget->setColor(djvStyle::global()->palette().background);
    _p->colorBackground2Widget->setColor(djvStyle::global()->palette().background2);
    _p->colorButtonWidget->setColor(djvStyle::global()->palette().button);
    _p->colorSelectWidget->setColor(djvStyle::global()->palette().select);

    _p->colorSwatchTransparencyWidget->setChecked(
        djvStyle::global()->hasColorSwatchTransparency());

    _p->sizeWidget->setCurrentIndex(djvStyle::global()->sizeMetricsIndex());
    _p->sizeValueWidget->setValue(djvStyle::global()->sizeMetric().fontSize);
    
    _p->fontNormalWidget->setCurrentFont(djvStyle::global()->fonts().normal);
    _p->fontFixedWidget->setCurrentFont(djvStyle::global()->fonts().fixed);
}

