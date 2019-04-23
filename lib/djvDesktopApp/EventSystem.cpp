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

#include <djvDesktopApp/EventSystem.h>

#include <djvUI/Style.h>
#include <djvUI/StyleSettings.h>
#include <djvUI/UISystem.h>
#include <djvUI/Widget.h>
#include <djvUI/Window.h>

#include <djvAV/OpenGLOffscreenBuffer.h>
#include <djvAV/Render2D.h>

#include <djvCore/Context.h>
#include <djvCore/Event.h>
#include <djvCore/IObject.h>

#include <GLFW/glfw3.h>

#include <codecvt>
#include <locale>

#undef GL_BLEND
#undef GL_COLOR_BUFFER_BIT
#undef GL_SCISSOR_TEST

using namespace djv::Core;

using namespace gl;

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
        {
            GLFWwindow * glfwWindow = nullptr;
            glm::ivec2 resize = glm::ivec2(0, 0);
            bool resizeRequest = true;
            bool redrawRequest = true;
            std::shared_ptr<AV::Render::Render2D> render;
            std::shared_ptr<AV::OpenGL::OffscreenBuffer> offscreenBuffer;
        };

        void EventSystem::_init(GLFWwindow * glfwWindow, Context * context)
        {
            UI::EventSystem::_init("djv::Desktop::EventSystem", context);

            DJV_PRIVATE_PTR();

            p.glfwWindow = glfwWindow;
            p.render = context->getSystemT<AV::Render::Render2D>();

            addDependency(context->getSystemT<UI::UISystem>());

            glfwGetFramebufferSize(glfwWindow, &p.resize.x, &p.resize.y);
            glfwSetFramebufferSizeCallback(glfwWindow, _resizeCallback);
            glfwSetWindowRefreshCallback(glfwWindow, _redrawCallback);
            glfwSetCursorPosCallback(glfwWindow, _pointerCallback);
            glfwSetMouseButtonCallback(glfwWindow, _buttonCallback);
            glfwSetDropCallback(glfwWindow, _dropCallback);
            glfwSetKeyCallback(glfwWindow, _keyCallback);
            glfwSetCharModsCallback(glfwWindow, _charCallback);
        }

        EventSystem::EventSystem() :
            _p(new Private)
        {}

        EventSystem::~EventSystem()
        {}

        std::shared_ptr<EventSystem> EventSystem::create(GLFWwindow * glfwWindow, Context * context)
        {
            auto out = std::shared_ptr<EventSystem>(new EventSystem);
            out->_init(glfwWindow, context);
            return out;
        }

        void EventSystem::tick(float dt)
        {
            UI::EventSystem::tick(dt);

            DJV_PRIVATE_PTR();
            if (p.resizeRequest)
            {
                p.offscreenBuffer = AV::OpenGL::OffscreenBuffer::create(
                    AV::Image::Info(p.resize, AV::Image::Type::RGBA_U8),
                    AV::OpenGL::OffscreenType::MultiSample);
            }
            auto rootObject = getRootObject();
        
            /*std::map<std::string, size_t> objectCounts;
            IObject::getObjectCounts(rootObject, objectCounts);
            size_t totalObjectCount = 0;
            for (const auto& i : objectCounts)
            {
                std::cout << i.first << ": " << i.second << std::endl;
                totalObjectCount += i.second;
            }
            std::cout << "Total: " << totalObjectCount << std::endl;
            std::cout << "Global object count: " << IObject::getGlobalObjectCount() << std::endl;
            std::cout << "Global widget count: " << UI::Widget::getGlobalWidgetCount() << std::endl;
            std::cout << std::endl;*/

            if (p.offscreenBuffer)
            {
                bool resizeRequest = p.resizeRequest;
                bool redrawRequest = p.redrawRequest;
                p.resizeRequest = false;
                p.redrawRequest = false;
                for (const auto & i : rootObject->getChildrenT<UI::Window>())
                {
                    resizeRequest |= _resizeRequest(i);
                    redrawRequest |= _redrawRequest(i);
                }

                const auto & size = p.offscreenBuffer->getInfo().size;
                if (resizeRequest)
                {
                    for (const auto & i : rootObject->getChildrenT<UI::Window>())
                    {
                        i->resize(size);

                        Event::PreLayout preLayout;
                        _preLayoutRecursive(i, preLayout);

                        if (i->isVisible())
                        {
                            Event::Layout layout;
                            _layoutRecursive(i, layout);

                            Event::Clip clip(BBox2f(0.f, 0.f, static_cast<float>(size.x), static_cast<float>(size.y)));
                            _clipRecursive(i, clip);
                        }
                    }
                }

                if (resizeRequest || redrawRequest)
                {
                    p.offscreenBuffer->bind();
                    p.render->beginFrame(size);
                    for (const auto & i : rootObject->getChildrenT<UI::Window>())
                    {
                        if (i->isVisible())
                        {
                            Event::Paint paintEvent(BBox2f(0.f, 0.f, static_cast<float>(size.x), static_cast<float>(size.y)));
                            Event::PaintOverlay paintOverlayEvent(BBox2f(0.f, 0.f, static_cast<float>(size.x), static_cast<float>(size.y)));
                            _paintRecursive(i, paintEvent, paintOverlayEvent);
                        }
                    }
                    p.render->endFrame();
                    p.offscreenBuffer->unbind();
                    _redraw();
                }
            }
        }

        void EventSystem::_pushClipRect(const Core::BBox2f & value)
        {
            _p->render->pushClipRect(value);
        }

        void EventSystem::_popClipRect()
        {
            _p->render->popClipRect();
        }

        void EventSystem::_initObject(const std::shared_ptr<IObject> & object)
        {
            UI::EventSystem::_initObject(object);
            if (auto window = std::dynamic_pointer_cast<UI::Window>(object))
            {
                getRootObject()->addChild(window);
            }
        }

        void EventSystem::_resize(const glm::ivec2 & size)
        {
            DJV_PRIVATE_PTR();
            p.resize = size;
            p.resizeRequest = true;
        }

        void EventSystem::_redraw()
        {
            DJV_PRIVATE_PTR();
            if (p.offscreenBuffer)
            {
                gl::glDisable(gl::GL_SCISSOR_TEST);
                gl::glDisable(gl::GL_BLEND);
                const auto & size = p.offscreenBuffer->getInfo().size;
                gl::glViewport(
                    0,
                    0,
                    GLsizei(size.x),
                    GLsizei(size.y));
                gl::glClearColor(0.f, 0.f, 0.f, 0.f);
                gl::glClear(gl::GL_COLOR_BUFFER_BIT);
                glBindFramebuffer(gl::GLenum(GL_READ_FRAMEBUFFER), p.offscreenBuffer->getID());
                glBindFramebuffer(gl::GLenum(GL_DRAW_FRAMEBUFFER), gl::GLuint(0));
                glBlitFramebuffer(
                    0, 0, size.x, size.y,
                    0, 0, size.x, size.y,
                    ClearBufferMask(gl::GL_COLOR_BUFFER_BIT),
                    gl::GLenum(GL_NEAREST));
                //gl::glFlush();
                glfwSwapBuffers(p.glfwWindow);
            }
        }

        void EventSystem::_hover(Event::PointerMove & event, std::shared_ptr<IObject> & hover)
        {
            auto rootObject = getRootObject();
            for (const auto & i : rootObject->getChildrenT<UI::Window>())
            {
                _hover(i, event, hover);
                if (event.isAccepted())
                {
                    break;
                }
            }
        }

        void EventSystem::_hover(const std::shared_ptr<UI::Widget> & widget, Event::PointerMove & event, std::shared_ptr<IObject> & hover)
        {
            const auto children = widget->getChildrenT<UI::Widget>();
            for (auto i = children.rbegin(); i != children.rend(); ++i)
            {
                if ((*i)->isVisible() &&
                    !(*i)->isClipped() &&
                    (*i)->getClipRect().contains(event.getPointerInfo().projectedPos))
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

        void EventSystem::_resizeCallback(GLFWwindow * window, int width, int height)
        {
            Context * context = reinterpret_cast<Context *>(glfwGetWindowUserPointer(window));
            if (auto system = context->getSystemT<EventSystem>())
            {
                system->_resize(glm::ivec2(width, height));
            }
        }

        void EventSystem::_redrawCallback(GLFWwindow * window)
        {
            Context * context = reinterpret_cast<Context *>(glfwGetWindowUserPointer(window));
            if (auto system = context->getSystemT<EventSystem>())
            {
                system->_redraw();
            }
        }

        void EventSystem::_pointerCallback(GLFWwindow * window, double x, double y)
        {
            Context * context = reinterpret_cast<Context *>(glfwGetWindowUserPointer(window));
            if (auto system = context->getSystemT<EventSystem>())
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

        void EventSystem::_buttonCallback(GLFWwindow * window, int button, int action, int modifiers)
        {
            Context * context = reinterpret_cast<Context *>(glfwGetWindowUserPointer(window));
            if (auto system = context->getSystemT<EventSystem>())
            {
                switch (action)
                {
                case GLFW_PRESS:   system->_buttonPress  (fromGLFWPointerButton(button)); break;
                case GLFW_RELEASE: system->_buttonRelease(fromGLFWPointerButton(button)); break;
                }
            }
        }

        void EventSystem::_dropCallback(GLFWwindow * window, int count, const char ** paths)
        {
            Context * context = reinterpret_cast<Context *>(glfwGetWindowUserPointer(window));
            if (auto system = context->getSystemT<EventSystem>())
            {
                std::vector<std::string> list;
                for (int i = 0; i < count; ++i)
                {
                    list.push_back(paths[i]);
                }
                system->_drop(list);
            }
        }

        void EventSystem::_keyCallback(GLFWwindow * window, int key, int scancode, int action, int modifiers)
        {
            Context * context = reinterpret_cast<Context *>(glfwGetWindowUserPointer(window));
            if (auto system = context->getSystemT<EventSystem>())
            {
                switch (action)
                {
                case GLFW_PRESS:   system->_keyPress  (key, modifiers); break;
                case GLFW_REPEAT:  system->_keyPress  (key, modifiers); break;
                case GLFW_RELEASE: system->_keyRelease(key, modifiers); break;
                }
            }
        }

        void EventSystem::_charCallback(GLFWwindow * window, unsigned int character, int modifiers)
        {
            Context * context = reinterpret_cast<Context *>(glfwGetWindowUserPointer(window));
            if (auto system = context->getSystemT<EventSystem>())
            {
                std::string text;
                try
                {
                    std::wstring_convert<std::codecvt_utf8<djv_char_t>, djv_char_t> utf32;
                    text = utf32.to_bytes(character);
                }
                catch (const std::exception & e)
                {
                    std::stringstream ss;
                    ss << "Error converting character" << " '" << character << "': " << e.what();
                    system->_log(ss.str(), LogLevel::Error);
                }
                system->_text(text, modifiers);
            }
        }

    } // namespace Desktop
} // namespace djv
