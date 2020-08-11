// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ImageControlsWidget.h>

#include <djvViewApp/ImageSettings.h>
#include <djvViewApp/ImageSystem.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/ViewWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Bellows.h>
#include <djvUI/ButtonGroup.h>
#include <djvUI/CheckBox.h>
#include <djvUI/ComboBox.h>
#include <djvUI/FloatSlider.h>
#include <djvUI/FormLayout.h>
#include <djvUI/ImageWidget.h>
#include <djvUI/Label.h>
#include <djvUI/PushButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ToolButton.h>

#include <djvAV/AVSystem.h>
#include <djvAV/Render2D.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ImageControlsWidget::Private
        {
            AV::Render2D::ImageOptions imageOptions;
            UI::ImageRotate rotate = UI::ImageRotate::First;
            UI::ImageAspectRatio aspectRatio = UI::ImageAspectRatio::First;
            bool frameStoreEnabled = false;
            std::shared_ptr<AV::Image::Image> frameStore;

            std::shared_ptr<MediaWidget> activeWidget;

            std::shared_ptr<UI::ComboBox> channelDisplayComboBox;
            std::shared_ptr<UI::ComboBox> alphaComboBox;

            std::shared_ptr<UI::CheckBox> mirrorCheckBoxes[2];
            std::shared_ptr<UI::HorizontalLayout> mirrorLayout;
            std::shared_ptr<UI::ComboBox> rotateComboBox;
            std::shared_ptr<UI::ComboBox> aspectRatioComboBox;

            std::shared_ptr<UI::ToolButton> colorEnabledButton;
            std::map<std::string, std::shared_ptr<UI::FloatSlider> > colorSliders;
            std::shared_ptr<UI::CheckBox> colorInvertCheckBox;
            std::shared_ptr<UI::ToolButton> levelsEnabledButton;
            std::map<std::string, std::shared_ptr<UI::FloatSlider> > levelsSliders;
            std::shared_ptr<UI::ToolButton> exposureEnabledButton;
            std::map<std::string, std::shared_ptr<UI::FloatSlider> > exposureSliders;
            std::shared_ptr<UI::ToolButton> softClipEnabledButton;
            std::shared_ptr<UI::FloatSlider> softClipSlider;

            std::shared_ptr<UI::ToolButton> frameStoreEnabledButton;
            std::shared_ptr<UI::PushButton> loadFrameStoreButton;
            std::shared_ptr<UI::PushButton> clearFrameStoreButton;
            std::shared_ptr<UI::ImageWidget> frameStoreWidget;

            std::shared_ptr<UI::LabelSizeGroup> sizeGroup;
            std::map<std::string, std::shared_ptr<UI::FormLayout> > formLayouts;
            std::map<std::string, std::shared_ptr<UI::Bellows> > bellows;

            std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<ValueObserver<AV::Render2D::ImageOptions> > imageOptionsObserver;
            std::shared_ptr<ValueObserver<UI::ImageRotate> > rotateObserver;
            std::shared_ptr<ValueObserver<UI::ImageAspectRatio> > aspectRatioObserver;
            std::shared_ptr<ValueObserver<bool> > frameStoreEnabledObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image::Image> > > frameStoreObserver;
        };

        void ImageControlsWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);

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
            p.colorSliders["Brightness"] = UI::FloatSlider::create(context);
            p.colorSliders["Brightness"]->setRange(FloatRange(0.F, 4.F));
            const AV::Render2D::ImageColor color;
            p.colorSliders["Brightness"]->setDefault(color.brightness);
            p.colorSliders["Contrast"] = UI::FloatSlider::create(context);
            p.colorSliders["Contrast"]->setRange(FloatRange(0.F, 4.F));
            p.colorSliders["Contrast"]->setDefault(color.contrast);
            p.colorSliders["Saturation"] = UI::FloatSlider::create(context);
            p.colorSliders["Saturation"]->setRange(FloatRange(0.F, 4.F));
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
            p.levelsSliders["InLow"] = UI::FloatSlider::create(context);
            const AV::Render2D::ImageLevels levels;
            p.levelsSliders["InLow"]->setDefault(levels.inLow);
            p.levelsSliders["InHigh"] = UI::FloatSlider::create(context);
            p.levelsSliders["InHigh"]->setDefault(levels.inHigh);
            p.levelsSliders["Gamma"] = UI::FloatSlider::create(context);
            p.levelsSliders["Gamma"]->setRange(FloatRange(.1F, 4.F));
            p.levelsSliders["Gamma"]->setDefault(levels.gamma);
            p.levelsSliders["OutLow"] = UI::FloatSlider::create(context);
            p.levelsSliders["OutLow"]->setDefault(levels.outLow);
            p.levelsSliders["OutHigh"] = UI::FloatSlider::create(context);
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
            p.exposureSliders["Exposure"] = UI::FloatSlider::create(context);
            p.exposureSliders["Exposure"]->setRange(FloatRange(-10.F, 10.F));
            const AV::Render2D::ImageExposure exposure;
            p.exposureSliders["Exposure"]->setDefault(exposure.exposure);
            p.exposureSliders["Defog"] = UI::FloatSlider::create(context);
            p.exposureSliders["Defog"]->setRange(FloatRange(0.F, .01F));
            p.exposureSliders["Defog"]->setDefault(exposure.defog);
            p.exposureSliders["KneeLow"] = UI::FloatSlider::create(context);
            p.exposureSliders["KneeLow"]->setRange(FloatRange(-3.F, 3.F));
            p.exposureSliders["KneeLow"]->setDefault(exposure.kneeLow);
            p.exposureSliders["KneeHigh"] = UI::FloatSlider::create(context);
            p.exposureSliders["KneeHigh"]->setRange(FloatRange(3.5F, 7.5F));
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
            p.softClipSlider = UI::FloatSlider::create(context);
            p.softClipSlider->setDefaultVisible(true);

            p.frameStoreEnabledButton = UI::ToolButton::create(context);
            p.frameStoreEnabledButton->setButtonType(UI::ButtonType::Toggle);
            p.frameStoreEnabledButton->setIcon("djvIconHidden");
            p.frameStoreEnabledButton->setCheckedIcon("djvIconVisible");
            p.frameStoreEnabledButton->setInsideMargin(UI::MetricsRole::None);
            p.loadFrameStoreButton = UI::PushButton::create(context);
            p.clearFrameStoreButton = UI::PushButton::create(context);
            p.frameStoreWidget = UI::ImageWidget::create(context);
            p.frameStoreWidget->setSizeRole(UI::MetricsRole::TextColumn);
            p.frameStoreWidget->setHAlign(UI::HAlign::Center);
            p.frameStoreWidget->setVAlign(UI::VAlign::Center);

            p.sizeGroup = UI::LabelSizeGroup::create();

            p.formLayouts["Channels"] = UI::FormLayout::create(context);
            p.formLayouts["Channels"]->addChild(p.channelDisplayComboBox);
            p.formLayouts["Channels"]->addChild(p.alphaComboBox);
            p.bellows["Channels"] = UI::Bellows::create(context);
            p.bellows["Channels"]->addChild(p.formLayouts["Channels"]);

            p.formLayouts["Transform"] = UI::FormLayout::create(context);
            p.mirrorLayout = UI::HorizontalLayout::create(context);
            for (size_t i = 0; i < 2; ++i)
            {
                p.mirrorLayout->addChild(p.mirrorCheckBoxes[i]);
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
            p.bellows["Color"]->addWidget(p.colorEnabledButton);
            p.bellows["Color"]->addChild(p.formLayouts["Color"]);

            p.formLayouts["Levels"] = UI::FormLayout::create(context);
            for (const auto& i : { "InLow", "InHigh", "Gamma", "OutLow", "OutHigh" })
            {
                p.formLayouts["Levels"]->addChild(p.levelsSliders[i]);
            }
            p.bellows["Levels"] = UI::Bellows::create(context);
            p.bellows["Levels"]->addWidget(p.levelsEnabledButton);
            p.bellows["Levels"]->addChild(p.formLayouts["Levels"]);

            p.formLayouts["Exposure"] = UI::FormLayout::create(context);
            for (const auto& i : { "Exposure", "Defog", "KneeLow", "KneeHigh" })
            {
                p.formLayouts["Exposure"]->addChild(p.exposureSliders[i]);
            }
            p.bellows["Exposure"] = UI::Bellows::create(context);
            p.bellows["Exposure"]->addWidget(p.exposureEnabledButton);
            p.bellows["Exposure"]->addChild(p.formLayouts["Exposure"]);

            p.formLayouts["SoftClip"] = UI::FormLayout::create(context);
            p.formLayouts["SoftClip"]->addChild(p.softClipSlider);
            p.bellows["SoftClip"] = UI::Bellows::create(context);
            p.bellows["SoftClip"]->addWidget(p.softClipEnabledButton);
            p.bellows["SoftClip"]->addChild(p.formLayouts["SoftClip"]);

            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setMargin(UI::MetricsRole::MarginSmall);
            vLayout->setSpacing(UI::MetricsRole::SpacingSmall);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::SpacingSmall);
            hLayout->addChild(p.loadFrameStoreButton);
            hLayout->addChild(p.clearFrameStoreButton);
            vLayout->addChild(hLayout);
            vLayout->addChild(p.frameStoreWidget);
            p.bellows["FrameStore"] = UI::Bellows::create(context);
            p.bellows["FrameStore"]->addWidget(p.frameStoreEnabledButton);
            p.bellows["FrameStore"]->addChild(vLayout);

            for (const auto& i : p.formLayouts)
            {
                i.second->setMargin(UI::MetricsRole::MarginSmall);
                i.second->setLabelSizeGroup(p.sizeGroup);
            }

            vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addChild(p.bellows["Channels"]);
            vLayout->addChild(p.bellows["Transform"]);
            vLayout->addChild(p.bellows["Color"]);
            vLayout->addChild(p.bellows["Levels"]);
            vLayout->addChild(p.bellows["Exposure"]);
            vLayout->addChild(p.bellows["SoftClip"]);
            vLayout->addChild(p.bellows["FrameStore"]);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setBackgroundRole(UI::ColorRole::Background);
            scrollWidget->setShadowOverlay({ UI::Side::Top });
            scrollWidget->addChild(vLayout);
            addChild(scrollWidget);

            _widgetUpdate();

            auto weak = std::weak_ptr<ImageControlsWidget>(std::dynamic_pointer_cast<ImageControlsWidget>(shared_from_this()));
            p.channelDisplayComboBox->setCallback(
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.channelDisplay = static_cast<AV::Render2D::ImageChannelDisplay>(value);
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });

            auto contextWeak = std::weak_ptr<Context>(context);
            p.alphaComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->imageOptions.alphaBlend = static_cast<AV::AlphaBlend>(value);
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getViewWidget()->setImageOptions(widget->_p->imageOptions);
                            }
                            auto avSystem = context->getSystemT<AV::AVSystem>();
                            avSystem->setAlphaBlend(static_cast<AV::AlphaBlend>(value));
                        }
                    }
                });

            p.mirrorCheckBoxes[0]->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.mirror.x = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });
            p.mirrorCheckBoxes[1]->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.mirror.y = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setImageOptions(widget->_p->imageOptions);
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
                            widget->_p->rotate = static_cast<UI::ImageRotate>(value);
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getViewWidget()->setImageRotate(widget->_p->rotate);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setRotate(widget->_p->rotate);
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
                            widget->_p->aspectRatio = static_cast<UI::ImageAspectRatio>(value);
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getViewWidget()->setImageAspectRatio(widget->_p->aspectRatio);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setAspectRatio(widget->_p->aspectRatio);
                        }
                    }
                });

            p.colorEnabledButton->setCheckedCallback(
                [weak](bool value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->imageOptions.colorEnabled = value;
                    widget->_widgetUpdate();
                    if (widget->_p->activeWidget)
                    {
                        widget->_p->activeWidget->getViewWidget()->setImageOptions(widget->_p->imageOptions);
                    }
                }
            });
            p.colorSliders["Brightness"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.color.brightness = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });
            p.colorSliders["Contrast"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.color.contrast = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });
            p.colorSliders["Saturation"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.color.saturation = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });
            p.colorInvertCheckBox->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.color.invert = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });

            p.levelsEnabledButton->setCheckedCallback(
                [weak](bool value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->imageOptions.levelsEnabled = value;
                    widget->_widgetUpdate();
                    if (widget->_p->activeWidget)
                    {
                        widget->_p->activeWidget->getViewWidget()->setImageOptions(widget->_p->imageOptions);
                    }
                }
            });
            p.levelsSliders["InLow"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.levels.inLow = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });
            p.levelsSliders["InHigh"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.levels.inHigh = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });
            p.levelsSliders["Gamma"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.levels.gamma = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });
            p.levelsSliders["OutLow"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.levels.outLow = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });
            p.levelsSliders["OutHigh"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.levels.outHigh = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });

            p.exposureEnabledButton->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.exposureEnabled = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });
            p.exposureSliders["Exposure"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.exposure.exposure = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });
            p.exposureSliders["Defog"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.exposure.defog = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });
            p.exposureSliders["KneeLow"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.exposure.kneeLow = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });
            p.exposureSliders["KneeHigh"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.exposure.kneeHigh = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });

            p.softClipEnabledButton->setCheckedCallback(
                [weak](bool value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->imageOptions.softClipEnabled = value;
                    widget->_widgetUpdate();
                    if (widget->_p->activeWidget)
                    {
                        widget->_p->activeWidget->getViewWidget()->setImageOptions(widget->_p->imageOptions);
                    }
                }
            });
            p.softClipSlider->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.softClip = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setImageOptions(widget->_p->imageOptions);
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

            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.activeWidgetObserver = ValueObserver<std::shared_ptr<MediaWidget> >::create(
                    windowSystem->observeActiveWidget(),
                    [weak](const std::shared_ptr<MediaWidget>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->activeWidget = value;
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->imageOptionsObserver = ValueObserver<AV::Render2D::ImageOptions>::create(
                                    widget->_p->activeWidget->getViewWidget()->observeImageOptions(),
                                    [weak](const AV::Render2D::ImageOptions& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->imageOptions = value;
                                            widget->_widgetUpdate();
                                        }
                                    });
                                widget->_p->rotateObserver = ValueObserver<UI::ImageRotate>::create(
                                    widget->_p->activeWidget->getViewWidget()->observeImageRotate(),
                                    [weak](UI::ImageRotate value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->rotate = value;
                                            widget->_widgetUpdate();
                                        }
                                    });
                                widget->_p->aspectRatioObserver = ValueObserver<UI::ImageAspectRatio>::create(
                                    widget->_p->activeWidget->getViewWidget()->observeImageAspectRatio(),
                                    [weak](UI::ImageAspectRatio value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->aspectRatio = value;
                                            widget->_widgetUpdate();
                                        }
                                    });
                            }
                            else
                            {
                                widget->_p->imageOptionsObserver.reset();
                                widget->_p->rotateObserver.reset();
                                widget->_p->aspectRatioObserver.reset();
                            }
                        }
                    });
            }

            auto imageSystem = context->getSystemT<ImageSystem>();
            p.frameStoreEnabledObserver = ValueObserver<bool>::create(
                imageSystem->observeFrameStoreEnabled(),
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->frameStoreEnabled = value;
                        widget->_widgetUpdate();
                    }
                });

            p.frameStoreObserver = ValueObserver<std::shared_ptr<AV::Image::Image> >::create(
                imageSystem->observeFrameStore(),
                [weak](const std::shared_ptr<AV::Image::Image>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->frameStore = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        ImageControlsWidget::ImageControlsWidget() :
            _p(new Private)
        {}

        ImageControlsWidget::~ImageControlsWidget()
        {}

        std::shared_ptr<ImageControlsWidget> ImageControlsWidget::create(const std::shared_ptr<Core::Context>& context)
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

        void ImageControlsWidget::_initLayoutEvent(Event::InitLayout&)
        {
            _p->sizeGroup->calcMinimumSize();
        }
        
        void ImageControlsWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                setTitle(_getText(DJV_TEXT("image_controls_title")));

                p.formLayouts["Channels"]->setText(p.channelDisplayComboBox, _getText(DJV_TEXT("image_controls_channels_display")) + ":");
                p.formLayouts["Channels"]->setText(p.alphaComboBox, _getText(DJV_TEXT("image_controls_channels_alpha_blend")) + ":");

                p.mirrorCheckBoxes[0]->setText(_getText(DJV_TEXT("image_controls_transform_mirror_horizontal")));
                p.mirrorCheckBoxes[1]->setText(_getText(DJV_TEXT("image_controls_transform_mirror_vertical")));
                p.formLayouts["Transform"]->setText(p.mirrorLayout, _getText(DJV_TEXT("image_controls_transform_mirror")) + ":");
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
            for (auto i : AV::Render2D::getImageChannelDisplayEnums())
            {
                std::stringstream ss;
                ss << i;
                items.push_back(_getText(ss.str()));
            }
            p.channelDisplayComboBox->setItems(items);
            p.channelDisplayComboBox->setCurrentItem(static_cast<int>(p.imageOptions.channelDisplay));

            items.clear();
            for (auto i : AV::getAlphaBlendEnums())
            {
                std::stringstream ss;
                ss << i;
                items.push_back(_getText(ss.str()));
            }
            p.alphaComboBox->setItems(items);
            p.alphaComboBox->setCurrentItem(static_cast<int>(p.imageOptions.alphaBlend));

            p.mirrorCheckBoxes[0]->setChecked(p.imageOptions.mirror.x);
            p.mirrorCheckBoxes[1]->setChecked(p.imageOptions.mirror.y);

            items.clear();
            for (auto i : UI::getImageRotateEnums())
            {
                std::stringstream ss;
                ss << i;
                items.push_back(_getText(ss.str()));
            }
            p.rotateComboBox->setItems(items);
            p.rotateComboBox->setCurrentItem(static_cast<int>(p.rotate));

            items.clear();
            for (auto i : UI::getImageAspectRatioEnums())
            {
                std::stringstream ss;
                ss << i;
                items.push_back(_getText(ss.str()));
            }
            p.aspectRatioComboBox->setItems(items);
            p.aspectRatioComboBox->setCurrentItem(static_cast<int>(p.aspectRatio));
            
            p.colorEnabledButton->setChecked(p.imageOptions.colorEnabled);
            p.colorSliders["Brightness"]->setValue(p.imageOptions.color.brightness);
            p.colorSliders["Contrast"]->setValue(p.imageOptions.color.contrast);
            p.colorSliders["Saturation"]->setValue(p.imageOptions.color.saturation);
            p.colorInvertCheckBox->setChecked(p.imageOptions.color.invert);

            p.levelsEnabledButton->setChecked(p.imageOptions.levelsEnabled);
            p.levelsSliders["InLow"]->setValue(p.imageOptions.levels.inLow);
            p.levelsSliders["InHigh"]->setValue(p.imageOptions.levels.inHigh);
            p.levelsSliders["Gamma"]->setValue(p.imageOptions.levels.gamma);
            p.levelsSliders["OutLow"]->setValue(p.imageOptions.levels.outLow);
            p.levelsSliders["OutHigh"]->setValue(p.imageOptions.levels.outHigh);

            p.exposureEnabledButton->setChecked(p.imageOptions.exposureEnabled);
            p.exposureSliders["Exposure"]->setValue(p.imageOptions.exposure.exposure);
            p.exposureSliders["Defog"]->setValue(p.imageOptions.exposure.defog);
            p.exposureSliders["KneeLow"]->setValue(p.imageOptions.exposure.kneeLow);
            p.exposureSliders["KneeHigh"]->setValue(p.imageOptions.exposure.kneeHigh);

            p.softClipEnabledButton->setChecked(p.imageOptions.softClipEnabled);
            p.softClipSlider->setValue(p.imageOptions.softClip);

            p.frameStoreEnabledButton->setChecked(p.frameStoreEnabled);
            p.frameStoreWidget->setImage(p.frameStore);
        }

    } // namespace ViewApp
} // namespace djv

