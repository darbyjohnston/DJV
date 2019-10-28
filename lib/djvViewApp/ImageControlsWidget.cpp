//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
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
#include <djvUI/FormLayout.h>
#include <djvUI/ImageWidget.h>
#include <djvUI/PushButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/TabWidget.h>

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
            ImageRotate rotate = ImageRotate::First;
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

            std::shared_ptr<UI::PushButton> loadFrameStoreButton;
            std::shared_ptr<UI::CheckBox> frameStoreCheckBox;
            std::shared_ptr<UI::ImageWidget> frameStoreWidget;
            std::shared_ptr<UI::ScrollWidget> frameStoreScrollWidget;

            std::shared_ptr<UI::TabWidget> tabWidget;

            std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<ValueObserver<AV::Render::ImageOptions> > imageOptionsObserver;
            std::shared_ptr<ValueObserver<ImageRotate> > rotateObserver;
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

            auto vLayout = UI::VerticalLayout::create(context);
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
                            widget->_p->rotate = static_cast<ImageRotate>(value);
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
                                widget->_p->rotateObserver = ValueObserver<ImageRotate>::create(
                                    widget->_p->activeWidget->getImageView()->observeImageRotate(),
                                    [weak](ImageRotate value)
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
        
        void ImageControlsWidget::_textUpdateEvent(Event::TextUpdate & event)
        {
            MDIWidget::_textUpdateEvent(event);
            DJV_PRIVATE_PTR();

            setTitle(_getText(DJV_TEXT("Image Controls")));
            
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
            p.channelsLayout->setText(p.alphaComboBox, _getText(DJV_TEXT("Alpha blend")) + ":");

            p.transformLayout->setText(p.mirrorCheckBoxes[0], _getText(DJV_TEXT("Mirror horizontal")) + ":");
            p.transformLayout->setText(p.mirrorCheckBoxes[1], _getText(DJV_TEXT("Mirror vertical")) + ":");
            p.rotateComboBox->clearItems();
            for (auto i : getImageRotateEnums())
            {
                std::stringstream ss;
                ss << i;
                p.rotateComboBox->addItem(_getText(ss.str()));
            }
            p.transformLayout->setText(p.rotateComboBox, _getText(DJV_TEXT("Rotate")) + ":");
            p.aspectRatioComboBox->clearItems();
            for (auto i : UI::getImageAspectRatioEnums())
            {
                std::stringstream ss;
                ss << i;
                p.aspectRatioComboBox->addItem(_getText(ss.str()));
            }
            p.transformLayout->setText(p.aspectRatioComboBox, _getText(DJV_TEXT("Aspect ratio")) + ":");

            p.loadFrameStoreButton->setText(_getText(DJV_TEXT("Load")));
            p.frameStoreCheckBox->setText(_getText(DJV_TEXT("Enabled")));

            p.tabWidget->setText(p.channelsScrollWidget, _getText(DJV_TEXT("Channels")));
            p.tabWidget->setText(p.transformScrollWidget, _getText(DJV_TEXT("Transform")));
            p.tabWidget->setText(p.frameStoreScrollWidget, _getText(DJV_TEXT("Frame Store")));

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
            p.frameStoreWidget->setImage(p.frameStore);
        }

    } // namespace ViewApp
} // namespace djv

