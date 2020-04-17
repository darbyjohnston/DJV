// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvDesktopApp/GLFWSystem.h>

#include <djvAV/GLFWSystem.h>
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
            
            unsigned char hiddenArrowPixel[] = { 0, 0, 0, 0 };

        } // namespace
        
        bool MonitorInfo::operator == (const MonitorInfo& other) const
        {
            return
                name == other.name &&
                geometry == other.geometry &&
                physicalSizeMM == other.physicalSizeMM;
        }

        struct GLFWSystem::Private
        {
            std::shared_ptr<ListSubject<MonitorInfo> > monitorInfo;
            std::shared_ptr<Time::Timer> monitorTimer;
            bool cursorVisible = true;
            GLFWcursor* arrowCursor  = nullptr;
            GLFWcursor* hiddenCursor = nullptr;
        };

        void GLFWSystem::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISystem::_init("djv::Desktop::GLFWSystem", context);
            DJV_PRIVATE_PTR();

            p.monitorInfo = ListSubject<MonitorInfo>::create();

            auto avGLFWSystem = context->getSystemT<AV::GLFW::System>();
            addDependency(avGLFWSystem);

            // Get monitor information.
            p.monitorTimer = Time::Timer::create(context);
            p.monitorTimer->setRepeating(true);
            auto weak = std::weak_ptr<GLFWSystem>(std::dynamic_pointer_cast<GLFWSystem>(shared_from_this()));
            p.monitorTimer->start(
                Time::getTime(Time::TimerValue::Slow),
                [weak](const std::chrono::steady_clock::time_point&, const Time::Duration&)
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

            // Setup window.
            auto glfwWindow = avGLFWSystem->getGLFWWindow();
            glm::vec2 contentScale = glm::vec2(1.F, 1.F);
            glfwGetWindowContentScale(glfwWindow, &contentScale.x, &contentScale.y);
            glfwSetWindowSize(
                glfwWindow,
                static_cast<int>(windowSize.x * contentScale.x),
                static_cast<int>(windowSize.y * contentScale.y));
            glfwSetWindowUserPointer(glfwWindow, context.get());

            p.arrowCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);

            GLFWimage image;
            image.width = 1;
            image.height = 1;
            image.pixels = hiddenArrowPixel;
            GLFWcursor* cursor = glfwCreateCursor(&image, 0, 0);
            p.hiddenCursor = cursor;
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

        bool GLFWSystem::isCursorVisible() const
        {
            return _p->cursorVisible;
        }

        void GLFWSystem::showCursor()
        {
            DJV_PRIVATE_PTR();
            if (!p.cursorVisible)
            {
                p.cursorVisible = true;
                if (auto context = getContext().lock())
                {
                    auto avGLFWSystem = context->getSystemT<AV::GLFW::System>();
                    auto glfwWindow = avGLFWSystem->getGLFWWindow();
                    glfwSetCursor(glfwWindow, p.arrowCursor);
                }
            }
        }

        void GLFWSystem::hideCursor()
        {
            DJV_PRIVATE_PTR();
            if (p.cursorVisible)
            {
                p.cursorVisible = false;
                if (auto context = getContext().lock())
                {
                    auto avGLFWSystem = context->getSystemT<AV::GLFW::System>();
                    auto glfwWindow = avGLFWSystem->getGLFWWindow();
                    glfwSetCursor(glfwWindow, p.hiddenCursor);
                }
            }
        }

    } // namespace Desktop
} // namespace djv
