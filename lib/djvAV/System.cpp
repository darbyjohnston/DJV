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

#include <djvAV/System.h>

#include <djvAV/AudioSystem.h>
#include <djvAV/FontSystem.h>
#include <djvAV/IconSystem.h>
#include <djvAV/IO.h>
#include <djvAV/OpenGL.h>
#include <djvAV/Render2DSystem.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>
#include <djvCore/OS.h>
#include <djvCore/LogSystem.h>
#include <djvCore/String.h>
#include <djvCore/Vector.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

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
                    logSystem->log("djv::AV::System", description);
                }
            }

        } // namespace

        struct System::Private
        {
            glm::vec2 dpi;
            GLFWwindow * glfwWindow = nullptr;
            std::vector<std::shared_ptr<ISystem> > avSystems;
        };

        void System::_init(Context * context)
        {
            ISystem::_init("djv::AV::System", context);

            DJV_PRIVATE_PTR();

            // Initialize GLFW.
            glfwSetErrorCallback(glfwErrorCallback);
            int glfwMajor = 0;
            int glfwMinor = 0;
            int glfwRevision = 0;
            glfwGetVersion(&glfwMajor, &glfwMinor, &glfwRevision);
            if (auto logSystem = context->getSystemT<LogSystem>().lock())
            {
                logSystemWeak = logSystem;
                std::stringstream ss;
                ss << "GLFW version: " << glfwMajor << "." << glfwMinor << "." << glfwRevision;
                logSystem->log("djv::AV::System", ss.str());
            }
            if (!glfwInit())
            {
                std::stringstream ss;
                ss << DJV_TEXT("Cannot initialize GLFW.");
                throw std::runtime_error(ss.str());
            }

            // Get the primary monitor DPI.
            if (auto primaryMonitor = glfwGetPrimaryMonitor())
            {
                const GLFWvidmode * mode = glfwGetVideoMode(primaryMonitor);
                glm::ivec2 mm;
                glfwGetMonitorPhysicalSize(primaryMonitor, &mm.x, &mm.y);
                _p->dpi.x = mode->width / (mm.x / 25.4f);
                _p->dpi.y = mode->height / (mm.y / 25.4f);
                if (auto logSystem = context->getSystemT<LogSystem>().lock())
                {
                    std::stringstream ss;
                    ss << "Primary monitor size: " << mm << "mm\n";
                    ss << "Primary monitor DPI: " << _p->dpi;
                    logSystem->log("djv::AV::System", ss.str());
                    ss.str(std::string());
                }
            }

            // Create an OpenGL context.
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
            p.glfwWindow = glfwCreateWindow(100, 100, context->getName().c_str(), NULL, NULL);
            if (!p.glfwWindow)
            {
                std::stringstream ss;
                ss << DJV_TEXT("Cannot create GLFW window.");
                throw std::runtime_error(ss.str());
            }
            if (auto logSystem = context->getSystemT<LogSystem>().lock())
            {
                int glMajor = glfwGetWindowAttrib(_p->glfwWindow, GLFW_CONTEXT_VERSION_MAJOR);
                int glMinor = glfwGetWindowAttrib(_p->glfwWindow, GLFW_CONTEXT_VERSION_MINOR);
                int glRevision = glfwGetWindowAttrib(_p->glfwWindow, GLFW_CONTEXT_REVISION);
                std::stringstream ss;
                ss << "OpenGL version: " << glMajor << "." << glMinor << "." << glRevision;
                logSystem->log("Gp::Desktop::Application", ss.str());
            }
            glfwMakeContextCurrent(p.glfwWindow);
            glbinding::initialize(glfwGetProcAddress);

            // Create the systems.
            _p->avSystems.push_back(IO::System::create(context));
            _p->avSystems.push_back(AudioSystem::create(context));
            _p->avSystems.push_back(FontSystem::create(_p->dpi, context));
            _p->avSystems.push_back(IconSystem::create(context));
            _p->avSystems.push_back(Render2DSystem::create(context));
        }

        System::System() :
            _p(new Private)
        {}

        System::~System()
        {
            DJV_PRIVATE_PTR();
            while (p.avSystems.size())
            {
                p.avSystems.pop_back();
            }
            if (p.glfwWindow)
            {
                glfwDestroyWindow(p.glfwWindow);
            }
            glfwTerminate();
        }

        std::shared_ptr<System> System::create(Context * context)
        {
            auto out = std::shared_ptr<System>(new System);
            out->_init(context);
            return out;
        }

        void System::makeGLContextCurrent()
        {
            DJV_PRIVATE_PTR();
            glfwMakeContextCurrent(p.glfwWindow);
        }

    } // namespace AV
} // namespace djv

