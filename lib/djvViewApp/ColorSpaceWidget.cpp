// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ColorSpaceWidget.h>

#include <djvViewApp/ColorSpaceWidgetPrivate.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/ButtonGroup.h>
#include <djvUI/ListButton.h>
#include <djvUI/Menu.h>
#include <djvUI/PopupMenu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SoloLayout.h>
#include <djvUI/ToolButton.h>

#include <djvAV/IO.h>
#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorSpaceWidget::Private
        {
            AV::OCIO::ConfigData configData;
            AV::OCIO::DisplayData displayData;
            AV::OCIO::ViewData viewData;
            std::map<std::string, std::string> imageColorSpaces;
            bool imageDeleteEnabled = false;

            std::map<std::string, std::shared_ptr<UI::ListButton> > buttons;
            std::shared_ptr<ColorSpaceConfigsWidget> configsWidget;
            std::shared_ptr<ColorSpaceDisplaysWidget> displaysWidget;
            std::shared_ptr<ColorSpaceViewsWidget> viewsWidget;
            std::shared_ptr<UI::ButtonGroup> imageButtonGroup;
            std::shared_ptr<UI::ButtonGroup> imageDeleteButtonGroup;
            std::shared_ptr<UI::ActionGroup> imageAddActionGroup;
            std::shared_ptr<UI::Menu> imageAddMenu;
            std::shared_ptr<UI::PopupMenu> imageAddButton;
            std::shared_ptr<UI::ToolButton> imageDeleteButton;
            std::shared_ptr<UI::VerticalLayout> imageButtonLayout;
            std::shared_ptr<UI::VerticalLayout> addButtonLayout;
            std::shared_ptr<ImageColorSpacesWidget> imageColorSpacesWidget;

            std::shared_ptr<UI::VerticalLayout> mainLayout;
            std::shared_ptr<UI::SoloLayout> layout;

            std::shared_ptr<ValueObserver<AV::OCIO::ConfigData> > configDataObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::DisplayData> > displayDataObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::ViewData> > viewDataObserver;
            std::shared_ptr<MapObserver<std::string, std::string> > imageColorSpacesObserver;
        };

        void ColorSpaceWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ColorSpaceWidget");

            p.buttons["Config"] = UI::ListButton::create(context);
            p.buttons["Display"] = UI::ListButton::create(context);
            p.buttons["View"] = UI::ListButton::create(context);
            for (const auto& i : p.buttons)
            {
                i.second->setRightIcon("djvIconArrowSmallRight");
            }

            p.configsWidget = ColorSpaceConfigsWidget::create(context);
            p.displaysWidget = ColorSpaceDisplaysWidget::create(context);
            p.viewsWidget = ColorSpaceViewsWidget::create(context);

            p.imageButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);
            p.imageDeleteButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);

            p.imageAddActionGroup = UI::ActionGroup::create(UI::ButtonType::Push);
            p.imageAddMenu = UI::Menu::create(context);
            p.imageAddMenu->setIcon("djvIconAdd");
            p.imageAddMenu->setMinimumSizeRole(UI::MetricsRole::None);
            p.imageAddButton = UI::PopupMenu::create(context);
            p.imageAddButton->setMenu(p.imageAddMenu);

            p.imageDeleteButton = UI::ToolButton::create(context);
            p.imageDeleteButton->setButtonType(UI::ButtonType::Toggle);
            p.imageDeleteButton->setIcon("djvIconClear");

            p.imageColorSpacesWidget = ImageColorSpacesWidget::create(context);

            p.mainLayout = UI::VerticalLayout::create(context);
            p.mainLayout->setSpacing(UI::MetricsRole::None);
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addChild(p.buttons["Config"]);
            vLayout->addChild(p.buttons["Display"]);
            vLayout->addChild(p.buttons["View"]);
            p.imageButtonLayout = UI::VerticalLayout::create(context);
            p.imageButtonLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addChild(p.imageButtonLayout);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
            scrollWidget->setBorder(false);
            scrollWidget->setMinimumSizeRole(UI::MetricsRole::Swatch);
            scrollWidget->addChild(vLayout);
            p.mainLayout->addChild(scrollWidget);
            p.mainLayout->setStretch(scrollWidget, UI::RowStretch::Expand);
            p.mainLayout->addSeparator();
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addExpander();
            hLayout->addChild(p.imageAddButton);
            hLayout->addChild(p.imageDeleteButton);
            p.mainLayout->addChild(hLayout);

            p.layout = UI::SoloLayout::create(context);
            p.layout->setBackgroundRole(UI::ColorRole::Background);
            p.layout->addChild(p.mainLayout);
            p.layout->addChild(p.configsWidget);
            p.layout->addChild(p.displaysWidget);
            p.layout->addChild(p.viewsWidget);
            p.layout->addChild(p.imageColorSpacesWidget);
            addChild(p.layout);

            auto weak = std::weak_ptr<ColorSpaceWidget>(std::dynamic_pointer_cast<ColorSpaceWidget>(shared_from_this()));
            p.buttons["Config"]->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->layout->setCurrentWidget(widget->_p->configsWidget, UI::Side::Left);
                    }
                });
            p.buttons["Display"]->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->layout->setCurrentWidget(widget->_p->displaysWidget, UI::Side::Left);
                    }
                });
            p.buttons["View"]->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->layout->setCurrentWidget(widget->_p->viewsWidget, UI::Side::Left);
                    }
                });
            p.imageButtonGroup->setPushCallback(
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        int index = 0;
                        for (const auto& i : widget->_p->imageColorSpaces)
                        {
                            if (value == index)
                            {
                                widget->_p->imageColorSpacesWidget->setImage(i.first);
                                widget->_p->layout->setCurrentWidget(widget->_p->imageColorSpacesWidget, UI::Side::Left);
                                break;
                            }
                            ++index;
                        }
                    }
                });

            p.configsWidget->setBackCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->layout->setCurrentWidget(widget->_p->mainLayout, UI::Side::Right);
                    }
                });
            p.displaysWidget->setBackCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->layout->setCurrentWidget(widget->_p->mainLayout, UI::Side::Right);
                    }
                });
            p.viewsWidget->setBackCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->layout->setCurrentWidget(widget->_p->mainLayout, UI::Side::Right);
                    }
                });
            p.imageColorSpacesWidget->setBackCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->layout->setCurrentWidget(widget->_p->mainLayout, UI::Side::Right);
                    }
                });

            auto contextWeak = std::weak_ptr<Context>(context);
            p.imageDeleteButtonGroup->setPushCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto imageColorSpaces = widget->_p->imageColorSpaces;
                            int index = 0;
                            for (auto i = imageColorSpaces.begin(); i != imageColorSpaces.end(); ++i, ++index)
                            {
                                if (index == value)
                                {
                                    imageColorSpaces.erase(i);
                                    break;
                                }
                            }
                            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                            ocioSystem->setImageColorSpaces(imageColorSpaces);
                        }
                    }
                });

            p.imageDeleteButton->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageDeleteEnabled = value;
                        widget->_widgetUpdate();
                    }
                });

            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
            p.configDataObserver = ValueObserver<AV::OCIO::ConfigData>::create(
                ocioSystem->observeConfigData(),
                [weak](const AV::OCIO::ConfigData& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->configData = value;
                        widget->_widgetUpdate();
                    }
                });
            p.displayDataObserver = ValueObserver<AV::OCIO::DisplayData>::create(
                ocioSystem->observeDisplayData(),
                [weak](const AV::OCIO::DisplayData& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->displayData = value;
                        widget->_widgetUpdate();
                    }
                });
            p.viewDataObserver = ValueObserver<AV::OCIO::ViewData>::create(
                ocioSystem->observeViewData(),
                [weak](const AV::OCIO::ViewData& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->viewData = value;
                        widget->_widgetUpdate();
                    }
                });
            p.imageColorSpacesObserver = MapObserver<std::string, std::string>::create(
                ocioSystem->observeImageColorSpaces(),
                [weak](const std::map<std::string, std::string>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageColorSpaces = value;
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

        void ColorSpaceWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                setTitle(_getText(DJV_TEXT("widget_color_space")));
                p.buttons["Config"]->setText(_getText(DJV_TEXT("widget_color_space_config")) + ":");
                p.buttons["Display"]->setText(_getText(DJV_TEXT("widget_color_space_display")) + ":");
                p.buttons["View"]->setText(_getText(DJV_TEXT("widget_color_space_view")) + ":");
                p.imageAddButton->setTooltip(_getText(DJV_TEXT("widget_color_space_add_image_tooltip")));
                p.imageDeleteButton->setTooltip(_getText(DJV_TEXT("widget_color_space_delete_images_tooltip")));
                _widgetUpdate();
            }
        }

        void ColorSpaceWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.buttons["Config"]->setRightText(
                    p.configData.current >= 0 && p.configData.current < static_cast<int>(p.configData.names.size()) ?
                    p.configData.names[p.configData.current] :
                    _getText(DJV_TEXT("av_ocio_config_none")));

                p.buttons["Display"]->setRightText(
                    p.displayData.current >= 0 && p.displayData.current < static_cast<int>(p.displayData.names.size()) ?
                    p.displayData.names[p.displayData.current] :
                    _getText(DJV_TEXT("av_ocio_display_none")));
                p.buttons["Display"]->setBackgroundRole(UI::ColorRole::Trough);

                p.buttons["View"]->setRightText(
                    p.viewData.current >= 0 && p.viewData.current < static_cast<int>(p.viewData.names.size()) ?
                    p.viewData.names[p.viewData.current] :
                    _getText(DJV_TEXT("av_ocio_view_none")));

                p.imageButtonGroup->clearButtons();
                p.imageDeleteButtonGroup->clearButtons();
                p.imageAddActionGroup->clearActions();
                p.imageAddMenu->clearActions();
                p.imageButtonLayout->clearChildren();

                size_t j = 0;
                for (const auto& i : p.imageColorSpaces)
                {
                    auto button = UI::ListButton::create(context);
                    button->setText((!i.first.empty() ? i.first : _getText(DJV_TEXT("av_ocio_images_default"))) + ":");
                    button->setRightText(!i.second.empty() ? i.second : _getText(DJV_TEXT("av_ocio_images_none")));
                    button->setRightIcon("djvIconArrowSmallRight");
                    p.imageButtonGroup->addButton(button);

                    auto imageDeleteButton = UI::ToolButton::create(context);
                    imageDeleteButton->setIcon("djvIconClearSmall");
                    imageDeleteButton->setInsideMargin(UI::MetricsRole::None);
                    imageDeleteButton->setVisible(p.imageDeleteEnabled);
                    imageDeleteButton->setTooltip(_getText(DJV_TEXT("widget_color_space_delete_image_tooltip")));
                    p.imageDeleteButtonGroup->addButton(imageDeleteButton);

                    auto hLayout = UI::HorizontalLayout::create(context);
                    hLayout->setSpacing(UI::MetricsRole::None);
                    hLayout->setBackgroundRole(0 == j % 2 ? UI::ColorRole::Trough : UI::ColorRole::None);
                    hLayout->addChild(button);
                    hLayout->setStretch(button, UI::RowStretch::Expand);
                    hLayout->addChild(imageDeleteButton);
                    p.imageButtonLayout->addChild(hLayout);

                    ++j;
                }

                auto io = context->getSystemT<AV::IO::System>();
                auto pluginNames = io->getPluginNames();
                pluginNames.insert(pluginNames.begin(), std::string());
                std::vector<std::string> pluginNamesList;
                for (const auto& j : pluginNames)
                {
                    const auto k = p.imageColorSpaces.find(j);
                    if (k == p.imageColorSpaces.end())
                    {
                        pluginNamesList.push_back(j);
                        auto action = UI::Action::create();
                        action->setText(!j.empty() ? j : _getText(DJV_TEXT("av_ocio_images_default")));
                        p.imageAddActionGroup->addAction(action);
                        p.imageAddMenu->addAction(action);
                    }
                }
                auto weak = std::weak_ptr<ColorSpaceWidget>(std::dynamic_pointer_cast<ColorSpaceWidget>(shared_from_this()));
                auto contextWeak = std::weak_ptr<Context>(context);
                p.imageAddActionGroup->setPushCallback(
                    [pluginNamesList, weak, contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                if (value >= 0 && value < static_cast<int>(pluginNamesList.size()))
                                {
                                    auto imageColorSpaces = widget->_p->imageColorSpaces;
                                    imageColorSpaces[pluginNamesList[value]] = std::string();
                                    auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                                    ocioSystem->setImageColorSpaces(imageColorSpaces);
                                }
                            }
                        }
                    });
                p.imageAddButton->setEnabled(!pluginNamesList.empty());
                p.imageDeleteButton->setEnabled(p.imageButtonGroup->getButtonCount() != 0);
            }
        }

    } // namespace ViewApp
} // namespace djv

