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

#include <djvDesktop/EventSystem.h>
#include <djvDesktop/WindowSystem.h>

#include <djvUI/System.h>
#include <djvUI/Window.h>

#include <djvAV/FontSystem.h>
#include <djvAV/IO.h>
#include <djvAV/IconSystem.h>
#include <djvAV/System.h>

#include <djvCore/OS.h>

#include <GLFW/glfw3.h>

#include <chrono>

using namespace djv::Core;

using namespace gl;

namespace djv
{
    namespace Desktop
    {
        struct Application::Private
        {
            bool running = false;
            std::shared_ptr<UI::System> uiSystem;
            GLFWwindow * glfwWindow = nullptr;
            std::shared_ptr<EventSystem> eventSystem;
            std::shared_ptr<WindowSystem> windowSystem;
        };

        void Application::_init(int argc, char* argv[])
        {
            Context::_init(argc, argv);
            DJV_PRIVATE_PTR();
            p.uiSystem = UI::System::create(this);
            if (auto avSystem = getSystemT<AV::System>().lock())
            {
                p.glfwWindow = avSystem->getGLFWWindow();
                p.eventSystem = EventSystem::create(p.glfwWindow, this);
                p.windowSystem = WindowSystem::create(p.glfwWindow, this);
                glfwSetWindowSize(p.glfwWindow, 1024, 768);
                glfwShowWindow(p.glfwWindow);
            }
        }
        
        Application::Application() :
            _p(new Private)
        {}
        
        Application::~Application()
        {}
        
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
                const auto now = std::chrono::system_clock::now();
                const std::chrono::duration<float> delta = now - time;
                const float dt = delta.count();
                glm::ivec2 frameBufferSize(0, 0);
                glfwGetFramebufferSize(p.glfwWindow, &frameBufferSize.x, &frameBufferSize.y);
                gl::glViewport(
                    0,
                    0,
                    GLsizei(frameBufferSize.x),
                    GLsizei(frameBufferSize.y));
                gl::glClearColor(0.f, 0.f, 0.f, 0.f);
                glClear(GL_COLOR_BUFFER_BIT);
                tick(dt);
                glfwSwapBuffers(p.glfwWindow);
                glfwPollEvents();
            }
            return 0;
        }

        void Application::exit()
        {
            _p->running = false;
        }

    } // namespace Desktop
} // namespace Gp
