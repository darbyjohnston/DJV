//------------------------------------------------------------------------------
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <djvViewApp/ImageControlsWidget.h>

#include <djvViewApp/ImageSettings.h>
#include <djvViewApp/ImageSystem.h>
#include <djvViewApp/ImageView.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/CheckBox.h>
#include <djvUI/ComboBox.h>
#include <djvUI/FloatSlider.h>
#include <djvUI/FormLayout.h>
#include <djvUI/ImageWidget.h>
#include <djvUI/PushButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/TabWidget.h>
#include <djvUI/ToggleButton.h>

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
            AV::Render::ImageOptions imageOptions;
            UI::ImageRotate rotate = UI::ImageRotate::First;
            UI::ImageAspectRatio aspectRatio = UI::ImageAspectRatio::First;
            bool frameStoreEnabled = false;
            std::shared_ptr<AV::Image::Image> frameStore;

            std::shared_ptr<MediaWidget> activeWidget;

            std::vector<std::shared_ptr<UI::CheckBox> > channelCheckBoxes;
            std::shared_ptr<UI::ButtonGroup> channelButtonGroup;
            std::shared_ptr<UI::ComboBox> alphaComboBox;
            std::shared_ptr<UI::FormLayout> channelsLayout;
            std::shared_ptr<UI::ScrollWidget> channelsScrollWidget;

            std::shared_ptr<UI::CheckBox> mirrorCheckBoxes[2];
            std::shared_ptr<UI::ComboBox> rotateComboBox;
            std::shared_ptr<UI::ComboBox> aspectRatioComboBox;
            std::shared_ptr<UI::FormLayout> transformLayout;
            std::shared_ptr<UI::ScrollWidget> transformScrollWidget;

            std::map<std::string, std::shared_ptr<UI::FloatSlider> > colorSliders;
            std::shared_ptr<UI::CheckBox> colorInvertCheckBox;
            std::shared_ptr<UI::FormLayout> colorLayout;
            std::shared_ptr<UI::ScrollWidget> colorScrollWidget;

            std::map<std::string, std::shared_ptr<UI::FloatSlider> > levelsSliders;
            std::shared_ptr<UI::FormLayout> levelsLayout;
            std::shared_ptr<UI::ScrollWidget> levelsScrollWidget;

            std::shared_ptr<UI::ToggleButton> exposureButton;
            std::map<std::string, std::shared_ptr<UI::FloatSlider> > exposureSliders;
            std::shared_ptr<UI::FormLayout> exposureLayout;
            std::shared_ptr<UI::ScrollWidget> exposureScrollWidget;

            std::shared_ptr<UI::FloatSlider> softClipSlider;
            std::shared_ptr<UI::ScrollWidget> softClipScrollWidget;

            std::shared_ptr<UI::PushButton> loadFrameStoreButton;
            std::shared_ptr<UI::CheckBox> frameStoreCheckBox;
            std::shared_ptr<UI::ImageWidget> frameStoreWidget;
            std::shared_ptr<UI::ScrollWidget> frameStoreScrollWidget;

            std::shared_ptr<UI::TabWidget> tabWidget;

            std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<ValueObserver<AV::Render::ImageOptions> > imageOptionsObserver;
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

            p.channelButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Exclusive);
            for (size_t i = 1; i < static_cast<size_t>(AV::Render::ImageChannel::Count); ++i)
            {
                auto checkBox = UI::CheckBox::create(context);
                p.channelButtonGroup->addButton(checkBox);
            }
            p.alphaComboBox = UI::ComboBox::create(context);

            for (size_t i = 0; i < 2; ++i)
            {
                p.mirrorCheckBoxes[i] = UI::CheckBox::create(context);
            }
            p.rotateComboBox = UI::ComboBox::create(context);
            p.aspectRatioComboBox = UI::ComboBox::create(context);

            p.colorSliders["Brightness"] = UI::FloatSlider::create(context);
            p.colorSliders["Brightness"]->setRange(FloatRange(0.F, 4.F));
            const AV::Render::ImageColor color;
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

            p.levelsSliders["InLow"] = UI::FloatSlider::create(context);
            const AV::Render::ImageLevels levels;
            p.levelsSliders["InLow"]->setDefault(levels.inLow);
            p.levelsSliders["InHigh"] = UI::FloatSlider::create(context);
            p.levelsSliders["InHigh"]->setDefault(levels.inHigh);
            p.levelsSliders["Gamma"] = UI::FloatSlider::create(context);
            p.levelsSliders["Gamma"]->setRange(FloatRange(0.F, 4.F));
            p.levelsSliders["Gamma"]->setDefault(levels.gamma);
            p.levelsSliders["OutLow"] = UI::FloatSlider::create(context);
            p.levelsSliders["OutLow"]->setDefault(levels.outLow);
            p.levelsSliders["OutHigh"] = UI::FloatSlider::create(context);
            p.levelsSliders["OutHigh"]->setDefault(levels.outHigh);
            for (const auto& slider : p.levelsSliders)
            {
                slider.second->setDefaultVisible(true);
            }

            p.exposureButton = UI::ToggleButton::create(context);
            p.exposureSliders["Exposure"] = UI::FloatSlider::create(context);
            p.exposureSliders["Exposure"]->setRange(FloatRange(-10.F, 10.F));
            const AV::Render::ImageExposure exposure;
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

            p.softClipSlider = UI::FloatSlider::create(context);
            p.softClipSlider->setDefaultVisible(true);

            p.loadFrameStoreButton = UI::PushButton::create(context);
            p.frameStoreCheckBox = UI::CheckBox::create(context);
            p.frameStoreWidget = UI::ImageWidget::create(context);
            p.frameStoreWidget->setSizeRole(UI::MetricsRole::TextColumn);
            p.frameStoreWidget->setHAlign(UI::HAlign::Center);
            p.frameStoreWidget->setVAlign(UI::VAlign::Center);

            p.channelsLayout = UI::FormLayout::create(context);
            p.channelsLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.channelsLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            p.channelsLayout->setShadowOverlay({ UI::Side::Top });
            for (const auto& i : p.channelButtonGroup->getButtons())
            {
                p.channelsLayout->addChild(i);
            }
            p.channelsLayout->addChild(p.alphaComboBox);
            p.channelsScrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.channelsScrollWidget->setBorder(false);
            p.channelsScrollWidget->addChild(p.channelsLayout);

            p.transformLayout = UI::FormLayout::create(context);
            p.transformLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.transformLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            for (size_t i = 0; i < 2; ++i)
            {
                p.transformLayout->addChild(p.mirrorCheckBoxes[i]);
            }
            p.transformLayout->addChild(p.rotateComboBox);
            p.transformLayout->addChild(p.aspectRatioComboBox);
            p.transformScrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.transformScrollWidget->setBorder(false);
            p.transformScrollWidget->addChild(p.transformLayout);

            p.colorLayout = UI::FormLayout::create(context);
            p.colorLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.colorLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            p.colorLayout->setShadowOverlay({ UI::Side::Top });
            for (const auto& i : { "Brightness", "Contrast", "Saturation" })
            {
                p.colorLayout->addChild(p.colorSliders[i]);
            }
            p.colorLayout->addChild(p.colorInvertCheckBox);
            p.colorScrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.colorScrollWidget->setBorder(false);
            p.colorScrollWidget->addChild(p.colorLayout);

            p.levelsLayout = UI::FormLayout::create(context);
            p.levelsLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.levelsLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            p.levelsLayout->setShadowOverlay({ UI::Side::Top });
            for (const auto& i : { "InLow", "InHigh", "Gamma", "OutLow", "OutHigh" })
            {
                p.levelsLayout->addChild(p.levelsSliders[i]);
            }
            p.levelsScrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.levelsScrollWidget->setBorder(false);
            p.levelsScrollWidget->addChild(p.levelsLayout);

            p.exposureLayout = UI::FormLayout::create(context);
            p.exposureLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.exposureLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            p.exposureLayout->addChild(p.exposureButton);
            p.exposureLayout->setShadowOverlay({ UI::Side::Top });
            for (const auto& i : { "Exposure", "Defog", "KneeLow", "KneeHigh" })
            {
                p.exposureLayout->addChild(p.exposureSliders[i]);
            }
            p.exposureScrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.exposureScrollWidget->setBorder(false);
            p.exposureScrollWidget->addChild(p.exposureLayout);

            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            vLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            vLayout->setShadowOverlay({ UI::Side::Top });
            vLayout->addChild(p.softClipSlider);
            p.softClipScrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.softClipScrollWidget->setBorder(false);
            p.softClipScrollWidget->addChild(vLayout);

            vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            vLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            vLayout->addChild(p.loadFrameStoreButton);
            vLayout->addChild(p.frameStoreCheckBox);
            vLayout->addChild(p.frameStoreWidget);
            p.frameStoreScrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.frameStoreScrollWidget->setBorder(false);
            p.frameStoreScrollWidget->addChild(vLayout);

            p.tabWidget = UI::TabWidget::create(context);
            p.tabWidget->setBackgroundRole(UI::ColorRole::Background);
            p.tabWidget->setShadowOverlay({ UI::Side::Top });
            p.tabWidget->addChild(p.channelsScrollWidget);
            p.tabWidget->addChild(p.transformScrollWidget);
            p.tabWidget->addChild(p.colorScrollWidget);
            p.tabWidget->addChild(p.levelsScrollWidget);
            p.tabWidget->addChild(p.exposureScrollWidget);
            p.tabWidget->addChild(p.softClipScrollWidget);
            p.tabWidget->addChild(p.frameStoreScrollWidget);
            addChild(p.tabWidget);

            _widgetUpdate();

            auto weak = std::weak_ptr<ImageControlsWidget>(std::dynamic_pointer_cast<ImageControlsWidget>(shared_from_this()));
            p.channelButtonGroup->setExclusiveCallback(
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.channel = static_cast<AV::Render::ImageChannel>(value + 1);
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getImageView()->setImageOptions(widget->_p->imageOptions);
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
                                widget->_p->activeWidget->getImageView()->setImageOptions(widget->_p->imageOptions);
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
                            widget->_p->activeWidget->getImageView()->setImageOptions(widget->_p->imageOptions);
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
                            widget->_p->activeWidget->getImageView()->setImageOptions(widget->_p->imageOptions);
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
                                widget->_p->activeWidget->getImageView()->setImageRotate(widget->_p->rotate);
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
                                widget->_p->activeWidget->getImageView()->setImageAspectRatio(widget->_p->aspectRatio);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                            imageSettings->setAspectRatio(widget->_p->aspectRatio);
                        }
                    }
                });

            p.colorSliders["Brightness"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.color.brightness = value;
                        widget->_p->imageOptions.colorEnabled = widget->_p->imageOptions.color != AV::Render::ImageColor();
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getImageView()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });
            p.colorSliders["Contrast"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.color.contrast = value;
                        widget->_p->imageOptions.colorEnabled = widget->_p->imageOptions.color != AV::Render::ImageColor();
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getImageView()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });
            p.colorSliders["Saturation"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.color.saturation = value;
                        widget->_p->imageOptions.colorEnabled = widget->_p->imageOptions.color != AV::Render::ImageColor();
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getImageView()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });
            p.colorInvertCheckBox->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.color.invert = value;
                        widget->_p->imageOptions.colorEnabled = widget->_p->imageOptions.color != AV::Render::ImageColor();
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getImageView()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });

            p.levelsSliders["InLow"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.levels.inLow = value;
                        widget->_p->imageOptions.levelsEnabled = widget->_p->imageOptions.levels != AV::Render::ImageLevels();
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getImageView()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });
            p.levelsSliders["InHigh"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.levels.inHigh = value;
                        widget->_p->imageOptions.levelsEnabled = widget->_p->imageOptions.levels != AV::Render::ImageLevels();
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getImageView()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });
            p.levelsSliders["Gamma"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.levels.gamma = value;
                        widget->_p->imageOptions.levelsEnabled = widget->_p->imageOptions.levels != AV::Render::ImageLevels();
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getImageView()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });
            p.levelsSliders["OutLow"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.levels.outLow = value;
                        widget->_p->imageOptions.levelsEnabled = widget->_p->imageOptions.levels != AV::Render::ImageLevels();
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getImageView()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });
            p.levelsSliders["OutHigh"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.levels.outHigh = value;
                        widget->_p->imageOptions.levelsEnabled = widget->_p->imageOptions.levels != AV::Render::ImageLevels();
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getImageView()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });

            p.exposureButton->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.exposureEnabled = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getImageView()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });

            p.exposureSliders["Exposure"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.exposureEnabled = true;
                        widget->_p->imageOptions.exposure.exposure = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getImageView()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });
            p.exposureSliders["Defog"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.exposureEnabled = true;
                        widget->_p->imageOptions.exposure.defog = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getImageView()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });
            p.exposureSliders["KneeLow"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.exposureEnabled = true;
                        widget->_p->imageOptions.exposure.kneeLow = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getImageView()->setImageOptions(widget->_p->imageOptions);
                        }
                    }
                });
            p.exposureSliders["KneeHigh"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageOptions.exposureEnabled = true;
                        widget->_p->imageOptions.exposure.kneeHigh = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getImageView()->setImageOptions(widget->_p->imageOptions);
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
                            widget->_p->activeWidget->getImageView()->setImageOptions(widget->_p->imageOptions);
                        }
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

            p.frameStoreCheckBox->setCheckedCallback(
                [contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto imageSystem = context->getSystemT<ImageSystem>();
                        imageSystem->setFrameStoreEnabled(value);
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
                                widget->_p->imageOptionsObserver = ValueObserver<AV::Render::ImageOptions>::create(
                                    widget->_p->activeWidget->getImageView()->observeImageOptions(),
                                    [weak](const AV::Render::ImageOptions& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->imageOptions = value;
                                            widget->_widgetUpdate();
                                        }
                                    });
                                widget->_p->rotateObserver = ValueObserver<UI::ImageRotate>::create(
                                    widget->_p->activeWidget->getImageView()->observeImageRotate(),
                                    [weak](UI::ImageRotate value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->rotate = value;
                                            widget->_widgetUpdate();
                                        }
                                    });
                                widget->_p->aspectRatioObserver = ValueObserver<UI::ImageAspectRatio>::create(
                                    widget->_p->activeWidget->getImageView()->observeImageAspectRatio(),
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
        
        int ImageControlsWidget::getCurrentTab() const
        {
            return _p->tabWidget->getCurrentTab();
        }

        void ImageControlsWidget::setCurrentTab(int value)
        {
            _p->tabWidget->setCurrentTab(value);
        }
        
        void ImageControlsWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            DJV_PRIVATE_PTR();

            setTitle(_getText(DJV_TEXT("image_controls_title")));
            
            const auto& channelButtons = p.channelButtonGroup->getButtons();
            const auto& channelEnums = AV::Render::getImageChannelEnums();
            size_t j = 0;
            for (size_t i = 1; i < static_cast<size_t>(AV::Render::ImageChannel::Count); ++i, ++j)
            {
                std::stringstream ss;
                ss << channelEnums[i];
                p.channelsLayout->setText(channelButtons[j], _getText(ss.str()) + ":");
            }

            p.alphaComboBox->clearItems();
            for (auto i : AV::getAlphaBlendEnums())
            {
                std::stringstream ss;
                ss << i;
                p.alphaComboBox->addItem(_getText(ss.str()));
            }
            p.channelsLayout->setText(p.alphaComboBox, _getText(DJV_TEXT("image_controls_channels_alpha_blend")) + ":");

            p.transformLayout->setText(p.mirrorCheckBoxes[0], _getText(DJV_TEXT("image_controls_transform_mirror_horizontal")) + ":");
            p.transformLayout->setText(p.mirrorCheckBoxes[1], _getText(DJV_TEXT("image_controls_transform_mirror_vertical")) + ":");
            p.rotateComboBox->clearItems();
            for (auto i : UI::getImageRotateEnums())
            {
                std::stringstream ss;
                ss << i;
                p.rotateComboBox->addItem(_getText(ss.str()));
            }
            p.transformLayout->setText(p.rotateComboBox, _getText(DJV_TEXT("image_controls_transform_rotate")) + ":");
            p.aspectRatioComboBox->clearItems();
            for (auto i : UI::getImageAspectRatioEnums())
            {
                std::stringstream ss;
                ss << i;
                p.aspectRatioComboBox->addItem(_getText(ss.str()));
            }
            p.transformLayout->setText(p.aspectRatioComboBox, _getText(DJV_TEXT("image_controls_transform_aspect_ratio")) + ":");

            p.colorLayout->setText(p.colorSliders["Brightness"], _getText(DJV_TEXT("image_controls_color_brightness")) + ":");
            p.colorLayout->setText(p.colorSliders["Contrast"], _getText(DJV_TEXT("image_controls_color_contrast")) + ":");
            p.colorLayout->setText(p.colorSliders["Saturation"], _getText(DJV_TEXT("image_controls_color_saturation")) + ":");
            p.colorLayout->setText(p.colorInvertCheckBox, _getText(DJV_TEXT("image_controls_color_invert")) + ":");

            p.levelsLayout->setText(p.levelsSliders["InLow"], _getText(DJV_TEXT("image_controls_levels_in_low")) + ":");
            p.levelsLayout->setText(p.levelsSliders["InHigh"], _getText(DJV_TEXT("image_controls_levels_in_high")) + ":");
            p.levelsLayout->setText(p.levelsSliders["Gamma"], _getText(DJV_TEXT("image_controls_levels_gamma")) + ":");
            p.levelsLayout->setText(p.levelsSliders["OutLow"], _getText(DJV_TEXT("image_controls_levels_out_low")) + ":");
            p.levelsLayout->setText(p.levelsSliders["OutHigh"], _getText(DJV_TEXT("image_controls_levels_out_high")) + ":");

            p.exposureLayout->setText(p.exposureButton, _getText(DJV_TEXT("image_controls_exposure_enabled")) + ":");
            p.exposureLayout->setText(p.exposureSliders["Exposure"], _getText(DJV_TEXT("image_controls_exposure_exposure")) + ":");
            p.exposureLayout->setText(p.exposureSliders["Defog"], _getText(DJV_TEXT("image_controls_exposure_defog")) + ":");
            p.exposureLayout->setText(p.exposureSliders["KneeLow"], _getText(DJV_TEXT("image_controls_exposure_knee_low")) + ":");
            p.exposureLayout->setText(p.exposureSliders["KneeHigh"], _getText(DJV_TEXT("image_controls_exposure_knee_high")) + ":");

            p.loadFrameStoreButton->setText(_getText(DJV_TEXT("load")));
            p.frameStoreCheckBox->setText(_getText(DJV_TEXT("image_controls_frame_store_enabled")));

            p.tabWidget->setText(p.channelsScrollWidget, _getText(DJV_TEXT("image_controls_section_channels")));
            p.tabWidget->setText(p.transformScrollWidget, _getText(DJV_TEXT("image_controls_section_transform")));
            p.tabWidget->setText(p.colorScrollWidget, _getText(DJV_TEXT("image_controls_section_color")));
            p.tabWidget->setText(p.levelsScrollWidget, _getText(DJV_TEXT("image_controls_section_levels")));
            p.tabWidget->setText(p.exposureScrollWidget, _getText(DJV_TEXT("image_controls_section_exposure")));
            p.tabWidget->setText(p.softClipScrollWidget, _getText(DJV_TEXT("image_controls_section_soft_clip")));
            p.tabWidget->setText(p.frameStoreScrollWidget, _getText(DJV_TEXT("image_controls_section_frame_store")));

            _widgetUpdate();
        }

        void ImageControlsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();

            p.channelButtonGroup->setChecked(static_cast<int>(p.imageOptions.channel) - 1);
            p.alphaComboBox->setCurrentItem(static_cast<int>(p.imageOptions.alphaBlend));

            p.mirrorCheckBoxes[0]->setChecked(p.imageOptions.mirror.x);
            p.mirrorCheckBoxes[1]->setChecked(p.imageOptions.mirror.y);
            p.rotateComboBox->setCurrentItem(static_cast<int>(p.rotate));
            p.aspectRatioComboBox->setCurrentItem(static_cast<int>(p.aspectRatio));
            
            p.colorSliders["Brightness"]->setValue(p.imageOptions.color.brightness);
            p.colorSliders["Contrast"]->setValue(p.imageOptions.color.contrast);
            p.colorSliders["Saturation"]->setValue(p.imageOptions.color.saturation);
            p.colorInvertCheckBox->setChecked(p.imageOptions.color.invert);

            p.levelsSliders["InLow"]->setValue(p.imageOptions.levels.inLow);
            p.levelsSliders["InHigh"]->setValue(p.imageOptions.levels.inHigh);
            p.levelsSliders["Gamma"]->setValue(p.imageOptions.levels.gamma);
            p.levelsSliders["OutLow"]->setValue(p.imageOptions.levels.outLow);
            p.levelsSliders["OutHigh"]->setValue(p.imageOptions.levels.outHigh);

            p.exposureButton->setChecked(p.imageOptions.exposureEnabled);
            p.exposureSliders["Exposure"]->setValue(p.imageOptions.exposure.exposure);
            p.exposureSliders["Defog"]->setValue(p.imageOptions.exposure.defog);
            p.exposureSliders["KneeLow"]->setValue(p.imageOptions.exposure.kneeLow);
            p.exposureSliders["KneeHigh"]->setValue(p.imageOptions.exposure.kneeHigh);

            p.softClipSlider->setValue(p.imageOptions.softClip);

            p.frameStoreWidget->setImage(p.frameStore);
        }

    } // namespace ViewApp
} // namespace djv

