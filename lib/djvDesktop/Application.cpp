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

#include <djvUI/UISystem.h>
#include <djvUI/Window.h>

#include <djvCore/LogSystem.h>
#include <djvCore/OS.h>

#include <GLFW/glfw3.h>

#include <chrono>

#undef GL_DEBUG_SEVERITY_HIGH
#undef GL_DEBUG_SEVERITY_MEDIUM

using namespace djv::Core;

using namespace gl;

namespace djv
{
    namespace Desktop
    {
        namespace
        {
            //! \todo [1.0 S] Should this be configurable?
            const glm::ivec2 windowSize(1280, 720);
            const size_t frameRate = 60;

            std::weak_ptr<LogSystem> logSystemWeak;

            void glfwErrorCallback(int error, const char * description)
            {
                if (auto logSystem = logSystemWeak.lock())
                {
                    logSystem->log("djv::Desktop::Application", description);
                }
            }

            void APIENTRY glDebugOutput(
                gl::GLenum source,
                gl::GLenum type,
                GLuint id,
                gl::GLenum severity,
                GLsizei length,
                const GLchar * message,
                const void * userParam)
            {
                switch (severity)
                {
                case gl::GL_DEBUG_SEVERITY_HIGH:
                case gl::GL_DEBUG_SEVERITY_MEDIUM:
                    if (auto log = reinterpret_cast<const Context *>(userParam)->getSystemT<LogSystem>().lock())
                    {
                        log->log("djv::Desktop::Application", message);
                    }
                    break;
                default: break;
                }
            }

        } // namespace

        struct Application::Private
        {
            bool running = false;
            GLFWwindow * glfwWindow = nullptr;
            int dpi = AV::dpiDefault;
            std::vector<std::shared_ptr<ISystem> > systems;
        };

        void Application::_init(int argc, char* argv[])
        {
            Context::_init(argc, argv);

            logSystemWeak = getSystemT<LogSystem>();

            DJV_PRIVATE_PTR();

            // Initialize GLFW.
            glfwSetErrorCallback(glfwErrorCallback);
            int glfwMajor = 0;
            int glfwMinor = 0;
            int glfwRevision = 0;
            glfwGetVersion(&glfwMajor, &glfwMinor, &glfwRevision);
            {
                std::stringstream ss;
                ss << "GLFW version: " << glfwMajor << "." << glfwMinor << "." << glfwRevision;
                log("djv::Desktop::Application", ss.str());
            }
            if (!glfwInit())
            {
                std::stringstream ss;
                ss << getText(DJV_TEXT("djv::Desktop", "Cannot initialize GLFW."));
                throw std::runtime_error(ss.str());
            }

            // Get the primary monitor information.
            if (auto primaryMonitor = glfwGetPrimaryMonitor())
            {
                const GLFWvidmode * mode = glfwGetVideoMode(primaryMonitor);
                glm::ivec2 mm;
                glfwGetMonitorPhysicalSize(primaryMonitor, &mm.x, &mm.y);
                glm::vec2 dpi;
                if (mm.x > 0 && mm.y > 0)
                {
                    dpi.x = mode->width / (mm.x / 25.4f);
                    dpi.y = mode->height / (mm.y / 25.4f);
                }
                {
                    std::stringstream ss;
                    ss << "Primary monitor resolution: " << mode->width << " " << mode->height << "\n";
                    ss << "Primary monitor size: " << mm << "mm\n";
                    ss << "Primary monitor DPI: " << dpi;
                    log("djv::Desktop::Application", ss.str());
                }
                p.dpi = static_cast<int>(dpi.x);
            }

            // Create a window.
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			//glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
            if (!OS::getEnv("DJV_OPENGL_DEBUG").empty())
            {
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
            }
            p.glfwWindow = glfwCreateWindow(
                static_cast<int>(ceilf(windowSize.x * (p.dpi / static_cast<float>(AV::dpiDefault)))),
                static_cast<int>(ceilf(windowSize.y * (p.dpi / static_cast<float>(AV::dpiDefault)))),
                getName().c_str(), NULL, NULL);
            if (!p.glfwWindow)
            {
                std::stringstream ss;
                ss << getText(DJV_TEXT("djv::Desktop", "Cannot create GLFW window."));
                throw std::runtime_error(ss.str());
            }
            {
                int glMajor = glfwGetWindowAttrib(_p->glfwWindow, GLFW_CONTEXT_VERSION_MAJOR);
                int glMinor = glfwGetWindowAttrib(_p->glfwWindow, GLFW_CONTEXT_VERSION_MINOR);
                int glRevision = glfwGetWindowAttrib(_p->glfwWindow, GLFW_CONTEXT_REVISION);
                std::stringstream ss;
                ss << "OpenGL version: " << glMajor << "." << glMinor << "." << glRevision;
                log("djv::Desktop::Application", ss.str());
            }
            glfwSetWindowUserPointer(p.glfwWindow, this);
            glfwMakeContextCurrent(p.glfwWindow);
            glbinding::initialize(glfwGetProcAddress);
            GLint flags = 0;
            glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
            if (flags & static_cast<GLint>(GL_CONTEXT_FLAG_DEBUG_BIT))
            {
                glEnable(GL_DEBUG_OUTPUT);
                glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                glDebugMessageCallback(glDebugOutput, this);
                glDebugMessageControl(
                    static_cast<gl::GLenum>(GL_DONT_CARE),
                    static_cast<gl::GLenum>(GL_DONT_CARE),
                    static_cast<gl::GLenum>(GL_DONT_CARE),
                    0,
                    nullptr,
                    GL_TRUE);
            }
			glfwSwapInterval(1);

            p.systems.push_back(UI::UISystem::create(p.dpi, this));
            p.systems.push_back(EventSystem::create(p.glfwWindow, this));
            p.systems.push_back(WindowSystem::create(p.glfwWindow, this));

            glfwShowWindow(p.glfwWindow);
        }
        
        Application::Application() :
            _p(new Private)
        {}
        
        Application::~Application()
        {
            DJV_PRIVATE_PTR();
            while (p.systems.size())
            {
                auto system = p.systems.back();
                system->setParent(nullptr);
                p.systems.pop_back();
            }
            if (p.glfwWindow)
            {
                glfwDestroyWindow(p.glfwWindow);
                p.glfwWindow = nullptr;
            }
            glfwTerminate();
        }
        
        std::unique_ptr<Application> Application::create(int argc, char* argv[])
        {
            auto out = std::unique_ptr<Application>(new Application);
            out->_init(argc, argv);
            return std::move(out);
        }

        GLFWwindow* Application::getGLFWWindow() const
        {
            return _p->glfwWindow;
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

                glfwPollEvents();
                tick(dt);

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
