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
#include <djvAV/OpenGL.h>
#include <djvAV/Render2DSystem.h>

#include <GLFW/glfw3.h>

#include <chrono>

using namespace djv::Core;

namespace djv
{
    namespace Desktop
    {
        struct Application::Private
        {
            bool running = false;
            GLFWwindow* glfwWindow = nullptr;
            glm::ivec2 dpi;
            std::shared_ptr<EventSystem> eventSystem;
            std::shared_ptr<WindowSystem> windowSystem;
            std::shared_ptr<UI::System> uiSystem;
        };

        void Application::_init(int argc, char* argv[])
        {
            Context::_init(argc, argv);

            _p->uiSystem = UI::System::create(shared_from_this());

            /*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            //glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
            _p->glfwWindow = glfwCreateWindow(1024, 768, getName().c_str(), NULL, NULL);
            if (!_p->glfwWindow)
            {
                throw std::runtime_error("Cannot create GLFW window");
            }

            _p->eventSystem = EventSystem::create(_p->glfwWindow, shared_from_this());
            _p->windowSystem = WindowSystem::create(_p->glfwWindow, shared_from_this());*/
        }
        
        Application::Application() :
            _p(new Private)
        {}
        
        Application::~Application()
        {
            if (_p->glfwWindow)
            {
                glfwDestroyWindow(_p->glfwWindow);
                _p->glfwWindow = nullptr;
            }
        }
        
        std::shared_ptr<Application> Application::create(int argc, char* argv[])
        {
            auto out = std::shared_ptr<Application>(new Application);
            out->_init(argc, argv);
            return out;
        }
        
        int Application::run()
        {
            _p->running = true;
            auto time = std::chrono::system_clock::now();
            while (_p->running && _p->glfwWindow && !glfwWindowShouldClose(_p->glfwWindow))
            {
                const auto now = std::chrono::system_clock::now();
                const std::chrono::duration<float> delta = now - time;
                const float dt = delta.count();
                glm::ivec2 frameBufferSize(0, 0);
                glfwGetFramebufferSize(_p->glfwWindow, &frameBufferSize.x, &frameBufferSize.y);
                glViewport(0, 0, static_cast<GLint>(frameBufferSize.x), static_cast<GLint>(frameBufferSize.y));
                glClearColor(0.f, 0.f, 0.f, 0.f);
                glClear(GL_COLOR_BUFFER_BIT);
                tick(dt);
                glfwSwapBuffers(_p->glfwWindow);
                glfwPollEvents();
            }
            return 0;
        }

        void Application::stop()
        {
            _p->running = false;
        }

        GLFWwindow* Application::getGLFWWindow() const
        {
            return _p->glfwWindow;
        }

    } // namespace Desktop
} // namespace Gp
