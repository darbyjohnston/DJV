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
            //! \todo [1.0 S] Should this be configurable?
            const size_t fadeSeconds = 3;
        }

        struct WindowSystem::Private
        {
            std::shared_ptr<WindowSettings> settings;
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
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
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
            p.maximized = ValueSubject<bool>::create();
            p.fade = ValueSubject<float>::create(1.f);
            p.pointerMotionTimer = Time::Timer::create(context);
            p.fadeAnimation = Animation::Animation::create(context);

            //! \todo Implement me!
            p.actions["Fit"] = UI::Action::create();
            p.actions["Fit"]->setIcon("djvIconWindowFit");
            p.actions["Fit"]->setShortcut(GLFW_KEY_F);
            p.actions["Fit"]->setEnabled(false);

            p.actions["FullScreen"] = UI::Action::create();
            p.actions["FullScreen"]->setIcon("djvIconWindowFullScreen");
            p.actions["FullScreen"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["FullScreen"]->setShortcut(GLFW_KEY_U);

            p.actions["Maximized"] = UI::Action::create();
            p.actions["Maximized"]->setIcon("djvIconViewLibSDI");
            p.actions["Maximized"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Maximized"]->setShortcut(GLFW_KEY_M);

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["Fit"]);
            p.menu->addAction(p.actions["FullScreen"]);
            p.menu->addAction(p.actions["Maximized"]);

            _actionUpdate();

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
            auto windowSettings = settingsSystem->getSettingsT<WindowSettings>();
            p.fadeObserver = ValueObserver<bool>::create(
                windowSettings->observeFade(),
                [weak](bool value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->fadeEnabled = value;
                    if (!value)
                    {
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
                _actionUpdate();
            }
        }

        std::shared_ptr<Core::IValueSubject<float> > WindowSystem::observeFade() const
        {
            return _p->fade;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > WindowSystem::getActions()
        {
            return _p->actions;
        }

        MenuData WindowSystem::getMenu()
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
                auto uiSystem = getContext()->getSystemT<UI::UISystem>();
                auto style = uiSystem->getStyle();
                const float h = style->getMetric(UI::MetricsRole::Handle);
                if (diff > h)
                {
                    start = true;
                    p.pointerMotion[info.id] = info.projectedPos;
                    if (p.fadeEnabled)
                    {
                        p.fadeAnimation->start(
                            p.fade->get(),
                            1.f,
                            Time::getMilliseconds(Time::TimerValue::Medium),
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
                    std::chrono::milliseconds(fadeSeconds * 1000),
                    [weak](float value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->fadeAnimation->start(
                            system->_p->fade->get(),
                            0.f,
                            Time::getMilliseconds(Time::TimerValue::Slow),
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
                });
            }
        }

        void WindowSystem::_actionUpdate()
        {
            DJV_PRIVATE_PTR();
            p.actions["Maximized"]->setChecked(static_cast<int>(p.maximized->get()));
        }

        void WindowSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            auto context = getContext();
            p.actions["Fit"]->setText(_getText(DJV_TEXT("Fit To Image")));
            p.actions["Fit"]->setTooltip(_getText(DJV_TEXT("Fit to image tooltip")));
            p.actions["FullScreen"]->setText(_getText(DJV_TEXT("Full Screen")));
            p.actions["FullScreen"]->setTooltip(_getText(DJV_TEXT("Full screen tooltip")));
            p.actions["Maximized"]->setText(_getText(DJV_TEXT("Maximized")));
            p.actions["Maximized"]->setTooltip(_getText(DJV_TEXT("Maximized tooltip")));

            p.menu->setText(_getText(DJV_TEXT("Window")));
        }

        void WindowSystem::Private::setFullScreen(bool value, Context * context)
        {
            if (auto glfwSystem = context->getSystemT<Desktop::GLFWSystem>())
            {
                auto glfwWindow = glfwSystem->getGLFWWindow();
                auto glfwMonitor = glfwGetWindowMonitor(glfwWindow);
                if (value && !glfwMonitor)
                {
                    glfwMonitor = glfwGetPrimaryMonitor();
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
        }

    } // namespace ViewApp
} // namespace djv

