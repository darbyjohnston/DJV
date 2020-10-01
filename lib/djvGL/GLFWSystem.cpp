// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvGL/GLFWSystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/CoreSystem.h>
#include <djvSystem/LogSystem.h>
#include <djvSystem/TextSystem.h>

#include <djvCore/OSFunc.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>

#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace GL
    {
        namespace GLFW
        {
            namespace
            {
                //! \todo Should this be configurable?
                const glm::ivec2 windowSize(1280, 720);
            
                //! \todo Should this be configurable?
                std::weak_ptr<System::LogSystem> logSystemWeak;

                void glfwErrorCallback(int, const char * description)
                {
                    if (auto logSystem = logSystemWeak.lock())
                    {
                        logSystem->log("djv::CmdLine::GLFW::System", description);
                    }
                }

#if defined(DJV_GL_ES2)
#else // DJV_GL_ES2
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
                        if (auto log = reinterpret_cast<const System::Context *>(userParam)->getSystemT<System::LogSystem>())
                        {
                            log->log("djv::CmdLine::GLFW::GLFWSystem", message);
                        }
                        break;
                    default: break;
                    }
                }
#endif // DJV_GL_ES2

                std::string getErrorMessage(ErrorString error, const std::shared_ptr<System::TextSystem>& textSystem)
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

            } // namespace

            Error::Error(const std::string& what) :
                std::runtime_error(what)
            {}

            struct GLFWSystem::Private
            {
                std::shared_ptr<System::TextSystem> textSystem;
                GLFWwindow* window = nullptr;
                std::shared_ptr<Observer::ValueSubject<SwapInterval> > swapInterval;
            };

            void GLFWSystem::_init(const std::shared_ptr<System::Context>& context)
            {
                ISystem::_init("djv::GL::GLFW::GLFWSystem", context);

                DJV_PRIVATE_PTR();

                addDependency(context->getSystemT<System::CoreSystem>());

                p.textSystem = context->getSystemT<System::TextSystem>();

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
#if defined(DJV_GL_ES2)
                glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else // DJV_GL_ES2
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
                glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif // DJV_GL_ES2
                glfwWindowHint(GLFW_SAMPLES, 1);
                glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
                glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
                int env = 0;
                if (OS::getIntEnv("DJV_GL_DEBUG", env) && env != 0)
                {
                    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
                }
                p.window = glfwCreateWindow(
                    windowSize.x,
                    windowSize.y,
                    context->getName().c_str(),
                    NULL,
                    NULL);
                if (!p.window)
                {
                    throw Error(getErrorMessage(ErrorString::Window, p.textSystem));
                }
                {
                    const int glMajor = glfwGetWindowAttrib(_p->window, GLFW_CONTEXT_VERSION_MAJOR);
                    const int glMinor = glfwGetWindowAttrib(_p->window, GLFW_CONTEXT_VERSION_MINOR);
                    const int glRevision = glfwGetWindowAttrib(_p->window, GLFW_CONTEXT_REVISION);
                    std::stringstream ss;
                    ss << "GLFW OpenGL version: " << glMajor << "." << glMinor << "." << glRevision;
                    _log(ss.str());
                }
                glfwSetWindowUserPointer(p.window, context.get());
                glfwMakeContextCurrent(p.window);
#if defined(DJV_GL_ES2)
                if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
#else
                if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
#endif
                {
                    throw Error(getErrorMessage(ErrorString::GLAD, p.textSystem));
                }
#if defined(DJV_GL_ES2)
#else // DJV_GL_ES2
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
#endif // DJV_GL_ES2

                {
                    std::stringstream ss;
                    ss << "OpenGL vendor: " << glGetString(GL_VENDOR);
                    _log(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "OpenGL renderer: " << glGetString(GL_RENDERER);
                    _log(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "OpenGL version: " << glGetString(GL_VERSION);
                    _log(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "OpenGL shading language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION);
                    _log(ss.str());
                }

                p.swapInterval = Observer::ValueSubject<SwapInterval>::create(SwapInterval::Default);
            }

            GLFWSystem::GLFWSystem() :
                _p(new Private)
            {}

            GLFWSystem::~GLFWSystem()
            {
                DJV_PRIVATE_PTR();
                if (p.window)
                {
                    glfwDestroyWindow(p.window);
                    p.window = nullptr;
                }
                glfwTerminate();
            }

            std::shared_ptr<GLFWSystem> GLFWSystem::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = context->getSystemT<GLFWSystem>();
                if (!out)
                {
                    out = std::shared_ptr<GLFWSystem>(new GLFWSystem);
                    out->_init(context);
                }
                return out;
            }

            GLFWwindow* GLFWSystem::getWindow() const
            {
                return _p->window;
            }

            std::shared_ptr<Observer::IValueSubject<SwapInterval> > GLFWSystem::observeSwapInterval() const
            {
                return _p->swapInterval;
            }

            void GLFWSystem::setSwapInterval(SwapInterval value)
            {
                DJV_PRIVATE_PTR();
                if (_p->swapInterval->setIfChanged(value))
                {
                    switch (value)
                    {
                    case SwapInterval::_0: glfwSwapInterval(0); break;
                    case SwapInterval::_1: glfwSwapInterval(1); break;
                    default: break;
                    }
                }
            }

        } // namespace GLFW
    } // namespace GL
} // namespace djv

