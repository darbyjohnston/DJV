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

#include <djvOCIO/OCIOSystem.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorSpaceWidget::Private
        {
            OCIO::ConfigMode configMode = OCIO::ConfigMode::First;
            OCIO::Config envConfig;
            OCIO::Config cmdLineConfig;
            OCIO::UserConfigs userConfigs;
            OCIO::Config currentConfig;

            std::shared_ptr<UI::ComboBox> configModeComboBox;
            std::shared_ptr<UI::PopupButton> configPopupButton;
            std::shared_ptr<UI::PopupButton> displayPopupButton;
            std::shared_ptr<UI::PopupButton> viewPopupButton;
            std::shared_ptr<ColorSpaceImagesWidget> imagesWidget;
            std::shared_ptr<UI::FormLayout> formLayout;
            std::map<std::string, std::shared_ptr<UI::Bellows> > bellows;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::shared_ptr<Observer::Value<OCIO::ConfigMode> > configModeObserver;
            std::shared_ptr<Observer::Value<OCIO::Config> > envConfigObserver;
            std::shared_ptr<Observer::Value<OCIO::Config> > cmdLineConfigObserver;
            std::shared_ptr<Observer::Value<OCIO::UserConfigs> > userConfigsObserver;
            std::shared_ptr<Observer::Value<OCIO::Config> > currentConfigObserver;
        };

        void ColorSpaceWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ColorSpaceWidget");

            p.configModeComboBox = UI::ComboBox::create(context);

            p.configPopupButton = UI::PopupButton::create(UI::MenuButtonStyle::ComboBox, context);
            p.configPopupButton->setPopupIcon("djvIconPopupMenu");
            p.configPopupButton->setTextElide(textElide);

            p.displayPopupButton = UI::PopupButton::create(UI::MenuButtonStyle::ComboBox, context);
            p.displayPopupButton->setPopupIcon("djvIconPopupMenu");
            p.displayPopupButton->setTextElide(textElide);

            p.viewPopupButton = UI::PopupButton::create(UI::MenuButtonStyle::ComboBox, context);
            p.viewPopupButton->setPopupIcon("djvIconPopupMenu");
            p.viewPopupButton->setTextElide(textElide);

            p.imagesWidget = ColorSpaceImagesWidget::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.formLayout = UI::FormLayout::create(context);
            p.formLayout->setMargin(UI::MetricsRole::MarginSmall);
            p.formLayout->addChild(p.configModeComboBox);
            p.formLayout->addChild(p.configPopupButton);
            p.formLayout->addChild(p.displayPopupButton);
            p.formLayout->addChild(p.viewPopupButton);
            p.bellows["Config"] = UI::Bellows::create(context);
            p.bellows["Config"]->addChild(p.formLayout);
            p.layout->addChild(p.bellows["Config"]);
            p.bellows["Images"] = UI::Bellows::create(context);
            p.bellows["Images"]->addChild(p.imagesWidget);
            p.layout->addChild(p.bellows["Images"]);
            addChild(p.layout);

            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.configModeComboBox->setCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
                        ocioSystem->setConfigMode(static_cast<OCIO::ConfigMode>(value));
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

            auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
            auto weak = std::weak_ptr<ColorSpaceWidget>(std::dynamic_pointer_cast<ColorSpaceWidget>(shared_from_this()));
            p.configModeObserver = Observer::Value<OCIO::ConfigMode>::create(
                ocioSystem->observeConfigMode(),
                [weak](const OCIO::ConfigMode& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->configMode = value;
                        widget->_widgetUpdate();
                    }
                });

            p.envConfigObserver = Observer::Value<OCIO::Config>::create(
                ocioSystem->observeEnvConfig(),
                [weak](const OCIO::Config& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->envConfig = value;
                        widget->_widgetUpdate();
                    }
                });

            p.cmdLineConfigObserver = Observer::Value<OCIO::Config>::create(
                ocioSystem->observeCmdLineConfig(),
                [weak](const OCIO::Config& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->cmdLineConfig = value;
                        widget->_widgetUpdate();
                    }
                });

            p.userConfigsObserver = Observer::Value<OCIO::UserConfigs>::create(
                ocioSystem->observeUserConfigs(),
                [weak](const OCIO::UserConfigs& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->userConfigs = value;
                        widget->_widgetUpdate();
                    }
                });

            p.currentConfigObserver = Observer::Value<OCIO::Config>::create(
                ocioSystem->observeCurrentConfig(),
                [weak](const OCIO::Config& value)
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

        std::shared_ptr<ColorSpaceWidget> ColorSpaceWidget::create(const std::shared_ptr<System::Context>& context)
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

        void ColorSpaceWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ColorSpaceWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ColorSpaceWidget::_initEvent(System::Event::Init & event)
        {
            Widget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.formLayout->setText(p.configModeComboBox, _getText(DJV_TEXT("widget_color_space_mode")) + ":");
                p.formLayout->setText(p.configPopupButton, _getText(DJV_TEXT("widget_color_space_name")) + ":");
                p.formLayout->setText(p.displayPopupButton, _getText(DJV_TEXT("widget_color_space_display")) + ":");
                p.formLayout->setText(p.viewPopupButton, _getText(DJV_TEXT("widget_color_space_view")) + ":");
                p.bellows["Config"]->setText(_getText(DJV_TEXT("widget_color_space_config")));
                p.bellows["Images"]->setText(_getText(DJV_TEXT("widget_color_space_images")));
                _widgetUpdate();
            }
        }

        void ColorSpaceWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                std::vector<std::shared_ptr<UI::Action> > items;
                for (const auto& i : OCIO::getConfigModeEnums())
                {
                    auto action = UI::Action::create();
                    std::stringstream ss;
                    ss << i;
                    action->setText(_getText(DJV_TEXT(ss.str())));
                    items.push_back(action);
                }
                items[static_cast<int>(OCIO::ConfigMode::Env)]->setEnabled(p.envConfig.isValid());
                items[static_cast<int>(OCIO::ConfigMode::CmdLine)]->setEnabled(p.cmdLineConfig.isValid());
                p.configModeComboBox->setItems(items);
                p.configModeComboBox->setCurrentItem(static_cast<int>(p.configMode));

                std::string text;
                switch (p.configMode)
                {
                case OCIO::ConfigMode::None:
                case OCIO::ConfigMode::Env:
                case OCIO::ConfigMode::CmdLine:
                {
                    std::stringstream ss;
                    ss << p.configMode;
                    text = _getText(ss.str());
                    break;
                }
                case OCIO::ConfigMode::User:
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

