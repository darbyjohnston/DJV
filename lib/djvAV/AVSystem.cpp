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
#include <djvAV/Render2DSystem.h>
#include <djvAV/ThumbnailSystem.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>
#include <djvCore/FileInfo.h>
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
            std::shared_ptr<ListSubject<int> > dpiList;
            GLFWwindow * glfwWindow = nullptr;
            std::vector<std::shared_ptr<ISystem> > avSystems;
        };

        void AVSystem::_init(Context * context)
        {
            ISystem::_init("djv::AV::AVSystem", context);

            logSystemWeak = context->getSystemT<LogSystem>();

            DJV_PRIVATE_PTR();

            // Find the DPI values.
            std::vector<int> dpiList;
            for (const auto& i : FileSystem::FileInfo::directoryList(context->getPath(FileSystem::ResourcePath::IconsDirectory)))
            {
                const std::string fileName = i.getFileName(Frame::Invalid, false);
                const size_t size = fileName.size();
                if (size > 3 &&
                    fileName[size - 3] == 'D' &&
                    fileName[size - 2] == 'P' &&
                    fileName[size - 1] == 'I')
                {
                    dpiList.push_back(std::stoi(fileName.substr(0, size - 3)));
                }
            }
            std::sort(dpiList.begin(), dpiList.end());
            for (const auto & i : dpiList)
            {
                std::stringstream ss;
                ss << "Found DPI: " << i;
                context->log("djv::AV::AVSystem", ss.str());
            }
            p.dpiList = ListSubject<int>::create(dpiList);

            // Initialize GLFW.
            glfwSetErrorCallback(glfwErrorCallback);
            int glfwMajor = 0;
            int glfwMinor = 0;
            int glfwRevision = 0;
            glfwGetVersion(&glfwMajor, &glfwMinor, &glfwRevision);
            {
                std::stringstream ss;
                ss << "GLFW version: " << glfwMajor << "." << glfwMinor << "." << glfwRevision;
                context->log("djv::AV::AVSystem", ss.str());
            }
            if (!glfwInit())
            {
                std::stringstream ss;
                ss << DJV_TEXT("Cannot initialize GLFW.");
                throw std::runtime_error(ss.str());
            }

            // Get primary monitor information.
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
                    context->log("djv::AV::AVSystem", ss.str());
                }
            }

            // Create an OpenGL context.
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
            if (!OS::getEnv("DJV_OPENGL_DEBUG").empty())
            {
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
            }
            p.glfwWindow = glfwCreateWindow(100, 100, context->getName().c_str(), NULL, NULL);
            if (!p.glfwWindow)
            {
                std::stringstream ss;
                ss << DJV_TEXT("Cannot create GLFW window.");
                throw std::runtime_error(ss.str());
            }
            {
                int glMajor = glfwGetWindowAttrib(_p->glfwWindow, GLFW_CONTEXT_VERSION_MAJOR);
                int glMinor = glfwGetWindowAttrib(_p->glfwWindow, GLFW_CONTEXT_VERSION_MINOR);
                int glRevision = glfwGetWindowAttrib(_p->glfwWindow, GLFW_CONTEXT_REVISION);
                std::stringstream ss;
                ss << "OpenGL version: " << glMajor << "." << glMinor << "." << glRevision;
                context->log("djv::AV::AVSystem", ss.str());
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
            _p->avSystems.push_back(IO::System::create(context));
            _p->avSystems.push_back(Audio::System::create(context));
            auto fontSystem = Font::System::create(context);
            fontSystem->setDPI(_p->dpi);
            _p->avSystems.push_back(fontSystem);
            _p->avSystems.push_back(Render::Render2DSystem::create(context));
            _p->avSystems.push_back(ThumbnailSystem::create(context));
            auto iconSystem = Image::IconSystem::create(context);
            iconSystem->setDPI(_p->dpi);
            _p->avSystems.push_back(iconSystem);
        }

        AVSystem::AVSystem() :
            _p(new Private)
        {}

        AVSystem::~AVSystem()
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

        std::shared_ptr<Core::IListSubject<int> > AVSystem::observeDPIList() const
        {
            return _p->dpiList;
        }

        void AVSystem::setDPI(int value)
        {
            if (value == _p->dpi)
                return;
            _p->dpi = value;
            auto context = getContext();
            if (auto iconSystem = context->getSystemT<Image::IconSystem>().lock())
            {
                iconSystem->setDPI(value);
            }
        }

    } // namespace AV
} // namespace djv

