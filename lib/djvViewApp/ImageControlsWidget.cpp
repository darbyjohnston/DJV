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
            std::shared_ptr<Image::Data> frameStoreImage;

            std::shared_ptr<UI::HorizontalLayout> channelsDisplayLayout;
            std::shared_ptr<UI::ComboBox> alphaComboBox;

            std::shared_ptr<UI::HorizontalLayout> mirrorLayout;
            std::shared_ptr<UI::ComboBox> rotateComboBox;
            std::shared_ptr<UI::ComboBox> aspectRatioComboBox;

            std::map<std::string, std::shared_ptr<UI::Numeric::FloatSlider> > colorSliders;
            std::shared_ptr<UI::CheckBox> colorInvertCheckBox;
            std::map<std::string, std::shared_ptr<UI::Numeric::FloatSlider> > levelsSliders;
            std::map<std::string, std::shared_ptr<UI::Numeric::FloatSlider> > exposureSliders;
            std::shared_ptr<UI::Numeric::FloatSlider> softClipSlider;

            std::shared_ptr<UI::ImageWidget> frameStoreImageWidget;

            std::map<std::string, std::shared_ptr<UI::FormLayout> > formLayouts;
            std::map<std::string, std::shared_ptr<UI::Bellows> > bellows;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::shared_ptr<Observer::Value<ImageData> > dataObserver;
            std::shared_ptr<Observer::Value<std::shared_ptr<Image::Data> > > frameStoreImageObserver;
        };

        void ImageControlsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::ImageControlsWidget");

            auto imageSystem = context->getSystemT<ImageSystem>();
            auto imageActions = imageSystem->getActions();
            std::vector<std::shared_ptr<UI::Action> > channelsDisplayActions =
            {
                imageActions["RedChannel"],
                imageActions["GreenChannel"],
                imageActions["BlueChannel"],
                imageActions["AlphaChannel"]
            };
            std::vector<std::shared_ptr<UI::CheckBox> > channelsDisplayCheckBoxes;
            for (const auto& i : channelsDisplayActions)
            {
                auto checkBox = UI::CheckBox::create(context);
                checkBox->addAction(i);
                channelsDisplayCheckBoxes.push_back(checkBox);
            }
            p.alphaComboBox = UI::ComboBox::create(context);

            std::vector<std::shared_ptr<UI::Action> > mirrorActions =
            {
                imageActions["MirrorH"],
                imageActions["MirrorV"]
            };
            std::vector<std::shared_ptr<UI::CheckBox> > mirrorCheckBoxes;
            for (const auto& i : mirrorActions)
            {
                auto checkBox = UI::CheckBox::create(context);
                checkBox->addAction(i);
                mirrorCheckBoxes.push_back(checkBox);
            }
            p.rotateComboBox = UI::ComboBox::create(context);
            p.aspectRatioComboBox = UI::ComboBox::create(context);

            auto colorEnabledButton = UI::ToolButton::create(context);
            colorEnabledButton->addAction(imageActions["ColorEnabled"]);
            colorEnabledButton->setInsideMargin(UI::MetricsRole::None);
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

            auto levelsEnabledButton = UI::ToolButton::create(context);
            levelsEnabledButton->addAction(imageActions["LevelsEnabled"]);
            levelsEnabledButton->setInsideMargin(UI::MetricsRole::None);
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

            auto exposureEnabledButton = UI::ToolButton::create(context);
            exposureEnabledButton->addAction(imageActions["ExposureEnabled"]);
            exposureEnabledButton->setInsideMargin(UI::MetricsRole::None);
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

            auto softClipEnabledButton = UI::ToolButton::create(context);
            softClipEnabledButton->addAction(imageActions["SoftClipEnabled"]);
            softClipEnabledButton->setInsideMargin(UI::MetricsRole::None);
            p.softClipSlider = UI::Numeric::FloatSlider::create(context);
            p.softClipSlider->setDefaultVisible(true);

            auto frameStoreEnabledButton = UI::ToolButton::create(context);
            frameStoreEnabledButton->addAction(imageActions["FrameStoreEnabled"]);
            frameStoreEnabledButton->setInsideMargin(UI::MetricsRole::None);
            auto loadFrameStoreButton = UI::PushButton::create(context);
            loadFrameStoreButton->addAction(imageActions["LoadFrameStore"]);
            auto clearFrameStoreButton = UI::PushButton::create(context);
            clearFrameStoreButton->addAction(imageActions["ClearFrameStore"]);
            p.frameStoreImageWidget = UI::ImageWidget::create(context);
            p.frameStoreImageWidget->setImageSizeRole(UI::MetricsRole::TextColumn);
            p.frameStoreImageWidget->setHAlign(UI::HAlign::Center);
            p.frameStoreImageWidget->setVAlign(UI::VAlign::Center);

            p.formLayouts["General"] = UI::FormLayout::create(context);
            p.channelsDisplayLayout = UI::HorizontalLayout::create(context);
            p.channelsDisplayLayout->setSpacing(UI::MetricsRole::SpacingSmall);
            for (const auto& i : channelsDisplayCheckBoxes)
            {
                p.channelsDisplayLayout->addChild(i);
            }
            p.formLayouts["General"]->addChild(p.channelsDisplayLayout);
            p.formLayouts["General"]->addChild(p.alphaComboBox);
            p.bellows["General"] = UI::Bellows::create(context);
            p.bellows["General"]->addChild(p.formLayouts["General"]);

            p.formLayouts["Transform"] = UI::FormLayout::create(context);
            p.mirrorLayout = UI::HorizontalLayout::create(context);
            p.mirrorLayout->setSpacing(UI::MetricsRole::SpacingSmall);
            for (const auto& i : mirrorCheckBoxes)
            {
                p.mirrorLayout->addChild(i);
            }
            p.formLayouts["Transform"]->addChild(p.mirrorLayout);
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
            p.bellows["Color"]->addButtonWidget(colorEnabledButton);
            p.bellows["Color"]->addChild(p.formLayouts["Color"]);

            p.formLayouts["Levels"] = UI::FormLayout::create(context);
            for (const auto& i : { "InLow", "InHigh", "Gamma", "OutLow", "OutHigh" })
            {
                p.formLayouts["Levels"]->addChild(p.levelsSliders[i]);
            }
            p.bellows["Levels"] = UI::Bellows::create(context);
            p.bellows["Levels"]->addButtonWidget(levelsEnabledButton);
            p.bellows["Levels"]->addChild(p.formLayouts["Levels"]);

            p.formLayouts["Exposure"] = UI::FormLayout::create(context);
            for (const auto& i : { "Exposure", "Defog", "KneeLow", "KneeHigh" })
            {
                p.formLayouts["Exposure"]->addChild(p.exposureSliders[i]);
            }
            p.bellows["Exposure"] = UI::Bellows::create(context);
            p.bellows["Exposure"]->addButtonWidget(exposureEnabledButton);
            p.bellows["Exposure"]->addChild(p.formLayouts["Exposure"]);

            p.formLayouts["SoftClip"] = UI::FormLayout::create(context);
            p.formLayouts["SoftClip"]->addChild(p.softClipSlider);
            p.bellows["SoftClip"] = UI::Bellows::create(context);
            p.bellows["SoftClip"]->addButtonWidget(softClipEnabledButton);
            p.bellows["SoftClip"]->addChild(p.formLayouts["SoftClip"]);

            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setMargin(UI::MetricsRole::MarginSmall);
            vLayout->setSpacing(UI::MetricsRole::SpacingSmall);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::SpacingSmall);
            hLayout->addChild(loadFrameStoreButton);
            hLayout->addChild(clearFrameStoreButton);
            vLayout->addChild(hLayout);
            vLayout->addChild(p.frameStoreImageWidget);
            p.bellows["FrameStore"] = UI::Bellows::create(context);
            p.bellows["FrameStore"]->addButtonWidget(frameStoreEnabledButton);
            p.bellows["FrameStore"]->addChild(vLayout);

            for (const auto& i : p.formLayouts)
            {
                i.second->setMargin(UI::MetricsRole::MarginSmall);
            }

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(p.bellows["General"]);
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
                p.formLayouts["General"]->setText(p.channelsDisplayLayout, _getText(DJV_TEXT("image_controls_channels_display")) + ":");
                p.formLayouts["General"]->setText(p.alphaComboBox, _getText(DJV_TEXT("image_controls_channels_alpha_blend")) + ":");

                p.formLayouts["Transform"]->setText(p.mirrorLayout, _getText(DJV_TEXT("image_controls_transform_mirror")) + ":");
                p.formLayouts["Transform"]->setText(p.rotateComboBox, _getText(DJV_TEXT("image_controls_transform_rotate")) + ":");
                p.formLayouts["Transform"]->setText(p.aspectRatioComboBox, _getText(DJV_TEXT("image_controls_transform_aspect_ratio")) + ":");

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

                p.bellows["General"]->setText(_getText(DJV_TEXT("image_controls_section_general")));
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
            for (auto i : Render2D::getAlphaBlendEnums())
            {
                std::stringstream ss;
                ss << i;
                items.push_back(_getText(ss.str()));
            }
            p.alphaComboBox->setItems(items);
            p.alphaComboBox->setCurrentItem(static_cast<int>(p.data.alphaBlend));

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
            
            p.colorSliders["Brightness"]->setValue(p.data.color.brightness);
            p.colorSliders["Contrast"]->setValue(p.data.color.contrast);
            p.colorSliders["Saturation"]->setValue(p.data.color.saturation);
            p.colorInvertCheckBox->setChecked(p.data.color.invert);

            p.levelsSliders["InLow"]->setValue(p.data.levels.inLow);
            p.levelsSliders["InHigh"]->setValue(p.data.levels.inHigh);
            p.levelsSliders["Gamma"]->setValue(p.data.levels.gamma);
            p.levelsSliders["OutLow"]->setValue(p.data.levels.outLow);
            p.levelsSliders["OutHigh"]->setValue(p.data.levels.outHigh);

            p.exposureSliders["Exposure"]->setValue(p.data.exposure.exposure);
            p.exposureSliders["Defog"]->setValue(p.data.exposure.defog);
            p.exposureSliders["KneeLow"]->setValue(p.data.exposure.kneeLow);
            p.exposureSliders["KneeHigh"]->setValue(p.data.exposure.kneeHigh);

            p.softClipSlider->setValue(p.data.softClip);

            p.frameStoreImageWidget->setImage(p.frameStoreImage);
        }

    } // namespace ViewApp
} // namespace djv

