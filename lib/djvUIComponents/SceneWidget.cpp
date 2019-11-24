//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
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

#include <djvUIComponents/SceneWidget.h>

#include <djvScene/Camera.h>
#include <djvScene/Render.h>
#include <djvScene/Scene.h>

#include <djvAV/OpenGLOffscreenBuffer.h>
#include <djvAV/Render2D.h>
#include <djvAV/Render3D.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct SceneWidget::Private
        {
            std::shared_ptr<AV::Render3D::Render> render3D;
            AV::Image::Size size;
            std::shared_ptr<Scene::Scene> scene;
            std::shared_ptr<Scene::PolarCamera> camera;
            std::shared_ptr<Scene::Render> render;
            std::shared_ptr<AV::OpenGL::OffscreenBuffer> offscreenBuffer;
            Event::PointerID pressedID = Event::InvalidID;
            std::map<int, bool> buttons;
            glm::vec2 pointerPos = glm::vec2(0.F, 0.F);
        };

        void SceneWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();
            setClassName("djv::UI::SceneWidget");
            setPointerEnabled(true);
            p.render3D = context->getSystemT<AV::Render3D::Render>();
            p.camera = Scene::PolarCamera::create();
            p.render = Scene::Render::create(context);
        }

        SceneWidget::SceneWidget() :
            _p(new Private)
        {}

        SceneWidget::~SceneWidget()
        {}

        std::shared_ptr<SceneWidget> SceneWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<SceneWidget>(new SceneWidget);
            out->_init(context);
            return out;
        }

        void SceneWidget::setScene(const std::shared_ptr<Scene::Scene>& value)
        {
            DJV_PRIVATE_PTR();
            p.scene = value;
            p.render->setScene(p.scene);
            _redraw();
        }

        size_t SceneWidget::getTriangleCount() const
        {
            return _p->render->getTriangleCount();
        }

        void SceneWidget::_layoutEvent(Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            const BBox2f& g = getGeometry();
            p.size.w = g.w();
            p.size.h = g.h();
            p.camera->setAspect(p.size.w / static_cast<float>(p.size.h > 0 ? p.size.h : 0));
        }

        void SceneWidget::_paintEvent(Event::Paint&)
        {
            DJV_PRIVATE_PTR();
            if (p.offscreenBuffer)
            {
                const BBox2f& g = getGeometry();
                auto& render = _getRender();
                render->setFillColor(AV::Image::Color(1.F, 1.F, 1.F));
                render->drawTexture(g, p.offscreenBuffer->getColorID());
            }
        }

        void SceneWidget::_pointerMoveEvent(Event::PointerMove & event)
        {
            Widget::_pointerMoveEvent(event);
            DJV_PRIVATE_PTR();
            if (p.pressedID)
            {
                event.accept();
                const auto& pointerInfo = event.getPointerInfo();
                auto i = p.buttons.find(1);
                if (i != p.buttons.end())
                {
                    p.camera->setLongitude(p.camera->getLongitude() + (pointerInfo.projectedPos.x - p.pointerPos.x) * .5F);
                    const float latitude = p.camera->getLatitude() + (pointerInfo.projectedPos.y - p.pointerPos.y) * .5F;
                    p.camera->setLatitude(Math::clamp(latitude, -89.F, 89.F));
                }
                i = p.buttons.find(2);
                if (i != p.buttons.end())
                {
                    const float distance = p.camera->getDistance() -
                        (pointerInfo.projectedPos.x - p.pointerPos.x) * .5F +
                        (pointerInfo.projectedPos.y - p.pointerPos.y) * .5F;
                    p.camera->setDistance(std::max(distance, 1.F));
                }
                p.pointerPos = pointerInfo.projectedPos;
                _redraw();
            }
        }

        void SceneWidget::_buttonPressEvent(Event::ButtonPress & event)
        {
            DJV_PRIVATE_PTR();
            if (p.pressedID)
                return;
            event.accept();
            const auto& pointerInfo = event.getPointerInfo();
            p.pressedID = pointerInfo.id;
            p.buttons = pointerInfo.buttons;
            p.pointerPos = pointerInfo.projectedPos;
        }

        void SceneWidget::_buttonReleaseEvent(Event::ButtonRelease & event)
        {
            DJV_PRIVATE_PTR();
            const auto& pointerInfo = event.getPointerInfo();
            if (pointerInfo.id == p.pressedID)
            {
                event.accept();
                p.pressedID = Event::InvalidID;
                p.buttons.clear();
            }
        }

        void SceneWidget::_updateEvent(Event::Update&)
        {
            DJV_PRIVATE_PTR();
            if (p.size.isValid())
            {
                bool create = !p.offscreenBuffer;
                create |= p.offscreenBuffer && p.size != p.offscreenBuffer->getSize();
                if (create)
                {
                    p.offscreenBuffer = AV::OpenGL::OffscreenBuffer::create(
                        p.size,
                        AV::Image::Type::RGBA_U8,
                        AV::OpenGL::OffscreenDepthType::_24);
                }
            }
            else
            {
                p.offscreenBuffer.reset();
            }
            if (p.offscreenBuffer)
            {
                const AV::OpenGL::OffscreenBufferBinding binding(p.offscreenBuffer);
                Scene::RenderOptions options;
                options.size = p.size;
                options.camera = p.camera;
                p.render->render(p.render3D, options);
            }
        }

    } // namespace UI
} // namespace djv
