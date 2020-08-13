// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvDesktopApp/EventSystem.h>

#include <djvUI/Style.h>
#include <djvUI/UISystem.h>
#include <djvUI/Widget.h>
#include <djvUI/Window.h>

#include <djvAV/OpenGLOffscreenBuffer.h>
#include <djvAV/Render2D.h>
#if defined(DJV_OPENGL_ES2)
#include <djvAV/OpenGLMesh.h>
#include <djvAV/OpenGLShader.h>
#include <djvAV/Shader.h>
#endif // DJV_OPENGL_ES2

#include <djvCore/Context.h>
#include <djvCore/Event.h>
#include <djvCore/IObject.h>
#if defined(DJV_OPENGL_ES2)
#include <djvCore/ResourceSystem.h>
#endif // DJV_OPENGL_ES2
#include <djvCore/Timer.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#if defined(DJV_OPENGL_ES2)
#include <glm/gtc/matrix_transform.hpp>
#endif // DJV_OPENGL_ES2

#include <codecvt>
#include <locale>

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
        {
            GLFWwindow * glfwWindow = nullptr;
            glm::ivec2 resize = glm::ivec2(0, 0);
            bool resizeRequest = true;
            glm::vec2 contentScale = glm::vec2(1.F, 1.F);
            bool redrawRequest = true;
            std::shared_ptr<AV::Render2D::Render> render;
            std::shared_ptr<AV::OpenGL::OffscreenBuffer> offscreenBuffer;
#if defined(DJV_OPENGL_ES2)
            std::shared_ptr<AV::OpenGL::Shader> shader;
#endif // DJV_OPENGL_ES2
            std::shared_ptr<Time::Timer> statsTimer;
        };

        void EventSystem::_init(GLFWwindow * glfwWindow, const std::shared_ptr<Context>& context)
        {
            UI::EventSystem::_init("djv::Desktop::EventSystem", context);

            DJV_PRIVATE_PTR();

            p.glfwWindow = glfwWindow;
            p.render = context->getSystemT<AV::Render2D::Render>();

            glm::vec2 contentScale = glm::vec2(1.F, 1.F);
            glfwGetWindowContentScale(p.glfwWindow, &contentScale.x, &contentScale.y);
            _contentScale(contentScale);

#if defined(DJV_OPENGL_ES2)
            auto resourceSystem = context->getSystemT<ResourceSystem>();
            const Core::FileSystem::Path shaderPath = resourceSystem->getPath(Core::FileSystem::ResourcePath::Shaders);
            p.shader = AV::OpenGL::Shader::create(AV::Render::Shader::create(
                Core::FileSystem::Path(shaderPath, "djvAVRender2DVertex.glsl"),
                Core::FileSystem::Path(shaderPath, "djvAVRender2DFragment.glsl")));
#endif // DJV_OPENGL_ES2

            glfwGetFramebufferSize(glfwWindow, &p.resize.x, &p.resize.y);
            {
                std::stringstream ss;
                ss << "Frame buffer size: " << p.resize;
                _log(ss.str());
            }
            glfwSetFramebufferSizeCallback(glfwWindow, _resizeCallback);
            glfwSetWindowContentScaleCallback(glfwWindow, _contentScaleCallback);
            glfwSetWindowRefreshCallback(glfwWindow, _redrawCallback);
            glfwSetWindowFocusCallback(p.glfwWindow, _focusCallback);
            glfwSetCursorEnterCallback(glfwWindow, _pointerEnterCallback);
            glfwSetCursorPosCallback(glfwWindow, _pointerCallback);
            glfwSetMouseButtonCallback(glfwWindow, _buttonCallback);
            glfwSetDropCallback(glfwWindow, _dropCallback);
            glfwSetKeyCallback(glfwWindow, _keyCallback);
            glfwSetCharModsCallback(glfwWindow, _charCallback);
            glfwSetScrollCallback(glfwWindow, _scrollCallback);

            p.statsTimer = Time::Timer::create(context);
            p.statsTimer->setRepeating(true);
            auto weak = std::weak_ptr<EventSystem>(std::dynamic_pointer_cast<EventSystem>(shared_from_this()));
            p.statsTimer->start(
                Time::getTime(Time::TimerValue::VerySlow),
                [weak](const std::chrono::steady_clock::time_point&, const Time::Duration&)
            {
                if (auto system = weak.lock())
                {
                    std::stringstream ss;
                    ss << "Frame buffer size: " << system->_p->resize;
                    system->_log(ss.str());
                }
            });
        }

        EventSystem::EventSystem() :
            _p(new Private)
        {}

        EventSystem::~EventSystem()
        {
            DJV_PRIVATE_PTR();
            glfwSetFramebufferSizeCallback(p.glfwWindow, nullptr);
            glfwSetWindowContentScaleCallback(p.glfwWindow, nullptr);
            glfwSetWindowRefreshCallback(p.glfwWindow, nullptr);
            glfwSetWindowFocusCallback(p.glfwWindow, nullptr);
            glfwSetCursorEnterCallback(p.glfwWindow, nullptr);
            glfwSetCursorPosCallback(p.glfwWindow, nullptr);
            glfwSetMouseButtonCallback(p.glfwWindow, nullptr);
            glfwSetDropCallback(p.glfwWindow, nullptr);
            glfwSetKeyCallback(p.glfwWindow, nullptr);
            glfwSetCharModsCallback(p.glfwWindow, nullptr);
            glfwSetScrollCallback(p.glfwWindow, nullptr);
        }

        std::shared_ptr<EventSystem> EventSystem::create(GLFWwindow * glfwWindow, const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<EventSystem>(new EventSystem);
            out->_init(glfwWindow, context);
            return out;
        }

        std::string EventSystem::getClipboard() const
        {
            DJV_PRIVATE_PTR();
            return glfwGetClipboardString(p.glfwWindow);
        }

        void EventSystem::setClipboard(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            glfwSetClipboardString(p.glfwWindow, value.c_str());
        }

        void EventSystem::tick()
        {
            UI::EventSystem::tick();
            DJV_PRIVATE_PTR();

            if (p.resizeRequest)
            {
                const AV::Image::Size size(p.resize.x, p.resize.y);
                if (size.isValid())
                {
                    p.offscreenBuffer = AV::OpenGL::OffscreenBuffer::create(size, AV::Image::Type::RGBA_U8);
                }
                else
                {
                    p.offscreenBuffer.reset();
                }
            }

            /*std::map<std::string, size_t> objectCounts;
            for (const auto& i : _getWindows())
            {
                if (auto window = i.lock())
                {
                    IObject::getObjectCounts(window, objectCounts);
                }
            }
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
                for (const auto& i : _getWindows())
                {
                    if (auto window = i.lock())
                    {
                        resizeRequest |= _resizeRequest(window);
                        redrawRequest |= _redrawRequest(window);
                    }
                }

                const auto& size = p.offscreenBuffer->getSize();
                if (resizeRequest)
                {
                    for (const auto& i : _getWindows())
                    {
                        if (auto window = i.lock())
                        {
                            window->resize(glm::vec2(size.w, size.h));

                            Event::InitLayout initLayout;
                            _initLayoutRecursive(window, initLayout);

                            Event::PreLayout preLayout;
                            _preLayoutRecursive(window, preLayout);

                            if (window->isVisible())
                            {
                                Event::Layout layout;
                                _layoutRecursive(window, layout);

                                Event::Clip clip(BBox2f(0.F, 0.F, static_cast<float>(size.w), static_cast<float>(size.h)));
                                _clipRecursive(window, clip);
                            }
                        }
                    }
                }

                if (resizeRequest || redrawRequest)
                {
                    p.offscreenBuffer->bind();
                    p.render->beginFrame(size);
                    for (const auto& i : _getWindows())
                    {
                        if (auto window = i.lock())
                        {
                            if (window->isVisible())
                            {
                                Event::Paint paintEvent(BBox2f(0.F, 0.F, static_cast<float>(size.w), static_cast<float>(size.h)));
                                Event::PaintOverlay paintOverlayEvent(BBox2f(0.F, 0.F, static_cast<float>(size.w), static_cast<float>(size.h)));
                                _paintRecursive(window, paintEvent, paintOverlayEvent);
                            }
                        }
                    }
                    p.render->endFrame();

                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                }
            }

            _redraw();
        }

        void EventSystem::_pushClipRect(const Core::BBox2f& value)
        {
            _p->render->pushClipRect(value);
        }

        void EventSystem::_popClipRect()
        {
            _p->render->popClipRect();
        }

        void EventSystem::_focus(bool value)
        {
            if (!value)
            {
                setTextFocus(nullptr);
            }
        }

        void EventSystem::_resize(const glm::ivec2& size)
        {
            DJV_PRIVATE_PTR();
            p.resize = size;
            p.resizeRequest = true;
        }

        void EventSystem::_contentScale(const glm::vec2& value)
        {
            if (auto context = getContext().lock())
            {
                DJV_PRIVATE_PTR();
                p.contentScale = value;
                const glm::vec2 dpi(AV::dpiDefault * p.contentScale.x, AV::dpiDefault * p.contentScale.y);
                {
                    std::stringstream ss;
                    ss << "Content scale: " << p.contentScale << std::endl;
                    ss << "DPI: " << dpi << std::endl;
                    _log(ss.str());
                }
                auto uiSystem = context->getSystemT<UI::UISystem>();
                uiSystem->getStyle()->setDPI(dpi);
            }
        }

        void EventSystem::_redraw()
        {
            DJV_PRIVATE_PTR();
            if (p.offscreenBuffer)
            {
                glDisable(GL_DEPTH_TEST);
                glDisable(GL_SCISSOR_TEST);
                glDisable(GL_BLEND);
                const auto& size = p.offscreenBuffer->getSize();
                glViewport(
                    0,
                    0,
                    GLsizei(size.w),
                    GLsizei(size.h));
#if defined(DJV_OPENGL_ES2)
                p.shader->bind();
                const auto viewMatrix = glm::ortho(
                    0.F,
                    static_cast<float>(size.w),
                    0.F,
                    static_cast<float>(size.h),
                    -1.F,
                    1.F);
                p.shader->setUniform("transform.mvp", viewMatrix);
                
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, p.offscreenBuffer->getColorID());
                
                p.shader->setUniform("transform.mvp", viewMatrix);
                p.shader->setUniform("imageFormat", 3);
                p.shader->setUniform("colorMode", 5);
                const GLfloat color[] = { 1.F, 1.F, 1.F, 1.F };
                p.shader->setUniform("color", color);
                p.shader->setUniform("textureSampler", 0);
                                
                auto vbo = AV::OpenGL::VBO::create(2 * 4, AV::OpenGL::VBOType::Pos2_F32_UV_U16);
                std::vector<uint8_t> vboData(6 * (2 * 4 + 2 * 2));
                struct Data
                {
                    float x;
                    float y;
                    uint16_t u;
                    uint16_t v;
                };
                Data* vboP = reinterpret_cast<Data*>(&vboData[0]);
                vboP->x = 0.F;
                vboP->y = 0.F;
                vboP->u = 0.F;
                vboP->v = 0.F;
                ++vboP;
                vboP->x = size.w;
                vboP->y = 0.F;
                vboP->u = 65535;
                vboP->v = 0;
                ++vboP;
                vboP->x = size.w;
                vboP->y = size.h;
                vboP->u = 65535;
                vboP->v = 65535;
                ++vboP;
                vboP->x = size.w;
                vboP->y = size.h;
                vboP->u = 65535;
                vboP->v = 65535;
                ++vboP;
                vboP->x = 0.F;
                vboP->y = size.h;
                vboP->u = 0;
                vboP->v = 65535;
                ++vboP;
                vboP->x = 0.F;
                vboP->y = 0.F;
                vboP->u = 0;
                vboP->v = 0;
                ++vboP;
                vbo->copy(vboData);
                auto vao = AV::OpenGL::VAO::create(AV::OpenGL::VBOType::Pos2_F32_UV_U16, vbo->getID());
                vao->draw(GL_TRIANGLES, 0, 6);
#else // DJV_OPENGL_ES2
                glBindFramebuffer(GL_READ_FRAMEBUFFER, p.offscreenBuffer->getID());
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
                glBlitFramebuffer(
                    0, 0, size.w, size.h,
                    0, 0, size.w, size.h,
                    GL_COLOR_BUFFER_BIT,
                    GL_NEAREST);
#endif // DJV_OPENGL_ES2
            }
        }

        void EventSystem::_hover(Event::PointerMove& event, std::shared_ptr<IObject>& hover)
        {
            const auto windows = _getWindows();
            for (auto i = windows.rbegin(); i != windows.rend(); ++i)
            {
                if (auto window = i->lock())
                {
                    if (window->isVisible())
                    {
                        _hover(window, event, hover);
                        if (event.isAccepted())
                        {
                            break;
                        }
                    }
                }
            }
        }

        void EventSystem::_hover(const std::shared_ptr<UI::Widget>& widget, Event::PointerMove& event, std::shared_ptr<IObject>& hover)
        {
            const auto children = widget->getChildWidgets();
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

        void EventSystem::_focusCallback(GLFWwindow* window, int value)
        {
            if (Context* context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window)))
            {
                if (auto system = context->getSystemT<EventSystem>())
                {
                    system->_focus(GL_TRUE == value);
                }
            }
        }

        void EventSystem::_resizeCallback(GLFWwindow * window, int width, int height)
        {
            if (Context* context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window)))
            {
                if (auto system = context->getSystemT<EventSystem>())
                {
                    system->_resize(glm::ivec2(width, height));
                }
            }
        }

        void EventSystem::_contentScaleCallback(GLFWwindow* window, float scaleX, float scaleY)
        {
            if (Context* context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window)))
            {
                if (auto system = context->getSystemT<EventSystem>())
                {
                    system->_contentScale(glm::vec2(scaleX, scaleY));
                }
            }
        }

        void EventSystem::_redrawCallback(GLFWwindow * window)
        {
            if (Context* context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window)))
            {
                if (auto system = context->getSystemT<EventSystem>())
                {
                    system->_redraw();
                }
            }
        }

        void EventSystem::_pointerEnterCallback(GLFWwindow* window, int value)
        {
            if (Context* context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window)))
            {
                if (auto system = context->getSystemT<EventSystem>())
                {
                    if (GLFW_FALSE == value)
                    {
                        Event::PointerInfo info;
                        info.id = pointerID;
                        info.pos.x = -1.F;
                        info.pos.y = -1.F;
                        info.pos.z = 0.F;
                        info.dir.x = 0.F;
                        info.dir.y = 0.F;
                        info.dir.z = 1.F;
                        info.projectedPos = info.pos;
                        system->_pointerMove(info);
                    }
                }
            }
        }

        void EventSystem::_pointerCallback(GLFWwindow * window, double x, double y)
        {
            if (Context* context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window)))
            {
                if (auto system = context->getSystemT<EventSystem>())
                {
                    Event::PointerInfo info;
                    info.id = pointerID;
                    info.pos.x = static_cast<float>(x);
                    info.pos.y = static_cast<float>(y);
#if defined(DJV_PLATFORM_MACOS)
                    info.pos.x *= system->_p->contentScale.x;
                    info.pos.y *= system->_p->contentScale.y;
#endif // DJV_PLATFORM_MACOS
                    info.pos.z = 0.F;
                    info.dir.x = 0.F;
                    info.dir.y = 0.F;
                    info.dir.z = 1.F;
                    info.projectedPos = info.pos;
                    system->_pointerMove(info);
                }
            }
        }

        void EventSystem::_buttonCallback(GLFWwindow * window, int button, int action, int modifiers)
        {
            if (Context* context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window)))
            {
                if (auto system = context->getSystemT<EventSystem>())
                {
                    switch (action)
                    {
                    case GLFW_PRESS:   system->_buttonPress  (fromGLFWPointerButton(button)); break;
                    case GLFW_RELEASE: system->_buttonRelease(fromGLFWPointerButton(button)); break;
                    default: break;
                    }
                }
            }
        }

        void EventSystem::_dropCallback(GLFWwindow * window, int count, const char ** paths)
        {
            if (Context* context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window)))
            {
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
        }

        void EventSystem::_keyCallback(GLFWwindow * window, int key, int scancode, int action, int modifiers)
        {
            if (Context* context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window)))
            {
                if (auto system = context->getSystemT<EventSystem>())
                {
                    switch (action)
                    {
                    case GLFW_PRESS:   system->_keyPress  (key, modifiers); break;
                    case GLFW_REPEAT:  system->_keyPress  (key, modifiers); break;
                    case GLFW_RELEASE: system->_keyRelease(key, modifiers); break;
                    default: break;
                    }
                }
            }
        }

        void EventSystem::_charCallback(GLFWwindow * window, unsigned int character, int modifiers)
        {
            if (Context* context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window)))
            {
                if (auto system = context->getSystemT<EventSystem>())
                {
                    std::basic_string<djv_char_t> utf32;
                    utf32.push_back(character);
                    system->_text(utf32, modifiers);
                }
            }
        }

        void EventSystem::_scrollCallback(GLFWwindow * window, double x, double y)
        {
            if (Context* context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window)))
            {
                if (auto system = context->getSystemT<EventSystem>())
                {
                    system->_scroll(x, y);
                }
            }
        }

    } // namespace Desktop
} // namespace djv
