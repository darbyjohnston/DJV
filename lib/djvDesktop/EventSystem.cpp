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

#include <djvDesktop/EventSystem.h>

#include <djvUI/Widget.h>
#include <djvUI/Window.h>

#include <djvCore/Context.h>
#include <djvCore/Event.h>
#include <djvCore/IObject.h>

#include <GLFW/glfw3.h>

#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace Desktop
    {
        namespace
        {
            const Event::PointerID pointerID = 1;

            int fromGLFWPointerButton(int value)
            {
                int out = 0;
                switch (value)
                {
                case GLFW_MOUSE_BUTTON_LEFT:   out = 1; break;
                case GLFW_MOUSE_BUTTON_MIDDLE: out = 2; break;
                case GLFW_MOUSE_BUTTON_RIGHT:  out = 3; break;
                default: break;
                }
                return out;
            }

        } // namespace

        struct EventSystem::Private
        {};

        void EventSystem::_init(GLFWwindow* glfwWindow, Context * context)
        {
            IEventSystem::_init("djv::Desktop::EventSystem", context);

            glfwSetCursorPosCallback(glfwWindow, _pointerCallback);
            glfwSetMouseButtonCallback(glfwWindow, _buttonCallback);
            glfwSetKeyCallback(glfwWindow, _keyCallback);
            glfwSetDropCallback(glfwWindow, _dropCallback);
        }

        EventSystem::EventSystem() :
            _p(new Private)
        {}

        EventSystem::~EventSystem()
        {}

        std::shared_ptr<EventSystem> EventSystem::create(GLFWwindow* glfwWindow, Context * context)
        {
            auto out = std::shared_ptr<EventSystem>(new EventSystem);
            out->_init(glfwWindow, context);
            return out;
        }

        void EventSystem::_hover(Event::PointerMove& event, std::shared_ptr<IObject>& hover)
        {
            auto rootObject = getContext()->getRootObject();
            for (const auto & i : rootObject->getChildrenRecursiveT<UI::Window>())
            {
                _hover(i, event, hover);
                if (event.isAccepted())
                {
                    break;
                }
            }
        }

        void EventSystem::_hover(const std::shared_ptr<UI::Widget>& widget, Event::PointerMove& event, std::shared_ptr<IObject>& hover)
        {
            const auto children = widget->getChildrenRecursiveT<UI::Widget>();
            for (auto i = children.rbegin(); i != children.rend(); ++i)
            {
                if ((*i)->isVisible() &&
                    !(*i)->isClipped() &&
                    (*i)->getGeometry().contains(event.getPointerInfo().projectedPos))
                {
                    _hover(*i, event, hover);
                    if (event.isAccepted())
                    {
                        break;
                    }
                }
            }
            if (!event.isAccepted())
            {
                widget->event(event);
                if (event.isAccepted())
                {
                    hover = widget;
                }
            }
        }

        void EventSystem::_pointerCallback(GLFWwindow* window, double x, double y)
        {
            Context* context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
            if (auto system = context->getSystemT<EventSystem>().lock())
            {
                Event::PointerInfo info;
                info.id = pointerID;
                info.pos.x = static_cast<float>(x);
                info.pos.y = static_cast<float>(y);
                info.pos.z = 0.f;
                info.dir.x = 0.f;
                info.dir.y = 0.f;
                info.dir.z = 1.f;
                info.projectedPos = info.pos;
                system->_pointerMove(info);
            }
        }

        void EventSystem::_buttonCallback(GLFWwindow* window, int button, int action, int mods)
        {
            Context* context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
            if (auto system = context->getSystemT<EventSystem>().lock())
            {
                switch (action)
                {
                case GLFW_PRESS:   system->_buttonPress  (fromGLFWPointerButton(button)); break;
                case GLFW_RELEASE: system->_buttonRelease(fromGLFWPointerButton(button)); break;
                }
            }
        }

        void EventSystem::_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            Context* context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
            if (auto system = context->getSystemT<EventSystem>().lock())
            {
                switch (action)
                {
                case GLFW_PRESS:   system->_keyPress  (key, mods); break;
                case GLFW_RELEASE: system->_keyRelease(key, mods); break;
                }
            }
        }

        void EventSystem::_dropCallback(GLFWwindow* window, int count, const char** paths)
        {
            Context* context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
            if (auto system = context->getSystemT<EventSystem>().lock())
            {
                std::vector<std::string> list;
                for (int i = 0; i < count; ++i)
                {
                    list.push_back(paths[i]);
                }
                system->_drop(list);
            }
        }

    } // namespace Desktop
} // namespace djv
