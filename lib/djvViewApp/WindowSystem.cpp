// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/WindowSystem.h>

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

#include <djvSystem/Animation.h>
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
            const size_t     fadeTimeout   = 3000;
            const size_t     fadeInTime    = 200;
            const size_t     fadeOutTime   = 1000;
        }

        struct WindowSystem::Private
        {
            std::shared_ptr<GL::GLFW::GLFWSystem> glGLFWSystem;
            std::shared_ptr<Desktop::GLFWSystem> desktopGLFWSystem;

            std::shared_ptr<WindowSettings> settings;
            std::shared_ptr<Observer::ValueSubject<std::shared_ptr<MediaWidget> > > activeWidget;
            std::shared_ptr<Observer::ValueSubject<bool> > fullScreen;
            std::shared_ptr<Observer::ValueSubject<bool> > presentation;
            std::shared_ptr<Observer::ValueSubject<bool> > floatOnTop;
            std::shared_ptr<Observer::ValueSubject<float> > fade;
            std::shared_ptr<System::Timer> fadeTimer;
            std::map<System::Event::PointerID, glm::vec2> pointerMotion;
            bool textFocusActive = false;
            bool textFocusInit = false;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::Menu> menu;
            glm::ivec2 monitorSize = glm::ivec2(0, 0);
            int monitorRefresh = 0;
            Math::BBox2i windowGeom = Math::BBox2i(0, 0, 0, 0);

            std::shared_ptr<Observer::Value<bool> > fullScreenObserver;
            std::shared_ptr<Observer::Value<bool> > floatOnTopObserver;
            std::shared_ptr<Observer::Value<System::Event::PointerInfo> > pointerObserver;
            std::shared_ptr<Observer::Value<bool> > textFocusActiveObserver;

            std::shared_ptr<System::Animation::Animation> fadeAnimation;

            void setFullScreen(bool);
            void setFloatOnTop(bool);
        };

        void WindowSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            IViewAppSystem::_init("djv::ViewApp::WindowSystem", context);

            DJV_PRIVATE_PTR();

            p.glGLFWSystem = context->getSystemT<GL::GLFW::GLFWSystem>();
            p.desktopGLFWSystem = context->getSystemT<Desktop::GLFWSystem>();

            p.settings = WindowSettings::create(context);

            auto glfwWindow = p.glGLFWSystem->getWindow();
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

            p.activeWidget = Observer::ValueSubject<std::shared_ptr<MediaWidget> >::create();
            p.fullScreen = Observer::ValueSubject<bool>::create(false);
            p.presentation = Observer::ValueSubject<bool>::create(false);
            p.floatOnTop = Observer::ValueSubject<bool>::create(false);
            p.fade = Observer::ValueSubject<float>::create(1.F);
            p.fadeTimer = System::Timer::create(context);
            p.fadeAnimation = System::Animation::Animation::create(context);

            p.actions["FullScreen"] = UI::Action::create();
            p.actions["FullScreen"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["FullScreen"]->setIcon("djvIconWindowFullScreen");
            p.actions["Presentation"] = UI::Action::create();
            p.actions["Presentation"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["FloatOnTop"] = UI::Action::create();
            p.actions["FloatOnTop"]->setButtonType(UI::ButtonType::Toggle);

            _addShortcut(DJV_TEXT("shortcut_window_full_screen"), GLFW_KEY_U);
            _addShortcut(DJV_TEXT("shortcut_window_presentation"), GLFW_KEY_E);
            _addShortcut(DJV_TEXT("shortcut_window_float_on_top"), 0);

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["FullScreen"]);
            p.menu->addAction(p.actions["Presentation"]);
            p.menu->addAction(p.actions["FloatOnTop"]);

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

            p.fullScreenObserver = Observer::Value<bool>::create(
                p.settings->observeFullScreen(),
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->setFullScreen(value);
                    }
                });

            p.floatOnTopObserver = Observer::Value<bool>::create(
                p.settings->observeFloatOnTop(),
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->setFloatOnTop(value);
                    }
                });

            auto eventSystem = context->getSystemT<System::Event::IEventSystem>();
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.pointerObserver = Observer::Value<System::Event::PointerInfo>::create(
                eventSystem->observePointer(),
                [weak, contextWeak](const System::Event::PointerInfo& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto system = weak.lock())
                        {
                            const auto i = system->_p->pointerMotion.find(value.id);
                            if (i != system->_p->pointerMotion.end())
                            {
                                system->_fadeStop();
                                if (system->_p->presentation->get() && !system->_p->textFocusActive)
                                {
                                    system->_p->fadeTimer->start(
                                        std::chrono::milliseconds(fadeTimeout),
                                        [weak](const std::chrono::steady_clock::time_point&, const Time::Duration&)
                                        {
                                            if (auto system = weak.lock())
                                            {
                                                system->_fadeStart();
                                            }
                                        });
                                }
                            }
                            system->_p->pointerMotion[value.id] = value.projectedPos;
                        }
                    }
                });

            p.textFocusActiveObserver = Observer::Value<bool>::create(
                eventSystem->observeTextFocusActive(),
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->textFocusActive = value;
                        if (value)
                        {
                            system->_p->textFocusInit = true;
                            system->_fadeStop();
                        }
                        else if (system->_p->presentation->get() && system->_p->textFocusInit)
                        {
                            system->_p->fadeTimer->start(
                                std::chrono::milliseconds(fadeTimeout),
                                [weak](const std::chrono::steady_clock::time_point&, const Time::Duration&)
                                {
                                    if (auto system = weak.lock())
                                    {
                                        system->_fadeStart();
                                    }
                                });
                        }
                    }
                });

            _logInitTime();
        }

        WindowSystem::WindowSystem() :
            _p(new Private)
        {}

        WindowSystem::~WindowSystem()
        {
            DJV_PRIVATE_PTR();
            auto glfwWindow = p.glGLFWSystem->getWindow();
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
            auto out = context->getSystemT<WindowSystem>();
            if (!out)
            {
                out = std::shared_ptr<WindowSystem>(new WindowSystem);
                out->_init(context);
            }
            return out;
        }

        std::shared_ptr<Observer::IValueSubject<std::shared_ptr<MediaWidget> > > WindowSystem::observeActiveWidget() const
        {
            return _p->activeWidget;
        }

        void WindowSystem::setActiveWidget(const std::shared_ptr<MediaWidget>& value)
        {
            _p->activeWidget->setIfChanged(value);
        }

        std::shared_ptr<Observer::IValueSubject<bool> > WindowSystem::observeFullScreen() const
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

        std::shared_ptr<Observer::IValueSubject<bool> > WindowSystem::observePresentation() const
        {
            return _p->presentation;
        }

        void WindowSystem::setPresentation(bool value)
        {
            DJV_PRIVATE_PTR();
            if (p.presentation->setIfChanged(value))
            {
                _presentationUpdate();
            }
        }

        std::shared_ptr<Observer::IValueSubject<bool> > WindowSystem::observeFloatOnTop() const
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

        std::shared_ptr<Observer::IValueSubject<float> > WindowSystem::observeFade() const
        {
            return _p->fade;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > WindowSystem::getActions() const
        {
            return _p->actions;
        }

        MenuData WindowSystem::getMenuData() const
        {
            return
            {
                { _p->menu },
                3
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
                p.actions["FloatOnTop"]->setShortcuts(_getShortcuts("shortcut_window_float_on_top"));
            }
        }

        void WindowSystem::_fadeStart()
        {
            DJV_PRIVATE_PTR();
            auto weak = std::weak_ptr<WindowSystem>(std::dynamic_pointer_cast<WindowSystem>(shared_from_this()));
            p.fadeAnimation->start(
                p.fade->get(),
                0.F,
                std::chrono::milliseconds(fadeOutTime),
                [weak](float value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->fade->setIfChanged(value);
                    }
                },
                [weak](float value)
                {
                    if (auto system = weak.lock())
                    {
                        if (auto context = system->getContext().lock())
                        {
                            system->_p->fade->setIfChanged(value);
                            system->_p->desktopGLFWSystem->hideCursor();
                        }
                    }
                });
        }

        void WindowSystem::_fadeStop()
        {
            DJV_PRIVATE_PTR();
            p.fadeTimer->stop();
            if (!p.desktopGLFWSystem->isCursorVisible())
            {
                p.desktopGLFWSystem->showCursor();
            }
            const float fade = p.fade->get();
            if (fade < 1.F)
            {
                auto weak = std::weak_ptr<WindowSystem>(std::dynamic_pointer_cast<WindowSystem>(shared_from_this()));
                p.fadeAnimation->start(
                    p.fade->get(),
                    1.F,
                    std::chrono::milliseconds(fadeInTime),
                    [weak](float value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->fade->setIfChanged(value);
                        }
                    },
                    [weak](float value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->fade->setIfChanged(value);
                        }
                    });
            }
        }

        void WindowSystem::_presentationUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.presentation->get() && !p.textFocusActive)
            {
                auto weak = std::weak_ptr<WindowSystem>(std::dynamic_pointer_cast<WindowSystem>(shared_from_this()));
                p.fadeTimer->start(
                    std::chrono::milliseconds(fadeTimeout),
                    [weak](const std::chrono::steady_clock::time_point&, const Time::Duration&)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_fadeStart();
                        }
                    });
            }
            else
            {
                _fadeStop();
            }
            _actionsUpdate();
        }

        void WindowSystem::_actionsUpdate()
        {
            DJV_PRIVATE_PTR();
            p.actions["FullScreen"]->setChecked(p.fullScreen->get());
            p.actions["Presentation"]->setChecked(p.presentation->get());
            p.actions["FloatOnTop"]->setChecked(p.floatOnTop->get());
        }

        void WindowSystem::Private::setFullScreen(bool value)
        {
            auto glfwWindow = glGLFWSystem->getWindow();
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
            if (auto glfwWindow = glGLFWSystem->getWindow())
            {
                glfwSetWindowAttrib(glfwWindow, GLFW_FLOATING, value);
            }
        }

    } // namespace ViewApp
} // namespace djv

