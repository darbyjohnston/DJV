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

#include <djvUI/Style.h>
#include <djvUI/StyleSettings.h>
#include <djvUI/Widget.h>
#include <djvUI/Window.h>

#include <djvAV/OpenGLOffscreenBuffer.h>
#include <djvAV/Render2DSystem.h>

#include <GLFW/glfw3.h>

//#pragma optimize("", off)

using namespace djv::Core;

using namespace gl;

namespace djv
{
    namespace Desktop
    {
        struct WindowSystem::Private
        {
            GLFWwindow * glfwWindow = nullptr;
            glm::ivec2 resize = glm::ivec2(0, 0);
            bool resizeRequest = false;
            bool redrawRequest = false;
            std::shared_ptr<AV::OpenGL::OffscreenBuffer> offscreenBuffer;
        };

        void WindowSystem::_init(GLFWwindow * glfwWindow, Context * context)
        {
            IWindowSystem::_init("djv::Desktop::WindowSystem", context);

            _p->glfwWindow = glfwWindow;
            glfwSetFramebufferSizeCallback(glfwWindow, _resizeCallback);
            glfwSetWindowRefreshCallback(glfwWindow, _redrawCallback);
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

        void WindowSystem::tick(float dt)
        {
            IWindowSystem::tick(dt);

            if (_p->resizeRequest)
            {
                _p->offscreenBuffer = AV::OpenGL::OffscreenBuffer::create(AV::Image::Info(_p->resize, AV::Image::Type::RGBA_U8));
            }
            auto rootObject = getContext()->getRootObject();
            if (_p->offscreenBuffer)
            {
                bool resizeRequest = _p->resizeRequest;
                bool redrawRequest = _p->redrawRequest;
                _p->resizeRequest = false;
                _p->redrawRequest = false;
                for (const auto& i : rootObject->getChildrenT<UI::Window>())
                {
                    resizeRequest |= _resizeRequest(i);
                    redrawRequest |= _redrawRequest(i);
                }

                const auto & size = _p->offscreenBuffer->getInfo().size;
                if (resizeRequest)
                {
                    for (const auto& i : rootObject->getChildrenT<UI::Window>())
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
                    if (auto system = getContext()->getSystemT<AV::Render::Render2DSystem>().lock())
                    {
                        _p->offscreenBuffer->bind();
                        system->beginFrame(size);
                        for (const auto& i : rootObject->getChildrenT<UI::Window>())
                        {
                            if (i->isVisible())
                            {
                                Event::Paint paintEvent(BBox2f(0.f, 0.f, static_cast<float>(size.x), static_cast<float>(size.y)));
                                _paintRecursive(i, paintEvent);
                            }
                        }
                        system->endFrame();
                        _p->offscreenBuffer->unbind();
                        _redraw();
                    }
                }
            }
        }

        void WindowSystem::_addWindow(const std::shared_ptr<UI::Window>& window)
        {
            IWindowSystem::_addWindow(window);
            window->setParent(getContext()->getRootObject());
        }

        void WindowSystem::_removeWindow(const std::shared_ptr<UI::Window>& window)
        {
            IWindowSystem::_removeWindow(window);
            window->setParent(nullptr);
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

        void WindowSystem::_resize(const glm::ivec2 & size)
        {
            _p->resize = size;
            _p->resizeRequest = true;
        }

        void WindowSystem::_redraw()
        {
            DJV_PRIVATE_PTR();
            if (p.offscreenBuffer)
            {
                glDisable(GL_SCISSOR_TEST);
                glDisable(GL_BLEND);
                const auto & size = p.offscreenBuffer->getInfo().size;
                gl::glViewport(
                    0,
                    0,
                    GLsizei(size.x),
                    GLsizei(size.y));
                gl::glClearColor(0.f, 0.f, 0.f, 0.f);
                glClear(GL_COLOR_BUFFER_BIT);
                glBindFramebuffer(gl::GLenum(GL_READ_FRAMEBUFFER), p.offscreenBuffer->getID());
                glBindFramebuffer(gl::GLenum(GL_DRAW_FRAMEBUFFER), gl::GLuint(0));
                glBlitFramebuffer(
                    0, 0, size.x, size.y,
                    0, 0, size.x, size.y,
                    ClearBufferMask(GL_COLOR_BUFFER_BIT),
                    gl::GLenum(GL_NEAREST));
                glfwSwapBuffers(p.glfwWindow);
            }
        }

        void WindowSystem::_resizeCallback(GLFWwindow* window, int width, int height)
        {
            Context* context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
            if (auto system = context->getSystemT<WindowSystem>().lock())
            {
                system->_resize(glm::ivec2(width, height));
            }
        }

        void WindowSystem::_redrawCallback(GLFWwindow* window)
        {
            Context* context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
            if (auto system = context->getSystemT<WindowSystem>().lock())
            {
                system->_redraw();
            }
        }

    } // namespace Desktop
} // namespace djv
