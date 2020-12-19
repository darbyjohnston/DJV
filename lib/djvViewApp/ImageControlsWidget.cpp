// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ImageControlsWidget.h>

#include <djvViewApp/ImageData.h>
#include <djvViewApp/ImageSettings.h>
#include <djvViewApp/ImageSystem.h>
#include <djvViewApp/ViewWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Bellows.h>
#include <djvUI/ButtonGroup.h>
#include <djvUI/CheckBox.h>
#include <djvUI/ComboBox.h>
#include <djvUI/EnumFunc.h>
#include <djvUI/FloatSlider.h>
#include <djvUI/FormLayout.h>
#include <djvUI/ImageWidget.h>
#include <djvUI/Label.h>
#include <djvUI/PushButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ToolButton.h>

#include <djvRender2D/EnumFunc.h>
#include <djvRender2D/DataFunc.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ImageControlsWidget::Private
        {
            ImageData data;
            bool frameStoreEnabled = false;
            std::shared_ptr<Image::Data> frameStoreImage;
            bool frameStoreActionEnabled = false;
            bool loadFrameStoreActionEnabled = false;
            bool clearFrameStoreActionEnabled = false;

            std::shared_ptr<UI::ComboBox> channelDisplayComboBox;
            std::shared_ptr<UI::ComboBox> alphaComboBox;

            std::shared_ptr<UI::CheckBox> mirrorCheckBoxes[2];
            std::shared_ptr<UI::ComboBox> rotateComboBox;
            std::shared_ptr<UI::ComboBox> aspectRatioComboBox;

            std::shared_ptr<UI::ToolButton> colorEnabledButton;
            std::map<std::string, std::shared_ptr<UI::Numeric::FloatSlider> > colorSliders;
            std::shared_ptr<UI::CheckBox> colorInvertCheckBox;
            std::shared_ptr<UI::ToolButton> levelsEnabledButton;
            std::map<std::string, std::shared_ptr<UI::Numeric::FloatSlider> > levelsSliders;
            std::shared_ptr<UI::ToolButton> exposureEnabledButton;
            std::map<std::string, std::shared_ptr<UI::Numeric::FloatSlider> > exposureSliders;
            std::shared_ptr<UI::ToolButton> softClipEnabledButton;
            std::shared_ptr<UI::Numeric::FloatSlider> softClipSlider;

            std::shared_ptr<UI::ToolButton> frameStoreEnabledButton;
            std::shared_ptr<UI::PushButton> loadFrameStoreButton;
            std::shared_ptr<UI::PushButton> clearFrameStoreButton;
            std::shared_ptr<UI::ImageWidget> frameStoreImageWidget;

            std::map<std::string, std::shared_ptr<UI::FormLayout> > formLayouts;
            std::map<std::string, std::shared_ptr<UI::Bellows> > bellows;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::shared_ptr<Observer::Value<ImageData> > dataObserver;
            std::shared_ptr<Observer::Value<bool> > frameStoreEnabledObserver;
            std::shared_ptr<Observer::Value<std::shared_ptr<Image::Data> > > frameStoreImageObserver;
            std::shared_ptr<Observer::Value<bool> > frameStoreActionEnabledObserver;
            std::shared_ptr<Observer::Value<bool> > loadFrameStoreActionEnabledObserver;
            std::shared_ptr<Observer::Value<bool> > clearFrameStoreActionEnabledObserver;
        };

        void ImageControlsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::ImageControlsWidget");

            p.channelDisplayComboBox = UI::ComboBox::create(context);
            p.alphaComboBox = UI::ComboBox::create(context);

            for (size_t i = 0; i < 2; ++i)
            {
                p.mirrorCheckBoxes[i] = UI::CheckBox::create(context);
            }
            p.rotateComboBox = UI::ComboBox::create(context);
            p.aspectRatioComboBox = UI::ComboBox::create(context);

            p.colorEnabledButton = UI::ToolButton::create(context);
            p.colorEnabledButton->setButtonType(UI::ButtonType::Toggle);
            p.colorEnabledButton->setIcon("djvIconHidden");
            p.colorEnabledButton->setCheckedIcon("djvIconVisible");
            p.colorEnabledButton->setInsideMargin(UI::MetricsRole::None);
            p.colorSliders["Brightness"] = UI::Numeric::FloatSlider::create(context);
            p.colorSliders["Brightness"]->setRange(Math::FloatRange(0.F, 4.F));
            const Render2D::ImageColor color;
            p.colorSliders["Brightness"]->setDefault(color.brightness);
            p.colorSliders["Contrast"] = UI::Numeric::FloatSlider::create(context);
            p.colorSliders["Contrast"]->setRange(Math::FloatRange(0.F, 4.F));
            p.colorSliders["Contrast"]->setDefault(color.contrast);
            p.colorSliders["Saturation"] = UI::Numeric::FloatSlider::create(context);
            p.colorSliders["Saturation"]->setRange(Math::FloatRange(0.F, 4.F));
            p.colorSliders["Saturation"]->setDefault(color.saturation);
            for (const auto& slider : p.colorSliders)
            {
                slider.second->setDefaultVisible(true);
            }
            p.colorInvertCheckBox = UI::CheckBox::create(context);

            p.levelsEnabledButton = UI::ToolButton::create(context);
            p.levelsEnabledButton->setButtonType(UI::ButtonType::Toggle);
            p.levelsEnabledButton->setIcon("djvIconHidden");
            p.levelsEnabledButton->setCheckedIcon("djvIconVisible");
            p.levelsEnabledButton->setInsideMargin(UI::MetricsRole::None);
            p.levelsSliders["InLow"] = UI::Numeric::FloatSlider::create(context);
            const Render2D::ImageLevels levels;
            p.levelsSliders["InLow"]->setDefault(levels.inLow);
            p.levelsSliders["InHigh"] = UI::Numeric::FloatSlider::create(context);
            p.levelsSliders["InHigh"]->setDefault(levels.inHigh);
            p.levelsSliders["Gamma"] = UI::Numeric::FloatSlider::create(context);
            p.levelsSliders["Gamma"]->setRange(Math::FloatRange(.1F, 4.F));
            p.levelsSliders["Gamma"]->setDefault(levels.gamma);
            p.levelsSliders["OutLow"] = UI::Numeric::FloatSlider::create(context);
            p.levelsSliders["OutLow"]->setDefault(levels.outLow);
            p.levelsSliders["OutHigh"] = UI::Numeric::FloatSlider::create(context);
            p.levelsSliders["OutHigh"]->setDefault(levels.outHigh);
            for (const auto& slider : p.levelsSliders)
            {
                slider.second->setDefaultVisible(true);
            }

            p.exposureEnabledButton = UI::ToolButton::create(context);
            p.exposureEnabledButton->setButtonType(UI::ButtonType::Toggle);
            p.exposureEnabledButton->setIcon("djvIconHidden");
            p.exposureEnabledButton->setCheckedIcon("djvIconVisible");
            p.exposureEnabledButton->setInsideMargin(UI::MetricsRole::None);
            p.exposureSliders["Exposure"] = UI::Numeric::FloatSlider::create(context);
            p.exposureSliders["Exposure"]->setRange(Math::FloatRange(-10.F, 10.F));
            const Render2D::ImageExposure exposure;
            p.exposureSliders["Exposure"]->setDefault(exposure.exposure);
            p.exposureSliders["Defog"] = UI::Numeric::FloatSlider::create(context);
            p.exposureSliders["Defog"]->setRange(Math::FloatRange(0.F, .01F));
            p.exposureSliders["Defog"]->setDefault(exposure.defog);
            p.exposureSliders["KneeLow"] = UI::Numeric::FloatSlider::create(context);
            p.exposureSliders["KneeLow"]->setRange(Math::FloatRange(-3.F, 3.F));
            p.exposureSliders["KneeLow"]->setDefault(exposure.kneeLow);
            p.exposureSliders["KneeHigh"] = UI::Numeric::FloatSlider::create(context);
            p.exposureSliders["KneeHigh"]->setRange(Math::FloatRange(3.5F, 7.5F));
            p.exposureSliders["KneeHigh"]->setDefault(exposure.kneeHigh);
            for (const auto& slider : p.exposureSliders)
            {
                slider.second->setDefaultVisible(true);
            }

            p.softClipEnabledButton = UI::ToolButton::create(context);
            p.softClipEnabledButton->setButtonType(UI::ButtonType::Toggle);
            p.softClipEnabledButton->setIcon("djvIconHidden");
            p.softClipEnabledButton->setCheckedIcon("djvIconVisible");
            p.softClipEnabledButton->setInsideMargin(UI::MetricsRole::None);
            p.softClipSlider = UI::Numeric::FloatSlider::create(context);
            p.softClipSlider->setDefaultVisible(true);

            p.frameStoreEnabledButton = UI::ToolButton::create(context);
            p.frameStoreEnabledButton->setButtonType(UI::ButtonType::Toggle);
            p.frameStoreEnabledButton->setIcon("djvIconHidden");
            p.frameStoreEnabledButton->setCheckedIcon("djvIconVisible");
            p.frameStoreEnabledButton->setInsideMargin(UI::MetricsRole::None);
            p.loadFrameStoreButton = UI::PushButton::create(context);
            p.clearFrameStoreButton = UI::PushButton::create(context);
            p.frameStoreImageWidget = UI::ImageWidget::create(context);
            p.frameStoreImageWidget->setImageSizeRole(UI::MetricsRole::TextColumn);
            p.frameStoreImageWidget->setHAlign(UI::HAlign::Center);
            p.frameStoreImageWidget->setVAlign(UI::VAlign::Center);

            p.formLayouts["Channels"] = UI::FormLayout::create(context);
            p.formLayouts["Channels"]->addChild(p.channelDisplayComboBox);
            p.formLayouts["Channels"]->addChild(p.alphaComboBox);
            p.bellows["Channels"] = UI::Bellows::create(context);
            p.bellows["Channels"]->addChild(p.formLayouts["Channels"]);

            p.formLayouts["Transform"] = UI::FormLayout::create(context);
            for (size_t i = 0; i < 2; ++i)
            {
                p.formLayouts["Transform"]->addChild(p.mirrorCheckBoxes[i]);
            }
            p.formLayouts["Transform"]->addChild(p.rotateComboBox);
            p.formLayouts["Transform"]->addChild(p.aspectRatioComboBox);
            p.bellows["Transform"] = UI::Bellows::create(context);
            p.bellows["Transform"]->addChild(p.formLayouts["Transform"]);

            p.formLayouts["Color"] = UI::FormLayout::create(context);
            for (const auto& i : { "Brightness", "Contrast", "Saturation" })
            {
                p.formLayouts["Color"]->addChild(p.colorSliders[i]);
            }
            p.formLayouts["Color"]->addChild(p.colorInvertCheckBox);
            p.bellows["Color"] = UI::Bellows::create(context);
            p.bellows["Color"]->addButtonWidget(p.colorEnabledButton);
            p.bellows["Color"]->addChild(p.formLayouts["Color"]);

            p.formLayouts["Levels"] = UI::FormLayout::create(context);
            for (const auto& i : { "InLow", "InHigh", "Gamma", "OutLow", "OutHigh" })
            {
                p.formLayouts["Levels"]->addChild(p.levelsSliders[i]);
            }
            p.bellows["Levels"] = UI::Bellows::create(context);
            p.bellows["Levels"]->addButtonWidget(p.levelsEnabledButton);
            p.bellows["Levels"]->addChild(p.formLayouts["Levels"]);

            p.formLayouts["Exposure"] = UI::FormLayout::create(context);
            for (const auto& i : { "Exposure", "Defog", "KneeLow", "KneeHigh" })
            {
                p.formLayouts["Exposure"]->addChild(p.exposureSliders[i]);
            }
            p.bellows["Exposure"] = UI::Bellows::create(context);
            p.bellows["Exposure"]->addButtonWidget(p.exposureEnabledButton);
            p.bellows["Exposure"]->addChild(p.formLayouts["Exposure"]);

            p.formLayouts["SoftClip"] = UI::FormLayout::create(context);
            p.formLayouts["SoftClip"]->addChild(p.softClipSlider);
            p.bellows["SoftClip"] = UI::Bellows::create(context);
            p.bellows["SoftClip"]->addButtonWidget(p.softClipEnabledButton);
            p.bellows["SoftClip"]->addChild(p.formLayouts["SoftClip"]);

            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setMargin(UI::MetricsRole::MarginSmall);
            vLayout->setSpacing(UI::MetricsRole::SpacingSmall);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::SpacingSmall);
            hLayout->addChild(p.loadFrameStoreButton);
            hLayout->addChild(p.clearFrameStoreButton);
            vLayout->addChild(hLayout);
            vLayout->addChild(p.frameStoreImageWidget);
            p.bellows["FrameStore"] = UI::Bellows::create(context);
            p.bellows["FrameStore"]->addButtonWidget(p.frameStoreEnabledButton);
            p.bellows["FrameStore"]->addChild(vLayout);

            for (const auto& i : p.formLayouts)
            {
                i.second->setMargin(UI::MetricsRole::MarginSmall);
            }

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(p.bellows["Channels"]);
            p.layout->addChild(p.bellows["Transform"]);
            p.layout->addChild(p.bellows["Color"]);
            p.layout->addChild(p.bellows["Levels"]);
            p.layout->addChild(p.bellows["Exposure"]);
            p.layout->addChild(p.bellows["SoftClip"]);
            p.layout->addChild(p.bellows["FrameStore"]);
            addChild(p.layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<ImageControlsWidget>(std::dynamic_pointer_cast<ImageControlsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.channelDisplayComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.channelDisplay = static_cast<Render2D::ImageChannelDisplay>(value);
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });

            p.alphaComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.alphaBlend = static_cast<Render2D::AlphaBlend>(value);
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });

            p.mirrorCheckBoxes[0]->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.mirror.x = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });
            p.mirrorCheckBoxes[1]->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.mirror.y = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });

            p.rotateComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.rotate = static_cast<UI::ImageRotate>(value);
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });

            p.aspectRatioComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.aspectRatio = static_cast<UI::ImageAspectRatio>(value);
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });

            p.colorEnabledButton->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.colorEnabled = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });
            p.colorSliders["Brightness"]->setValueCallback(
                [weak, contextWeak](float value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.color.brightness = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });
            p.colorSliders["Contrast"]->setValueCallback(
                [weak, contextWeak](float value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.color.contrast = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });
            p.colorSliders["Saturation"]->setValueCallback(
                [weak, contextWeak](float value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.color.saturation = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });
            p.colorInvertCheckBox->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.color.invert = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });

            p.levelsEnabledButton->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.levelsEnabled = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });
            p.levelsSliders["InLow"]->setValueCallback(
                [weak, contextWeak](float value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.levels.inLow = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });
            p.levelsSliders["InHigh"]->setValueCallback(
                [weak, contextWeak](float value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.levels.inHigh = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });
            p.levelsSliders["Gamma"]->setValueCallback(
                [weak, contextWeak](float value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.levels.gamma = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });
            p.levelsSliders["OutLow"]->setValueCallback(
                [weak, contextWeak](float value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.levels.outLow = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });
            p.levelsSliders["OutHigh"]->setValueCallback(
                [weak, contextWeak](float value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.levels.outHigh = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });

            p.exposureEnabledButton->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.exposureEnabled = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });
            p.exposureSliders["Exposure"]->setValueCallback(
                [weak, contextWeak](float value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.exposure.exposure = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });
            p.exposureSliders["Defog"]->setValueCallback(
                [weak, contextWeak](float value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.exposure.defog = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });
            p.exposureSliders["KneeLow"]->setValueCallback(
                [weak, contextWeak](float value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.exposure.kneeLow = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });
            p.exposureSliders["KneeHigh"]->setValueCallback(
                [weak, contextWeak](float value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.exposure.kneeHigh = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });

            p.softClipEnabledButton->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.softClipEnabled = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });
            p.softClipSlider->setValueCallback(
                [weak, contextWeak](float value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.softClip = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setData(data);
                        }
                    }
                });

            p.frameStoreEnabledButton->setCheckedCallback(
                [contextWeak](bool value)
            {
                if (auto context = contextWeak.lock())
                {
                    auto imageSystem = context->getSystemT<ImageSystem>();
                    imageSystem->setFrameStoreEnabled(value);
                }
            });

            p.loadFrameStoreButton->setClickedCallback(
                [contextWeak]
            {
                if (auto context = contextWeak.lock())
                {
                    auto imageSystem = context->getSystemT<ImageSystem>();
                    imageSystem->loadFrameStore();
                }
            });

            p.clearFrameStoreButton->setClickedCallback(
                [contextWeak]
            {
                if (auto context = contextWeak.lock())
                {
                    auto imageSystem = context->getSystemT<ImageSystem>();
                    imageSystem->clearFrameStore();
                }
            });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
            p.dataObserver = Observer::Value<ImageData>::create(
                imageSettings->observeData(),
                [weak](const ImageData& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->data = value;
                        widget->_widgetUpdate();
                    }
                });

            auto imageSystem = context->getSystemT<ImageSystem>();
            p.frameStoreEnabledObserver = Observer::Value<bool>::create(
                imageSystem->observeFrameStoreEnabled(),
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->frameStoreEnabled = value;
                        widget->_widgetUpdate();
                    }
                });
            p.frameStoreImageObserver = Observer::Value<std::shared_ptr<Image::Data> >::create(
                imageSystem->observeFrameStoreImage(),
                [weak](const std::shared_ptr<Image::Data>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->frameStoreImage = value;
                        widget->_widgetUpdate();
                    }
                });
            p.frameStoreActionEnabledObserver = Observer::Value<bool>::create(
                imageSystem->observeFrameStoreActionEnabled(),
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->frameStoreActionEnabled = value;
                        widget->_widgetUpdate();
                    }
                });
            p.loadFrameStoreActionEnabledObserver = Observer::Value<bool>::create(
                imageSystem->observeLoadFrameStoreActionEnabled(),
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->loadFrameStoreActionEnabled = value;
                        widget->_widgetUpdate();
                    }
                });
            p.clearFrameStoreActionEnabledObserver = Observer::Value<bool>::create(
                imageSystem->observeClearFrameStoreActionEnabled(),
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->clearFrameStoreActionEnabled = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        ImageControlsWidget::ImageControlsWidget() :
            _p(new Private)
        {}

        ImageControlsWidget::~ImageControlsWidget()
        {}

        std::shared_ptr<ImageControlsWidget> ImageControlsWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ImageControlsWidget>(new ImageControlsWidget);
            out->_init(context);
            return out;
        }

        std::map<std::string, bool> ImageControlsWidget::getBellowsState() const
        {
            DJV_PRIVATE_PTR();
            std::map<std::string, bool> out;
            for (const auto& i : p.bellows)
            {
                out[i.first] = i.second->isOpen();
            }
            return out;
        }

        void ImageControlsWidget::setBellowsState(const std::map<std::string, bool>& value)
        {
            DJV_PRIVATE_PTR();
            for (const auto& i : value)
            {
                const auto j = p.bellows.find(i.first);
                if (j != p.bellows.end())
                {
                    j->second->setOpen(i.second, false);
                }
            }
        }

        void ImageControlsWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ImageControlsWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ImageControlsWidget::_initEvent(System::Event::Init & event)
        {
            Widget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.formLayouts["Channels"]->setText(p.channelDisplayComboBox, _getText(DJV_TEXT("image_controls_channels_display")) + ":");
                p.formLayouts["Channels"]->setText(p.alphaComboBox, _getText(DJV_TEXT("image_controls_channels_alpha_blend")) + ":");

                p.formLayouts["Transform"]->setText(p.mirrorCheckBoxes[0], _getText(DJV_TEXT("image_controls_transform_mirror_h")) + ":");
                p.formLayouts["Transform"]->setText(p.mirrorCheckBoxes[1], _getText(DJV_TEXT("image_controls_transform_mirror_v")) + ":");
                p.formLayouts["Transform"]->setText(p.rotateComboBox, _getText(DJV_TEXT("image_controls_transform_rotate")) + ":");
                p.formLayouts["Transform"]->setText(p.aspectRatioComboBox, _getText(DJV_TEXT("image_controls_transform_aspect_ratio")) + ":");

                p.colorEnabledButton->setTooltip(_getText(DJV_TEXT("image_controls_color_enabled_tooltip")));
                p.levelsEnabledButton->setTooltip(_getText(DJV_TEXT("image_controls_levels_enabled_tooltip")));
                p.exposureEnabledButton->setTooltip(_getText(DJV_TEXT("image_controls_exposure_enabled_tooltip")));
                p.softClipEnabledButton->setTooltip(_getText(DJV_TEXT("image_controls_soft_clip_enabled_tooltip")));

                p.formLayouts["Color"]->setText(p.colorSliders["Brightness"], _getText(DJV_TEXT("image_controls_color_brightness")) + ":");
                p.formLayouts["Color"]->setText(p.colorSliders["Contrast"], _getText(DJV_TEXT("image_controls_color_contrast")) + ":");
                p.formLayouts["Color"]->setText(p.colorSliders["Saturation"], _getText(DJV_TEXT("image_controls_color_saturation")) + ":");
                p.formLayouts["Color"]->setText(p.colorInvertCheckBox, _getText(DJV_TEXT("image_controls_color_invert")) + ":");

                p.formLayouts["Levels"]->setText(p.levelsSliders["InLow"], _getText(DJV_TEXT("image_controls_levels_in_low")) + ":");
                p.formLayouts["Levels"]->setText(p.levelsSliders["InHigh"], _getText(DJV_TEXT("image_controls_levels_in_high")) + ":");
                p.formLayouts["Levels"]->setText(p.levelsSliders["Gamma"], _getText(DJV_TEXT("image_controls_levels_gamma")) + ":");
                p.formLayouts["Levels"]->setText(p.levelsSliders["OutLow"], _getText(DJV_TEXT("image_controls_levels_out_low")) + ":");
                p.formLayouts["Levels"]->setText(p.levelsSliders["OutHigh"], _getText(DJV_TEXT("image_controls_levels_out_high")) + ":");

                p.formLayouts["Exposure"]->setText(p.exposureSliders["Exposure"], _getText(DJV_TEXT("image_controls_exposure_exposure")) + ":");
                p.formLayouts["Exposure"]->setText(p.exposureSliders["Defog"], _getText(DJV_TEXT("image_controls_exposure_defog")) + ":");
                p.formLayouts["Exposure"]->setText(p.exposureSliders["KneeLow"], _getText(DJV_TEXT("image_controls_exposure_knee_low")) + ":");
                p.formLayouts["Exposure"]->setText(p.exposureSliders["KneeHigh"], _getText(DJV_TEXT("image_controls_exposure_knee_high")) + ":");

                p.frameStoreEnabledButton->setTooltip(_getText(DJV_TEXT("image_controls_frame_store_enabled_tooltip")));
                p.loadFrameStoreButton->setText(_getText(DJV_TEXT("image_controls_frame_store_load")));
                p.clearFrameStoreButton->setText(_getText(DJV_TEXT("image_controls_frame_store_clear")));

                p.bellows["Channels"]->setText(_getText(DJV_TEXT("image_controls_section_channels")));
                p.bellows["Transform"]->setText(_getText(DJV_TEXT("image_controls_section_transform")));
                p.bellows["Color"]->setText(_getText(DJV_TEXT("image_controls_section_color")));
                p.bellows["Levels"]->setText(_getText(DJV_TEXT("image_controls_section_levels")));
                p.bellows["Exposure"]->setText(_getText(DJV_TEXT("image_controls_section_exposure")));
                p.bellows["SoftClip"]->setText(_getText(DJV_TEXT("image_controls_section_soft_clip")));
                p.bellows["FrameStore"]->setText(_getText(DJV_TEXT("image_controls_section_frame_store")));

                _widgetUpdate();
            }
        }

        void ImageControlsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();

            std::vector<std::string> items;
            for (auto i : Render2D::getImageChannelDisplayEnums())
            {
                std::stringstream ss;
                ss << i;
                items.push_back(_getText(ss.str()));
            }
            p.channelDisplayComboBox->setItems(items);
            p.channelDisplayComboBox->setCurrentItem(static_cast<int>(p.data.channelDisplay));

            items.clear();
            for (auto i : Render2D::getAlphaBlendEnums())
            {
                std::stringstream ss;
                ss << i;
                items.push_back(_getText(ss.str()));
            }
            p.alphaComboBox->setItems(items);
            p.alphaComboBox->setCurrentItem(static_cast<int>(p.data.alphaBlend));

            p.mirrorCheckBoxes[0]->setChecked(p.data.mirror.x);
            p.mirrorCheckBoxes[1]->setChecked(p.data.mirror.y);

            items.clear();
            for (auto i : UI::getImageRotateEnums())
            {
                std::stringstream ss;
                ss << i;
                items.push_back(_getText(ss.str()));
            }
            p.rotateComboBox->setItems(items);
            p.rotateComboBox->setCurrentItem(static_cast<int>(p.data.rotate));

            items.clear();
            for (auto i : UI::getImageAspectRatioEnums())
            {
                std::stringstream ss;
                ss << i;
                items.push_back(_getText(ss.str()));
            }
            p.aspectRatioComboBox->setItems(items);
            p.aspectRatioComboBox->setCurrentItem(static_cast<int>(p.data.aspectRatio));
            
            p.colorEnabledButton->setChecked(p.data.colorEnabled);
            p.colorSliders["Brightness"]->setValue(p.data.color.brightness);
            p.colorSliders["Contrast"]->setValue(p.data.color.contrast);
            p.colorSliders["Saturation"]->setValue(p.data.color.saturation);
            p.colorInvertCheckBox->setChecked(p.data.color.invert);

            p.levelsEnabledButton->setChecked(p.data.levelsEnabled);
            p.levelsSliders["InLow"]->setValue(p.data.levels.inLow);
            p.levelsSliders["InHigh"]->setValue(p.data.levels.inHigh);
            p.levelsSliders["Gamma"]->setValue(p.data.levels.gamma);
            p.levelsSliders["OutLow"]->setValue(p.data.levels.outLow);
            p.levelsSliders["OutHigh"]->setValue(p.data.levels.outHigh);

            p.exposureEnabledButton->setChecked(p.data.exposureEnabled);
            p.exposureSliders["Exposure"]->setValue(p.data.exposure.exposure);
            p.exposureSliders["Defog"]->setValue(p.data.exposure.defog);
            p.exposureSliders["KneeLow"]->setValue(p.data.exposure.kneeLow);
            p.exposureSliders["KneeHigh"]->setValue(p.data.exposure.kneeHigh);

            p.softClipEnabledButton->setChecked(p.data.softClipEnabled);
            p.softClipSlider->setValue(p.data.softClip);

            p.frameStoreEnabledButton->setChecked(p.frameStoreEnabled);
            p.frameStoreEnabledButton->setEnabled(p.frameStoreActionEnabled);
            p.loadFrameStoreButton->setEnabled(p.loadFrameStoreActionEnabled);
            p.clearFrameStoreButton->setEnabled(p.clearFrameStoreActionEnabled);
            p.frameStoreImageWidget->setImage(p.frameStoreImage);
        }

    } // namespace ViewApp
} // namespace djv

