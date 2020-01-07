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
                [weak](const std::chrono::steady_clock::time_point&, const Time::Unit&)
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
