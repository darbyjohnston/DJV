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

#include <djvDesktop/WindowSystem.h>

#include <djvDesktop/EventSystem.h>

#include <djvUI/Widget.h>
#include <djvUI/Window.h>

#include <djvAV/Render2DSystem.h>

#include <GLFW/glfw3.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace Desktop
    {
        namespace
        {
            class RootObject : public IObject {};

        } // namespace

        struct WindowSystem::Private
        {
            GLFWwindow * glfwWindow = nullptr;
            std::shared_ptr<RootObject> rootObject;
        };

        void WindowSystem::_init(GLFWwindow * glfwWindow, Context * context)
        {
            IWindowSystem::_init("djv::Desktop::WindowSystem", context);

            _p->glfwWindow = glfwWindow;

            _p->rootObject = std::shared_ptr<RootObject>(new RootObject);
            if (auto eventSystem = context->getSystemT<EventSystem>().lock())
            {
                eventSystem->setRootObject(_p->rootObject);
            }
        }

        WindowSystem::WindowSystem() :
            _p(new Private)
        {}

        WindowSystem::~WindowSystem()
        {}

        std::shared_ptr<WindowSystem> WindowSystem::create(GLFWwindow * glfwWindow, Context * context)
        {
            auto out = std::shared_ptr<WindowSystem>(new WindowSystem);
            out->_init(glfwWindow, context);
            return out;
        }

        void WindowSystem::_addWindow(const std::shared_ptr<UI::Window>& value)
        {
            value->setParent(_p->rootObject);
            IWindowSystem::_addWindow(value);
        }

        void WindowSystem::_removeWindow(const std::shared_ptr<UI::Window>& value)
        {
            IWindowSystem::_removeWindow(value);
        }

        void WindowSystem::_pushClipRect(const Core::BBox2f & value)
        {
            if (auto system = getContext()->getSystemT<AV::Render::Render2DSystem>().lock())
            {
                system->pushClipRect(value);
            }
        }

        void WindowSystem::_popClipRect()
        {
            if (auto system = getContext()->getSystemT<AV::Render::Render2DSystem>().lock())
            {
                system->popClipRect();
            }
        }

        void WindowSystem::_tick(float dt)
        {
            IWindowSystem::_tick(dt);

            glm::ivec2 frameBufferSize(0, 0);
            glfwGetFramebufferSize(_p->glfwWindow, &frameBufferSize.x, &frameBufferSize.y);
            const auto& windows = getWindows();
            for (const auto& i : windows)
            {
                i->resize(frameBufferSize);

                Event::Update update(dt);
                _updateRecursive(i, update);

                Event::PreLayout preLayout;
                _preLayoutRecursive(i, preLayout);

                if (i->isVisible())
                {
                    Event::Layout layout;
                    _layoutRecursive(i, layout);

                    Event::Clip clip(BBox2f(0.f, 0.f, static_cast<float>(frameBufferSize.x), static_cast<float>(frameBufferSize.y)));
                    _clipRecursive(i, clip);
                }
            }

            if (auto system = getContext()->getSystemT<AV::Render::Render2DSystem>().lock())
            {
                system->beginFrame(frameBufferSize);
                for (const auto& i : windows)
                {
                    if (i->isVisible())
                    {
                        Event::Paint paintEvent(BBox2f(0.f, 0.f, static_cast<float>(frameBufferSize.x), static_cast<float>(frameBufferSize.y)));
                        _paintRecursive(i, paintEvent);
                    }
                }
                system->endFrame();
            }
        }

    } // namespace Desktop
} // namespace djv
