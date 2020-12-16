// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ColorSpaceWidgetPrivate.h>

#include <djvUIComponents/FileBrowserDialog.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/FormLayout.h>
#include <djvUI/ListButton.h>
#include <djvUI/ListWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Spacer.h>
#include <djvUI/ToolBar.h>
#include <djvUI/ToolButton.h>

#include <djvOCIO/OCIOSystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/FileInfo.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorSpaceConfigWidget::Private
        {
            OCIO::ConfigMode configMode = OCIO::ConfigMode::First;
            OCIO::UserConfigs userConfigs;
            System::File::Path fileBrowserPath = System::File::Path(".");
            bool deleteEnabled = false;

            std::shared_ptr<UI::ButtonGroup> userConfigButtonGroup;
            std::shared_ptr<UI::ButtonGroup> userConfigDeleteButtonGroup;
            std::shared_ptr<UI::ToolButton> userConfigAddButton;
            std::shared_ptr<UI::ToolButton> userConfigDeleteButton;
            std::shared_ptr<UI::VerticalLayout> userConfigButtonLayout;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::shared_ptr<UIComponents::FileBrowser::Dialog> fileBrowserDialog;

            std::shared_ptr<Observer::Value<OCIO::ConfigMode> > configModeObserver;
            std::shared_ptr<Observer::Value<OCIO::UserConfigs> > userConfigsObserver;
        };

        void ColorSpaceConfigWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            p.userConfigButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Exclusive);
            p.userConfigAddButton = UI::ToolButton::create(context);
            p.userConfigAddButton->setIcon("djvIconAddSmall");
            p.userConfigAddButton->setInsideMargin(UI::MetricsRole::None);
            p.userConfigDeleteButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);
            p.userConfigDeleteButton = UI::ToolButton::create(context);
            p.userConfigDeleteButton->setButtonType(UI::ButtonType::Toggle);
            p.userConfigDeleteButton->setIcon("djvIconClearSmall");
            p.userConfigDeleteButton->setInsideMargin(UI::MetricsRole::None);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            auto toolBar = UI::ToolBar::create(context);
            toolBar->addExpander();
            toolBar->addChild(p.userConfigAddButton);
            toolBar->addChild(p.userConfigDeleteButton);
            p.layout->addChild(toolBar);
            p.layout->addSeparator();
            p.userConfigButtonLayout = UI::VerticalLayout::create(context);
            p.userConfigButtonLayout->setSpacing(UI::MetricsRole::None);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
            scrollWidget->setBorder(false);
            scrollWidget->addChild(p.userConfigButtonLayout);
            p.layout->addChild(scrollWidget);
            p.layout->setStretch(scrollWidget);
            addChild(p.layout);

            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.userConfigButtonGroup->setExclusiveCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
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
                            widget->_p->fileBrowserDialog = UIComponents::FileBrowser::Dialog::create(UI::SelectionType::Single, context);
                            widget->_p->fileBrowserDialog->setFileExtensions({ ".ocio" });
                            widget->_p->fileBrowserDialog->setPath(widget->_p->fileBrowserPath);
                            widget->_p->fileBrowserDialog->setCallback(
                                [weak, contextWeak](const std::vector<System::File::Info>& value)
                                {
                                    if (auto context = contextWeak.lock())
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            const auto temp = value;
                                            if (widget->_p->fileBrowserDialog)
                                            {
                                                widget->_p->fileBrowserPath = widget->_p->fileBrowserDialog->getPath();
                                                widget->_p->fileBrowserDialog->close();
                                                widget->_p->fileBrowserDialog.reset();
                                            }
                                            if (!temp.empty())
                                            {
                                                auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
                                                ocioSystem->addUserConfig(temp[0].getPath().get());
                                            }
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
                        auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
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

            auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
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

        std::shared_ptr<ColorSpaceConfigWidget> ColorSpaceConfigWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceConfigWidget>(new ColorSpaceConfigWidget);
            out->_init(context);
            return out;
        }

        void ColorSpaceConfigWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ColorSpaceConfigWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ColorSpaceConfigWidget::_initEvent(System::Event::Init& event)
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
                const bool userConfigMode = OCIO::ConfigMode::User == p.configMode;
                p.userConfigButtonLayout->clearChildren();
                std::vector<std::shared_ptr<UI::Button::IButton> > buttons;
                std::vector<std::shared_ptr<UI::Button::IButton> > deleteButtons;
                auto contextWeak = std::weak_ptr<System::Context>(context);
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
                    deleteButtons.push_back(deleteButton);

                    auto hLayout = UI::HorizontalLayout::create(context);
                    hLayout->setSpacing(UI::MetricsRole::None);
                    hLayout->addChild(button);
                    hLayout->setStretch(button);
                    hLayout->addChild(deleteButton);
                    p.userConfigButtonLayout->addChild(hLayout);
                }
                p.userConfigButtonGroup->setButtons(buttons);
                p.userConfigButtonGroup->setChecked(p.userConfigs.second);
                p.userConfigDeleteButtonGroup->setButtons(deleteButtons);
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

