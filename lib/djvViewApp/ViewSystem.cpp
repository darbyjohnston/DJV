// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ViewSystem.h>

#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/ToolTitleBar.h>
#include <djvViewApp/ViewData.h>
#include <djvViewApp/ViewOptionsWidget.h>
#include <djvViewApp/ViewPanZoomWidget.h>
#include <djvViewApp/ViewSettings.h>
#include <djvViewApp/ViewWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Menu.h>
#include <djvUI/MouseSettings.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ShortcutDataFunc.h>
#include <djvUI/Style.h>
#include <djvUI/UISystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/TextSystem.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ViewSystem::Private
        {
            std::shared_ptr<ViewSettings> settings;

            std::map<std::string, bool> bellowsState;
            GridOptions gridOptions;
            HUDOptions hudOptions;
            ViewBackgroundOptions backgroundOptions;
            glm::vec2 hoverPos = glm::vec2(0.F, 0.F);
            glm::vec2 dragStart = glm::vec2(0.F, 0.F);
            glm::vec2 dragImagePos = glm::vec2(0.F, 0.F);
            std::shared_ptr<MediaWidget> activeWidget;
            std::shared_ptr<Observer::ValueSubject<ViewLock> > lock;
            
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> lockActionGroup;
            std::shared_ptr<UI::Menu> menu;
            std::weak_ptr<ViewOptionsWidget> viewOptionsWidget;
            
            std::shared_ptr<Observer::Value<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<Observer::Value<ViewLock> > lockObserver;
            std::shared_ptr<Observer::Value<GridOptions> > gridOptionsObserver;
            std::shared_ptr<Observer::Value<HUDOptions> > hudOptionsObserver;
            std::shared_ptr<Observer::Value<ViewBackgroundOptions> > backgroundOptionsObserver;
            std::shared_ptr<Observer::Value<PointerData> > hoverObserver;
            std::shared_ptr<Observer::Value<PointerData> > dragObserver;
            std::shared_ptr<Observer::Value<ScrollData> > scrollObserver;

            void drag(const PointerData&);
            void scroll(const ScrollData&, const std::weak_ptr<System::Context>&);
        };

        void ViewSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            IViewAppSystem::_init("djv::ViewApp::ViewSystem", context);

            DJV_PRIVATE_PTR();

            p.settings = ViewSettings::create(context);
            p.bellowsState = p.settings->getBellowsState();

            p.lock = Observer::ValueSubject<ViewLock>::create();

            p.actions["PanZoom"] = UI::Action::create();
            p.actions["PanZoom"]->setIcon("djvIconMove");
            p.actions["ViewOptions"] = UI::Action::create();
            p.actions["ViewOptions"]->setIcon("djvIconVisible");
            p.actions["Left"] = UI::Action::create();
            p.actions["Right"] = UI::Action::create();
            p.actions["Up"] = UI::Action::create();
            p.actions["Down"] = UI::Action::create();
            p.actions["NW"] = UI::Action::create();
            p.actions["NE"] = UI::Action::create();
            p.actions["SE"] = UI::Action::create();
            p.actions["SW"] = UI::Action::create();
            p.actions["ZoomIn"] = UI::Action::create();
            p.actions["ZoomIn"]->setIcon("djvIconZoomIn");
            p.actions["ZoomOut"] = UI::Action::create();
            p.actions["ZoomOut"]->setIcon("djvIconZoomOut");
            p.actions["ZoomReset"] = UI::Action::create();
            p.actions["ZoomReset"]->setIcon("djvIconZoomReset");
            p.actions["Frame"] = UI::Action::create();
            p.actions["Frame"]->setIcon("djvIconViewFrame");
            p.actions["FrameLock"] = UI::Action::create();
            p.actions["FrameLock"]->setIcon("djvIconViewFrame");
            p.actions["Center"] = UI::Action::create();
            p.actions["Center"]->setIcon("djvIconViewCenter");
            p.actions["CenterLock"] = UI::Action::create();
            p.actions["CenterLock"]->setIcon("djvIconViewCenter");
            p.lockActionGroup = UI::ActionGroup::create(UI::ButtonType::Exclusive);
            p.lockActionGroup->setActions({
                p.actions["FrameLock"],
                p.actions["CenterLock"] });
            p.actions["Grid"] = UI::Action::create();
            p.actions["Grid"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Grid"]->setIcon("djvIconHidden");
            p.actions["Grid"]->setCheckedIcon("djvIconVisible");
            p.actions["HUD"] = UI::Action::create();
            p.actions["HUD"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["HUD"]->setIcon("djvIconHidden");
            p.actions["HUD"]->setCheckedIcon("djvIconVisible");
            p.actions["Border"] = UI::Action::create();
            p.actions["Border"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Border"]->setIcon("djvIconHidden");
            p.actions["Border"]->setCheckedIcon("djvIconVisible");

            _addShortcut(DJV_TEXT("shortcut_view_pan_zoom"), GLFW_KEY_N, UI::getSystemModifier());
            _addShortcut(DJV_TEXT("shortcut_view_options"), GLFW_KEY_W, UI::getSystemModifier());
            _addShortcut(DJV_TEXT("shortcut_view_left"), GLFW_KEY_KP_4);
            _addShortcut(DJV_TEXT("shortcut_view_right"), GLFW_KEY_KP_6);
            _addShortcut(DJV_TEXT("shortcut_view_up"), GLFW_KEY_KP_8);
            _addShortcut(DJV_TEXT("shortcut_view_down"), GLFW_KEY_KP_2);
            _addShortcut(DJV_TEXT("shortcut_view_nw"), GLFW_KEY_KP_7);
            _addShortcut(DJV_TEXT("shortcut_view_ne"), GLFW_KEY_KP_9);
            _addShortcut(DJV_TEXT("shortcut_view_se"), GLFW_KEY_KP_3);
            _addShortcut(DJV_TEXT("shortcut_view_sw"), GLFW_KEY_KP_1);
            _addShortcut(DJV_TEXT("shortcut_view_zoom_in"), {
                UI::ShortcutData(GLFW_KEY_EQUAL),
                UI::ShortcutData(GLFW_KEY_KP_ADD) });
            _addShortcut(DJV_TEXT("shortcut_view_zoom_out"), {
                UI::ShortcutData(GLFW_KEY_MINUS),
                UI::ShortcutData(GLFW_KEY_KP_SUBTRACT) });
            _addShortcut(DJV_TEXT("shortcut_view_zoom_reset"), {
                UI::ShortcutData(GLFW_KEY_0),
                UI::ShortcutData(GLFW_KEY_KP_0) });
            _addShortcut(DJV_TEXT("shortcut_view_frame"), {
                UI::ShortcutData(GLFW_KEY_BACKSPACE),
                UI::ShortcutData(GLFW_KEY_KP_DECIMAL) });
            _addShortcut(DJV_TEXT("shortcut_view_frame_lock"), {
                UI::ShortcutData(GLFW_KEY_BACKSPACE, GLFW_MOD_SHIFT),
                UI::ShortcutData(GLFW_KEY_KP_DECIMAL, GLFW_MOD_SHIFT) });
            _addShortcut(DJV_TEXT("shortcut_view_center"), {
                UI::ShortcutData(GLFW_KEY_BACKSLASH),
                UI::ShortcutData(GLFW_KEY_KP_5) });
            _addShortcut(DJV_TEXT("shortcut_view_center_lock"), {
                UI::ShortcutData(GLFW_KEY_BACKSLASH, GLFW_MOD_SHIFT),
                UI::ShortcutData(GLFW_KEY_KP_5, GLFW_MOD_SHIFT) });
            _addShortcut(DJV_TEXT("shortcut_view_grid"), GLFW_KEY_G, UI::getSystemModifier());
            _addShortcut(DJV_TEXT("shortcut_view_hud"), GLFW_KEY_U, UI::getSystemModifier());
            _addShortcut(DJV_TEXT("shortcut_view_border"), 0);

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["Left"]);
            p.menu->addAction(p.actions["Right"]);
            p.menu->addAction(p.actions["Up"]);
            p.menu->addAction(p.actions["Down"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["ZoomIn"]);
            p.menu->addAction(p.actions["ZoomOut"]);
            p.menu->addAction(p.actions["ZoomReset"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["Frame"]);
            p.menu->addAction(p.actions["FrameLock"]);
            p.menu->addAction(p.actions["Center"]);
            p.menu->addAction(p.actions["CenterLock"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["Grid"]);
            p.menu->addAction(p.actions["HUD"]);
            p.menu->addAction(p.actions["Border"]);

            _actionsUpdate();
            _textUpdate();
            _shortcutsUpdate();

            auto weak = std::weak_ptr<ViewSystem>(std::dynamic_pointer_cast<ViewSystem>(shared_from_this()));
            p.lockActionGroup->setExclusiveCallback(
                [weak](int index)
                {
                    if (auto system = weak.lock())
                    {
                        ViewLock lock = ViewLock::None;
                        switch (index)
                        {
                        case 0: lock = ViewLock::Frame;  break;
                        case 1: lock = ViewLock::Center; break;
                        }
                        system->_p->settings->setLock(lock);
                    }
                });

            p.actions["Left"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        system->_panImage(glm::vec2(-1.F, 0.F));
                    }
                });

            p.actions["Right"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->settings->setLock(ViewLock::None);
                        system->_panImage(glm::vec2(1.F, 0.F));
                    }
                });

            p.actions["Up"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->settings->setLock(ViewLock::None);
                        system->_panImage(glm::vec2(0.F, -1.F));
                    }
                });

            p.actions["Down"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->settings->setLock(ViewLock::None);
                        system->_panImage(glm::vec2(0.F, 1.F));
                    }
                });

           p.actions["NW"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->settings->setLock(ViewLock::None);
                        system->_panImage(glm::vec2(-1.F, -1.F));
                    }
                });

            p.actions["NE"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->settings->setLock(ViewLock::None);
                        system->_panImage(glm::vec2(1.F, -1.F));
                    }
                });

            p.actions["SE"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->settings->setLock(ViewLock::None);
                        system->_panImage(glm::vec2(1.F, 1.F));
                    }
                });

            p.actions["SW"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->settings->setLock(ViewLock::None);
                        system->_panImage(glm::vec2(-1.F, 1.F));
                    }
                });

            p.actions["ZoomIn"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        system->_zoomAction(2.F);
                    }
                });

            p.actions["ZoomOut"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        system->_zoomAction(.5F);
                    }
                });

            p.actions["ZoomReset"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->settings->setLock(ViewLock::None);
                        system->_zoomImage(1.F);
                    }
                });

            p.actions["Frame"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        if (system->_p->activeWidget)
                        {
                            system->_p->activeWidget->getViewWidget()->imageFrame(true);
                        }
                    }
                });

            p.actions["Center"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        if (system->_p->activeWidget)
                        {
                            system->_p->activeWidget->getViewWidget()->imageCenter(true);
                        }
                    }
                });

            p.actions["Grid"]->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        GridOptions options = system->_p->gridOptions;
                        options.enabled = value;
                        system->_p->settings->setGridOptions(options);
                    }
                });

            p.actions["HUD"]->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        HUDOptions options = system->_p->hudOptions;
                        options.enabled = value;
                        system->_p->settings->setHUDOptions(options);
                    }
                });

            p.actions["Border"]->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        ViewBackgroundOptions options = system->_p->backgroundOptions;
                        options.border = value;
                        system->_p->settings->setBackgroundOptions(options);
                    }
                });

            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.activeWidgetObserver = Observer::Value<std::shared_ptr<MediaWidget> >::create(
                    windowSystem->observeActiveWidget(),
                    [weak](const std::shared_ptr<MediaWidget>& value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->activeWidget = value;
                            if (system->_p->activeWidget)
                            {
                                system->_p->hoverObserver = Observer::Value<PointerData>::create(
                                    system->_p->activeWidget->observeHover(),
                                    [weak](const PointerData& value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->hoverPos = value.pos;
                                        }
                                    });
                                system->_p->dragObserver = Observer::Value<PointerData>::create(
                                    system->_p->activeWidget->observeDrag(),
                                    [weak](const PointerData& value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->drag(value);
                                        }
                                    });
                                system->_p->scrollObserver = Observer::Value<ScrollData>::create(
                                    system->_p->activeWidget->observeScroll(),
                                    [weak](const ScrollData& value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->scroll(value, system->getContext());
                                        }
                                    });
                            }
                            else
                            {
                                system->_p->hoverObserver.reset();
                                system->_p->dragObserver.reset();
                                system->_p->scrollObserver.reset();
                            }
                            system->_actionsUpdate();
                        }
                    });
            }

            p.lockObserver = Observer::Value<ViewLock>::create(
                p.settings->observeLock(),
                [weak](ViewLock value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->lock->setIfChanged(value);
                        switch (value)
                        {
                        case ViewLock::None:
                            system->_p->lockActionGroup->setChecked(-1);
                            break;
                        case ViewLock::Frame:
                            system->_p->lockActionGroup->setChecked(0);
                            break;
                        case ViewLock::Center:
                            system->_p->lockActionGroup->setChecked(1);
                            break;
                        default: break;
                        }
                    }
                });

            p.gridOptionsObserver = Observer::Value<GridOptions>::create(
                p.settings->observeGridOptions(),
                [weak](const GridOptions& value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->gridOptions = value;
                        system->_actionsUpdate();
                    }
                });
            p.hudOptionsObserver = Observer::Value<HUDOptions>::create(
                p.settings->observeHUDOptions(),
                [weak](const HUDOptions& value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->hudOptions = value;
                        system->_actionsUpdate();
                    }
                });
            p.backgroundOptionsObserver = Observer::Value<ViewBackgroundOptions>::create(
                p.settings->observeBackgroundOptions(),
                [weak](const ViewBackgroundOptions& value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->backgroundOptions = value;
                        system->_actionsUpdate();
                    }
                });

            _logInitTime();
        }

        ViewSystem::ViewSystem() :
            _p(new Private)
        {}

        ViewSystem::~ViewSystem()
        {
            DJV_PRIVATE_PTR();
            p.settings->setBellowsState(p.bellowsState);
        }

        std::shared_ptr<ViewSystem> ViewSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = context->getSystemT<ViewSystem>();
            if (!out)
            {
                out = std::shared_ptr<ViewSystem>(new ViewSystem);
                out->_init(context);
            }
            return out;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > ViewSystem::getActions() const
        {
            return _p->actions;
        }

        MenuData ViewSystem::getMenuData() const
        {
            return
            {
                { _p->menu },
                4
            };
        }

        ActionData ViewSystem::getToolBarActionData() const
        {
            return
            {
                {
                    _p->actions["FrameLock"],
                    _p->actions["CenterLock"]
                },
                4
            };
        }

        ActionData ViewSystem::getToolActionData() const
        {
            return
            {
                {
                    _p->actions["PanZoom"],
                    _p->actions["ViewOptions"]
                },
                4
            };
        }

        ToolWidgetData ViewSystem::createToolWidget(const std::shared_ptr<UI::Action>& value)
        {
            DJV_PRIVATE_PTR();
            ToolWidgetData out;
            if (auto context = getContext().lock())
            {
                if (value == p.actions["PanZoom"])
                {
                    auto titleBar = ToolTitleBar::create(DJV_TEXT("view_pan_zoom"), context);

                    auto widget = ViewPanZoomWidget::create(context);

                    out.titleBar = titleBar;
                    out.widget = widget;
                }
                else if (value == p.actions["ViewOptions"])
                {
                    auto titleBar = ToolTitleBar::create(DJV_TEXT("view_options"), context);

                    auto widget = ViewOptionsWidget::create(context);
                    widget->setBellowsState(p.bellowsState);
                    p.viewOptionsWidget = widget;

                    out.titleBar = titleBar;
                    out.widget = widget;
                }
            }
            return out;
        }

        void ViewSystem::deleteToolWidget(const std::shared_ptr<UI::Action>& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.actions["ViewOptions"])
            {
                if (auto widget = p.viewOptionsWidget.lock())
                {
                    p.bellowsState = widget->getBellowsState();
                    p.viewOptionsWidget.reset();
                }
            }
        }

        void ViewSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["PanZoom"]->setText(_getText(DJV_TEXT("menu_view_pan_zoom")));
                p.actions["PanZoom"]->setTooltip(_getText(DJV_TEXT("menu_view_pan_zoom_tooltip")));
                p.actions["ViewOptions"]->setText(_getText(DJV_TEXT("menu_view_options")));
                p.actions["ViewOptions"]->setTooltip(_getText(DJV_TEXT("menu_view_options_tooltip")));
                p.actions["Left"]->setText(_getText(DJV_TEXT("menu_view_left")));
                p.actions["Left"]->setTooltip(_getText(DJV_TEXT("menu_view_left_tooltip")));
                p.actions["Right"]->setText(_getText(DJV_TEXT("menu_view_right")));
                p.actions["Right"]->setTooltip(_getText(DJV_TEXT("menu_view_right_tooltip")));
                p.actions["Up"]->setText(_getText(DJV_TEXT("menu_view_up")));
                p.actions["Up"]->setTooltip(_getText(DJV_TEXT("menu_view_up_tooltip")));
                p.actions["Down"]->setText(_getText(DJV_TEXT("menu_view_down")));
                p.actions["Down"]->setTooltip(_getText(DJV_TEXT("menu_view_down_tooltip")));
                p.actions["NW"]->setText(_getText(DJV_TEXT("north_west")));
                p.actions["NW"]->setTooltip(_getText(DJV_TEXT("north_west_tooltip")));
                p.actions["NE"]->setText(_getText(DJV_TEXT("north_east")));
                p.actions["NE"]->setTooltip(_getText(DJV_TEXT("north_east_tooltip")));
                p.actions["SE"]->setText(_getText(DJV_TEXT("south_east")));
                p.actions["SE"]->setTooltip(_getText(DJV_TEXT("south_east_tooltip")));
                p.actions["SW"]->setText(_getText(DJV_TEXT("south_west")));
                p.actions["SW"]->setTooltip(_getText(DJV_TEXT("south_west_tooltip")));
                p.actions["ZoomIn"]->setText(_getText(DJV_TEXT("menu_view_zoom_in")));
                p.actions["ZoomIn"]->setTooltip(_getText(DJV_TEXT("menu_view_zoom_in_tooltip")));
                p.actions["ZoomOut"]->setText(_getText(DJV_TEXT("menu_view_zoom_out")));
                p.actions["ZoomOut"]->setTooltip(_getText(DJV_TEXT("menu_view_zoom_out_tooltip")));
                p.actions["ZoomReset"]->setText(_getText(DJV_TEXT("menu_view_zoom_reset")));
                p.actions["ZoomReset"]->setTooltip(_getText(DJV_TEXT("menu_view_zoom_reset_tooltip")));
                p.actions["Frame"]->setText(_getText(DJV_TEXT("menu_view_frame")));
                p.actions["Frame"]->setTooltip(_getText(DJV_TEXT("menu_view_frame_tooltip")));
                p.actions["FrameLock"]->setText(_getText(DJV_TEXT("menu_view_lock_frame")));
                p.actions["FrameLock"]->setTooltip(_getText(DJV_TEXT("menu_view_lock_frame_tooltip")));
                p.actions["Center"]->setText(_getText(DJV_TEXT("menu_view_center")));
                p.actions["Center"]->setTooltip(_getText(DJV_TEXT("menu_view_center_tooltip")));
                p.actions["CenterLock"]->setText(_getText(DJV_TEXT("menu_view_lock_center")));
                p.actions["CenterLock"]->setTooltip(_getText(DJV_TEXT("menu_view_lock_center_tooltip")));
                p.actions["Grid"]->setText(_getText(DJV_TEXT("menu_view_grid")));
                p.actions["Grid"]->setTooltip(_getText(DJV_TEXT("menu_view_grid_tooltip")));
                p.actions["HUD"]->setText(_getText(DJV_TEXT("menu_view_hud")));
                p.actions["HUD"]->setTooltip(_getText(DJV_TEXT("menu_view_hud_tooltip")));
                p.actions["Border"]->setText(_getText(DJV_TEXT("menu_view_border")));
                p.actions["Border"]->setTooltip(_getText(DJV_TEXT("menu_view_border_tooltip")));

                p.menu->setText(_getText(DJV_TEXT("menu_view")));
            }
        }

        void ViewSystem::_shortcutsUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["PanZoom"]->setShortcuts(_getShortcuts("shortcut_view_pan_zoom"));
                p.actions["ViewOptions"]->setShortcuts(_getShortcuts("shortcut_view_options"));
                p.actions["Left"]->setShortcuts(_getShortcuts("shortcut_view_left"));
                p.actions["Right"]->setShortcuts(_getShortcuts("shortcut_view_right"));
                p.actions["Up"]->setShortcuts(_getShortcuts("shortcut_view_up"));
                p.actions["Down"]->setShortcuts(_getShortcuts("shortcut_view_down"));
                p.actions["NW"]->setShortcuts(_getShortcuts("shortcut_view_nw"));
                p.actions["NE"]->setShortcuts(_getShortcuts("shortcut_view_ne"));
                p.actions["SE"]->setShortcuts(_getShortcuts("shortcut_view_se"));
                p.actions["SW"]->setShortcuts(_getShortcuts("shortcut_view_sw"));
                p.actions["ZoomIn"]->setShortcuts(_getShortcuts("shortcut_view_zoom_in"));
                p.actions["ZoomOut"]->setShortcuts(_getShortcuts("shortcut_view_zoom_out"));
                p.actions["ZoomReset"]->setShortcuts(_getShortcuts("shortcut_view_zoom_reset"));
                p.actions["Frame"]->setShortcuts(_getShortcuts("shortcut_view_frame"));
                p.actions["FrameLock"]->setShortcuts(_getShortcuts("shortcut_view_frame_lock"));
                p.actions["Center"]->setShortcuts(_getShortcuts("shortcut_view_center"));
                p.actions["CenterLock"]->setShortcuts(_getShortcuts("shortcut_view_center_lock"));
                p.actions["Grid"]->setShortcuts(_getShortcuts("shortcut_view_grid"));
                p.actions["HUD"]->setShortcuts(_getShortcuts("shortcut_view_hud"));
                p.actions["Border"]->setShortcuts(_getShortcuts("shortcut_view_border"));
            }
        }

        float ViewSystem::_getScrollWheelSpeed(UI::ScrollWheelSpeed value)
        {
            const float values[] =
            {
                .1,
                .25,
                .5
            };
            return values[static_cast<size_t>(value)];
        }

        void ViewSystem::_panImage(const glm::vec2& value)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                if (auto widget = p.activeWidget)
                {
                    auto uiSystem = context->getSystemT<UI::UISystem>();
                    auto style = uiSystem->getStyle();
                    const float m = style->getMetric(UI::MetricsRole::Move);
                    auto viewWidget = widget->getViewWidget();
                    viewWidget->setImagePos(viewWidget->observeImagePos()->get() + value * m);
                }
            }
        }

        void ViewSystem::_zoomImage(float value)
        {
            DJV_PRIVATE_PTR();
            if (auto widget = p.activeWidget)
            {
                auto viewWidget = widget->getViewWidget();
                const float w = viewWidget->getWidth();
                const float h = viewWidget->getHeight();
                glm::vec2 focus = glm::vec2(0.F, 0.F);
                if (Math::BBox2f(0.F, 0.F, w, h).contains(p.hoverPos))
                {
                    focus = p.hoverPos;
                }
                else
                {
                    focus.x = w / 2.F;
                    focus.y = h / 2.F;
                }
                viewWidget->setImageZoomFocus(value, focus, true);
            }
        }
        
        void ViewSystem::_zoomAction(float value)
        {
            DJV_PRIVATE_PTR();
            p.settings->setLock(ViewLock::None);
            if (auto widget = p.activeWidget)
            {
                auto viewWidget = widget->getViewWidget();
                const float zoom = viewWidget->observeImageZoom()->get();
                _zoomImage(zoom * value);
            }
        }

        void ViewSystem::_actionsUpdate()
        {
            DJV_PRIVATE_PTR();
            const bool activeWidget = p.activeWidget.get();
            p.actions["Left"]->setEnabled(activeWidget);
            p.actions["Right"]->setEnabled(activeWidget);
            p.actions["Up"]->setEnabled(activeWidget);
            p.actions["Down"]->setEnabled(activeWidget);
            p.actions["NW"]->setEnabled(activeWidget);
            p.actions["NE"]->setEnabled(activeWidget);
            p.actions["SE"]->setEnabled(activeWidget);
            p.actions["SW"]->setEnabled(activeWidget);
            p.actions["ZoomIn"]->setEnabled(activeWidget);
            p.actions["ZoomOut"]->setEnabled(activeWidget);
            p.actions["ZoomReset"]->setEnabled(activeWidget);
            p.actions["Grid"]->setChecked(p.gridOptions.enabled);
        }

        void ViewSystem::Private::drag(const PointerData& value)
        {
            if (auto viewWidget = activeWidget->getViewWidget())
            {
                bool pan = false;
                auto i = value.buttons.find(1);
                pan |=
                    1 == value.buttons.size() &&
                    i != value.buttons.end() &&
                    actions["PanZoom"]->observeChecked()->get();
                i = value.buttons.find(2);
                pan |=
                    1 == value.buttons.size() &&
                    i != value.buttons.end() &&
                    0 == value.key &&
                    0 == value.keyModifiers;
                if (pan)
                {
                    settings->setLock(ViewLock::None);
                    auto viewWidget = activeWidget->getViewWidget();
                    switch (value.state)
                    {
                    case PointerState::Start:
                        dragStart = value.pos;
                        dragImagePos = viewWidget->observeImagePos()->get();
                        break;
                    case PointerState::Move:
                        viewWidget->setImagePos(dragImagePos + (value.pos - dragStart));
                        break;
                    default: break;
                    }
                }
            }
        }

        void ViewSystem::Private::scroll(const ScrollData& value, const std::weak_ptr<System::Context>& contextWeak)
        {
            if (auto viewWidget = activeWidget->getViewWidget())
            {
                bool zoom = false;
                zoom |=
                    (value.delta.x != 0.F || value.delta.y != 0.F) &&
                    0 == value.key &&
                    0 == value.keyModifiers;
                if (zoom)
                {
                    if (auto context = contextWeak.lock())
                    {
                        settings->setLock(ViewLock::None);
                        const float zoom = viewWidget->observeImageZoom()->get();
                        auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                        auto mouseSettings = settingsSystem->getSettingsT<UI::Settings::Mouse>();
                        const float speed = _getScrollWheelSpeed(mouseSettings->observeScrollWheelSpeed()->get());
                        viewWidget->setImageZoomFocus(zoom * (1.F + value.delta.y * speed), hoverPos);
                    }
                }
            }
        }

    } // namespace ViewApp
} // namespace djv

