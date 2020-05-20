// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ViewSystem.h>

#include <djvViewApp/ImageView.h>
#include <djvViewApp/InputSettings.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/ViewControlsWidget.h>
#include <djvViewApp/ViewSettings.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Shortcut.h>
#include <djvUI/Style.h>
#include <djvUI/UISystem.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

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

            int widgetCurrentTab = 0;
            GridOptions gridOptions;
            bool currentTool = false;
            glm::vec2 hoverPos = glm::vec2(0.F, 0.F);
            glm::vec2 dragStart = glm::vec2(0.F, 0.F);
            glm::vec2 dragImagePos = glm::vec2(0.F, 0.F);
            std::shared_ptr<MediaWidget> activeWidget;
            std::shared_ptr<ValueSubject<ImageViewLock> > lock;
            
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> lockActionGroup;
            std::shared_ptr<UI::Menu> menu;
            std::weak_ptr<ViewControlsWidget> viewControlsWidget;
            
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > actionObservers;
            std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<ValueObserver<ImageViewLock> > lockObserver;
            std::shared_ptr<ValueObserver<GridOptions> > gridOptionsObserver;
            std::shared_ptr<ValueObserver<PointerData> > hoverObserver;
            std::shared_ptr<ValueObserver<PointerData> > dragObserver;
            std::shared_ptr<ValueObserver<ScrollData> > scrollObserver;

            void drag(const PointerData&);
            void scroll(const ScrollData&, const std::weak_ptr<Context>&);
        };

        void ViewSystem::_init(const std::shared_ptr<Context>& context)
        {
            IToolSystem::_init("djv::ViewApp::ViewSystem", context);

            DJV_PRIVATE_PTR();

            p.settings = ViewSettings::create(context);
            _setWidgetGeom(p.settings->getWidgetGeom());

            p.gridOptions = p.settings->observeGridOptions()->get();
            p.lock = ValueSubject<ImageViewLock>::create();

            p.actions["ViewControls"] = UI::Action::create();
            p.actions["ViewControls"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["ViewControls"]->addShortcut(GLFW_KEY_W, UI::Shortcut::getSystemModifier());
            p.actions["Tool"] = UI::Action::create();
            p.actions["Tool"]->setIcon("djvIconMove");
            p.actions["Tool"]->addShortcut(GLFW_KEY_N, UI::Shortcut::getSystemModifier());
            p.actions["Left"] = UI::Action::create();
            p.actions["Left"]->addShortcut(GLFW_KEY_KP_4);
            p.actions["Right"] = UI::Action::create();
            p.actions["Right"]->addShortcut(GLFW_KEY_KP_6);
            p.actions["Up"] = UI::Action::create();
            p.actions["Up"]->addShortcut(GLFW_KEY_KP_8);
            p.actions["Down"] = UI::Action::create();
            p.actions["Down"]->addShortcut(GLFW_KEY_KP_2);
            p.actions["NW"] = UI::Action::create();
            p.actions["NW"]->addShortcut(GLFW_KEY_KP_7);
            p.actions["NE"] = UI::Action::create();
            p.actions["NE"]->addShortcut(GLFW_KEY_KP_9);
            p.actions["SE"] = UI::Action::create();
            p.actions["SE"]->addShortcut(GLFW_KEY_KP_3);
            p.actions["SW"] = UI::Action::create();
            p.actions["SW"]->addShortcut(GLFW_KEY_KP_1);
            p.actions["ZoomIn"] = UI::Action::create();
            p.actions["ZoomIn"]->setIcon("djvIconZoomIn");
            p.actions["ZoomIn"]->addShortcut(GLFW_KEY_EQUAL);
            p.actions["ZoomIn"]->addShortcut(GLFW_KEY_KP_ADD);
            p.actions["ZoomOut"] = UI::Action::create();
            p.actions["ZoomOut"]->setIcon("djvIconZoomOut");
            p.actions["ZoomOut"]->addShortcut(GLFW_KEY_MINUS);
            p.actions["ZoomOut"]->addShortcut(GLFW_KEY_KP_SUBTRACT);
            p.actions["ZoomReset"] = UI::Action::create();
            p.actions["ZoomReset"]->setIcon("djvIconZoomReset");
            p.actions["ZoomReset"]->addShortcut(GLFW_KEY_0);
            p.actions["ZoomReset"]->addShortcut(GLFW_KEY_KP_0);
            p.actions["Fill"] = UI::Action::create();
            p.actions["Fill"]->setIcon("djvIconViewFill");
            p.actions["Fill"]->addShortcut(GLFW_KEY_BACKSPACE);
            p.actions["Fill"]->addShortcut(GLFW_KEY_KP_MULTIPLY);
            p.actions["FillLock"] = UI::Action::create();
            p.actions["FillLock"]->setIcon("djvIconViewFill");
            p.actions["FillLock"]->addShortcut(GLFW_KEY_BACKSPACE, GLFW_MOD_SHIFT);
            p.actions["FillLock"]->addShortcut(GLFW_KEY_KP_MULTIPLY, GLFW_MOD_SHIFT);
            p.actions["Frame"] = UI::Action::create();
            p.actions["Frame"]->setIcon("djvIconViewFrame");
            p.actions["Frame"]->addShortcut(GLFW_KEY_PERIOD);
            p.actions["Frame"]->addShortcut(GLFW_KEY_KP_DECIMAL);
            p.actions["FrameLock"] = UI::Action::create();
            p.actions["FrameLock"]->setIcon("djvIconViewFrame");
            p.actions["FrameLock"]->addShortcut(GLFW_KEY_PERIOD, GLFW_MOD_SHIFT);
            p.actions["FrameLock"]->addShortcut(GLFW_KEY_KP_DECIMAL, GLFW_MOD_SHIFT);
            p.actions["Center"] = UI::Action::create();
            p.actions["Center"]->setIcon("djvIconViewCenter");
            p.actions["Center"]->addShortcut(GLFW_KEY_BACKSLASH);
            p.actions["Center"]->addShortcut(GLFW_KEY_KP_5);
            p.actions["CenterLock"] = UI::Action::create();
            p.actions["CenterLock"]->setIcon("djvIconViewCenter");
            p.actions["CenterLock"]->addShortcut(GLFW_KEY_BACKSLASH, GLFW_MOD_SHIFT);
            p.actions["CenterLock"]->addShortcut(GLFW_KEY_KP_5, GLFW_MOD_SHIFT);
            p.lockActionGroup = UI::ActionGroup::create(UI::ButtonType::Exclusive);
            p.lockActionGroup->addAction(p.actions["FillLock"]);
            p.lockActionGroup->addAction(p.actions["FrameLock"]);
            p.lockActionGroup->addAction(p.actions["CenterLock"]);
            p.actions["GridEnabled"] = UI::Action::create();
            p.actions["GridEnabled"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["GridEnabled"]->addShortcut(GLFW_KEY_G, GLFW_MOD_CONTROL);
            //! \todo Implement me!
            //p.actions["HUD"] = UI::Action::create();
            //p.actions["HUD"]->setShortcut(GLFW_KEY_H);
            //p.actions["HUD"]->setEnabled(false);

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["ViewControls"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["Left"]);
            p.menu->addAction(p.actions["Right"]);
            p.menu->addAction(p.actions["Up"]);
            p.menu->addAction(p.actions["Down"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["ZoomIn"]);
            p.menu->addAction(p.actions["ZoomOut"]);
            p.menu->addAction(p.actions["ZoomReset"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["Fill"]);
            p.menu->addAction(p.actions["FillLock"]);
            p.menu->addAction(p.actions["Frame"]);
            p.menu->addAction(p.actions["FrameLock"]);
            p.menu->addAction(p.actions["Center"]);
            p.menu->addAction(p.actions["CenterLock"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["GridEnabled"]);
            //p.menu->addAction(p.actions["HUD"]);

            _actionsUpdate();
            _textUpdate();

            auto weak = std::weak_ptr<ViewSystem>(std::dynamic_pointer_cast<ViewSystem>(shared_from_this()));
            p.lockActionGroup->setExclusiveCallback(
                [weak](int index)
                {
                    if (auto system = weak.lock())
                    {
                        ImageViewLock lock = ImageViewLock::None;
                        switch (index)
                        {
                        case 0: lock = ImageViewLock::Fill;   break;
                        case 1: lock = ImageViewLock::Frame;  break;
                        case 2: lock = ImageViewLock::Center; break;
                        }
                        system->_p->settings->setLock(lock);
                    }
                });

            auto contextWeak = std::weak_ptr<Context>(context);
            p.actionObservers["ViewControls"] = ValueObserver<bool>::create(
                p.actions["ViewControls"]->observeChecked(),
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto system = weak.lock())
                        {
                            if (value)
                            {
                                auto widget = ViewControlsWidget::create(context);
                                widget->setCurrentTab(system->_p->widgetCurrentTab);
                                system->_p->viewControlsWidget = widget;
                                system->_openWidget("ViewControls", widget);
                            }
                            else
                            {
                                system->_closeWidget("ViewControls");
                            }
                        }
                    }
                });

            p.actionObservers["Left"] = ValueObserver<bool>::create(
                p.actions["Left"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_panImage(glm::vec2(-1.F, 0.F));
                        }
                    }
                });

            p.actionObservers["Right"] = ValueObserver<bool>::create(
                p.actions["Right"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->settings->setLock(ImageViewLock::None);
                            system->_panImage(glm::vec2(1.F, 0.F));
                        }
                    }
                });

            p.actionObservers["Up"] = ValueObserver<bool>::create(
                p.actions["Up"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->settings->setLock(ImageViewLock::None);
                            system->_panImage(glm::vec2(0.F, -1.F));
                        }
                    }
                });

            p.actionObservers["Down"] = ValueObserver<bool>::create(
                p.actions["Down"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->settings->setLock(ImageViewLock::None);
                            system->_panImage(glm::vec2(0.F, 1.F));
                        }
                    }
                });

            p.actionObservers["NW"] = ValueObserver<bool>::create(
                p.actions["NW"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->settings->setLock(ImageViewLock::None);
                            system->_panImage(glm::vec2(-1.F, -1.F));
                        }
                    }
                });

            p.actionObservers["NE"] = ValueObserver<bool>::create(
                p.actions["NE"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->settings->setLock(ImageViewLock::None);
                            system->_panImage(glm::vec2(1.F, -1.F));
                        }
                    }
                });

            p.actionObservers["SE"] = ValueObserver<bool>::create(
                p.actions["SE"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->settings->setLock(ImageViewLock::None);
                            system->_panImage(glm::vec2(1.F, 1.F));
                        }
                    }
                });

            p.actionObservers["SW"] = ValueObserver<bool>::create(
                p.actions["SW"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->settings->setLock(ImageViewLock::None);
                            system->_panImage(glm::vec2(-1.F, 1.F));
                        }
                    }
                });

            p.actionObservers["ZoomIn"] = ValueObserver<bool>::create(
                p.actions["ZoomIn"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_zoomAction(2.F);
                        }
                    }
                });

            p.actionObservers["ZoomOut"] = ValueObserver<bool>::create(
                p.actions["ZoomOut"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_zoomAction(.5F);
                        }
                    }
                });

            p.actionObservers["ZoomReset"] = ValueObserver<bool>::create(
                p.actions["ZoomReset"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->settings->setLock(ImageViewLock::None);
                            system->_zoomImage(1.F);
                        }
                    }
                });

            p.actionObservers["Fill"] = ValueObserver<bool>::create(
                p.actions["Fill"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            if (system->_p->activeWidget)
                            {
                                system->_p->activeWidget->getImageView()->imageFill(true);
                            }
                        }
                    }
                });

            p.actionObservers["Frame"] = ValueObserver<bool>::create(
                p.actions["Frame"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            if (system->_p->activeWidget)
                            {
                                system->_p->activeWidget->getImageView()->imageFrame(true);
                            }
                        }
                    }
                });

            p.actionObservers["Center"] = ValueObserver<bool>::create(
                p.actions["Center"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            if (system->_p->activeWidget)
                            {
                                system->_p->activeWidget->getImageView()->imageCenter(true);
                            }
                        }
                    }
                });

            p.actionObservers["GridEnabled"] = ValueObserver<bool>::create(
                p.actions["GridEnabled"]->observeChecked(),
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->gridOptions.enabled = value;
                        system->_p->settings->setGridOptions(system->_p->gridOptions);
                        if (system->_p->activeWidget)
                        {
                            system->_p->activeWidget->getImageView()->setGridOptions(system->_p->gridOptions);
                        }
                    }
                });

            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.activeWidgetObserver = ValueObserver<std::shared_ptr<MediaWidget> >::create(
                    windowSystem->observeActiveWidget(),
                    [weak](const std::shared_ptr<MediaWidget>& value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->activeWidget = value;
                            if (system->_p->activeWidget)
                            {
                                system->_p->gridOptionsObserver = ValueObserver<GridOptions>::create(
                                    system->_p->activeWidget->getImageView()->observeGridOptions(),
                                    [weak](const GridOptions& value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->gridOptions = value;
                                            system->_actionsUpdate();
                                        }
                                    });
                                system->_p->hoverObserver = ValueObserver<PointerData>::create(
                                    system->_p->activeWidget->observeHover(),
                                    [weak](const PointerData& value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->hoverPos = value.pos;
                                        }
                                    });
                                system->_p->dragObserver = ValueObserver<PointerData>::create(
                                    system->_p->activeWidget->observeDrag(),
                                    [weak](const PointerData& value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->drag(value);
                                        }
                                    });
                                system->_p->scrollObserver = ValueObserver<ScrollData>::create(
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
                                system->_p->gridOptionsObserver.reset();
                                system->_p->hoverObserver.reset();
                                system->_p->dragObserver.reset();
                                system->_p->scrollObserver.reset();
                            }
                            system->_actionsUpdate();
                        }
                    });
            }

            p.lockObserver = ValueObserver<ImageViewLock>::create(
                p.settings->observeLock(),
                [weak](ImageViewLock value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->lock->setIfChanged(value);
                        switch (value)
                        {
                        case ImageViewLock::None:
                            system->_p->lockActionGroup->setChecked(-1);
                            break;
                        case ImageViewLock::Fill:
                            system->_p->lockActionGroup->setChecked(0);
                            break;
                        case ImageViewLock::Frame:
                            system->_p->lockActionGroup->setChecked(1);
                            break;
                        case ImageViewLock::Center:
                            system->_p->lockActionGroup->setChecked(2);
                            break;
                        default: break;
                        }
                    }
                });
        }

        ViewSystem::ViewSystem() :
            _p(new Private)
        {}

        ViewSystem::~ViewSystem()
        {
            DJV_PRIVATE_PTR();
            _closeWidget("ViewControls");
            p.settings->setWidgetCurrentTab(p.widgetCurrentTab);
            p.settings->setWidgetGeom(_getWidgetGeom());
        }

        std::shared_ptr<ViewSystem> ViewSystem::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ViewSystem>(new ViewSystem);
            out->_init(context);
            return out;
        }

        ToolActionData ViewSystem::getToolAction() const
        {
            return
            {
                _p->actions["Tool"],
                "A"
            };
        }

        void ViewSystem::setCurrentTool(bool value)
        {
            _p->currentTool = value;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > ViewSystem::getActions() const
        {
            return _p->actions;
        }

        MenuData ViewSystem::getMenu() const
        {
            return
            {
                _p->menu,
                "C"
            };
        }

        void ViewSystem::_closeWidget(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if ("ViewControls" == value)
            {
                if (auto widget = p.viewControlsWidget.lock())
                {
                    p.widgetCurrentTab = widget->getCurrentTab();
                }
                p.viewControlsWidget.reset();
            }
            const auto i = p.actions.find(value);
            if (i != p.actions.end())
            {
                i->second->setChecked(false);
            }
            IViewSystem::_closeWidget(value);
        }

        void ViewSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["ViewControls"]->setText(_getText(DJV_TEXT("menu_view_controls")));
                p.actions["ViewControls"]->setTooltip(_getText(DJV_TEXT("menu_view_controls_widget_tooltip")));
                p.actions["Tool"]->setText(_getText(DJV_TEXT("menu_tools_pan_view")));
                p.actions["Tool"]->setTooltip(_getText(DJV_TEXT("menu_tools_pan_view_tooltip")));
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
                p.actions["Fill"]->setText(_getText(DJV_TEXT("menu_view_fill")));
                p.actions["Fill"]->setTooltip(_getText(DJV_TEXT("menu_view_fill_tooltip")));
                p.actions["FillLock"]->setText(_getText(DJV_TEXT("menu_view_lock_fill")));
                p.actions["FillLock"]->setTooltip(_getText(DJV_TEXT("menu_view_lock_fill_tooltip")));
                p.actions["Frame"]->setText(_getText(DJV_TEXT("menu_view_frame")));
                p.actions["Frame"]->setTooltip(_getText(DJV_TEXT("menu_view_frame_tooltip")));
                p.actions["FrameLock"]->setText(_getText(DJV_TEXT("menu_view_lock_frame")));
                p.actions["FrameLock"]->setTooltip(_getText(DJV_TEXT("menu_view_lock_frame_tooltip")));
                p.actions["Center"]->setText(_getText(DJV_TEXT("menu_view_center")));
                p.actions["Center"]->setTooltip(_getText(DJV_TEXT("menu_view_center_tooltip")));
                p.actions["CenterLock"]->setText(_getText(DJV_TEXT("menu_view_lock_center")));
                p.actions["CenterLock"]->setTooltip(_getText(DJV_TEXT("menu_view_lock_center_tooltip")));
                p.actions["GridEnabled"]->setText(_getText(DJV_TEXT("menu_view_grid")));
                p.actions["GridEnabled"]->setTooltip(_getText(DJV_TEXT("menu_view_grid_tooltip")));
                //p.actions["HUD"]->setText(_getText(DJV_TEXT("view_hud")));
                //p.actions["HUD"]->setTooltip(_getText(DJV_TEXT("view_hud_tooltip")));

                p.menu->setText(_getText(DJV_TEXT("menu_view")));
            }
        }

        float ViewSystem::_getScrollWheelSpeed(ScrollWheelSpeed value)
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
                    auto imageView = widget->getImageView();
                    imageView->setImagePos(imageView->observeImagePos()->get() + value * m);
                }
            }
        }

        void ViewSystem::_zoomImage(float value)
        {
            DJV_PRIVATE_PTR();
            if (auto widget = p.activeWidget)
            {
                auto imageView = widget->getImageView();
                const float w = imageView->getWidth();
                const float h = imageView->getHeight();
                glm::vec2 focus = glm::vec2(0.F, 0.F);
                if (BBox2f(0.F, 0.F, w, h).contains(p.hoverPos))
                {
                    focus = p.hoverPos;
                }
                else
                {
                    focus.x = w / 2.F;
                    focus.y = h / 2.F;
                }
                imageView->setImageZoomFocus(value, focus, true);
            }
        }
        
        void ViewSystem::_zoomAction(float value)
        {
            DJV_PRIVATE_PTR();
            p.settings->setLock(ImageViewLock::None);
            if (auto widget = p.activeWidget)
            {
                auto imageView = widget->getImageView();
                const float zoom = imageView->observeImageZoom()->get();
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
            p.actions["GridEnabled"]->setChecked(p.gridOptions.enabled);
        }

        void ViewSystem::Private::drag(const PointerData& value)
        {
            if (auto imageView = activeWidget->getImageView())
            {
                bool pan = false;
                auto i = value.buttons.find(1);
                pan |=
                    1 == value.buttons.size() &&
                    i != value.buttons.end() &&
                    currentTool;
                i = value.buttons.find(2);
                pan |=
                    1 == value.buttons.size() &&
                    i != value.buttons.end() &&
                    0 == value.key &&
                    0 == value.keyModifiers;
                if (pan)
                {
                    settings->setLock(ImageViewLock::None);
                    auto imageView = activeWidget->getImageView();
                    switch (value.state)
                    {
                    case PointerState::Start:
                        dragStart = value.pos;
                        dragImagePos = imageView->observeImagePos()->get();
                        break;
                    case PointerState::Move:
                        imageView->setImagePos(dragImagePos + (value.pos - dragStart));
                        break;
                    default: break;
                    }
                }
            }
        }

        void ViewSystem::Private::scroll(const ScrollData& value, const std::weak_ptr<Context>& contextWeak)
        {
            if (auto imageView = activeWidget->getImageView())
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
                        settings->setLock(ImageViewLock::None);
                        const float zoom = imageView->observeImageZoom()->get();
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        auto inputSettings = settingsSystem->getSettingsT<InputSettings>();
                        const float speed = _getScrollWheelSpeed(inputSettings->observeScrollWheelSpeed()->get());
                        imageView->setImageZoomFocus(zoom * (1.F + value.delta.y * speed), hoverPos);
                    }
                }
            }
        }

    } // namespace ViewApp
} // namespace djv

