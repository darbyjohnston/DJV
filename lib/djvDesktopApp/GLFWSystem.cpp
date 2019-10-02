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

#include <djvDesktopApp/GLFWSystem.h>

#include <djvAV/OpenGL.h>

#include <djvCore/Context.h>
#include <djvCore/CoreSystem.h>
#include <djvCore/LogSystem.h>
#include <djvCore/OS.h>
#include <djvCore/Timer.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace Desktop
    {
        namespace
        {
            //! \todo Should this be configurable?
            const glm::ivec2 windowSize(1280, 720);

            std::weak_ptr<LogSystem> logSystemWeak;

            void glfwErrorCallback(int error, const char * description)
            {
                if (auto logSystem = logSystemWeak.lock())
                {
                    logSystem->log("djv::Desktop::GLFWSystem", description);
                }
            }

#if defined(DJV_OPENGL_ES2)
#else // DJV_OPENGL_ES2
            void APIENTRY glDebugOutput(
                GLenum         source,
                GLenum         type,
                GLuint         id,
                GLenum         severity,
                GLsizei        length,
                const GLchar * message,
                const void *   userParam)
            {
                switch (severity)
                {
                case GL_DEBUG_SEVERITY_HIGH_KHR:
                case GL_DEBUG_SEVERITY_MEDIUM_KHR:
                    if (auto log = reinterpret_cast<const Context*>(userParam)->getSystemT<LogSystem>())
                    {
                        log->log("djv::Desktop::GLFWSystem", message);
                    }
                    break;
                default: break;
                }
            }
#endif // DJV_OPENGL_ES2

            unsigned char hiddenArrowPixel[] = { 0, 0, 0, 0 };

            enum class Error
            {
                Init,
                CreateWindow,
                GLAD
            };
            
            std::string getErrorMessage(Error error)
            {
                std::stringstream ss;
                switch (error)
                {
                case Error::Init:
                    ss << DJV_TEXT("Cannot initialize GLFW.");
                    break;
                case Error::CreateWindow:
                    ss << DJV_TEXT("Cannot create GLFW window.");
                    break;
                case Error::GLAD:
                    ss << DJV_TEXT("Cannot initialize GLAD.");
                    break;
                }
                return ss.str();
            }

        } // namespace
        
        GLFWError::GLFWError(const std::string& what) :
            std::runtime_error(what)
        {}
        
        struct GLFWSystem::Private
        {
            GLFWwindow* glfwWindow   = nullptr;
            std::shared_ptr<ListSubject<MonitorInfo> > monitorInfo;
            std::shared_ptr<Time::Timer> monitorTimer;
            GLFWcursor* arrowCursor  = nullptr;
            GLFWcursor* hiddenCursor = nullptr;
        };

        bool MonitorInfo::operator == (const MonitorInfo& other) const
        {
            return
                name == other.name &&
                geometry == other.geometry &&
                physicalSizeMM == other.physicalSizeMM;
        }

        void GLFWSystem::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISystem::_init("djv::Desktop::GLFWSystem", context);
            DJV_PRIVATE_PTR();

            logSystemWeak = context->getSystemT<LogSystem>();

            p.monitorInfo = ListSubject<MonitorInfo>::create();

            addDependency(context->getSystemT<CoreSystem>());

            // Initialize GLFW.
            glfwSetErrorCallback(glfwErrorCallback);
            int glfwMajor    = 0;
            int glfwMinor    = 0;
            int glfwRevision = 0;
            glfwGetVersion(&glfwMajor, &glfwMinor, &glfwRevision);
            {
                std::stringstream ss;
                ss << "GLFW version: " << glfwMajor << "." << glfwMinor << "." << glfwRevision;
                _log(ss.str());
            }
            if (!glfwInit())
            {
                throw GLFWError(getErrorMessage(Error::Init));
            }

            // Get monitor information.
            p.monitorTimer = Time::Timer::create(context);
            p.monitorTimer->setRepeating(true);
            auto weak = std::weak_ptr<GLFWSystem>(std::dynamic_pointer_cast<GLFWSystem>(shared_from_this()));
            p.monitorTimer->start(
                Time::getMilliseconds(Time::TimerValue::Slow),
                [weak](float)
                {
                    if (auto system = weak.lock())
                    {
                        std::vector<MonitorInfo> info;
                        int count = 0;
                        GLFWmonitor** monitors = glfwGetMonitors(&count);
                        for (int i = 0; i < count; ++i)
                        {
                            MonitorInfo m;
                            std::stringstream ss;
                            ss << i << ": " << glfwGetMonitorName(monitors[0]);
                            m.name = ss.str();
                            int x = 0;
                            int y = 0;
                            int w = 0;
                            int h = 0;
                            glfwGetMonitorWorkarea(monitors[0], &x, &y, &w, &h);
                            m.geometry = BBox2i(x, y, w, h);
                            glfwGetMonitorPhysicalSize(monitors[0], &m.physicalSizeMM.x, &m.physicalSizeMM.y);
                            info.push_back(m);
                        }
                        system->_p->monitorInfo->setIfChanged(info);
                    }
                });

            // Create a window.
#if defined(DJV_OPENGL_ES2)
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else // DJV_OPENGL_ES2
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_TRUE);
#endif // DJV_OPENGL_ES2
            //glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
            if (OS::getIntEnv("DJV_OPENGL_DEBUG") != 0)
            {
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
            }
            p.glfwWindow = glfwCreateWindow(
                windowSize.x,
                windowSize.y,
                context->getName().c_str(), NULL, NULL);
            if (!p.glfwWindow)
            {
                throw GLFWError(getErrorMessage(Error::CreateWindow));
            }
            glm::vec2 contentScale = glm::vec2(1.F, 1.F);
            glfwGetWindowContentScale(p.glfwWindow, &contentScale.x, &contentScale.y);
            {
                std::stringstream ss;
                ss << "Window content scale: " << contentScale.x << "x" << contentScale.y;
                _log(ss.str());
            }
            glfwSetWindowSize(
                p.glfwWindow,
                static_cast<int>(windowSize.x * contentScale.x),
                static_cast<int>(windowSize.y * contentScale.y));
            {
                int glMajor = glfwGetWindowAttrib(p.glfwWindow, GLFW_CONTEXT_VERSION_MAJOR);
                int glMinor = glfwGetWindowAttrib(p.glfwWindow, GLFW_CONTEXT_VERSION_MINOR);
                int glRevision = glfwGetWindowAttrib(p.glfwWindow, GLFW_CONTEXT_REVISION);
                std::stringstream ss;
                ss << "OpenGL version: " << glMajor << "." << glMinor << "." << glRevision;
                _log(ss.str());
            }
            glfwSetWindowUserPointer(p.glfwWindow, context.get());
            glfwMakeContextCurrent(p.glfwWindow);
