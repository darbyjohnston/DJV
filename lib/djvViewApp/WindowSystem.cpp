//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

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
#include <djvUI/Shortcut.h>
#include <djvUI/UISystem.h>

#include <djvAV/AVSystem.h>
#include <djvAV/GLFWSystem.h>

#include <djvCore/Animation.h>
#include <djvCore/Context.h>
#include <djvCore/IEventSystem.h>
#include <djvCore/TextSystem.h>
#include <djvCore/Timer.h>

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
            std::shared_ptr<AV::GLFW::System> glfwSystem;
            
            std::shared_ptr<WindowSettings> settings;
            std::weak_ptr<MediaCanvas> canvas;
            std::shared_ptr<ValueSubject<std::shared_ptr<MediaWidget> > > activeWidget;
            std::shared_ptr<ValueSubject<bool> > fullScreen;
            std::shared_ptr<ValueSubject<bool> > maximize;
            std::shared_ptr<ValueSubject<float> > fade;
            bool fadeEnabled = false;
            Event::PointerInfo pointerInfo;
            std::map<Event::PointerID, glm::vec2> pointerMotion;
            std::shared_ptr<Time::Timer> pointerMotionTimer;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::Menu> menu;
            glm::ivec2 monitorSize = glm::ivec2(0, 0);
            int monitorRefresh = 0;
            BBox2i windowGeom = BBox2i(0, 0, 0, 0);
            
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > actionObservers;
            std::shared_ptr<ValueObserver<bool> > maximizeObserver;
            std::shared_ptr<ValueObserver<Event::PointerInfo> > pointerObserver;
            std::shared_ptr<ValueObserver<bool> > fadeObserver;

            std::shared_ptr<Animation::Animation> fadeAnimation;

            void setFullScreen(bool, const std::shared_ptr<Context>& context);
        };

        void WindowSystem::_init(const std::shared_ptr<Context>& context)
        {
            IViewSystem::_init("djv::ViewApp::WindowSystem", context);

            DJV_PRIVATE_PTR();

            p.glfwSystem = context->getSystemT<AV::GLFW::System>();

            p.settings = WindowSettings::create(context);

            auto glfwWindow = p.glfwSystem->getGLFWWindow();
            const glm::ivec2& windowSize = p.settings->getWindowSize();
            glfwSetWindowSize(
                glfwWindow,
                std::max(windowSize.x, windowSizeMin.x),
                std::max(windowSize.y, windowSizeMin.y));

            p.activeWidget = ValueSubject<std::shared_ptr<MediaWidget> >::create();
            p.fullScreen = ValueSubject<bool>::create(false);
            p.maximize = ValueSubject<bool>::create(false);
            p.fade = ValueSubject<float>::create(1.F);
            p.pointerMotionTimer = Time::Timer::create(context);
            p.fadeAnimation = Animation::Animation::create(context);

            p.actions["FullScreen"] = UI::Action::create();
            p.actions["FullScreen"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["FullScreen"]->setIcon("djvIconWindowFullScreen");
            p.actions["FullScreen"]->setShortcut(GLFW_KEY_U);

            p.actions["Maximize"] = UI::Action::create();
            p.actions["Maximize"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Maximize"]->setIcon("djvIconMDI");
            p.actions["Maximize"]->setCheckedIcon("djvIconSDI");
            p.actions["Maximize"]->setShortcut(GLFW_KEY_M);

            p.actions["Fit"] = UI::Action::create();
            p.actions["Fit"]->setShortcut(GLFW_KEY_F, UI::Shortcut::getSystemModifier());

            p.actions["AutoHide"] = UI::Action::create();
            p.actions["AutoHide"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["AutoHide"]->setIcon("djvIconVisible");
            p.actions["AutoHide"]->setCheckedIcon("djvIconHidden");
            p.actions["AutoHide"]->setShortcut(GLFW_KEY_H, UI::Shortcut::getSystemModifier());

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["FullScreen"]);
            p.menu->addAction(p.actions["Maximize"]);
            p.menu->addAction(p.actions["Fit"]);
            p.menu->addAction(p.actions["AutoHide"]);

            _actionsUpdate();
            _textUpdate();

            auto weak = std::weak_ptr<WindowSystem>(std::dynamic_pointer_cast<WindowSystem>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.actionObservers["FullScreen"] = ValueObserver<bool>::create(
                p.actions["FullScreen"]->observeChecked(),
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto system = weak.lock())
                        {
                            system->setFullScreen(value);
                        }
                    }
                });

            p.actionObservers["Maximize"] = ValueObserver<bool>::create(
                p.actions["Maximize"]->observeChecked(),
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->setMaximize(value);
                    }
                });

            p.actionObservers["Fit"] = ValueObserver<bool>::create(
                p.actions["Fit"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            if (auto widget = system->_p->activeWidget->get())
                            {
                                widget->fitWindow();
                            }
                        }
                    }
                });

            p.fadeObserver = ValueObserver<bool>::create(
                p.settings->observeAutoHide(),
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        if (!value)
                        {
                            system->_p->fade->setIfChanged(1.F);
                        }
                        system->_p->fadeEnabled = value;
                        system->_p->fadeAnimation->stop();
                        system->_p->pointerMotion.clear();
                        system->_p->pointerMotionTimer->stop();
                        system->_pointerUpdate();
                        system->_actionsUpdate();
                    }
                });

            p.actionObservers["AutoHide"] = ValueObserver<bool>::create(
                p.actions["AutoHide"]->observeChecked(),
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->settings->setAutoHide(value);
                        }
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

            auto eventSystem = context->getSystemT<Event::IEventSystem>();
            p.pointerObserver = ValueObserver<Event::PointerInfo>::create(
                eventSystem->observePointer(),
                [weak](const Event::PointerInfo & value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->pointerInfo = value;
                    system->_pointerUpdate();
                }
            });
        }

        WindowSystem::WindowSystem() :
            _p(new Private)
        {}

        WindowSystem::~WindowSystem()
        {
            DJV_PRIVATE_PTR();
            auto glfwWindow = p.glfwSystem->getGLFWWindow();
            glm::ivec2 windowSize = p.settings->getWindowSize();
            glfwGetWindowSize(glfwWindow, &windowSize.x, &windowSize.y);
            p.settings->setWindowSize(windowSize);
        }

        std::shared_ptr<WindowSystem> WindowSystem::create(const std::shared_ptr<Context>& context)
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
                if (auto context = getContext().lock())
                {
                    p.setFullScreen(value, context);
                    _actionsUpdate();
                }
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

        std::shared_ptr<IValueSubject<float> > WindowSystem::observeFade() const
        {
            return _p->fade;
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
                "B"
            };
        }

        void WindowSystem::_pointerUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                bool start = false;
                auto weak = std::weak_ptr<WindowSystem>(std::dynamic_pointer_cast<WindowSystem>(shared_from_this()));
                const auto j = p.pointerMotion.find(p.pointerInfo.id);
                if (j != p.pointerMotion.end())
                {
                    const float diff = glm::length(p.pointerInfo.projectedPos - j->second);
                    auto uiSystem = context->getSystemT<UI::UISystem>();
                    auto style = uiSystem->getStyle();
                    const float h = style->getMetric(UI::MetricsRole::Handle);
                    if (diff > h)
                    {
                        start = true;
                        p.pointerMotion[p.pointerInfo.id] = p.pointerInfo.projectedPos;
                        if (auto glfwSystem = context->getSystemT<Desktop::GLFWSystem>())
                        {
                            if (!glfwSystem->isCursorVisible())
                            {
                                glfwSystem->showCursor();
                            }
                        }
                        const float fade = p.fade->get();
                        if (fade < 1.F)
                        {
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
                }
                else
                {
                    start = true;
                    p.pointerMotion[p.pointerInfo.id] = p.pointerInfo.projectedPos;
                }
                if (start && p.fadeEnabled)
                {
                    p.pointerMotionTimer->start(
                        std::chrono::milliseconds(fadeTimeout),
                        [weak](const std::chrono::steady_clock::time_point&, const Time::Duration&)
                        {
                            if (auto system = weak.lock())
                            {
                                system->_p->fadeAnimation->start(
                                    system->_p->fade->get(),
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
                                                if (auto glfwSystem = context->getSystemT<Desktop::GLFWSystem>())
                                                {
                                                    glfwSystem->hideCursor();
                                                }
                                            }
                                        }
                                    });
                            }
                        });
                }
            }
        }

        void WindowSystem::_actionsUpdate()
        {
            DJV_PRIVATE_PTR();
            p.actions["FullScreen"]->setChecked(p.fullScreen->get());
            p.actions["Maximize"]->setChecked(p.maximize->get());
            p.actions["AutoHide"]->setChecked(p.fadeEnabled);
        }

        void WindowSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["FullScreen"]->setText(_getText(DJV_TEXT("menu_window_full_screen")));
                p.actions["FullScreen"]->setTooltip(_getText(DJV_TEXT("menu_window_full_screen_tooltip")));
                p.actions["Maximize"]->setText(_getText(DJV_TEXT("menu_window_maximize")));
                p.actions["Maximize"]->setTooltip(_getText(DJV_TEXT("menu_window_maximize_tooltip")));
                p.actions["Fit"]->setText(_getText(DJV_TEXT("menu_window_fit")));
                p.actions["Fit"]->setTooltip(_getText(DJV_TEXT("menu_window_fit_tooltip")));
                p.actions["AutoHide"]->setText(_getText(DJV_TEXT("menu_window_auto_hide_interface")));
                p.actions["AutoHide"]->setTooltip(_getText(DJV_TEXT("menu_window_auto_hide_tooltip")));

                p.menu->setText(_getText(DJV_TEXT("menu_window")));
            }
        }

        void WindowSystem::Private::setFullScreen(bool value, const std::shared_ptr<Context>& context)
        {
            auto avGLFWSystem = context->getSystemT<AV::GLFW::System>();
            auto glfwWindow = avGLFWSystem->getGLFWWindow();
            auto glfwMonitor = glfwGetWindowMonitor(glfwWindow);
            if (value && !glfwMonitor)
            {
                int monitor = settings->observeFullscreenMonitor()->get();
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
                windowGeom = BBox2i(x, y, w, h);

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

    } // namespace ViewApp
} // namespace djv

