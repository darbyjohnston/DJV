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

#include <djvStylePrefsWidget.h>

#include <djvColorSwatch.h>
#include <djvGuiContext.h>
#include <djvIntEdit.h>
#include <djvPrefsGroupBox.h>
#include <djvStyle.h>

#include <djvSignalBlocker.h>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFontComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvStylePrefsWidgetPrivate
//------------------------------------------------------------------------------

struct djvStylePrefsWidgetPrivate
{
    djvStylePrefsWidgetPrivate() :
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

djvStylePrefsWidget::djvStylePrefsWidget(djvGuiContext * context, QWidget * parent) :
    djvAbstractPrefsWidget(
        qApp->translate("djvStylePrefsWidget", "Style"), context, parent),
    _p(new djvStylePrefsWidgetPrivate)
{
    // Create the widgets.
    _p->colorWidget = new QComboBox;
    _p->colorWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->colorWidget->addItems(context->style()->paletteNames());

    _p->colorForegroundWidget = new djvColorSwatch(context);
    _p->colorForegroundWidget->setSwatchSize(djvColorSwatch::SWATCH_SMALL);
    _p->colorForegroundWidget->setColorDialogEnabled(true);
    _p->colorForegroundWidget->setToolTip(
        qApp->translate("djvStylePrefsWidget", "Foreground"));

    _p->colorBackgroundWidget = new djvColorSwatch(context);
    _p->colorBackgroundWidget->setSwatchSize(djvColorSwatch::SWATCH_SMALL);
    _p->colorBackgroundWidget->setColorDialogEnabled(true);
    _p->colorBackgroundWidget->setToolTip(
        qApp->translate("djvStylePrefsWidget", "Background"));

    _p->colorBackground2Widget = new djvColorSwatch(context);
    _p->colorBackground2Widget->setSwatchSize(djvColorSwatch::SWATCH_SMALL);
    _p->colorBackground2Widget->setColorDialogEnabled(true);
    _p->colorBackground2Widget->setToolTip(
        qApp->translate("djvStylePrefsWidget", "Background 2"));

    _p->colorButtonWidget = new djvColorSwatch(context);
    _p->colorButtonWidget->setSwatchSize(djvColorSwatch::SWATCH_SMALL);
    _p->colorButtonWidget->setColorDialogEnabled(true);
    _p->colorButtonWidget->setToolTip(
        qApp->translate("djvStylePrefsWidget", "Button"));

    _p->colorSelectWidget = new djvColorSwatch(context);
    _p->colorSelectWidget->setSwatchSize(djvColorSwatch::SWATCH_SMALL);
    _p->colorSelectWidget->setColorDialogEnabled(true);
    _p->colorSelectWidget->setToolTip(
        qApp->translate("djvStylePrefsWidget", "Select"));

    _p->colorSwatchTransparencyWidget = new QCheckBox(
        qApp->translate("djvStylePrefsWidget", "Show transparency in color swatches"));

    _p->sizeWidget = new QComboBox;
    _p->sizeWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->sizeWidget->addItems(context->style()->sizeMetricNames());

    _p->sizeValueWidget = new djvIntEdit;
    _p->sizeValueWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    _p->fontNormalWidget = new QFontComboBox;
    _p->fontNormalWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    _p->fontFixedWidget = new QFontComboBox;
    _p->fontFixedWidget->setFontFilters(QFontComboBox::MonospacedFonts);
    _p->fontFixedWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    // Layout the widgets.
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setSpacing(context->style()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvStylePrefsWidget", "Colors"), context);
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    QHBoxLayout * hLayout = new QHBoxLayout;
    hLayout->addWidget(_p->colorWidget);
    hLayout->addWidget(_p->colorForegroundWidget);
    hLayout->addWidget(_p->colorBackgroundWidget);
    hLayout->addWidget(_p->colorBackground2Widget);
    hLayout->addWidget(_p->colorButtonWidget);
    hLayout->addWidget(_p->colorSelectWidget);
    formLayout->addRow(
        qApp->translate("djvStylePrefsWidget", "Color:"),
        hLayout);
    formLayout->addRow(_p->colorSwatchTransparencyWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvStylePrefsWidget", "Size"), context);
    formLayout = prefsGroupBox->createLayout();
    hLayout = new QHBoxLayout;
    hLayout->setMargin(0);
    hLayout->addWidget(_p->sizeWidget);
    hLayout->addWidget(_p->sizeValueWidget);
    formLayout->addRow(
        qApp->translate("djvStylePrefsWidget", "Size:"),
        hLayout);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvStylePrefsWidget", "Fonts"), context);
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(
        qApp->translate("djvStylePrefsWidget", "Normal:"),
        _p->fontNormalWidget);
    formLayout->addRow(
        qApp->translate("djvStylePrefsWidget", "Fixed:"),
        _p->fontFixedWidget);
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
    context()->style()->setPalettes(
        djvStyle::palettesDefault());
    context()->style()->setPalettesIndex(
        djvStyle::palettesIndexDefault());
    context()->style()->setColorSwatchTransparency(
        djvStyle::colorSwatchTransparencyDefault());
    context()->style()->setSizeMetrics(
        djvStyle::sizeMetricsDefault());
    context()->style()->setSizeMetricsIndex(
        djvStyle::sizeMetricsIndexDefault());
    context()->style()->setFonts(
        djvStyle::fontsDefault());
    widgetUpdate();
}

void djvStylePrefsWidget::colorCallback(int index)
{
    context()->style()->setPalettesIndex(index);
    widgetUpdate();
}

void djvStylePrefsWidget::colorForegroundCallback(const djvColor & color)
{
    djvStyle::Palette tmp = context()->style()->palette();
    tmp.foreground = color;
    context()->style()->setPalette(tmp);
}

void djvStylePrefsWidget::colorBackgroundCallback(const djvColor & color)
{
    djvStyle::Palette tmp = context()->style()->palette();
    tmp.background = color;
    context()->style()->setPalette(tmp);
}

void djvStylePrefsWidget::colorBackground2Callback(const djvColor & color)
{
    djvStyle::Palette tmp = context()->style()->palette();
    tmp.background2 = color;
    context()->style()->setPalette(tmp);
}

void djvStylePrefsWidget::colorButtonCallback(const djvColor & color)
{
    djvStyle::Palette tmp = context()->style()->palette();
    tmp.button = color;
    context()->style()->setPalette(tmp);
}

void djvStylePrefsWidget::colorSelectCallback(const djvColor & color)
{
    djvStyle::Palette tmp = context()->style()->palette();
    tmp.select = color;
    context()->style()->setPalette(tmp);
}

void djvStylePrefsWidget::colorSwatchTransparencyCallback(bool in)
{
    context()->style()->setColorSwatchTransparency(in);
}

void djvStylePrefsWidget::sizeCallback(int index)
{
    context()->style()->setSizeMetricsIndex(index);
    widgetUpdate();
}

void djvStylePrefsWidget::sizeValueCallback(int size)
{
    context()->style()->setSizeMetric(
        djvStyle::SizeMetric(context()->style()->sizeMetric().name, size));
}

void djvStylePrefsWidget::fontNormalCallback(const QFont & font)
{
    djvStyle::Fonts fonts = context()->style()->fonts();
    fonts.normal = font;
    context()->style()->setFonts(fonts);
}

void djvStylePrefsWidget::fontFixedCallback(const QFont & font)
{
    djvStyle::Fonts fonts = context()->style()->fonts();
    fonts.fixed = font;
    context()->style()->setFonts(fonts);
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
    _p->colorWidget->setCurrentIndex(context()->style()->palettesIndex());
    _p->colorForegroundWidget->setColor(context()->style()->palette().foreground);
    _p->colorBackgroundWidget->setColor(context()->style()->palette().background);
    _p->colorBackground2Widget->setColor(context()->style()->palette().background2);
    _p->colorButtonWidget->setColor(context()->style()->palette().button);
    _p->colorSelectWidget->setColor(context()->style()->palette().select);
    _p->colorSwatchTransparencyWidget->setChecked(
        context()->style()->hasColorSwatchTransparency());
    _p->sizeWidget->setCurrentIndex(context()->style()->sizeMetricsIndex());
    _p->sizeValueWidget->setValue(context()->style()->sizeMetric().fontSize);
    _p->fontNormalWidget->setCurrentFont(context()->style()->fonts().normal);
    _p->fontFixedWidget->setCurrentFont(context()->style()->fonts().fixed);
}

