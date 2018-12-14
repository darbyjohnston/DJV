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

#include <djvDesktop/WindowSystem.h>

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
            const PointerID pointerID = 1;

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
        {
            glm::vec2 pointerPos;
            std::shared_ptr<IObject> hover;
            std::shared_ptr<IObject> grab;
            std::shared_ptr<IObject> focus;
        };

        void EventSystem::_init(GLFWwindow* glfwWindow, Context * context)
        {
            IEventSystem::_init("djv::Desktop::EventSystem", context);

            glfwSetWindowUserPointer(glfwWindow, this);
            glfwSetCursorPosCallback(glfwWindow, _pointerCallback);
            glfwSetMouseButtonCallback(glfwWindow, _buttonCallback);
            glfwSetDropCallback(glfwWindow, _dropCallback);
        }

        EventSystem::EventSystem() :
            _p(new Private)
        {}

        EventSystem::~EventSystem()
        {}

        void EventSystem::_pointerMove(const PointerInfo& info)
        {
            DJV_PRIVATE_PTR();
            PointerMoveEvent moveEvent(info);
            if (p.grab)
            {
                p.grab->event(moveEvent);
            }
            else if (auto windowSystem = getContext()->getSystemT<WindowSystem>().lock())
            {
                std::shared_ptr<UI::Widget> hover;
                for (const auto& window : windowSystem->getWindows())
                {
                    if (window->isVisible() && window->getGeometry().contains(info.pos))
                    {
                        _hover(window, moveEvent, hover);
                        break;
                    }
                }
                /*if (hover)
                {
                    std::stringstream ss;
                    ss << "Hover: " << hover->getClassName();
                    getContext()->log("djv::Core::IEventSystem", ss.str());
                }*/
                if (hover != p.hover)
                {
                    if (p.hover)
                    {
                        PointerLeaveEvent leaveEvent(info);
                        p.hover->event(leaveEvent);
                        /*{
                            std::stringstream ss;
                            ss << "Leave: " << p.hover->getClassName();
                            getContext()->log("djv::Core::IEventSystem", ss.str());
                        }*/
                    }
                    p.hover = hover;
                    if (p.hover)
                    {
                        PointerEnterEvent enterEvent(info);
                        p.hover->event(enterEvent);
                        /*{
                            std::stringstream ss;
                            ss << "Enter: " << p.hover->getClassName();
                            getContext()->log("djv::Core::IEventSystem", ss.str());
                        }*/
                    }
                }
            }
        }

        void EventSystem::_buttonPress(const PointerInfo& info)
        {
            DJV_PRIVATE_PTR();
            ButtonPressEvent event(info);
            if (p.hover)
            {
                p.hover->event(event);
                if (event.isAccepted())
                {
                    p.grab = p.hover;
                }
            }
        }

        void EventSystem::_buttonRelease(const PointerInfo& info)
        {
            DJV_PRIVATE_PTR();
            ButtonReleaseEvent event(info);
            if (p.grab)
            {
                p.grab->event(event);
                p.grab = nullptr;
            }
        }

        void EventSystem::_hover(const std::shared_ptr<UI::Widget>& widget, PointerMoveEvent& event, std::shared_ptr<UI::Widget>& hover)
        {
            const auto children = widget->getChildrenT<UI::Widget>();
            for (auto i = children.rbegin(); i != children.rend(); ++i)
            {
                if ((*i)->isVisible() &&
                    !(*i)->isClipped() &&
                    (*i)->isEnabled() &&
                    (*i)->getGeometry().contains(event.getPointerInfo().projectedPos))
                {
                    _hover(*i, event, hover);
                    break;
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

        std::shared_ptr<EventSystem> EventSystem::create(GLFWwindow* glfwWindow, Context * context)
        {
            auto out = std::shared_ptr<EventSystem>(new EventSystem);
            out->_init(glfwWindow, context);
            return out;
        }

        void EventSystem::_pointerCallback(GLFWwindow* window, double x, double y)
        {
            EventSystem* system = reinterpret_cast<EventSystem*>(glfwGetWindowUserPointer(window));
            system->_p->pointerPos = glm::vec2(static_cast<float>(x), static_cast<float>(y));
            PointerInfo info;
            info.id = pointerID;
            info.pos.x = system->_p->pointerPos.x;
            info.pos.y = system->_p->pointerPos.y;
            info.pos.z = 0.f;
            info.dir.x = 0.f;
            info.dir.y = 0.f;
            info.dir.z = 1.f;
            info.projectedPos = info.pos;
            system->_pointerMove(info);
        }

        void EventSystem::_buttonCallback(GLFWwindow* window, int button, int action, int mods)
        {
            EventSystem* system = reinterpret_cast<EventSystem*>(glfwGetWindowUserPointer(window));
            PointerInfo info;
            info.id = pointerID;
            info.pos.x = system->_p->pointerPos.x;
            info.pos.y = system->_p->pointerPos.y;
            info.pos.z = 0.f;
            info.dir.x = 0.f;
            info.dir.y = 0.f;
            info.dir.z = 1.f;
            info.projectedPos = info.pos;
            info.button = fromGLFWPointerButton(button);
            switch (action)
            {
            case GLFW_PRESS:
                system->_buttonPress(info);
                break;
            case GLFW_RELEASE:
                system->_buttonRelease(info);
                break;
            }
        }

        void EventSystem::_dropCallback(GLFWwindow* window, int count, const char** paths)
        {
            EventSystem* system = reinterpret_cast<EventSystem*>(glfwGetWindowUserPointer(window));
            std::vector<std::string> list;
            for (int i = 0; i < count; ++i)
            {
                list.push_back(paths[i]);
            }
        }

    } // namespace Desktop
} // namespace djv
