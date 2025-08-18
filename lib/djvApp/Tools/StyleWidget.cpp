// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Tools/SettingsToolPrivate.h>

#include <djvApp/App.h>

#include <feather-tk/ui/ComboBox.h>
#include <feather-tk/ui/ColorSwatch.h>
#include <feather-tk/ui/FloatEditSlider.h>
#include <feather-tk/ui/FormLayout.h>
#include <feather-tk/ui/PushButton.h>
#include <feather-tk/ui/RowLayout.h>

#include <feather-tk/core/Format.h>

namespace djv
{
    namespace app
    {
        struct StyleSettingsWidget::Private
        {
            std::shared_ptr<SettingsModel> model;

            const std::vector<float> displayScales =
            {
                0.F,
                1.F,
                1.5F,
                2.F,
                2.5F,
                3.F,
                3.5F,
                4.F
            };

            std::shared_ptr<feather_tk::ComboBox> colorStyleComboBox;
            std::shared_ptr<feather_tk::FloatEditSlider> brightnessSlider;
            std::shared_ptr<feather_tk::FloatEditSlider> contrastSlider;
            std::shared_ptr<feather_tk::ComboBox> displayScaleComboBox;
            std::shared_ptr<feather_tk::FormLayout> layout;

            std::shared_ptr<feather_tk::ValueObserver<StyleSettings> > settingsObserver;
        };

        void StyleSettingsWidget::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            ISettingsWidget::_init(context, "djv::app::StyleSettingsWidget", parent);
            FEATHER_TK_P();

            p.model = app->getSettingsModel();

            p.colorStyleComboBox = feather_tk::ComboBox::create(context, feather_tk::getColorStyleLabels());
            p.colorStyleComboBox->setHStretch(feather_tk::Stretch::Expanding);

            p.brightnessSlider = feather_tk::FloatEditSlider::create(context);
            p.brightnessSlider->setRange(.5F, 1.5F);
            p.brightnessSlider->setDefaultValue(1.F);

            p.contrastSlider = feather_tk::FloatEditSlider::create(context);
            p.contrastSlider->setRange(.5F, 1.5F);
            p.contrastSlider->setDefaultValue(1.F);

            std::vector<std::string> labels;
            for (auto d : p.displayScales)
            {
                labels.push_back(feather_tk::Format("{0}").arg(d).operator std::string());
            }
            p.displayScaleComboBox = feather_tk::ComboBox::create(context, labels);
            p.displayScaleComboBox->setHStretch(feather_tk::Stretch::Expanding);

            p.layout = feather_tk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(feather_tk::SizeRole::Margin);
            p.layout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            p.layout->addRow("Color style:", p.colorStyleComboBox);
            p.layout->addRow("Brightness:", p.brightnessSlider);
            p.layout->addRow("Contrast:", p.contrastSlider);
            p.layout->addRow("Display scale:", p.displayScaleComboBox);

            p.settingsObserver = feather_tk::ValueObserver<StyleSettings>::create(
                app->getSettingsModel()->observeStyle(),
                [this](const StyleSettings& value)
                {
                    _widgetUpdate(value);
                });

            p.colorStyleComboBox->setIndexCallback(
                [this](int value)
                {
                    FEATHER_TK_P();
                    auto settings = p.model->getStyle();
                    settings.colorStyle = static_cast<feather_tk::ColorStyle>(value);
                    p.model->setStyle(settings);
                });

            p.brightnessSlider->setCallback(
                [this](float value)
                {
                    FEATHER_TK_P();
                    auto settings = p.model->getStyle();
                    settings.colorControls.brightness = value;
                    p.model->setStyle(settings);
                });

            p.contrastSlider->setCallback(
                [this](float value)
                {
                    FEATHER_TK_P();
                    auto settings = p.model->getStyle();
                    settings.colorControls.contrast = value;
                    p.model->setStyle(settings);
                });

            p.displayScaleComboBox->setIndexCallback(
                [this](int value)
                {
                    FEATHER_TK_P();
                    auto settings = p.model->getStyle();
                    if (value >= 0 && value < p.displayScales.size())
                    {
                        settings.displayScale = p.displayScales[value];
                    }
                    p.model->setStyle(settings);
                });
        }

        StyleSettingsWidget::StyleSettingsWidget() :
            _p(new Private)
        {}

        StyleSettingsWidget::~StyleSettingsWidget()
        {}

        std::shared_ptr<StyleSettingsWidget> StyleSettingsWidget::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<StyleSettingsWidget>(new StyleSettingsWidget);
            out->_init(context, app, parent);
            return out;
        }

        void StyleSettingsWidget::setGeometry(const feather_tk::Box2I& value)
        {
            ISettingsWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void StyleSettingsWidget::sizeHintEvent(const feather_tk::SizeHintEvent& event)
        {
            ISettingsWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }

        void StyleSettingsWidget::_widgetUpdate(const StyleSettings& value)
        {
            FEATHER_TK_P();

            p.colorStyleComboBox->setCurrentIndex(static_cast<int>(value.colorStyle));

            p.brightnessSlider->setValue(value.colorControls.brightness);
            p.contrastSlider->setValue(value.colorControls.contrast);

            const auto i = std::find(
                p.displayScales.begin(),
                p.displayScales.end(),
                value.displayScale);
            p.displayScaleComboBox->setCurrentIndex(
                i != p.displayScales.end() ?
                (i - p.displayScales.begin()) :
                -1);
        }
    }
}
