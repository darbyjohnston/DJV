// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ColorSpaceWidgetPrivate.h>

#include <djvUIComponents/FileBrowserDialog.h>
#include <djvUIComponents/SearchBox.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/FormLayout.h>
#include <djvUI/ListButton.h>
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
        namespace
        {
            class ModeWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(ModeWidget);

            protected:
                void _init(const std::shared_ptr<Context>&);
                ModeWidget();

            public:
                ~ModeWidget() override;

                static std::shared_ptr<ModeWidget> create(const std::shared_ptr<Context>&);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

                void _initEvent(Event::Init&) override;

            private:
                void _widgetUpdate();

                AV::OCIO::ConfigMode _configMode = AV::OCIO::ConfigMode::First;
                AV::OCIO::Config _envConfig;
                AV::OCIO::Config _cmdLineConfig;

                std::shared_ptr<UI::ButtonGroup> _buttonGroup;
                std::shared_ptr<UI::VerticalLayout> _buttonLayout;
                std::shared_ptr<UI::ScrollWidget> _scrollWidget;

                std::shared_ptr<ValueObserver<AV::OCIO::ConfigMode> > _configModeObserver;
                std::shared_ptr<ValueObserver<AV::OCIO::Config> > _envConfigObserver;
                std::shared_ptr<ValueObserver<AV::OCIO::Config> > _cmdLineConfigObserver;
            };

            void ModeWidget::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                _buttonGroup = UI::ButtonGroup::create(UI::ButtonType::Radio);

                _buttonLayout = UI::VerticalLayout::create(context);
                _buttonLayout->setSpacing(UI::MetricsRole::None);
                _scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
                _scrollWidget->setBorder(false);
                _scrollWidget->setMinimumSizeRole(UI::MetricsRole::None);
                _scrollWidget->addChild(_buttonLayout);
                addChild(_scrollWidget);

                auto contextWeak = std::weak_ptr<Context>(context);
                _buttonGroup->setRadioCallback(
                    [contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                            ocioSystem->setConfigMode(static_cast<AV::OCIO::ConfigMode>(value));
                        }
                    });

                auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                auto weak = std::weak_ptr<ModeWidget>(std::dynamic_pointer_cast<ModeWidget>(shared_from_this()));
                _configModeObserver = ValueObserver<AV::OCIO::ConfigMode>::create(
                    ocioSystem->observeConfigMode(),
                    [weak](const AV::OCIO::ConfigMode& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_configMode = value;
                            widget->_widgetUpdate();
                        }
                    });

                _envConfigObserver = ValueObserver<AV::OCIO::Config>::create(
                    ocioSystem->observeEnvConfig(),
                    [weak](const AV::OCIO::Config& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_envConfig = value;
                            widget->_widgetUpdate();
                        }
                    });

                _cmdLineConfigObserver = ValueObserver<AV::OCIO::Config>::create(
                    ocioSystem->observeCmdLineConfig(),
                    [weak](const AV::OCIO::Config& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_cmdLineConfig = value;
                            widget->_widgetUpdate();
                        }
                    });
            }

            ModeWidget::ModeWidget()
            {}

            ModeWidget::~ModeWidget()
            {}

            std::shared_ptr<ModeWidget> ModeWidget::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<ModeWidget>(new ModeWidget);
                out->_init(context);
                return out;
            }

            void ModeWidget::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_scrollWidget->getMinimumSize());
            }

            void ModeWidget::_layoutEvent(Event::Layout&)
            {
                _scrollWidget->setGeometry(getGeometry());
            }

            void ModeWidget::_initEvent(Event::Init& event)
            {
                if (event.getData().text)
                {
                    _widgetUpdate();
                }
            }

            void ModeWidget::_widgetUpdate()
            {
                if (auto context = getContext().lock())
                {
                    _buttonGroup->clearButtons();
                    _buttonLayout->clearChildren();
                    std::vector<std::shared_ptr<UI::Button::IButton> > buttons;
                    for (const auto& i : AV::OCIO::getConfigModeEnums())
                    {
                        std::stringstream ss;
                        ss << i;
                        auto button = UI::ListButton::create(context);
                        std::string text = _getText(DJV_TEXT(ss.str()));
                        button->setText(text);
                        buttons.push_back(button);
                        _buttonLayout->addChild(button);
                    }
                    buttons[static_cast<int>(AV::OCIO::ConfigMode::Env)]->setEnabled(_envConfig.isValid());
                    buttons[static_cast<int>(AV::OCIO::ConfigMode::CmdLine)]->setEnabled(_cmdLineConfig.isValid());
                    _buttonGroup->setButtons(buttons, static_cast<int>(_configMode));
                }
            }

            class UserConfigWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(UserConfigWidget);

            protected:
                void _init(const std::shared_ptr<Context>&);
                UserConfigWidget();

            public:
                ~UserConfigWidget() override;

                static std::shared_ptr<UserConfigWidget> create(const std::shared_ptr<Context>&);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

                void _initEvent(Event::Init&) override;

            private:
                void _widgetUpdate();

                AV::OCIO::UserConfigs _userConfigs;
                Core::FileSystem::Path _fileBrowserPath = Core::FileSystem::Path(".");
                bool _deleteEnabled = false;

                std::shared_ptr<UI::ButtonGroup> _buttonGroup;
                std::shared_ptr<UI::ButtonGroup> _deleteButtonGroup;
                std::shared_ptr<UI::ToolButton> _addButton;
                std::shared_ptr<UI::ToolButton> _deleteButton;
                std::shared_ptr<UI::VerticalLayout> _buttonLayout;
                std::shared_ptr<UI::VerticalLayout> _layout;
                std::shared_ptr<UI::FileBrowser::Dialog> _fileBrowserDialog;

                std::shared_ptr<ValueObserver<AV::OCIO::UserConfigs> > _userConfigsObserver;
            };

            void UserConfigWidget::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                _buttonGroup = UI::ButtonGroup::create(UI::ButtonType::Exclusive);

                _addButton = UI::ToolButton::create(context);
                _addButton->setIcon("djvIconAdd");

                _deleteButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);
                _deleteButton = UI::ToolButton::create(context);
                _deleteButton->setButtonType(UI::ButtonType::Toggle);
                _deleteButton->setIcon("djvIconClear");

                _layout = UI::VerticalLayout::create(context);
                _layout->setSpacing(UI::MetricsRole::None);
                auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
                scrollWidget->setBorder(false);
                _buttonLayout = UI::VerticalLayout::create(context);
                _buttonLayout->setSpacing(UI::MetricsRole::None);
                scrollWidget->addChild(_buttonLayout);
                _layout->addChild(scrollWidget);
                _layout->setStretch(scrollWidget, UI::RowStretch::Expand);
                _layout->addSeparator();
                auto hLayout = UI::HorizontalLayout::create(context);
                hLayout->setSpacing(UI::MetricsRole::None);
                hLayout->addExpander();
                hLayout->addChild(_addButton);
                hLayout->addChild(_deleteButton);
                _layout->addChild(hLayout);
                addChild(_layout);

                auto contextWeak = std::weak_ptr<Context>(context);
                _buttonGroup->setExclusiveCallback(
                    [contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                            ocioSystem->setCurrentUserConfig(static_cast<int>(value));
                        }
                    });

                auto weak = std::weak_ptr<UserConfigWidget>(std::dynamic_pointer_cast<UserConfigWidget>(shared_from_this()));
                _addButton->setClickedCallback(
                    [weak, contextWeak]
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                if (widget->_fileBrowserDialog)
                                {
                                    widget->_fileBrowserDialog->close();
                                    widget->_fileBrowserDialog.reset();
                                }
                                widget->_fileBrowserDialog = UI::FileBrowser::Dialog::create(context);
                                widget->_fileBrowserDialog->setFileExtensions({ ".ocio" });
                                widget->_fileBrowserDialog->setPath(widget->_fileBrowserPath);
                                widget->_fileBrowserDialog->setCallback(
                                    [weak, contextWeak](const Core::FileSystem::FileInfo& value)
                                    {
                                        if (auto context = contextWeak.lock())
                                        {
                                            if (auto widget = weak.lock())
                                            {
                                                widget->_fileBrowserPath = widget->_fileBrowserDialog->getPath();
                                                widget->_fileBrowserDialog->close();
                                                widget->_fileBrowserDialog.reset();
                                                auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                                                ocioSystem->addUserConfig(value.getPath().get());
                                            }
                                        }
                                    });
                                widget->_fileBrowserDialog->setCloseCallback(
                                    [weak]
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_fileBrowserPath = widget->_fileBrowserDialog->getPath();
                                            widget->_fileBrowserDialog->close();
                                            widget->_fileBrowserDialog.reset();
                                        }
                                    });
                                widget->_fileBrowserDialog->show();
                            }
                        }
                    });

                _deleteButtonGroup->setPushCallback(
                    [contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                            ocioSystem->removeUserConfig(static_cast<int>(value));
                        }
                    });

                _deleteButton->setCheckedCallback(
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_deleteEnabled = value;
                            widget->_widgetUpdate();
                        }
                    });

                auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                _userConfigsObserver = ValueObserver<AV::OCIO::UserConfigs>::create(
                    ocioSystem->observeUserConfigs(),
                    [weak](const AV::OCIO::UserConfigs& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_userConfigs = value;
                            widget->_widgetUpdate();
                        }
                    });
            }

            UserConfigWidget::UserConfigWidget()
            {}

            UserConfigWidget::~UserConfigWidget()
            {
                if (_fileBrowserDialog)
                {
                    _fileBrowserDialog->close();
                }
            }

            std::shared_ptr<UserConfigWidget> UserConfigWidget::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<UserConfigWidget>(new UserConfigWidget);
                out->_init(context);
                return out;
            }

            void UserConfigWidget::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void UserConfigWidget::_layoutEvent(Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

            void UserConfigWidget::_initEvent(Event::Init& event)
            {
                if (event.getData().text)
                {
                    _addButton->setTooltip(_getText(DJV_TEXT("widget_color_space_add_config_tooltip")));
                    _deleteButton->setTooltip(_getText(DJV_TEXT("widget_color_space_delete_configs_tooltip")));
                    _widgetUpdate();
                }
            }

            void UserConfigWidget::_widgetUpdate()
            {
                if (auto context = getContext().lock())
                {
                    auto contextWeak = std::weak_ptr<Context>(context);
                    _buttonGroup->clearButtons();
                    _deleteButtonGroup->clearButtons();
                    _buttonLayout->clearChildren();
                    std::vector<std::shared_ptr<UI::Button::IButton> > buttons;
                    for (size_t i = 0; i < _userConfigs.first.size(); ++i)
                    {
                        const auto& config = _userConfigs.first[i];
                        auto button = UI::ListButton::create(context);
                        button->setText(config.name);
                        button->setTooltip(config.fileName);
                        buttons.push_back(button);

                        auto deleteButton = UI::ToolButton::create(context);
                        deleteButton->setIcon("djvIconClearSmall");
                        deleteButton->setInsideMargin(UI::MetricsRole::None);
                        deleteButton->setVisible(_deleteEnabled);
                        deleteButton->setTooltip(_getText(DJV_TEXT("widget_color_space_delete_config_tooltip")));
                        _deleteButtonGroup->addButton(deleteButton);

                        auto hLayout = UI::HorizontalLayout::create(context);
                        hLayout->setSpacing(UI::MetricsRole::None);
                        hLayout->addChild(button);
                        hLayout->setStretch(button, UI::RowStretch::Expand);
                        hLayout->addChild(deleteButton);
                        _buttonLayout->addChild(hLayout);
                    }
                    _buttonGroup->setButtons(buttons, _userConfigs.second);
                    if (_userConfigs.first.empty())
                    {
                        _deleteEnabled = false;
                        _deleteButton->setChecked(false);
                    }
                    _deleteButton->setEnabled(!_userConfigs.first.empty());
                }
            }

        } // namespace

        struct ColorSpaceConfigWidget::Private
        {
            AV::OCIO::ConfigMode configMode = AV::OCIO::ConfigMode::First;
            AV::OCIO::UserConfigs userConfigs;

            std::shared_ptr<UI::PopupButton> modePopupButton;
            std::shared_ptr<UI::PopupButton> userConfigPopupButton;
            std::shared_ptr<UI::FormLayout> layout;

            std::shared_ptr<ValueObserver<AV::OCIO::ConfigMode> > configModeObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::UserConfigs> > userConfigsObserver;
        };

        void ColorSpaceConfigWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ColorSpaceConfigWidget");

            p.modePopupButton = UI::PopupButton::create(UI::MenuButtonStyle::ComboBox, context);
            p.modePopupButton->setPopupIcon("djvIconPopupMenu");

            p.userConfigPopupButton = UI::PopupButton::create(UI::MenuButtonStyle::ComboBox, context);
            p.userConfigPopupButton->setPopupIcon("djvIconPopupMenu");

            p.layout = UI::FormLayout::create(context);
            p.layout->setMargin(UI::MetricsRole::MarginSmall);
            p.layout->setSpacing(UI::MetricsRole::SpacingSmall);
            p.layout->addChild(p.modePopupButton);
            p.layout->addChild(p.userConfigPopupButton);
            addChild(p.layout);

            auto contextWeak = std::weak_ptr<Context>(context);
            p.modePopupButton->setOpenCallback(
                [contextWeak]() -> std::shared_ptr<UI::Widget>
                {
                    std::shared_ptr<UI::Widget> out;
                    if (auto context = contextWeak.lock())
                    {
                        out = ModeWidget::create(context);
                    }
                    return out;
                });

            p.userConfigPopupButton->setOpenCallback(
                [contextWeak]() -> std::shared_ptr<UI::Widget>
                {
                    std::shared_ptr<UI::Widget> out;
                    if (auto context = contextWeak.lock())
                    {
                        out = UserConfigWidget::create(context);
                    }
                    return out;
                });

            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
            auto weak = std::weak_ptr<ColorSpaceConfigWidget>(std::dynamic_pointer_cast<ColorSpaceConfigWidget>(shared_from_this()));
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
        }

        ColorSpaceConfigWidget::ColorSpaceConfigWidget() :
            _p(new Private)
        {}

        ColorSpaceConfigWidget::~ColorSpaceConfigWidget()
        {}

        std::shared_ptr<ColorSpaceConfigWidget> ColorSpaceConfigWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceConfigWidget>(new ColorSpaceConfigWidget);
            out->_init(context);
            return out;
        }

        void ColorSpaceConfigWidget::setSizeGroup(const std::shared_ptr<UI::LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void ColorSpaceConfigWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ColorSpaceConfigWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ColorSpaceConfigWidget::_initEvent(Event::Init & event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.modePopupButton, _getText(DJV_TEXT("widget_color_space_config_mode")) + ":");
                p.layout->setText(p.userConfigPopupButton, _getText(DJV_TEXT("widget_color_space_config_user")) + ":");
                _widgetUpdate();
            }
        }

        void ColorSpaceConfigWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                std::stringstream ss;
                ss << p.configMode;
                p.modePopupButton->setText(_getText(ss.str()));
                p.userConfigPopupButton->setText(
                    p.userConfigs.second >= 0 &&
                    p.userConfigs.second < static_cast<int>(p.userConfigs.first.size()) ?
                    p.userConfigs.first[p.userConfigs.second].name :
                    _getText(DJV_TEXT("av_ocio_config_none")));
            }
        }

    } // namespace ViewApp
} // namespace djv

