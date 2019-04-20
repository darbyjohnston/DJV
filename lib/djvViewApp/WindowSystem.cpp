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

#include <djvViewApp/WindowSystemSettings.h>

#include <djvDesktopApp/Application.h>
#include <djvDesktopApp/GLFWSystem.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>

#include <djvAV/AVSystem.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct WindowSystem::Private
        {
            std::shared_ptr<WindowSystemSettings> settings;
            std::shared_ptr<ValueSubject<WindowMode> > windowMode;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> windowModeActionGroup;
            std::shared_ptr<UI::Menu> menu;
            glm::ivec2 monitorSize = glm::ivec2(0, 0);
            int monitorRefresh = 0;
            BBox2i windowGeom = BBox2i(0, 0, 0, 0);
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
            std::shared_ptr<ValueObserver<bool> > fullScreenObserver;
            std::shared_ptr<ValueObserver<WindowMode> > windowModeObserver;
            std::shared_ptr<ValueObserver<std::string> > localeObserver;

            void setFullScreen(bool, Context * context);
        };

        void WindowSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewApp::WindowSystem", context);

            DJV_PRIVATE_PTR();

            p.settings = WindowSystemSettings::create(context);
            p.windowMode = ValueSubject<WindowMode>::create();

            //! \todo Implement me!
            p.actions["Fit"] = UI::Action::create();
            p.actions["Fit"]->setIcon("djvIconWindowFit");
            p.actions["Fit"]->setShortcut(GLFW_KEY_F);
            p.actions["Fit"]->setEnabled(false);

            p.actions["FullScreen"] = UI::Action::create();
            p.actions["FullScreen"]->setIcon("djvIconWindowFullScreen");
            p.actions["FullScreen"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["FullScreen"]->setShortcut(GLFW_KEY_U);

            p.actions["SDI"] = UI::Action::create();
            p.actions["SDI"]->setIcon("djvIconViewLibSDI");
            p.actions["MDI"] = UI::Action::create();
            p.actions["MDI"]->setIcon("djvIconViewLibMDI");
            p.actions["Playlist"] = UI::Action::create();
            p.actions["Playlist"]->setIcon("djvIconViewLibPlaylist");
            p.windowModeActionGroup = UI::ActionGroup::create(UI::ButtonType::Radio);
            p.windowModeActionGroup->addAction(p.actions["SDI"]);
            p.actions["SDI"]->setShortcut(GLFW_KEY_1);
            p.windowModeActionGroup->addAction(p.actions["MDI"]);
            p.actions["MDI"]->setShortcut(GLFW_KEY_2);
            p.windowModeActionGroup->addAction(p.actions["Playlist"]);
            p.actions["Playlist"]->setShortcut(GLFW_KEY_3);

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["Fit"]);
            p.menu->addAction(p.actions["FullScreen"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["SDI"]);
            p.menu->addAction(p.actions["MDI"]);
            p.menu->addAction(p.actions["Playlist"]);

            _actionUpdate();

            auto weak = std::weak_ptr<WindowSystem>(std::dynamic_pointer_cast<WindowSystem>(shared_from_this()));
            p.windowModeActionGroup->setRadioCallback(
                [weak](int index)
            {
                if (auto system = weak.lock())
                {
                    system->_p->settings->setWindowMode(static_cast<WindowMode>(index));
                }
            });

            p.fullScreenObserver = ValueObserver<bool>::create(
                p.actions["FullScreen"]->observeChecked(),
                [weak, context](bool value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->setFullScreen(value, context);
                }
            });

            p.windowModeObserver = ValueObserver<WindowMode>::create(
                p.settings->observeWindowMode(),
                [weak, context](WindowMode value)
            {
                if (auto system = weak.lock())
                {
                    system->setWindowMode(value);
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

        std::shared_ptr<Core::IValueSubject<WindowMode> > WindowSystem::observeWindowMode() const
        {
            return _p->windowMode;
        }

        void WindowSystem::setWindowMode(WindowMode value)
        {
            DJV_PRIVATE_PTR();
            if (p.windowMode->setIfChanged(value))
            {
                p.settings->setWindowMode(value);
                _actionUpdate();
            }
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

        void WindowSystem::_actionUpdate()
        {
            DJV_PRIVATE_PTR();
            p.windowModeActionGroup->setChecked(static_cast<int>(p.windowMode->get()));
        }

        void WindowSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            auto context = getContext();
            p.actions["Fit"]->setText(_getText(DJV_TEXT("Fit To Image")));
            p.actions["Fit"]->setTooltip(_getText(DJV_TEXT("Fit to image tooltip")));
            p.actions["FullScreen"]->setText(_getText(DJV_TEXT("Full Screen")));
            p.actions["FullScreen"]->setTooltip(_getText(DJV_TEXT("Full screen tooltip")));
            p.actions["SDI"]->setText(_getText(DJV_TEXT("Single Window")));
            p.actions["SDI"]->setTooltip(_getText(DJV_TEXT("Single window tooltip")));
            p.actions["MDI"]->setText(_getText(DJV_TEXT("Multiple Windows")));
            p.actions["MDI"]->setTooltip(_getText(DJV_TEXT("Multiple windows tooltip")));
            p.actions["Playlist"]->setText(_getText(DJV_TEXT("Playlist")));
            p.actions["Playlist"]->setTooltip(_getText(DJV_TEXT("Playlist tooltip")));

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

