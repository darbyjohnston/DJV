// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Tools/SettingsToolPrivate.h>

#include <djvApp/App.h>

#include <dtk/ui/ComboBox.h>
#include <dtk/ui/ColorSwatch.h>
#include <dtk/ui/FloatEditSlider.h>
#include <dtk/ui/FormLayout.h>
#include <dtk/ui/PushButton.h>
#include <dtk/ui/RowLayout.h>

#include <dtk/core/Format.h>

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

            std::shared_ptr<dtk::ComboBox> displayScaleComboBox;
            std::shared_ptr<dtk::FloatEditSlider> brightnessSlider;
            std::shared_ptr<dtk::FloatEditSlider> contrastSlider;
            std::shared_ptr<dtk::FloatEditSlider> saturationSlider;
            std::shared_ptr<dtk::FloatEditSlider> tintSlider;
            std::shared_ptr<dtk::ComboBox> colorStyleComboBox;
            std::map<dtk::ColorRole, std::shared_ptr<dtk::ColorSwatch> > customColorSwatches;
            std::shared_ptr<dtk::PushButton> defaultPaletteButton;
            std::shared_ptr<dtk::FormLayout> formLayout;
            std::shared_ptr<dtk::VerticalLayout> layout;

            std::shared_ptr<dtk::ValueObserver<StyleSettings> > settingsObserver;
        };

        void StyleSettingsWidget::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IWidget::_init(context, "tl::play_app::StyleSettingsWidget", parent);
            DTK_P();

            p.model = app->getSettingsModel();

            std::vector<std::string> labels;
            for (auto d : p.displayScales)
            {
                labels.push_back(0.F == d ?
                    std::string("Automatic") :
                    dtk::Format("{0}").arg(d).operator std::string());
            }
            p.displayScaleComboBox = dtk::ComboBox::create(context, labels);
            p.displayScaleComboBox->setHStretch(dtk::Stretch::Expanding);

            p.brightnessSlider = dtk::FloatEditSlider::create(context);
            p.brightnessSlider->setRange(dtk::RangeF(.5F, 1.5F));
            p.brightnessSlider->setDefaultValue(1.F);
            p.contrastSlider = dtk::FloatEditSlider::create(context);
            p.contrastSlider->setRange(dtk::RangeF(.5F, 1.5F));
            p.contrastSlider->setDefaultValue(1.F);
            p.saturationSlider = dtk::FloatEditSlider::create(context);
            p.saturationSlider->setRange(dtk::RangeF(.0F, 2.F));
            p.saturationSlider->setDefaultValue(1.F);
            p.tintSlider = dtk::FloatEditSlider::create(context);
            p.tintSlider->setRange(dtk::RangeF(0.F, 1.F));
            p.tintSlider->setDefaultValue(0.F);

            p.colorStyleComboBox = dtk::ComboBox::create(context, dtk::getColorStyleLabels());
            p.colorStyleComboBox->setHStretch(dtk::Stretch::Expanding);

            for (auto colorRole : dtk::getColorRoleEnums())
            {
                auto colorSwatch = dtk::ColorSwatch::create(context);
                colorSwatch->setEditable(true);
                p.customColorSwatches[colorRole] = colorSwatch;
            }

            p.defaultPaletteButton = dtk::PushButton::create(context, "Default Palette");

            p.layout = dtk::VerticalLayout::create(context, shared_from_this());
            p.layout->setMarginRole(dtk::SizeRole::MarginSmall);
            p.layout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            p.formLayout = dtk::FormLayout::create(context, p.layout);
            p.formLayout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            p.formLayout->addRow("Display scale:", p.displayScaleComboBox);
            p.formLayout->addRow("Brightness:", p.brightnessSlider);
            p.formLayout->addRow("Contrast:", p.contrastSlider);
            p.formLayout->addRow("Satuation:", p.saturationSlider);
            p.formLayout->addRow("Tint:", p.tintSlider);
            p.formLayout->addRow("Color style:", p.colorStyleComboBox);
            for (auto i : p.customColorSwatches)
            {
                p.formLayout->addRow(dtk::Format("{0}:").arg(dtk::getLabel(i.first)), i.second);
            }
            p.defaultPaletteButton->setParent(p.layout);

            p.settingsObserver = dtk::ValueObserver<StyleSettings>::create(
                app->getSettingsModel()->observeStyle(),
                [this](const StyleSettings& value)
                {
                    _widgetUpdate(value);
                });

            p.displayScaleComboBox->setIndexCallback(
                [this](int value)
                {
                    DTK_P();
                    auto settings = p.model->getStyle();
                    if (value >= 0 && value < p.displayScales.size())
                    {
                        settings.displayScale = p.displayScales[value];
                    }
                    p.model->setStyle(settings);
                });

            p.brightnessSlider->setCallback(
                [this](float value)
                {
                    DTK_P();
                    auto settings = p.model->getStyle();
                    settings.colorControls.brightness = value;
                    p.model->setStyle(settings);
                });
            p.contrastSlider->setCallback(
                [this](float value)
                {
                    DTK_P();
                    auto settings = p.model->getStyle();
                    settings.colorControls.contrast = value;
                    p.model->setStyle(settings);
                });
            p.saturationSlider->setCallback(
                [this](float value)
                {
                    DTK_P();
                    auto settings = p.model->getStyle();
                    settings.colorControls.saturation = value;
                    p.model->setStyle(settings);
                });
            p.tintSlider->setCallback(
                [this](float value)
                {
                    DTK_P();
                    auto settings = p.model->getStyle();
                    settings.colorControls.tint = value;
                    p.model->setStyle(settings);
                });

            p.colorStyleComboBox->setIndexCallback(
                [this](int value)
                {
                    DTK_P();
                    auto settings = p.model->getStyle();
                    settings.colorStyle = static_cast<dtk::ColorStyle>(value);
                    p.model->setStyle(settings);
                });

            for (auto i : p.customColorSwatches)
            {
                const dtk::ColorRole colorRole = i.first;
                i.second->setColorCallback(
                    [this, colorRole](const dtk::Color4F& value)
                    {
                        DTK_P();
                        auto settings = p.model->getStyle();
                        auto i = settings.customColorRoles.find(colorRole);
                        if (i != settings.customColorRoles.end())
                        {
                            settings.customColorRoles[colorRole] = value;
                        }
                        p.model->setStyle(settings);
                    });
            }

            p.defaultPaletteButton->setClickedCallback(
                [this]
                {
                    DTK_P();
                    auto settings = p.model->getStyle();
                    settings.customColorRoles = dtk::getCustomColorRoles();
                    p.model->setStyle(settings);
                });
        }

        StyleSettingsWidget::StyleSettingsWidget() :
            _p(new Private)
        {}

        StyleSettingsWidget::~StyleSettingsWidget()
        {}

        std::shared_ptr<StyleSettingsWidget> StyleSettingsWidget::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<StyleSettingsWidget>(new StyleSettingsWidget);
            out->_init(context, app, parent);
            return out;
        }

        void StyleSettingsWidget::setGeometry(const dtk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void StyleSettingsWidget::sizeHintEvent(const dtk::SizeHintEvent& event)
        {
            IWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }

        void StyleSettingsWidget::_widgetUpdate(const StyleSettings& value)
        {
            DTK_P();

            const auto i = std::find(
                p.displayScales.begin(),
                p.displayScales.end(),
                value.displayScale);
            p.displayScaleComboBox->setCurrentIndex(
                i != p.displayScales.end() ?
                (i - p.displayScales.begin()) :
                -1);

            p.brightnessSlider->setValue(value.colorControls.brightness);
            p.contrastSlider->setValue(value.colorControls.contrast);
            p.saturationSlider->setValue(value.colorControls.saturation);
            p.tintSlider->setValue(value.colorControls.tint);

            p.colorStyleComboBox->setCurrentIndex(
                static_cast<int>(value.colorStyle));

            for (const auto j : p.customColorSwatches)
            {
                p.formLayout->setRowVisible(j.second, dtk::ColorStyle::Custom == value.colorStyle);
                const auto k = value.customColorRoles.find(j.first);
                if (k != value.customColorRoles.end())
                {
                    j.second->setColor(k->second);
                }
            }

            p.defaultPaletteButton->setVisible(dtk::ColorStyle::Custom == value.colorStyle);
        }
    }
}
