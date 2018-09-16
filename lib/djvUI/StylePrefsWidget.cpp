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

#include <djvUI/StylePrefsWidget.h>

#include <djvUI/ColorSwatch.h>
#include <djvUI/UIContext.h>
#include <djvUI/IntEdit.h>
#include <djvUI/PrefsGroupBox.h>
#include <djvUI/Style.h>

#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFontComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        struct StylePrefsWidget::Private
        {
            QComboBox * colorWidget = nullptr;
            ColorSwatch * colorForegroundWidget = nullptr;
            ColorSwatch * colorBackgroundWidget = nullptr;
            ColorSwatch * colorBackground2Widget = nullptr;
            ColorSwatch * colorButtonWidget = nullptr;
            ColorSwatch * colorSelectWidget = nullptr;
            QCheckBox * colorSwatchTransparencyWidget = nullptr;
            QComboBox * sizeWidget = nullptr;
            IntEdit * sizeValueWidget = nullptr;
            QFontComboBox *  fontNormalWidget = nullptr;
            QFontComboBox *  fontFixedWidget = nullptr;
        };

        StylePrefsWidget::StylePrefsWidget(UIContext * context, QWidget * parent) :
            AbstractPrefsWidget(qApp->translate("djv::UI::StylePrefsWidget", "Style"), context, parent),
            _p(new Private)
        {
            // Create the widgets.
            _p->colorWidget = new QComboBox;
            _p->colorWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->colorWidget->addItems(context->style()->paletteNames());

            _p->colorForegroundWidget = new ColorSwatch(context);
            _p->colorForegroundWidget->setSwatchSize(ColorSwatch::SWATCH_SMALL);
            _p->colorForegroundWidget->setColorDialogEnabled(true);
            _p->colorForegroundWidget->setToolTip(
                qApp->translate("djv::UI::StylePrefsWidget", "Foreground"));

            _p->colorBackgroundWidget = new ColorSwatch(context);
            _p->colorBackgroundWidget->setSwatchSize(ColorSwatch::SWATCH_SMALL);
            _p->colorBackgroundWidget->setColorDialogEnabled(true);
            _p->colorBackgroundWidget->setToolTip(
                qApp->translate("djv::UI::StylePrefsWidget", "Background"));

            _p->colorBackground2Widget = new ColorSwatch(context);
            _p->colorBackground2Widget->setSwatchSize(ColorSwatch::SWATCH_SMALL);
            _p->colorBackground2Widget->setColorDialogEnabled(true);
            _p->colorBackground2Widget->setToolTip(
                qApp->translate("djv::UI::StylePrefsWidget", "Background 2"));

            _p->colorButtonWidget = new ColorSwatch(context);
            _p->colorButtonWidget->setSwatchSize(ColorSwatch::SWATCH_SMALL);
            _p->colorButtonWidget->setColorDialogEnabled(true);
            _p->colorButtonWidget->setToolTip(
                qApp->translate("djv::UI::StylePrefsWidget", "Button"));

            _p->colorSelectWidget = new ColorSwatch(context);
            _p->colorSelectWidget->setSwatchSize(ColorSwatch::SWATCH_SMALL);
            _p->colorSelectWidget->setColorDialogEnabled(true);
            _p->colorSelectWidget->setToolTip(
                qApp->translate("djv::UI::StylePrefsWidget", "Select"));

            _p->colorSwatchTransparencyWidget = new QCheckBox(
                qApp->translate("djv::UI::StylePrefsWidget", "Show transparency in color swatches"));

            _p->sizeWidget = new QComboBox;
            _p->sizeWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->sizeWidget->addItems(context->style()->sizeMetricNames());

            _p->sizeValueWidget = new IntEdit;
            _p->sizeValueWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            _p->fontNormalWidget = new QFontComboBox;
            _p->fontNormalWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            _p->fontFixedWidget = new QFontComboBox;
            _p->fontFixedWidget->setFontFilters(QFontComboBox::MonospacedFonts);
            _p->fontFixedWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            // Layout the widgets.
            QVBoxLayout * layout = new QVBoxLayout(this);
            layout->setSpacing(context->style()->sizeMetric().largeSpacing);

            PrefsGroupBox * prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::StylePrefsWidget", "Colors"), context);
            QFormLayout * formLayout = prefsGroupBox->createLayout();
            QHBoxLayout * hLayout = new QHBoxLayout;
            hLayout->addWidget(_p->colorWidget);
            hLayout->addWidget(_p->colorForegroundWidget);
            hLayout->addWidget(_p->colorBackgroundWidget);
            hLayout->addWidget(_p->colorBackground2Widget);
            hLayout->addWidget(_p->colorButtonWidget);
            hLayout->addWidget(_p->colorSelectWidget);
            formLayout->addRow(
                qApp->translate("djv::UI::StylePrefsWidget", "Color:"),
                hLayout);
            formLayout->addRow(_p->colorSwatchTransparencyWidget);
            layout->addWidget(prefsGroupBox);

            prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::StylePrefsWidget", "Size"), context);
            formLayout = prefsGroupBox->createLayout();
            hLayout = new QHBoxLayout;
            hLayout->setMargin(0);
            hLayout->addWidget(_p->sizeWidget);
            hLayout->addWidget(_p->sizeValueWidget);
            formLayout->addRow(
                qApp->translate("djv::UI::StylePrefsWidget", "Size:"),
                hLayout);
            layout->addWidget(prefsGroupBox);

            prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::StylePrefsWidget", "Fonts"), context);
            formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::UI::StylePrefsWidget", "Normal:"),
                _p->fontNormalWidget);
            formLayout->addRow(
                qApp->translate("djv::UI::StylePrefsWidget", "Fixed:"),
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

        StylePrefsWidget::~StylePrefsWidget()
        {}

        void StylePrefsWidget::resetPreferences()
        {
            context()->style()->setPalettes(
                Style::palettesDefault());
            context()->style()->setPalettesIndex(
                Style::palettesIndexDefault());
            context()->style()->setColorSwatchTransparency(
                Style::colorSwatchTransparencyDefault());
            context()->style()->setSizeMetrics(
                Style::sizeMetricsDefault());
            context()->style()->setSizeMetricsIndex(
                Style::sizeMetricsIndexDefault());
            context()->style()->setFonts(
                Style::fontsDefault());
            widgetUpdate();
        }

        void StylePrefsWidget::colorCallback(int index)
        {
            context()->style()->setPalettesIndex(index);
            widgetUpdate();
        }

        void StylePrefsWidget::colorForegroundCallback(const Graphics::Color & color)
        {
            Style::Palette tmp = context()->style()->palette();
            tmp.foreground = color;
            context()->style()->setPalette(tmp);
        }

        void StylePrefsWidget::colorBackgroundCallback(const Graphics::Color & color)
        {
            Style::Palette tmp = context()->style()->palette();
            tmp.background = color;
            context()->style()->setPalette(tmp);
        }

        void StylePrefsWidget::colorBackground2Callback(const Graphics::Color & color)
        {
            Style::Palette tmp = context()->style()->palette();
            tmp.background2 = color;
            context()->style()->setPalette(tmp);
        }

        void StylePrefsWidget::colorButtonCallback(const Graphics::Color & color)
        {
            Style::Palette tmp = context()->style()->palette();
            tmp.button = color;
            context()->style()->setPalette(tmp);
        }

        void StylePrefsWidget::colorSelectCallback(const Graphics::Color & color)
        {
            Style::Palette tmp = context()->style()->palette();
            tmp.select = color;
            context()->style()->setPalette(tmp);
        }

        void StylePrefsWidget::colorSwatchTransparencyCallback(bool in)
        {
            context()->style()->setColorSwatchTransparency(in);
        }

        void StylePrefsWidget::sizeCallback(int index)
        {
            context()->style()->setSizeMetricsIndex(index);
            widgetUpdate();
        }

        void StylePrefsWidget::sizeValueCallback(int size)
        {
            context()->style()->setSizeMetric(
                Style::SizeMetric(context()->style()->sizeMetric().name, size));
        }

        void StylePrefsWidget::fontNormalCallback(const QFont & font)
        {
            Style::Fonts fonts = context()->style()->fonts();
            fonts.normal = font;
            context()->style()->setFonts(fonts);
        }

        void StylePrefsWidget::fontFixedCallback(const QFont & font)
        {
            Style::Fonts fonts = context()->style()->fonts();
            fonts.fixed = font;
            context()->style()->setFonts(fonts);
        }

        void StylePrefsWidget::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(QObjectList() <<
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

    } // namespace UI
} // namespace djv
