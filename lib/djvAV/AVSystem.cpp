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

#include <djvAV/AVSystem.h>

#include <djvAV/AudioSystem.h>
#include <djvAV/FontSystem.h>
#include <djvAV/IconSystem.h>
#include <djvAV/IO.h>
#include <djvAV/OpenGL.h>
#include <djvAV/Render2D.h>
#include <djvAV/ThumbnailSystem.h>

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
                    logSystem->log("djv::AV::AVSystem", description);
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
                        log->log("djv::AV::AVSystem", message);
                    }
                    break;
                default: break;
                }
            }

        } // namespace

        struct AVSystem::Private
        {
            int dpi = dpiDefault;
            GLFWwindow * glfwWindow = nullptr;
            std::vector<std::shared_ptr<ISystem> > systems;
        };

        void AVSystem::_init(Context * context)
        {
            ISystem::_init("djv::AV::AVSystem", context);

            logSystemWeak = context->getSystemT<LogSystem>();

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
                _log(ss.str());
            }
            if (!glfwInit())
            {
                std::stringstream ss;
                ss << context->getText(DJV_TEXT("djv::AV", "Cannot initialize GLFW."));
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
                    _log(ss.str());
                }
                p.dpi = static_cast<int>(dpi.x);
            }

            // Create an OpenGL context.
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_SAMPLES, 4);
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
            if (!OS::getEnv("DJV_OPENGL_DEBUG").empty())
            {
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
            }
            p.glfwWindow = glfwCreateWindow(100, 100, context->getName().c_str(), NULL, NULL);
            if (!p.glfwWindow)
            {
                std::stringstream ss;
                ss << context->getText(DJV_TEXT("djv::AV", "Cannot create GLFW window."));
                throw std::runtime_error(ss.str());
            }
            {
                int glMajor = glfwGetWindowAttrib(_p->glfwWindow, GLFW_CONTEXT_VERSION_MAJOR);
                int glMinor = glfwGetWindowAttrib(_p->glfwWindow, GLFW_CONTEXT_VERSION_MINOR);
                int glRevision = glfwGetWindowAttrib(_p->glfwWindow, GLFW_CONTEXT_REVISION);
                std::stringstream ss;
                ss << "OpenGL version: " << glMajor << "." << glMinor << "." << glRevision;
                _log(ss.str());
            }
            glfwSetWindowUserPointer(p.glfwWindow, context);
            glfwMakeContextCurrent(p.glfwWindow);
            glbinding::initialize(glfwGetProcAddress);
            GLint flags = 0;
            glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
            if (flags & static_cast<GLint>(GL_CONTEXT_FLAG_DEBUG_BIT))
            {
                glEnable(GL_DEBUG_OUTPUT);
                glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                glDebugMessageCallback(glDebugOutput, context);
                glDebugMessageControl(
                    static_cast<gl::GLenum>(GL_DONT_CARE),
                    static_cast<gl::GLenum>(GL_DONT_CARE),
                    static_cast<gl::GLenum>(GL_DONT_CARE),
                    0,
                    nullptr,
                    GL_TRUE);
            }

            // Create the systems.
            _p->systems.push_back(IO::System::create(context));
            _p->systems.push_back(Audio::System::create(context));
            _p->systems.push_back(Font::System::create(context));
            _p->systems.push_back(Render::Render2D::create(context));
            _p->systems.push_back(ThumbnailSystem::create(context));
            _p->systems.push_back(Image::IconSystem::create(context));
        }

        AVSystem::AVSystem() :
            _p(new Private)
        {}

        AVSystem::~AVSystem()
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

        std::shared_ptr<AVSystem> AVSystem::create(Context * context)
        {
            auto out = std::shared_ptr<AVSystem>(new AVSystem);
            out->_init(context);
            return out;
        }

        GLFWwindow* AVSystem::getGLFWWindow() const
        {
            return _p->glfwWindow;
        }

        void AVSystem::makeGLContextCurrent()
        {
            DJV_PRIVATE_PTR();
            glfwMakeContextCurrent(p.glfwWindow);
        }

        int AVSystem::getDPI() const
        {
            return _p->dpi;
        }

    } // namespace AV
} // namespace djv

