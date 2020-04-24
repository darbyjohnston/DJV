// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/GLFWSystem.h>

#include <djvAV/OpenGL.h>

#include <djvCore/Context.h>
#include <djvCore/CoreSystem.h>
#include <djvCore/LogSystem.h>
#include <djvCore/OS.h>
#include <djvCore/TextSystem.h>
#include <djvCore/Vector.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace GLFW
        {
            namespace
            {
                //! \todo Should this be configurable?
                const glm::ivec2 windowSize(1280, 720);
            
                //! \todo Should this be configurable?
                std::weak_ptr<LogSystem> logSystemWeak;

                void glfwErrorCallback(int, const char * description)
                {
                    if (auto logSystem = logSystemWeak.lock())
                    {
                        logSystem->log("djv::CmdLine::GLFW::System", description);
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
                        if (auto log = reinterpret_cast<const Context *>(userParam)->getSystemT<LogSystem>())
                        {
                            log->log("djv::CmdLine::GLFW::System", message);
                        }
                        break;
                    default: break;
                    }
                }
#endif // DJV_OPENGL_ES2

            } // namespace

            std::string getErrorMessage(ErrorString error, const std::shared_ptr<TextSystem>& textSystem)
            {
                std::stringstream ss;
                switch (error)
                {
                case ErrorString::Init:
                    ss << textSystem->getText(DJV_TEXT("error_glfw_init"));
                    break;
                case ErrorString::Window:
                    ss << textSystem->getText(DJV_TEXT("error_glfw_window_creation"));
                    break;
                case ErrorString::GLAD:
                    ss << textSystem->getText(DJV_TEXT("error_glad_init"));
                    break;
                default: break;
                }
                return ss.str();
            }

            Error::Error(const std::string& what) :
                std::runtime_error(what)
            {}

            struct System::Private
            {
                std::shared_ptr<TextSystem> textSystem;
                GLFWwindow* glfwWindow = nullptr;
            };

            void System::_init(const std::shared_ptr<Context>& context)
            {
                ISystem::_init("djv::AV::GLFW::System", context);

                DJV_PRIVATE_PTR();

                addDependency(context->getSystemT<CoreSystem>());

                p.textSystem = context->getSystemT<TextSystem>();

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
                    throw Error(getErrorMessage(ErrorString::Init, p.textSystem));
                }

                // Create a window.
#if defined(DJV_OPENGL_ES2)
                glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else // DJV_OPENGL_ES2
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
                glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif // DJV_OPENGL_ES2
                glfwWindowHint(GLFW_SAMPLES, 1);
                glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
                glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
                if (OS::getIntEnv("DJV_OPENGL_DEBUG") != 0)
                {
                    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
                }
                p.glfwWindow = glfwCreateWindow(
                    windowSize.x,
                    windowSize.y,
                    context->getName().c_str(),
                    NULL,
                    NULL);
                if (!p.glfwWindow)
                {
                    throw Error(AV::GLFW::getErrorMessage(ErrorString::Window, p.textSystem));
                }
                {
                    const int glMajor = glfwGetWindowAttrib(_p->glfwWindow, GLFW_CONTEXT_VERSION_MAJOR);
                    const int glMinor = glfwGetWindowAttrib(_p->glfwWindow, GLFW_CONTEXT_VERSION_MINOR);
                    const int glRevision = glfwGetWindowAttrib(_p->glfwWindow, GLFW_CONTEXT_REVISION);
                    std::stringstream ss;
                    ss << "OpenGL version: " << glMajor << "." << glMinor << "." << glRevision;
                    _log(ss.str());
                }
                glfwSetWindowUserPointer(p.glfwWindow, context.get());
                glfwMakeContextCurrent(p.glfwWindow);
#if defined(DJV_OPENGL_ES2)
                if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
#else
                if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
#endif
                {
                    throw Error(AV::GLFW::getErrorMessage(ErrorString::GLAD, p.textSystem));
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
                        GLFW_TRUE);
                }
#endif // DJV_OPENGL_ES2
                glfwSwapInterval(1);
            }

            System::System() :
                _p(new Private)
            {}

            System::~System()
            {
                DJV_PRIVATE_PTR();
                if (p.glfwWindow)
                {
                    glfwDestroyWindow(p.glfwWindow);
                    p.glfwWindow = nullptr;
                }
                glfwTerminate();
            }

            std::shared_ptr<System> System::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<System>(new System);
                out->_init(context);
                return out;
            }

            GLFWwindow* System::getGLFWWindow() const
            {
                return _p->glfwWindow;
            }

        } // namespace GLFW
    } // namespace AV
} // namespace djv

