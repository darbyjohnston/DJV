//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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
#include <djvViewApp/UISettings.h>
#include <djvViewApp/WindowSettings.h>

#include <djvDesktopApp/Application.h>
#include <djvDesktopApp/GLFWSystem.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/UISystem.h>

#include <djvAV/AVSystem.h>

#include <djvCore/Animation.h>
#include <djvCore/Context.h>
#include <djvCore/IEventSystem.h>
#include <djvCore/TextSystem.h>
#include <djvCore/Timer.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            //! \todo Should this be configurable?
            const size_t fadeTimeout = 3000;
            const size_t fadeInTime  = 200;
            const size_t fadeOutTime = 1000;
        }

        struct WindowSystem::Private
        {
            std::shared_ptr<WindowSettings> settings;
            std::weak_ptr<MediaCanvas> canvas;
            std::shared_ptr<ValueSubject<std::shared_ptr<MediaWidget> > > activeWidget;
            std::shared_ptr<ValueSubject<bool> > maximized;
            std::shared_ptr<ValueSubject<float> > fade;
            bool fadeEnabled = true;
            std::map<Core::Event::PointerID, glm::vec2> pointerMotion;
            std::shared_ptr<Time::Timer> pointerMotionTimer;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::Menu> menu;
            glm::ivec2 monitorSize = glm::ivec2(0, 0);
            int monitorRefresh = 0;
            BBox2i windowGeom = BBox2i(0, 0, 0, 0);
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > actionObservers;
            std::shared_ptr<ValueObserver<bool> > fullScreenObserver;
            std::shared_ptr<ValueObserver<bool> > maximizedObserver;
            std::shared_ptr<ValueObserver<bool> > maximizedObserver2;
            std::shared_ptr<ValueObserver<Event::PointerInfo> > pointerObserver;
            std::shared_ptr<ValueObserver<bool> > fadeObserver;
            std::shared_ptr<ValueObserver<std::string> > localeObserver;

            std::shared_ptr<Animation::Animation> fadeAnimation;

            void setFullScreen(bool, Context * context);
        };

        void WindowSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewApp::WindowSystem", context);

            DJV_PRIVATE_PTR();

            p.settings = WindowSettings::create(context);
            p.activeWidget = ValueSubject<std::shared_ptr<MediaWidget> >::create();
            p.maximized = ValueSubject<bool>::create();
            p.fade = ValueSubject<float>::create(1.f);
            p.pointerMotionTimer = Time::Timer::create(context);
            p.fadeAnimation = Animation::Animation::create(context);

            p.actions["FullScreen"] = UI::Action::create();
            p.actions["FullScreen"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["FullScreen"]->setIcon("djvIconWindowFullScreen");
            p.actions["FullScreen"]->setShortcut(GLFW_KEY_U);

            p.actions["Maximized"] = UI::Action::create();
            p.actions["Maximized"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Maximized"]->setIcon("djvIconMDI");
            p.actions["Maximized"]->setCheckedIcon("djvIconSDI");
            p.actions["Maximized"]->setShortcut(GLFW_KEY_M);

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["FullScreen"]);
            p.menu->addAction(p.actions["Maximized"]);

            _actionsUpdate();

            auto weak = std::weak_ptr<WindowSystem>(std::dynamic_pointer_cast<WindowSystem>(shared_from_this()));
            p.fullScreenObserver = ValueObserver<bool>::create(
                p.actions["FullScreen"]->observeChecked(),
                [weak, context](bool value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->setFullScreen(value, context);
                }
            });

            p.maximizedObserver = ValueObserver<bool>::create(
                p.actions["Maximized"]->observeChecked(),
                [weak](bool value)
            {
                if (auto system = weak.lock())
                {
                    system->setMaximized(value);
                }
            });

            p.maximizedObserver2 = ValueObserver<bool>::create(
                p.settings->observeMaximized(),
                [weak](bool value)
            {
                if (auto system = weak.lock())
                {
                    system->setMaximized(value);
                }
            });

            auto eventSystem = context->getSystemT<Event::IEventSystem>();
            p.pointerObserver = ValueObserver<Event::PointerInfo>::create(
                eventSystem->observePointer(),
                [weak](const Event::PointerInfo & value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_pointerUpdate(value);
                }
            });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            auto uiSettings = settingsSystem->getSettingsT<UISettings>();
            p.fadeObserver = ValueObserver<bool>::create(
                uiSettings->observeAutoHide(),
                [weak](bool value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->fadeEnabled = value;
                    if (!value)
                    {
                        system->_p->fadeAnimation->stop();
                        system->_p->pointerMotionTimer->stop();
                        system->_p->fade->setIfChanged(1.f);
                    }
                }
            });

            p.localeObserver = ValueObserver<std::string>::create(
                context->getSystemT<TextSystem>()->observeCurrentLocale(),
                [weak](const std::string & value)
            {
                if (auto system = weak.lock())
                {
                    system->_textUpdate();
                }
            });
        }

        WindowSystem::WindowSystem() :
            _p(new Private)
        {}

        WindowSystem::~WindowSystem()
        {}

        std::shared_ptr<WindowSystem> WindowSystem::create(Context * context)
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

        std::shared_ptr<Core::IValueSubject<std::shared_ptr<MediaWidget> > > WindowSystem::observeActiveWidget() const
        {
            return _p->activeWidget;
        }

        std::shared_ptr<Core::IValueSubject<bool> > WindowSystem::observeMaximized() const
        {
            return _p->maximized;
        }

        void WindowSystem::setMaximized(bool value)
        {
            DJV_PRIVATE_PTR();
            if (p.maximized->setIfChanged(value))
            {
                p.settings->setMaximized(value);
                _actionsUpdate();
            }
        }

        std::shared_ptr<Core::IValueSubject<float> > WindowSystem::observeFade() const
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

        void WindowSystem::_pointerUpdate(const Core::Event::PointerInfo& info)
        {
            DJV_PRIVATE_PTR();
            bool start = false;
            auto weak = std::weak_ptr<WindowSystem>(std::dynamic_pointer_cast<WindowSystem>(shared_from_this()));
            const auto j = p.pointerMotion.find(info.id);
            if (j != p.pointerMotion.end())
            {
                const float diff = glm::length(info.projectedPos - j->second);
                auto context = getContext();
                auto uiSystem = context->getSystemT<UI::UISystem>();
                auto style = uiSystem->getStyle();
                const float h = style->getMetric(UI::MetricsRole::Handle);
                if (diff > h)
                {
                    start = true;
                    p.pointerMotion[info.id] = info.projectedPos;
                    if (p.fadeEnabled)
                    {
                        if (auto glfwSystem = context->getSystemT<Desktop::GLFWSystem>())
                        {
                            glfwSystem->showCursor();
                        }
                        p.fadeAnimation->start(
                            p.fade->get(),
                            1.f,
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
                p.pointerMotion[info.id] = info.projectedPos;
            }
            if (start && p.fadeEnabled)
            {
                p.pointerMotionTimer->start(
                    std::chrono::milliseconds(fadeTimeout),
                    [weak](float value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->fadeAnimation->start(
                            system->_p->fade->get(),
                            0.f,
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
                                system->_p->fade->setIfChanged(value);
                                if (auto glfwSystem = system->getContext()->getSystemT<Desktop::GLFWSystem>())
                                {
                                    glfwSystem->hideCursor();
                                }
                            }
                        });
                    }
                });
            }
        }

        void WindowSystem::_actionsUpdate()
        {
            DJV_PRIVATE_PTR();
            p.actions["Maximized"]->setChecked(static_cast<int>(p.maximized->get()));
        }

        void WindowSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            p.actions["FullScreen"]->setText(_getText(DJV_TEXT("Full Screen")));
            p.actions["FullScreen"]->setTooltip(_getText(DJV_TEXT("Full screen tooltip")));
            p.actions["Maximized"]->setText(_getText(DJV_TEXT("Maximized")));
            p.actions["Maximized"]->setTooltip(_getText(DJV_TEXT("Maximized tooltip")));

            p.menu->setText(_getText(DJV_TEXT("Window")));
        }

        void WindowSystem::Private::setFullScreen(bool value, Context * context)
        {
            auto glfwSystem = context->getSystemT<Desktop::GLFWSystem>();
            auto glfwWindow = glfwSystem->getGLFWWindow();
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

                if (glfwMonitor != glfwGetPrimaryMonitor())
                {
                    glfwSetWindowAttrib(glfwWindow, GLFW_AUTO_ICONIFY, GL_FALSE);
                }

                int x = 0;
                int y = 0;
                int w = 0;
                int h = 0;
                glfwGetWindowPos(glfwWindow, &x, &y);
                glfwGetWindowSize(glfwWindow, &w, &h);
                windowGeom = BBox2i(x, y, w, h);

                glfwSetWindowMonitor(
                    glfwWindow,
                    glfwMonitor,
                    0,
                    0,
                    glfwMonitorMode->width,
                    glfwMonitorMode->height,
                    glfwMonitorMode->refreshRate);
            }
            else if (!value && glfwMonitor)
            {
                glfwSetWindowAttrib(glfwWindow, GLFW_AUTO_ICONIFY, GL_TRUE);

                glfwSetWindowMonitor(
                    glfwWindow,
                    NULL,
                    windowGeom.x(),
                    windowGeom.y(),
                    windowGeom.w(),
                    windowGeom.h(),
                    0);
            }
        }

    } // namespace ViewApp
} // namespace djv

