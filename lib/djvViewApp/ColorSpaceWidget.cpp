// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ColorSpaceWidget.h>

#include <djvViewApp/ColorSpaceWidgetPrivate.h>

#include <djvUI/Action.h>
#include <djvUI/Bellows.h>
#include <djvUI/ComboBox.h>
#include <djvUI/FormLayout.h>
#include <djvUI/Label.h>
#include <djvUI/PopupButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>

#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorSpaceWidget::Private
        {
            AV::OCIO::ConfigMode configMode = AV::OCIO::ConfigMode::First;
            AV::OCIO::Config envConfig;
            AV::OCIO::Config cmdLineConfig;
            AV::OCIO::UserConfigs userConfigs;
            AV::OCIO::Config currentConfig;

            std::shared_ptr<UI::ComboBox> configModeComboBox;
            std::shared_ptr<UI::PopupButton> configPopupButton;
            std::shared_ptr<UI::PopupButton> displayPopupButton;
            std::shared_ptr<UI::PopupButton> viewPopupButton;
            std::shared_ptr<UI::LabelSizeGroup> sizeGroup;
            std::shared_ptr<ColorSpaceImageWidget> imageWidget;
            std::shared_ptr<UI::FormLayout> formLayout;
            std::map<std::string, std::shared_ptr<UI::Bellows> > bellows;
            std::shared_ptr<UI::ScrollWidget> scrollWidget;

            std::shared_ptr<ValueObserver<AV::OCIO::ConfigMode> > configModeObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::Config> > envConfigObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::Config> > cmdLineConfigObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::UserConfigs> > userConfigsObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::Config> > currentConfigObserver;
        };

        void ColorSpaceWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ColorSpaceWidget");

            p.configModeComboBox = UI::ComboBox::create(context);

            p.configPopupButton = UI::PopupButton::create(UI::MenuButtonStyle::ComboBox, context);
            p.configPopupButton->setPopupIcon("djvIconPopupMenu");
            p.configPopupButton->setElide(labelElide);

            p.displayPopupButton = UI::PopupButton::create(UI::MenuButtonStyle::ComboBox, context);
            p.displayPopupButton->setPopupIcon("djvIconPopupMenu");
            p.displayPopupButton->setElide(labelElide);

            p.viewPopupButton = UI::PopupButton::create(UI::MenuButtonStyle::ComboBox, context);
            p.viewPopupButton->setPopupIcon("djvIconPopupMenu");
            p.viewPopupButton->setElide(labelElide);

            p.sizeGroup = UI::LabelSizeGroup::create();

            p.imageWidget = ColorSpaceImageWidget::create(context);
            p.imageWidget->setSizeGroup(p.sizeGroup);
            p.bellows["Image"] = UI::Bellows::create(context);
            p.bellows["Image"]->addChild(p.imageWidget);

            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            p.formLayout = UI::FormLayout::create(context);
            p.formLayout->setMargin(UI::MetricsRole::MarginSmall);
            p.formLayout->setLabelSizeGroup(p.sizeGroup);
            p.formLayout->addChild(p.configModeComboBox);
            p.formLayout->addChild(p.configPopupButton);
            p.formLayout->addChild(p.displayPopupButton);
            p.formLayout->addChild(p.viewPopupButton);
            vLayout->addChild(p.formLayout);
            vLayout->addChild(p.bellows["Image"]);
            p.scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.scrollWidget->setBorder(false);
            p.scrollWidget->setMinimumSizeRole(UI::MetricsRole::ScrollAreaSmall);
            p.scrollWidget->setBackgroundRole(UI::ColorRole::Background);
            p.scrollWidget->setShadowOverlay({ UI::Side::Top });
            p.scrollWidget->addChild(vLayout);
            addChild(p.scrollWidget);

            auto contextWeak = std::weak_ptr<Context>(context);
            p.configModeComboBox->setCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                        ocioSystem->setConfigMode(static_cast<AV::OCIO::ConfigMode>(value));
                    }
                });

            p.configPopupButton->setOpenCallback(
                [contextWeak]() -> std::shared_ptr<UI::Widget>
                {
                    std::shared_ptr<UI::Widget> out;
                    if (auto context = contextWeak.lock())
                    {
                        out = ColorSpaceConfigWidget::create(context);
                    }
                    return out;
                });

            p.displayPopupButton->setOpenCallback(
                [contextWeak]() -> std::shared_ptr<UI::Widget>
                {
                    std::shared_ptr<UI::Widget> out;
                    if (auto context = contextWeak.lock())
                    {
                        out = ColorSpaceDisplayWidget::create(context);
                    }
                    return out;
                });

            p.viewPopupButton->setOpenCallback(
                [contextWeak]() -> std::shared_ptr<UI::Widget>
                {
                    std::shared_ptr<UI::Widget> out;
                    if (auto context = contextWeak.lock())
                    {
                        out = ColorSpaceViewWidget::create(context);
                    }
                    return out;
                });

            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
            auto weak = std::weak_ptr<ColorSpaceWidget>(std::dynamic_pointer_cast<ColorSpaceWidget>(shared_from_this()));
            p.configModeObserver = ValueObserver<AV::OCIO::ConfigMode>::create(
                ocioSystem->observeConfigMode(),
                [weak](const AV::OCIO::ConfigMode& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->configMode = value;
                        widget->_widgetUpdate();
                    }
                });

            p.envConfigObserver = ValueObserver<AV::OCIO::Config>::create(
                ocioSystem->observeEnvConfig(),
                [weak](const AV::OCIO::Config& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->envConfig = value;
                        widget->_widgetUpdate();
                    }
                });

            p.cmdLineConfigObserver = ValueObserver<AV::OCIO::Config>::create(
                ocioSystem->observeCmdLineConfig(),
                [weak](const AV::OCIO::Config& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->cmdLineConfig = value;
                        widget->_widgetUpdate();
                    }
                });

            p.userConfigsObserver = ValueObserver<AV::OCIO::UserConfigs>::create(
                ocioSystem->observeUserConfigs(),
                [weak](const AV::OCIO::UserConfigs& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->userConfigs = value;
                        widget->_widgetUpdate();
                    }
                });

            p.currentConfigObserver = ValueObserver<AV::OCIO::Config>::create(
                ocioSystem->observeCurrentConfig(),
                [weak](const AV::OCIO::Config& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->currentConfig = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        ColorSpaceWidget::ColorSpaceWidget() :
            _p(new Private)
        {}

        ColorSpaceWidget::~ColorSpaceWidget()
        {}

        std::shared_ptr<ColorSpaceWidget> ColorSpaceWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceWidget>(new ColorSpaceWidget);
            out->_init(context);
            return out;
        }

        std::map<std::string, bool> ColorSpaceWidget::getBellowsState() const
        {
            DJV_PRIVATE_PTR();
            std::map<std::string, bool> out;
            for (const auto& i : p.bellows)
            {
                out[i.first] = i.second->isOpen();
            }
            return out;
        }

        void ColorSpaceWidget::setBellowsState(const std::map<std::string, bool>& value)
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

        void ColorSpaceWidget::_initLayoutEvent(Event::InitLayout&)
        {
            _p->sizeGroup->calcMinimumSize();
        }

        void ColorSpaceWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                setTitle(_getText(DJV_TEXT("widget_color_space")));
                p.formLayout->setText(p.configModeComboBox, _getText(DJV_TEXT("widget_color_space_mode")) + ":");
                p.formLayout->setText(p.configPopupButton, _getText(DJV_TEXT("widget_color_space_config")) + ":");
                p.formLayout->setText(p.displayPopupButton, _getText(DJV_TEXT("widget_color_space_display")) + ":");
                p.formLayout->setText(p.viewPopupButton, _getText(DJV_TEXT("widget_color_space_view")) + ":");
                p.bellows["Image"]->setText(_getText(DJV_TEXT("widget_color_space_image")));
                _widgetUpdate();
            }
        }

        void ColorSpaceWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                std::vector<std::shared_ptr<UI::Action> > items;
                for (const auto& i : AV::OCIO::getConfigModeEnums())
                {
                    auto action = UI::Action::create();
                    std::stringstream ss;
                    ss << i;
                    action->setText(_getText(DJV_TEXT(ss.str())));
                    items.push_back(action);
                }
                items[static_cast<int>(AV::OCIO::ConfigMode::Env)]->setEnabled(p.envConfig.isValid());
                items[static_cast<int>(AV::OCIO::ConfigMode::CmdLine)]->setEnabled(p.cmdLineConfig.isValid());
                p.configModeComboBox->setItems(items);
                p.configModeComboBox->setCurrentItem(static_cast<int>(p.configMode));

                std::string text;
                switch (p.configMode)
                {
                case AV::OCIO::ConfigMode::None:
                case AV::OCIO::ConfigMode::Env:
                case AV::OCIO::ConfigMode::CmdLine:
                {
                    std::stringstream ss;
                    ss << p.configMode;
                    text = _getText(ss.str());
                    break;
                }
                case AV::OCIO::ConfigMode::User:
                    text = p.userConfigs.second >= 0 &&
                        p.userConfigs.second < static_cast<int>(p.userConfigs.first.size()) ?
                        p.userConfigs.first[p.userConfigs.second].name :
                        _getText(DJV_TEXT("av_ocio_config_none"));
                    break;
                default: break;
                }
                p.configPopupButton->setText(text);

                p.displayPopupButton->setText(
                    !p.currentConfig.display.empty() ?
                    p.currentConfig.display :
                    _getText(DJV_TEXT("av_ocio_display_none")));

                p.viewPopupButton->setText(
                    !p.currentConfig.view.empty() ?
                    p.currentConfig.view :
                    _getText(DJV_TEXT("av_ocio_view_none")));
            }
        }

    } // namespace ViewApp
} // namespace djv

