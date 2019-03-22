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

#include <djvAV/Application.h>

#include <djvAV/AVSystem.h>
#include <djvAV/OpenGL.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>
#include <djvCore/OS.h>
#include <djvCore/LogSystem.h>
#include <djvCore/String.h>
#include <djvCore/Vector.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

using namespace gl;

#undef GL_DEBUG_SEVERITY_HIGH
#undef GL_DEBUG_SEVERITY_MEDIUM

namespace djv
{
    namespace AV
    {
        namespace
        {
            std::weak_ptr<LogSystem> logSystemWeak;

            void glfwErrorCallback(int error, const char * description)
            {
                if (auto logSystem = logSystemWeak.lock())
                {
                    logSystem->log("djv::AV::Application", description);
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
                    if (auto log = reinterpret_cast<const Context *>(userParam)->getSystemT<LogSystem>())
                    {
                        log->log("djv::AV::Application", message);
                    }
                    break;
                default: break;
                }
            }

        } // namespace

        struct Application::Private
        {
            GLFWwindow * glfwWindow = nullptr;
            std::vector<std::shared_ptr<ISystem> > systems;
        };

        void Application::_init(int & argc, char * argv[])
        {
            Context::_init(argc, argv);

            DJV_PRIVATE_PTR();

            // Initialize GLFW.
            glfwSetErrorCallback(glfwErrorCallback);
            int glfwMajor = 0;
            int glfwMinor = 0;
            int glfwRevision = 0;
            glfwGetVersion(&glfwMajor, &glfwMinor, &glfwRevision);
            if (auto logSystem = getSystemT<LogSystem>())
            {
                std::stringstream ss;
                ss << "GLFW version: " << glfwMajor << "." << glfwMinor << "." << glfwRevision;
                logSystem->log("djv::AV::Application", ss.str());
            }
            if (!glfwInit())
            {
                std::stringstream ss;
                ss << DJV_TEXT("GLFW cannot be initialized.");
                throw std::runtime_error(ss.str());
            }

            // Create an OpenGL context.
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_SAMPLES, 1);
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
            if (OS::getIntEnv("DJV_OPENGL_DEBUG") != 0)
            {
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
            }
            p.glfwWindow = glfwCreateWindow(100, 100, getName().c_str(), NULL, NULL);
            if (!p.glfwWindow)
            {
                std::stringstream ss;
                ss << DJV_TEXT("GLFW window cannot be created.");
                throw std::runtime_error(ss.str());
            }
            if (auto logSystem = getSystemT<LogSystem>())
            {
                int glMajor = glfwGetWindowAttrib(_p->glfwWindow, GLFW_CONTEXT_VERSION_MAJOR);
                int glMinor = glfwGetWindowAttrib(_p->glfwWindow, GLFW_CONTEXT_VERSION_MINOR);
                int glRevision = glfwGetWindowAttrib(_p->glfwWindow, GLFW_CONTEXT_REVISION);
                std::stringstream ss;
                ss << "OpenGL version: " << glMajor << "." << glMinor << "." << glRevision;
                logSystem->log("djv::AV::Application", ss.str());
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

            // Create the systems.
            _p->systems.push_back(AV::AVSystem::create(this));
        }

        Application::Application() :
            _p(new Private)
        {}

        Application::~Application()
        {
            DJV_PRIVATE_PTR();
            auto rootObject = getRootObject();
            while (p.systems.size())
            {
                p.systems.pop_back();
            }
            if (p.glfwWindow)
            {
                glfwDestroyWindow(p.glfwWindow);
                p.glfwWindow = nullptr;
            }
            glfwTerminate();
        }

        std::shared_ptr<Application> Application::create(int & argc, char * argv[])
        {
            auto out = std::shared_ptr<Application>(new Application);
            out->_init(argc, argv);
            return out;
        }

        GLFWwindow * Application::getGLFWWindow() const
        {
            return _p->glfwWindow;
        }

    } // namespace AV
} // namespace djv

