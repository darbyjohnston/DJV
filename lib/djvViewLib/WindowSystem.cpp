//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvViewLib/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/Menu.h>

#include <djvAV/AVSystem.h>

#include <djvCore/Context.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct WindowSystem::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
            std::shared_ptr<ValueObserver<bool> > fullScreenObserver;
            glm::ivec2 monitorSize = glm::ivec2(0, 0);
            int monitorRefresh = 0;
            BBox2i windowGeom = BBox2i(0, 0, 0, 0);

            void setFullScreen(bool, Context * context);
        };

        void WindowSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewLib::WindowSystem", context);

            DJV_PRIVATE_PTR();
            //! \todo Implement me!
            p.actions["Duplicate"] = UI::Action::create();
            p.actions["Duplicate"]->setIcon("djvIconWindowDuplicate");
            p.actions["Duplicate"]->setText(DJV_TEXT("djv::ViewLib", "Duplicate"));
            p.actions["Duplicate"]->setShortcut(GLFW_KEY_D, GLFW_MOD_CONTROL);
            p.actions["Duplicate"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["Fit"] = UI::Action::create();
            p.actions["Fit"]->setIcon("djvIconWindowFit");
            p.actions["Fit"]->setText(DJV_TEXT("djv::ViewLib", "Fit"));
            p.actions["Fit"]->setShortcut(GLFW_KEY_F);
            p.actions["Fit"]->setEnabled(false);

            p.actions["FullScreen"] = UI::Action::create();
            p.actions["FullScreen"]->setIcon("djvIconWindowFullScreen");
            p.actions["FullScreen"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["FullScreen"]->setText(DJV_TEXT("djv::ViewLib", "Full Screen"));
            p.actions["FullScreen"]->setShortcut(GLFW_KEY_U);

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

        std::map<std::string, std::shared_ptr<UI::Action> > WindowSystem::getActions()
        {
            return _p->actions;
        }

        std::string WindowSystem::getMenuSortKey() const
        {
            return "1";
        }

        std::shared_ptr<UI::Menu> WindowSystem::createMenu()
        {
            DJV_PRIVATE_PTR();
            auto menu = UI::Menu::create("Window", getContext());
            menu->addAction(p.actions["Duplicate"]);
            menu->addAction(p.actions["Fit"]);
            menu->addAction(p.actions["FullScreen"]);
            return menu;
        }

        void WindowSystem::Private::setFullScreen(bool value, Context * context)
        {
            if (auto avSystem = context->getSystemT<AV::AVSystem>().lock())
            {
                auto glfwWindow = avSystem->getGLFWWindow();
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

    } // namespace ViewLib
} // namespace djv

