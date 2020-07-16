// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ColorSpaceWidgetPrivate.h>

#include <djvUIComponents/FileBrowserDialog.h>
#include <djvUIComponents/SearchBox.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/CheckBox.h>
#include <djvUI/FormLayout.h>
#include <djvUI/ListButton.h>
#include <djvUI/ListWidget.h>
#include <djvUI/PopupButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Spacer.h>
#include <djvUI/ToolButton.h>

#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorSpaceConfigWidget::Private
        {
            AV::OCIO::ConfigMode configMode = AV::OCIO::ConfigMode::First;
            AV::OCIO::UserConfigs userConfigs;
            AV::OCIO::Config envConfig;
            AV::OCIO::Config cmdLineConfig;
            Core::FileSystem::Path fileBrowserPath = Core::FileSystem::Path(".");
            bool deleteEnabled = false;

            std::shared_ptr<UI::ButtonGroup> modeButtonGroup;
            std::shared_ptr<UI::VerticalLayout> modeButtonLayout;
            std::shared_ptr<UI::ButtonGroup> userConfigButtonGroup;
            std::shared_ptr<UI::ButtonGroup> userConfigDeleteButtonGroup;
            std::shared_ptr<UI::ToolButton> userConfigAddButton;
            std::shared_ptr<UI::ToolButton> userConfigDeleteButton;
            std::shared_ptr<UI::VerticalLayout> userConfigButtonLayout;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::shared_ptr<UI::FileBrowser::Dialog> fileBrowserDialog;

            std::shared_ptr<ValueObserver<AV::OCIO::ConfigMode> > configModeObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::UserConfigs> > userConfigsObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::Config> > envConfigObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::Config> > cmdLineConfigObserver;
        };

        void ColorSpaceConfigWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            p.modeButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Radio);

            p.userConfigButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Exclusive);

            p.userConfigAddButton = UI::ToolButton::create(context);
            p.userConfigAddButton->setIcon("djvIconAdd");

            p.userConfigDeleteButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);
            p.userConfigDeleteButton = UI::ToolButton::create(context);
            p.userConfigDeleteButton->setButtonType(UI::ButtonType::Toggle);
            p.userConfigDeleteButton->setIcon("djvIconClear");

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.modeButtonLayout = UI::VerticalLayout::create(context);
            p.modeButtonLayout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(p.modeButtonLayout);
            p.layout->addSeparator();
            p.userConfigButtonLayout = UI::VerticalLayout::create(context);
            p.userConfigButtonLayout->setSpacing(UI::MetricsRole::None);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
            scrollWidget->setBorder(false);
            scrollWidget->addChild(p.userConfigButtonLayout);
            p.layout->addChild(scrollWidget);
            p.layout->setStretch(scrollWidget, UI::RowStretch::Expand);
            p.layout->addSeparator();
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addExpander();
            hLayout->addChild(p.userConfigAddButton);
            hLayout->addChild(p.userConfigDeleteButton);
            p.layout->addChild(hLayout);
            addChild(p.layout);

            auto contextWeak = std::weak_ptr<Context>(context);
            p.modeButtonGroup->setRadioCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                        ocioSystem->setConfigMode(static_cast<AV::OCIO::ConfigMode>(value));
                    }
                });

            p.userConfigButtonGroup->setExclusiveCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                        ocioSystem->setCurrentUserConfig(static_cast<int>(value));
                    }
                });

            auto weak = std::weak_ptr<ColorSpaceConfigWidget>(std::dynamic_pointer_cast<ColorSpaceConfigWidget>(shared_from_this()));
            p.userConfigAddButton->setClickedCallback(
                [weak, contextWeak]
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            if (widget->_p->fileBrowserDialog)
                            {
                                widget->_p->fileBrowserDialog->close();
                                widget->_p->fileBrowserDialog.reset();
                            }
                            widget->_p->fileBrowserDialog = UI::FileBrowser::Dialog::create(context);
                            widget->_p->fileBrowserDialog->setFileExtensions({ ".ocio" });
                            widget->_p->fileBrowserDialog->setPath(widget->_p->fileBrowserPath);
                            widget->_p->fileBrowserDialog->setCallback(
                                [weak, contextWeak](const Core::FileSystem::FileInfo& value)
                                {
                                    if (auto context = contextWeak.lock())
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->fileBrowserPath = widget->_p->fileBrowserDialog->getPath();
                                            widget->_p->fileBrowserDialog->close();
                                            widget->_p->fileBrowserDialog.reset();
                                            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                                            ocioSystem->addUserConfig(value.getPath().get());
                                        }
                                    }
                                });
                            widget->_p->fileBrowserDialog->setCloseCallback(
                                [weak]
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        if (widget->_p->fileBrowserDialog)
                                        {
                                            widget->_p->fileBrowserPath = widget->_p->fileBrowserDialog->getPath();
                                            widget->_p->fileBrowserDialog->close();
                                            widget->_p->fileBrowserDialog.reset();
                                        }
                                    }
                                });
                            widget->_p->fileBrowserDialog->show();
                        }
                    }
                });

            p.userConfigDeleteButtonGroup->setPushCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                        ocioSystem->removeUserConfig(static_cast<int>(value));
                    }
                });

            p.userConfigDeleteButton->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->deleteEnabled = value;
                        widget->_widgetUpdate();
                    }
                });

            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
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
        }

        ColorSpaceConfigWidget::ColorSpaceConfigWidget() :
            _p(new Private)
        {}

        ColorSpaceConfigWidget::~ColorSpaceConfigWidget()
        {
            DJV_PRIVATE_PTR();
            if (p.fileBrowserDialog)
            {
                p.fileBrowserDialog->close();
            }
        }

        std::shared_ptr<ColorSpaceConfigWidget> ColorSpaceConfigWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceConfigWidget>(new ColorSpaceConfigWidget);
            out->_init(context);
            return out;
        }

        void ColorSpaceConfigWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ColorSpaceConfigWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ColorSpaceConfigWidget::_initEvent(Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.userConfigAddButton->setTooltip(_getText(DJV_TEXT("widget_color_space_add_config_tooltip")));
                p.userConfigDeleteButton->setTooltip(_getText(DJV_TEXT("widget_color_space_delete_configs_tooltip")));
                _widgetUpdate();
            }
        }

        void ColorSpaceConfigWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.modeButtonGroup->clearButtons();
                p.modeButtonLayout->clearChildren();
                std::vector<std::shared_ptr<UI::Button::IButton> > buttons;
                for (const auto& i : AV::OCIO::getConfigModeEnums())
                {
                    std::stringstream ss;
                    ss << i;
                    auto button = UI::CheckBox::create(context);
                    std::string text = _getText(DJV_TEXT(ss.str()));
                    button->setText(text);
                    buttons.push_back(button);
                    p.modeButtonLayout->addChild(button);
                }
                buttons[static_cast<int>(AV::OCIO::ConfigMode::Env)]->setEnabled(p.envConfig.isValid());
                buttons[static_cast<int>(AV::OCIO::ConfigMode::CmdLine)]->setEnabled(p.cmdLineConfig.isValid());
                p.modeButtonGroup->setButtons(buttons, static_cast<int>(p.configMode));

                const bool userConfigMode = AV::OCIO::ConfigMode::User == p.configMode;
                p.userConfigButtonGroup->clearButtons();
                p.userConfigDeleteButtonGroup->clearButtons();
                p.userConfigButtonLayout->clearChildren();
                buttons.clear();
                auto contextWeak = std::weak_ptr<Context>(context);
                for (size_t i = 0; i < p.userConfigs.first.size(); ++i)
                {
                    const auto& config = p.userConfigs.first[i];
                    auto button = UI::ListButton::create(context);
                    button->setText(config.name);
                    button->setTooltip(config.fileName);
                    button->setEnabled(userConfigMode);
                    buttons.push_back(button);

                    auto deleteButton = UI::ToolButton::create(context);
                    deleteButton->setIcon("djvIconClearSmall");
                    deleteButton->setInsideMargin(UI::MetricsRole::None);
                    deleteButton->setVisible(p.deleteEnabled);
                    deleteButton->setTooltip(_getText(DJV_TEXT("widget_color_space_delete_config_tooltip")));
                    p.userConfigDeleteButtonGroup->addButton(deleteButton);

                    auto hLayout = UI::HorizontalLayout::create(context);
                    hLayout->setSpacing(UI::MetricsRole::None);
                    hLayout->addChild(button);
                    hLayout->setStretch(button, UI::RowStretch::Expand);
                    hLayout->addChild(deleteButton);
                    p.userConfigButtonLayout->addChild(hLayout);
                }
                p.userConfigButtonGroup->setButtons(buttons, p.userConfigs.second);
                p.userConfigAddButton->setEnabled(userConfigMode);
                if (p.userConfigs.first.empty())
                {
                    p.deleteEnabled = false;
                    p.userConfigDeleteButton->setChecked(false);
                }
                p.userConfigDeleteButton->setEnabled(userConfigMode && !p.userConfigs.first.empty());
            }
        }

    } // namespace ViewApp
} // namespace djv

