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

#include <djvDesktop/Application.h>

#include <djvDesktop/EventLoop.h>
#include <djvDesktop/WindowSystem.h>

#include <djvUI/UISystem.h>
#include <djvUI/Window.h>

#include <djvAV/FontSystem.h>
#include <djvAV/IO.h>
#include <djvAV/IconSystem.h>
#include <djvAV/AVSystem.h>

#include <djvCore/OS.h>

#include <GLFW/glfw3.h>

#include <chrono>

using namespace djv::Core;

using namespace gl;

namespace djv
{
    namespace Desktop
    {
        namespace
        {
            //! \todo [1.0 S] Should this be configurable?
            const size_t frameRate = 60;

        } // namespace

        struct Application::Private
        {
            bool running = false;
            GLFWwindow * glfwWindow = nullptr;
            std::shared_ptr<UI::UISystem> uiSystem;
            std::shared_ptr<WindowSystem> windowSystem;
        };

        void Application::_init(int argc, char* argv[])
        {
            Context::_init(argc, argv);
            DJV_PRIVATE_PTR();
            p.uiSystem = UI::UISystem::create(this);
            if (auto avSystem = getSystemT<AV::AVSystem>().lock())
            {
                p.glfwWindow = avSystem->getGLFWWindow();
                p.windowSystem = WindowSystem::create(p.glfwWindow, this);

                setEventLoop(EventLoop::create(p.glfwWindow, this));

                glfwSetWindowSize(p.glfwWindow, 1024, 768);
                glfwShowWindow(p.glfwWindow);
            }
        }
        
        Application::Application() :
            _p(new Private)
        {}
        
        Application::~Application()
        {
            DJV_PRIVATE_PTR();
            p.windowSystem->setParent(nullptr);
            p.uiSystem->setParent(nullptr);
        }
        
        std::unique_ptr<Application> Application::create(int argc, char* argv[])
        {
            auto out = std::unique_ptr<Application>(new Application);
            out->_init(argc, argv);
            return std::move(out);
        }
        
        int Application::run()
        {
            DJV_PRIVATE_PTR();
            p.running = true;
            auto time = std::chrono::system_clock::now();
            while (p.running && p.glfwWindow && !glfwWindowShouldClose(p.glfwWindow))
            {
                auto now = std::chrono::system_clock::now();
                std::chrono::duration<float> delta = now - time;
                time = now;
                float dt = delta.count();

                tick(dt);
                glfwPollEvents();

                now = std::chrono::system_clock::now();
                delta = now - time;
                dt = delta.count();
                const float sleep = 1 / static_cast<float>(frameRate) - dt;
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(sleep * 1000)));
            }
            return 0;
        }

        void Application::exit()
        {
            _p->running = false;
        }

    } // namespace Desktop
} // namespace Gp
