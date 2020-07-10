// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ColorSpaceWidgetPrivate.h>

#include <djvUIComponents/FileBrowserDialog.h>
#include <djvUIComponents/SearchBox.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/ButtonGroup.h>
#include <djvUI/CheckBox.h>
#include <djvUI/EventSystem.h>
#include <djvUI/FormLayout.h>
#include <djvUI/Label.h>
#include <djvUI/ListButton.h>
#include <djvUI/ListWidget.h>
#include <djvUI/Menu.h>
#include <djvUI/PopupMenu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Spacer.h>
#include <djvUI/ToolButton.h>

#include <djvAV/IO.h>
#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorSpaceConfigsWidget::Private
        {
            AV::OCIO::ConfigType configType = AV::OCIO::ConfigType::First;
            AV::OCIO::UserConfigs userConfigs;
            AV::OCIO::Config envConfig;
            AV::OCIO::Config cmdLineConfig;
            AV::OCIO::Config currentConfig;
            Core::FileSystem::Path fileBrowserPath = Core::FileSystem::Path(".");
            bool deleteEnabled = false;

            std::shared_ptr<UI::ListButton> backButton;
            std::map<std::string, std::shared_ptr<UI::CheckBox> > configCheckBoxes;
            std::shared_ptr<UI::ButtonGroup> configTypeButtonGroup;
            std::shared_ptr<UI::ButtonGroup> userConfigButtonGroup;
            std::shared_ptr<UI::ButtonGroup> deleteUserConfigButtonGroup;
            std::shared_ptr<UI::ToolButton> addUserConfigButton;
            std::shared_ptr<UI::ToolButton> deleteUserConfigsButton;
            std::shared_ptr<UI::VerticalLayout> userConfigButtonLayout;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::shared_ptr<UI::FileBrowser::Dialog> fileBrowserDialog;

            std::function<void()> backCallback;

            std::shared_ptr<ValueObserver<AV::OCIO::ConfigType> > configTypeObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::UserConfigs> > userConfigsObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::Config> > envConfigObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::Config> > cmdLineConfigObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::Config> > currentConfigObserver;
        };

        void ColorSpaceConfigsWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ColorSpaceConfigsWidget");

            p.backButton = UI::ListButton::create(context);
            p.backButton->setIcon("djvIconArrowSmallLeft");

            p.configCheckBoxes["User"] = UI::CheckBox::create(context);
            p.configCheckBoxes["Env"] = UI::CheckBox::create(context);
            p.configCheckBoxes["CmdLine"] = UI::CheckBox::create(context);
            p.configTypeButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Exclusive);
            p.configTypeButtonGroup->addButton(p.configCheckBoxes["User"]);
            p.configTypeButtonGroup->addButton(p.configCheckBoxes["Env"]);
            p.configTypeButtonGroup->addButton(p.configCheckBoxes["CmdLine"]);

            p.userConfigButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Exclusive);
            p.deleteUserConfigButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);

            p.addUserConfigButton = UI::ToolButton::create(context);
            p.addUserConfigButton->setIcon("djvIconAdd");

            p.deleteUserConfigsButton = UI::ToolButton::create(context);
            p.deleteUserConfigsButton->setButtonType(UI::ButtonType::Toggle);
            p.deleteUserConfigsButton->setIcon("djvIconClear");

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->setBackgroundRole(UI::ColorRole::Background);
            p.layout->addChild(p.backButton);
            p.layout->addSeparator();
            p.layout->addChild(p.configCheckBoxes["CmdLine"]);
            p.layout->addSeparator();
            p.layout->addChild(p.configCheckBoxes["Env"]);
            p.layout->addSeparator();
            p.layout->addChild(p.configCheckBoxes["User"]);
            p.userConfigButtonLayout = UI::VerticalLayout::create(context);
            p.userConfigButtonLayout->setSpacing(UI::MetricsRole::None);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
            scrollWidget->setBorder(false);
            scrollWidget->setMinimumSizeRole(UI::MetricsRole::Swatch);
            scrollWidget->addChild(p.userConfigButtonLayout);
            p.layout->addChild(scrollWidget);
            p.layout->setStretch(scrollWidget, UI::RowStretch::Expand);
            p.layout->addSeparator();
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addExpander();
            hLayout->addChild(p.addUserConfigButton);
            hLayout->addChild(p.deleteUserConfigsButton);
            p.layout->addChild(hLayout);
            addChild(p.layout);

            auto weak = std::weak_ptr<ColorSpaceConfigsWidget>(std::dynamic_pointer_cast<ColorSpaceConfigsWidget>(shared_from_this()));
            p.backButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->backCallback)
                        {
                            widget->_p->backCallback();
                        }
                    }
                });

            auto contextWeak = std::weak_ptr<Context>(context);
            p.configTypeButtonGroup->setExclusiveCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                        ocioSystem->setConfigType(static_cast<AV::OCIO::ConfigType>(value + 1));
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

            p.deleteUserConfigButtonGroup->setPushCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                        ocioSystem->removeUserConfig(static_cast<int>(value));
                    }
                });

            p.addUserConfigButton->setClickedCallback(
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
                                        widget->_p->fileBrowserPath = widget->_p->fileBrowserDialog->getPath();
                                        widget->_p->fileBrowserDialog->close();
                                        widget->_p->fileBrowserDialog.reset();
                                    }
                                });
                            widget->_p->fileBrowserDialog->show();
                        }
                    }
                });

            p.deleteUserConfigsButton->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->deleteEnabled = value;
                        widget->_widgetUpdate();
                    }
                });

            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
            p.configTypeObserver = ValueObserver<AV::OCIO::ConfigType>::create(
                ocioSystem->observeConfigType(),
                [weak](const AV::OCIO::ConfigType& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->configType = value;
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

        ColorSpaceConfigsWidget::ColorSpaceConfigsWidget() :
            _p(new Private)
        {}

        ColorSpaceConfigsWidget::~ColorSpaceConfigsWidget()
        {
            DJV_PRIVATE_PTR();
            if (p.fileBrowserDialog)
            {
                p.fileBrowserDialog->close();
            }
        }

        std::shared_ptr<ColorSpaceConfigsWidget> ColorSpaceConfigsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceConfigsWidget>(new ColorSpaceConfigsWidget);
            out->_init(context);
            return out;
        }

        void ColorSpaceConfigsWidget::setBackCallback(const std::function<void()>& value)
        {
            _p->backCallback = value;
        }

        void ColorSpaceConfigsWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ColorSpaceConfigsWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ColorSpaceConfigsWidget::_initEvent(Event::Init & event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.backButton->setText(_getText(DJV_TEXT("widget_color_space_config")));
                p.configCheckBoxes["User"]->setText(_getText(DJV_TEXT("widget_color_space_config_user")));
                p.configCheckBoxes["Env"]->setText(_getText(DJV_TEXT("widget_color_space_config_env")));
                p.configCheckBoxes["CmdLine"]->setText(_getText(DJV_TEXT("widget_color_space_config_command_line")));
                p.addUserConfigButton->setTooltip(_getText(DJV_TEXT("widget_color_space_add_config_tooltip")));
                p.deleteUserConfigsButton->setTooltip(_getText(DJV_TEXT("widget_color_space_delete_configs_tooltip")));
                _widgetUpdate();
            }
        }

        void ColorSpaceConfigsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.configCheckBoxes["Env"]->setEnabled(p.envConfig.isValid());
                p.configCheckBoxes["Env"]->setTooltip(p.envConfig.fileName);
                p.configCheckBoxes["CmdLine"]->setEnabled(p.cmdLineConfig.isValid());
                p.configCheckBoxes["CmdLine"]->setTooltip(p.cmdLineConfig.fileName);
                p.configTypeButtonGroup->setChecked(static_cast<int>(p.configType) - 1);

                auto contextWeak = std::weak_ptr<Context>(context);
                p.userConfigButtonGroup->clearButtons();
                p.deleteUserConfigButtonGroup->clearButtons();
                p.userConfigButtonLayout->clearChildren();
                std::vector<std::shared_ptr<UI::Button::IButton> > buttons;
                for (size_t i = 0; i < p.userConfigs.first.size(); ++i)
                {
                    const auto& config = p.userConfigs.first[i];
                    auto button = UI::ListButton::create(context);
                    button->setText(config.name);
                    button->setTooltip(config.fileName);
                    buttons.push_back(button);
                    
                    auto deleteButton = UI::ToolButton::create(context);
                    deleteButton->setIcon("djvIconClearSmall");
                    deleteButton->setInsideMargin(UI::MetricsRole::None);
                    deleteButton->setVisible(p.deleteEnabled);
                    deleteButton->setTooltip(_getText(DJV_TEXT("widget_color_space_delete_config_tooltip")));
                    p.deleteUserConfigButtonGroup->addButton(deleteButton);

                    auto hLayout = UI::HorizontalLayout::create(context);
                    hLayout->setSpacing(UI::MetricsRole::None);
                    hLayout->addChild(button);
                    hLayout->setStretch(button, UI::RowStretch::Expand);
                    hLayout->addChild(deleteButton);
                    p.userConfigButtonLayout->addChild(hLayout);
                }
                p.userConfigButtonGroup->setButtons(buttons, p.userConfigs.second);
            }
        }

        struct ColorSpaceDisplaysWidget::Private
        {
            AV::OCIO::Displays displays;

            std::shared_ptr<UI::ListButton> backButton;
            std::shared_ptr<UI::ListWidget> listWidget;
            std::shared_ptr<UI::SearchBox> searchBox;

            std::shared_ptr<UI::VerticalLayout> layout;

            std::function<void()> backCallback;

            std::shared_ptr<ValueObserver<AV::OCIO::Displays> > displaysObserver;
        };

        void ColorSpaceDisplaysWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ColorSpaceDisplaysWidget");

            p.backButton = UI::ListButton::create(context);
            p.backButton->setIcon("djvIconArrowSmallLeft");

            p.listWidget = UI::ListWidget::create(UI::ButtonType::Radio, context);
            p.listWidget->setAlternateRowsRoles(UI::ColorRole::None, UI::ColorRole::Trough);

            p.searchBox = UI::SearchBox::create(context);

            p.layout = UI::VerticalLayout::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->setBackgroundRole(UI::ColorRole::Background);
            p.layout->addChild(p.backButton);
            p.layout->addSeparator();
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
            scrollWidget->setBorder(false);
            scrollWidget->setMinimumSizeRole(UI::MetricsRole::Swatch);
            scrollWidget->addChild(p.listWidget);
            p.layout->addChild(scrollWidget);
            p.layout->setStretch(scrollWidget, UI::RowStretch::Expand);
            p.layout->addSeparator();
            p.layout->addChild(p.searchBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<ColorSpaceDisplaysWidget>(std::dynamic_pointer_cast<ColorSpaceDisplaysWidget>(shared_from_this()));
            p.backButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->backCallback)
                        {
                            widget->_p->backCallback();
                        }
                    }
                });

            auto contextWeak = std::weak_ptr<Context>(context);
            p.listWidget->setRadioCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                        ocioSystem->setCurrentDisplay(value);
                    }
                });

            p.searchBox->setFilterCallback(
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->listWidget->setFilter(value);
                    }
                });

            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
            p.displaysObserver = ValueObserver<AV::OCIO::Displays>::create(
                ocioSystem->observeDisplays(),
                [weak](const AV::OCIO::Displays& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->displays = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        ColorSpaceDisplaysWidget::ColorSpaceDisplaysWidget() :
            _p(new Private)
        {}

        ColorSpaceDisplaysWidget::~ColorSpaceDisplaysWidget()
        {}

        std::shared_ptr<ColorSpaceDisplaysWidget> ColorSpaceDisplaysWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceDisplaysWidget>(new ColorSpaceDisplaysWidget);
            out->_init(context);
            return out;
        }

        void ColorSpaceDisplaysWidget::setBackCallback(const std::function<void()>& value)
        {
            _p->backCallback = value;
        }

        void ColorSpaceDisplaysWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ColorSpaceDisplaysWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ColorSpaceDisplaysWidget::_initEvent(Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.backButton->setText(_getText(DJV_TEXT("widget_color_space_display")));
                _widgetUpdate();
            }
        }

        void ColorSpaceDisplaysWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                std::vector<UI::ListItem> items;
                for (size_t i = 0; i < p.displays.first.size(); ++i)
                {
                    const auto& display = p.displays.first[i];
                    UI::ListItem item;
                    item.text = !display.empty() ? display : _getText(DJV_TEXT("av_ocio_display_none"));
                    items.emplace_back(item);
                }
                p.listWidget->setItems(items, p.displays.second);
            }
        }

        struct ColorSpaceViewsWidget::Private
        {
            AV::OCIO::Views views;

            std::shared_ptr<UI::ListButton> backButton;
            std::shared_ptr<UI::ListWidget> listWidget;
            std::shared_ptr<UI::SearchBox> searchBox;

            std::shared_ptr<UI::VerticalLayout> layout;

            std::function<void()> backCallback;

            std::shared_ptr<ValueObserver<AV::OCIO::Views> > viewsObserver;
        };

        void ColorSpaceViewsWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ColorSpaceViewsWidget");

            p.backButton = UI::ListButton::create(context);
            p.backButton->setIcon("djvIconArrowSmallLeft");

            p.listWidget = UI::ListWidget::create(UI::ButtonType::Radio, context);
            p.listWidget->setAlternateRowsRoles(UI::ColorRole::None, UI::ColorRole::Trough);

            p.searchBox = UI::SearchBox::create(context);

            p.layout = UI::VerticalLayout::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->setBackgroundRole(UI::ColorRole::Background);
            p.layout->addChild(p.backButton);
            p.layout->addSeparator();
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
            scrollWidget->setBorder(false);
            scrollWidget->setMinimumSizeRole(UI::MetricsRole::Swatch);
            scrollWidget->addChild(p.listWidget);
            p.layout->addChild(scrollWidget);
            p.layout->setStretch(scrollWidget, UI::RowStretch::Expand);
            p.layout->addSeparator();
            p.layout->addChild(p.searchBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<ColorSpaceViewsWidget>(std::dynamic_pointer_cast<ColorSpaceViewsWidget>(shared_from_this()));
            p.backButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->backCallback)
                        {
                            widget->_p->backCallback();
                        }
                    }
                });

            auto contextWeak = std::weak_ptr<Context>(context);
            p.listWidget->setRadioCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                        ocioSystem->setCurrentView(value);
                    }
                });

            p.searchBox->setFilterCallback(
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->listWidget->setFilter(value);
                    }
                });

            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
            p.viewsObserver = ValueObserver<AV::OCIO::Views>::create(
                ocioSystem->observeViews(),
                [weak](const AV::OCIO::Views& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->views = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        ColorSpaceViewsWidget::ColorSpaceViewsWidget() :
            _p(new Private)
        {}

        ColorSpaceViewsWidget::~ColorSpaceViewsWidget()
        {}

        std::shared_ptr<ColorSpaceViewsWidget> ColorSpaceViewsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceViewsWidget>(new ColorSpaceViewsWidget);
            out->_init(context);
            return out;
        }

        void ColorSpaceViewsWidget::setBackCallback(const std::function<void()>& value)
        {
            _p->backCallback = value;
        }

        void ColorSpaceViewsWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ColorSpaceViewsWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ColorSpaceViewsWidget::_initEvent(Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.backButton->setText(_getText(DJV_TEXT("widget_color_space_view")));
                _widgetUpdate();
            }
        }

        void ColorSpaceViewsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                std::vector<UI::ListItem> items;
                for (size_t i = 0; i < p.views.first.size(); ++i)
                {
                    const auto& view = p.views.first[i];
                    UI::ListItem item;
                    item.text = !view.empty() ? view : _getText(DJV_TEXT("av_ocio_view_none"));
                    items.emplace_back(item);
                }
                p.listWidget->setItems(items, p.views.second);
            }
        }

        struct ImageColorSpacesWidget::Private
        {
            std::string image;
            std::vector<std::string> colorSpaces;
            AV::OCIO::ImageColorSpaces imageColorSpaces;

            std::shared_ptr<UI::ListButton> backButton;
            std::shared_ptr<UI::ListWidget> listWidget;
            std::shared_ptr<UI::SearchBox> searchBox;

            std::shared_ptr<UI::VerticalLayout> layout;

            std::function<void()> backCallback;

            std::shared_ptr<ListObserver<std::string> > colorSpacesObserver;
            std::shared_ptr<MapObserver<std::string, std::string> > imageColorSpacesObserver;
        };

        void ImageColorSpacesWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ImageColorSpacesWidget");

            p.backButton = UI::ListButton::create(context);
            p.backButton->setIcon("djvIconArrowSmallLeft");

            p.listWidget = UI::ListWidget::create(UI::ButtonType::Radio, context);
            p.listWidget->setAlternateRowsRoles(UI::ColorRole::None, UI::ColorRole::Trough);

            p.searchBox = UI::SearchBox::create(context);

            p.layout = UI::VerticalLayout::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->setBackgroundRole(UI::ColorRole::Background);
            p.layout->addChild(p.backButton);
            p.layout->addSeparator();
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
            scrollWidget->setBorder(false);
            scrollWidget->setMinimumSizeRole(UI::MetricsRole::Swatch);
            scrollWidget->addChild(p.listWidget);
            p.layout->addChild(scrollWidget);
            p.layout->setStretch(scrollWidget, UI::RowStretch::Expand);
            p.layout->addSeparator();
            p.layout->addChild(p.searchBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<ImageColorSpacesWidget>(std::dynamic_pointer_cast<ImageColorSpacesWidget>(shared_from_this()));
            p.backButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->backCallback)
                        {
                            widget->_p->backCallback();
                        }
                    }
                });

            auto contextWeak = std::weak_ptr<Context>(context);
            p.listWidget->setRadioCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto imageColorSpaces = widget->_p->imageColorSpaces;
                            imageColorSpaces[widget->_p->image] =
                                value >= 0 && value < static_cast<int>(widget->_p->colorSpaces.size()) ?
                                widget->_p->colorSpaces[value] :
                                std::string();
                            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                            ocioSystem->setImageColorSpaces(imageColorSpaces);
                        }
                    }
                });

            p.searchBox->setFilterCallback(
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->listWidget->setFilter(value);
                    }
                });

            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
            p.colorSpacesObserver = ListObserver<std::string>::create(
                ocioSystem->observeColorSpaces(),
                [weak](const std::vector<std::string>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->colorSpaces = value;
                        widget->_widgetUpdate();
                    }
                });

            p.imageColorSpacesObserver = MapObserver<std::string, std::string>::create(
                ocioSystem->observeImageColorSpaces(),
                [weak](const AV::OCIO::ImageColorSpaces& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageColorSpaces = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        ImageColorSpacesWidget::ImageColorSpacesWidget() :
            _p(new Private)
        {}

        ImageColorSpacesWidget::~ImageColorSpacesWidget()
        {}

        std::shared_ptr<ImageColorSpacesWidget> ImageColorSpacesWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ImageColorSpacesWidget>(new ImageColorSpacesWidget);
            out->_init(context);
            return out;
        }

        void ImageColorSpacesWidget::setImage(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.image)
                return;
            p.image = value;
            _widgetUpdate();
        }

        void ImageColorSpacesWidget::setBackCallback(const std::function<void()>& value)
        {
            _p->backCallback = value;
        }

        void ImageColorSpacesWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ImageColorSpacesWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ImageColorSpacesWidget::_initEvent(Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.backButton->setText(_getText(DJV_TEXT("widget_color_space_view")));
                _widgetUpdate();
            }
        }

        void ImageColorSpacesWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.backButton->setText(
                    !p.image.empty() ?
                    p.image :
                    _getText(DJV_TEXT("av_ocio_images_default")));
                std::vector<UI::ListItem> items;
                for (size_t i = 0; i < p.colorSpaces.size(); ++i)
                {
                    UI::ListItem item;
                    item.text = !p.colorSpaces[i].empty() ?
                        p.colorSpaces[i] :
                        _getText(DJV_TEXT("av_ocio_images_none"));
                    items.emplace_back(item);
                }
                int index = -1;
                const auto i = p.imageColorSpaces.find(p.image);
                if (i != p.imageColorSpaces.end())
                {
                    const auto j = std::find(p.colorSpaces.begin(), p.colorSpaces.end(), i->second);
                    if (j != p.colorSpaces.end())
                    {
                        index = static_cast<int>(j - p.colorSpaces.begin());
                    }
                }
                p.listWidget->setItems(items, index);
            }
        }

    } // namespace ViewApp
} // namespace djv

