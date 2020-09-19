// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/WindowSystem.h>

#include <djvViewApp/MediaCanvas.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/WindowSettings.h>

#include <djvDesktopApp/Application.h>
#include <djvDesktopApp/GLFWSystem.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ShortcutData.h>
#include <djvUI/UISystem.h>

#include <djvAV/AVSystem.h>

#include <djvGL/GLFWSystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/IEventSystem.h>
#include <djvSystem/TextSystem.h>
#include <djvSystem/Timer.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            //! \todo Should this be configurable?
            const glm::ivec2 windowSizeMin = glm::ivec2(320, 240);
        }

        struct WindowSystem::Private
        {
            std::shared_ptr<GL::GLFW::GLFWSystem> glGLFWSystem;
            std::shared_ptr<Desktop::GLFWSystem> desktopGLFWSystem;

            std::shared_ptr<WindowSettings> settings;
            std::weak_ptr<MediaCanvas> canvas;
            std::shared_ptr<ValueSubject<std::shared_ptr<MediaWidget> > > activeWidget;
            std::shared_ptr<ValueSubject<bool> > fullScreen;
            std::shared_ptr<ValueSubject<bool> > presentation;
            std::shared_ptr<ValueSubject<bool> > floatOnTop;
            std::shared_ptr<ValueSubject<bool> > maximize;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::Menu> menu;
            glm::ivec2 monitorSize = glm::ivec2(0, 0);
            int monitorRefresh = 0;
            Math::BBox2i windowGeom = Math::BBox2i(0, 0, 0, 0);
            
            std::shared_ptr<ValueObserver<bool> > fullScreenObserver;
            std::shared_ptr<ValueObserver<bool> > floatOnTopObserver;
            std::shared_ptr<ValueObserver<bool> > maximizeObserver;

            void setFullScreen(bool);
            void setFloatOnTop(bool);
        };

        void WindowSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            IViewSystem::_init("djv::ViewApp::WindowSystem", context);

            DJV_PRIVATE_PTR();

            p.glGLFWSystem = context->getSystemT<GL::GLFW::GLFWSystem>();
            p.desktopGLFWSystem = context->getSystemT<Desktop::GLFWSystem>();

            p.settings = WindowSettings::create(context);

            auto glfwWindow = p.glGLFWSystem->getGLFWWindow();
            if (p.settings->observeRestoreSize()->get())
            {
                const glm::ivec2& windowSize = p.settings->getWindowSize();
                glfwSetWindowSize(
                    glfwWindow,
                    std::max(windowSize.x, windowSizeMin.x),
                    std::max(windowSize.y, windowSizeMin.y));
            }
            else
            {
                const glm::ivec2& windowSizeDefault = p.settings->getWindowSizeDefault();
                glfwSetWindowSize(
                    glfwWindow,
                    std::max(windowSizeDefault.x, windowSizeMin.x),
                    std::max(windowSizeDefault.y, windowSizeMin.y));
            }
            if (p.settings->observeRestorePos()->get())
            {
                const glm::ivec2& windowPos = p.settings->getWindowPos();
                glfwSetWindowPos(glfwWindow, windowPos.x, windowPos.y);
            }

            p.activeWidget = ValueSubject<std::shared_ptr<MediaWidget> >::create();
            p.fullScreen = ValueSubject<bool>::create(false);
            p.presentation = ValueSubject<bool>::create(false);
            p.floatOnTop = ValueSubject<bool>::create(false);
            p.maximize = ValueSubject<bool>::create(false);

            p.actions["FullScreen"] = UI::Action::create();
            p.actions["FullScreen"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["FullScreen"]->setIcon("djvIconWindowFullScreen");
            p.actions["Presentation"] = UI::Action::create();
            p.actions["Presentation"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["FloatOnTop"] = UI::Action::create();
            p.actions["FloatOnTop"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Maximize"] = UI::Action::create();
            p.actions["Maximize"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Maximize"]->setIcon("djvIconMDI");
            p.actions["Maximize"]->setCheckedIcon("djvIconSDI");
            p.actions["Fit"] = UI::Action::create();

            _addShortcut("shortcut_window_full_screen", GLFW_KEY_U);
            _addShortcut("shortcut_window_presentation", GLFW_KEY_E);
            _addShortcut("shortcut_window_maximize", GLFW_KEY_M);
            _addShortcut("shortcut_window_fit", GLFW_KEY_F, UI::ShortcutData::getSystemModifier());
            _addShortcut("shortcut_window_auto_hide", GLFW_KEY_H, GLFW_MOD_SHIFT | UI::ShortcutData::getSystemModifier());

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["FullScreen"]);
            p.menu->addAction(p.actions["Presentation"]);
            p.menu->addAction(p.actions["FloatOnTop"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["Maximize"]);
            p.menu->addAction(p.actions["Fit"]);

            _actionsUpdate();
            _textUpdate();
            _shortcutsUpdate();

            auto weak = std::weak_ptr<WindowSystem>(std::dynamic_pointer_cast<WindowSystem>(shared_from_this()));
            p.actions["FullScreen"]->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->setFullScreen(value);
                    }
                });

            p.actions["Presentation"]->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->setPresentation(value);
                    }
                });

            p.actions["FloatOnTop"]->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->setFloatOnTop(value);
                    }
                });

            p.actions["Maximize"]->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->setMaximize(value);
                    }
                });

            p.actions["Fit"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        if (auto widget = system->_p->activeWidget->get())
                        {
                            widget->fitWindow();
                        }
                    }
                });

            p.fullScreenObserver = ValueObserver<bool>::create(
                p.settings->observeFullScreen(),
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->setFullScreen(value);
                    }
                });

            p.floatOnTopObserver = ValueObserver<bool>::create(
                p.settings->observeFloatOnTop(),
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->setFloatOnTop(value);
                    }
                });

            p.maximizeObserver = ValueObserver<bool>::create(
                p.settings->observeMaximize(),
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->setMaximize(value);
                    }
                });
        }

        WindowSystem::WindowSystem() :
            _p(new Private)
        {}

        WindowSystem::~WindowSystem()
        {
            DJV_PRIVATE_PTR();
            auto glfwWindow = p.glGLFWSystem->getGLFWWindow();
            if (!p.fullScreen->get())
            {
                glm::ivec2 pos(0.F, 0.F);
                glfwGetWindowPos(glfwWindow, &pos.x, &pos.y);
                p.settings->setWindowPos(pos);
                glm::ivec2 size(0.F, 0.F);
                glfwGetWindowSize(glfwWindow, &size.x, &size.y);
                p.settings->setWindowSize(size);
            }
        }

        std::shared_ptr<WindowSystem> WindowSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<WindowSystem>(new WindowSystem);
            out->_init(context);
            return out;
        }

        void WindowSystem::setMediaCanvas(const std::shared_ptr<MediaCanvas>& value)
        {
            DJV_PRIVATE_PTR();
            if (auto canvas = p.canvas.lock())
            {
                canvas->setActiveCallback(nullptr);
            }
            p.canvas = value;
            auto weak = std::weak_ptr<WindowSystem>(std::dynamic_pointer_cast<WindowSystem>(shared_from_this()));
            if (auto canvas = p.canvas.lock())
            {
                canvas->setActiveCallback(
                    [weak](const std::shared_ptr<MediaWidget>& value)
                    {
                        if (auto system = weak.lock())
                        {
                            if (system->_p->activeWidget->setIfChanged(value))
                            {
                                system->_actionsUpdate();
                            }
                        }
                    });
            }
        }

        std::shared_ptr<IValueSubject<std::shared_ptr<MediaWidget> > > WindowSystem::observeActiveWidget() const
        {
            return _p->activeWidget;
        }

        std::shared_ptr<IValueSubject<bool> > WindowSystem::observeFullScreen() const
        {
            return _p->fullScreen;
        }

        void WindowSystem::setFullScreen(bool value)
        {
            DJV_PRIVATE_PTR();
            if (p.fullScreen->setIfChanged(value))
            {
                p.setFullScreen(value);
                p.settings->setFullScreen(value);
                _actionsUpdate();
            }
        }

        std::shared_ptr<IValueSubject<bool> > WindowSystem::observePresentation() const
        {
            return _p->presentation;
        }

        void WindowSystem::setPresentation(bool value)
        {
            DJV_PRIVATE_PTR();
            if (p.presentation->setIfChanged(value))
            {
                if (value)
                {
                    p.desktopGLFWSystem->hideCursor();
                }
                else
                {
                    p.desktopGLFWSystem->showCursor();
                }
                _actionsUpdate();
            }
        }

        std::shared_ptr<IValueSubject<bool> > WindowSystem::observeFloatOnTop() const
        {
            return _p->floatOnTop;
        }

        void WindowSystem::setFloatOnTop(bool value)
        {
            DJV_PRIVATE_PTR();
            if (p.floatOnTop->setIfChanged(value))
            {
                p.setFloatOnTop(value);
                p.settings->setFloatOnTop(value);
                _actionsUpdate();
            }
        }

        std::shared_ptr<IValueSubject<bool> > WindowSystem::observeMaximize() const
        {
            return _p->maximize;
        }

        void WindowSystem::setMaximize(bool value)
        {
            DJV_PRIVATE_PTR();
            if (p.maximize->setIfChanged(value))
            {
                p.settings->setMaximize(value);
                _actionsUpdate();
            }
        }

        std::map<std::string, std::shared_ptr<UI::Action> > WindowSystem::getActions() const
        {
            return _p->actions;
        }

        MenuData WindowSystem::getMenu() const
        {
            return
            {
                _p->menu,
                "C"
            };
        }

        void WindowSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["FullScreen"]->setText(_getText(DJV_TEXT("menu_window_full_screen")));
                p.actions["FullScreen"]->setTooltip(_getText(DJV_TEXT("menu_window_full_screen_tooltip")));
                p.actions["Presentation"]->setText(_getText(DJV_TEXT("menu_window_presentation")));
                p.actions["Presentation"]->setTooltip(_getText(DJV_TEXT("menu_window_presentation_tooltip")));
                p.actions["FloatOnTop"]->setText(_getText(DJV_TEXT("menu_window_float_on_top")));
                p.actions["FloatOnTop"]->setTooltip(_getText(DJV_TEXT("menu_window_float_on_top_tooltip")));
                p.actions["Maximize"]->setText(_getText(DJV_TEXT("menu_window_maximize")));
                p.actions["Maximize"]->setTooltip(_getText(DJV_TEXT("menu_window_maximize_tooltip")));
                p.actions["Fit"]->setText(_getText(DJV_TEXT("menu_window_fit")));
                p.actions["Fit"]->setTooltip(_getText(DJV_TEXT("menu_window_fit_tooltip")));

                p.menu->setText(_getText(DJV_TEXT("menu_window")));
            }
        }

        void WindowSystem::_shortcutsUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["FullScreen"]->setShortcuts(_getShortcuts("shortcut_window_full_screen"));
                p.actions["Presentation"]->setShortcuts(_getShortcuts("shortcut_window_presentation"));
                p.actions["Maximize"]->setShortcuts(_getShortcuts("shortcut_window_maximize"));
                p.actions["Fit"]->setShortcuts(_getShortcuts("shortcut_window_fit"));
            }
        }

        void WindowSystem::_actionsUpdate()
        {
            DJV_PRIVATE_PTR();
            p.actions["FullScreen"]->setChecked(p.fullScreen->get());
            p.actions["Presentation"]->setChecked(p.presentation->get());
            p.actions["FloatOnTop"]->setChecked(p.floatOnTop->get());
            p.actions["Maximize"]->setChecked(p.maximize->get());
        }

        void WindowSystem::Private::setFullScreen(bool value)
        {
            auto glfwWindow = glGLFWSystem->getGLFWWindow();
            auto glfwMonitor = glfwGetWindowMonitor(glfwWindow);
            if (value && glfwWindow && !glfwMonitor)
            {
                int monitor = settings->observeFullScreenMonitor()->get();
                int monitorsCount = 0;
                GLFWmonitor** monitors = glfwGetMonitors(&monitorsCount);
                if (monitor >= 0 && monitor < monitorsCount)
                {
                    glfwMonitor = monitors[monitor];
                }
                else
                {
                    glfwMonitor = glfwGetPrimaryMonitor();
                }
                auto glfwMonitorMode = glfwGetVideoMode(glfwMonitor);
                monitorSize.x = glfwMonitorMode->width;
                monitorSize.y = glfwMonitorMode->height;
                monitorRefresh = glfwMonitorMode->refreshRate;

                int x = 0;
                int y = 0;
                int w = 0;
                int h = 0;
                glfwGetWindowPos(glfwWindow, &x, &y);
                glfwGetWindowSize(glfwWindow, &w, &h);
                windowGeom = Math::BBox2i(x, y, w, h);

                glfwSetWindowAttrib(glfwWindow, GLFW_AUTO_ICONIFY, glfwMonitor == glfwGetPrimaryMonitor());
                glfwSetWindowMonitor(
                    glfwWindow,
                    glfwMonitor,
                    0,
                    0,
                    glfwMonitorMode->width,
                    glfwMonitorMode->height,
                    glfwMonitorMode->refreshRate);
                glfwFocusWindow(glfwWindow);
            }
            else if (!value && glfwMonitor)
            {
                glfwSetWindowMonitor(
                    glfwWindow,
                    NULL,
                    windowGeom.x(),
                    windowGeom.y(),
                    windowGeom.w(),
                    windowGeom.h(),
                    0);
                glfwFocusWindow(glfwWindow);

                //! \bug Why is it neccessary to resize the window to get the correct geometry
                //! when restoring full screen from a different monitor?
                glfwSetWindowSize(
                    glfwWindow,
                    static_cast<int>(windowGeom.w()),
                    static_cast<int>(windowGeom.h()));
            }
        }

        void WindowSystem::Private::setFloatOnTop(bool value)
        {
            if (auto glfwWindow = glGLFWSystem->getGLFWWindow())
            {
                glfwSetWindowAttrib(glfwWindow, GLFW_FLOATING, value);
            }
        }

    } // namespace ViewApp
} // namespace djv