#if defined(DJV_OPENGL_ES2)
            if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
#else // DJV_OPENGL_ES2
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
#endif // DJV_OPENGL_ES2
            {
                throw GLFWError(getErrorMessage(Error::GLAD));
            }
#if defined(DJV_OPENGL_ES2)
#else // DJV_OPENGL_ES2
            GLint flags = 0;
            glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
            if (flags & static_cast<GLint>(GL_CONTEXT_FLAG_DEBUG_BIT))
            {
                glEnable(GL_DEBUG_OUTPUT);
                glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                glDebugMessageCallback(glDebugOutput, context.get());
                glDebugMessageControl(
                    static_cast<GLenum>(GL_DONT_CARE),
                    static_cast<GLenum>(GL_DONT_CARE),
                    static_cast<GLenum>(GL_DONT_CARE),
                    0,
                    nullptr,
                    GL_TRUE);
            }
#endif // DJV_OPENGL_ES2
            glfwSwapInterval(1);

            p.arrowCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);

            GLFWimage image;
            image.width = 1;
            image.height = 1;
            image.pixels = hiddenArrowPixel;
            GLFWcursor* cursor = glfwCreateCursor(&image, 0, 0);
            p.hiddenCursor = cursor;

            glfwShowWindow(p.glfwWindow);
        }

        GLFWSystem::GLFWSystem() :
            _p(new Private)
        {}

        GLFWSystem::~GLFWSystem()
        {
            DJV_PRIVATE_PTR();
            if (p.hiddenCursor)
            {
                glfwDestroyCursor(p.hiddenCursor);
            }
            if (p.arrowCursor)
            {
                glfwDestroyCursor(p.arrowCursor);
            }
            if (p.glfwWindow)
            {
                glfwDestroyWindow(p.glfwWindow);
                p.glfwWindow = nullptr;
            }
            glfwTerminate();
        }

        std::shared_ptr<GLFWSystem> GLFWSystem::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<GLFWSystem>(new GLFWSystem);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Core::IListSubject<MonitorInfo> > GLFWSystem::observeMonitorInfo() const
        {
            return _p->monitorInfo;
        }

        GLFWwindow * GLFWSystem::getGLFWWindow() const
        {
            return _p->glfwWindow;
        }

        void GLFWSystem::showCursor()
        {
            DJV_PRIVATE_PTR();
            glfwSetCursor(p.glfwWindow, p.arrowCursor);
        }

        void GLFWSystem::hideCursor()
        {
            DJV_PRIVATE_PTR();
            glfwSetCursor(p.glfwWindow, p.hiddenCursor);
        }

    } // namespace Desktop
} // namespace djv
