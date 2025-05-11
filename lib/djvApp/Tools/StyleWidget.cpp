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

            std::shared_ptr<dtk::ComboBox> colorStyleComboBox;
            std::shared_ptr<dtk::FloatEditSlider> brightnessSlider;
            std::shared_ptr<dtk::FloatEditSlider> contrastSlider;
            std::shared_ptr<dtk::ComboBox> displayScaleComboBox;
            std::shared_ptr<dtk::FormLayout> layout;

            std::shared_ptr<dtk::ValueObserver<StyleSettings> > settingsObserver;
        };

        void StyleSettingsWidget::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            ISettingsWidget::_init(context, "djv::app::StyleSettingsWidget", parent);
            DTK_P();

            p.model = app->getSettingsModel();

            p.colorStyleComboBox = dtk::ComboBox::create(context, dtk::getColorStyleLabels());
            p.colorStyleComboBox->setHStretch(dtk::Stretch::Expanding);

            p.brightnessSlider = dtk::FloatEditSlider::create(context);
            p.brightnessSlider->setRange(dtk::RangeF(.5F, 1.5F));
            p.brightnessSlider->setDefaultValue(1.F);

            p.contrastSlider = dtk::FloatEditSlider::create(context);
            p.contrastSlider->setRange(dtk::RangeF(.5F, 1.5F));
            p.contrastSlider->setDefaultValue(1.F);

            std::vector<std::string> labels;
            for (auto d : p.displayScales)
            {
                labels.push_back(0.F == d ?
                    std::string("Automatic") :
                    dtk::Format("{0}").arg(d).operator std::string());
            }
            p.displayScaleComboBox = dtk::ComboBox::create(context, labels);
            p.displayScaleComboBox->setHStretch(dtk::Stretch::Expanding);

            p.layout = dtk::FormLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            p.layout->addRow("Color style:", p.colorStyleComboBox);
            p.layout->addRow("Brightness:", p.brightnessSlider);
            p.layout->addRow("Contrast:", p.contrastSlider);
            p.layout->addRow("Display scale:", p.displayScaleComboBox);

            p.settingsObserver = dtk::ValueObserver<StyleSettings>::create(
                app->getSettingsModel()->observeStyle(),
                [this](const StyleSettings& value)
                {
                    _widgetUpdate(value);
                });

            p.colorStyleComboBox->setIndexCallback(
                [this](int value)
                {
                    DTK_P();
                    auto settings = p.model->getStyle();
                    settings.colorStyle = static_cast<dtk::ColorStyle>(value);
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

        void StyleSettingsWidget::setMarginRole(dtk::SizeRole value)
        {
            _p->layout->setMarginRole(value);
        }

        void StyleSettingsWidget::setGeometry(const dtk::Box2I& value)
        {
            ISettingsWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void StyleSettingsWidget::sizeHintEvent(const dtk::SizeHintEvent& event)
        {
            ISettingsWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }

        void StyleSettingsWidget::_widgetUpdate(const StyleSettings& value)
        {
            DTK_P();

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
