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
#include <djvCore/Timer.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            //! \todo Should these be configurable?
            const float rotationSensitivity = .5F;
            const float zoomSensitivity     = .1F;
            const float nearMult            = .0001F;
            const float farMult             = 10.F;
        
        } // namespace

        bool SceneRenderOptions::operator == (const SceneRenderOptions& other) const
        {
            return
                shaderMode == other.shaderMode &&
                depthBufferMode == other.depthBufferMode &&
                depthBufferType == other.depthBufferType &&
                multiSampling == other.multiSampling;
        }

        struct SceneWidget::Private
        {
            std::shared_ptr<AV::Render3D::Render> render3D;
            AV::Image::Size size;
            std::shared_ptr<Scene::Scene> scene;
            std::shared_ptr<ValueSubject<SceneRotate> > sceneRotate;
            std::shared_ptr<Scene::PolarCamera> camera;
            std::shared_ptr<ValueSubject<Scene::PolarCameraData> > cameraData;
            std::shared_ptr<ValueSubject<SceneRenderOptions> > renderOptions;
            std::shared_ptr<Scene::Render> render;
            std::shared_ptr<AV::OpenGL::OffscreenBuffer> offscreenBuffer;
            std::shared_ptr<AV::OpenGL::OffscreenBuffer> offscreenBuffer2;
            Event::PointerID pressedID = Event::InvalidID;
            std::map<int, bool> buttons;
            glm::vec2 pointerPos = glm::vec2(0.F, 0.F);
            std::shared_ptr<ValueSubject<BBox3f> > bbox;
            std::shared_ptr<ValueSubject<size_t> > primitivesCount;
            std::shared_ptr<ValueSubject<size_t> > pointCount;
            std::shared_ptr<djv::Core::Time::Timer> statsTimer;
        };

        void SceneWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::UI::SceneWidget");
            setPointerEnabled(true);

            p.sceneRotate = ValueSubject<SceneRotate>::create(SceneRotate::None);
            p.render3D = context->getSystemT<AV::Render3D::Render>();
            p.camera = Scene::PolarCamera::create();
            p.cameraData = ValueSubject<Scene::PolarCameraData>::create(p.camera->getData());
            p.renderOptions = ValueSubject<SceneRenderOptions>::create(SceneRenderOptions());
            p.render = Scene::Render::create(context);
            p.bbox = ValueSubject<BBox3f>::create(BBox3f(0.F, 0.F, 0.F, 0.F, 0.F, 0.F));
            p.primitivesCount = ValueSubject<size_t>::create(0);
            p.pointCount = ValueSubject<size_t>::create(0);

            p.statsTimer = Core::Time::Timer::create(context);
            p.statsTimer->setRepeating(true);
            auto weak = std::weak_ptr<SceneWidget>(std::dynamic_pointer_cast<SceneWidget>(shared_from_this()));
            p.statsTimer->start(
                Core::Time::getMilliseconds(Core::Time::TimerValue::Slow),
                [weak](float)
                {
                    if (auto widget = weak.lock())
                    {
                        BBox3f bbox(0.F, 0.F, 0.F, 0.F, 0.F, 0.F);
                        if (widget->_p->scene)
                        {
                            bbox = widget->_p->scene->getBBox();
                        }
                        widget->_p->bbox->setIfChanged(bbox);
                        widget->_p->primitivesCount->setIfChanged(widget->_p->render->getPrimitivesCount());
                        widget->_p->pointCount->setIfChanged(widget->_p->render->getPointCount());
                    }
                });
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
            _sceneUpdate();
        }

        std::shared_ptr<Core::IValueSubject<SceneRotate> > SceneWidget::observeSceneRotate() const
        {
            return _p->sceneRotate;
        }

        void SceneWidget::setSceneRotate(SceneRotate value)
        {
            if (_p->sceneRotate->setIfChanged(value))
            {
                _sceneUpdate();
            }
        }

        std::shared_ptr<Core::IValueSubject<Scene::PolarCameraData> > SceneWidget::observeCameraData() const
        {
            return _p->cameraData;
        }

        void SceneWidget::setCameraData(const Scene::PolarCameraData& value)
        {
            if (_p->cameraData->setIfChanged(value))
            {
                _p->camera->setData(value);
                _redraw();
            }
        }

        std::shared_ptr<Core::IValueSubject<SceneRenderOptions> > SceneWidget::observeRenderOptions() const
        {
            return _p->renderOptions;
        }

        void SceneWidget::setRenderOptions(const SceneRenderOptions& value)
        {
            DJV_PRIVATE_PTR();
            if (p.renderOptions->setIfChanged(value))
            {
                p.offscreenBuffer.reset();
                p.offscreenBuffer2.reset();
                _redraw();
            }
        }

        void SceneWidget::frameView()
        {
            DJV_PRIVATE_PTR();
            if (p.scene)
            {
                const BBox3f& bbox = p.scene->getBBox();
                const glm::vec3& center = bbox.getCenter();
                const float max = p.scene->getBBoxMax();
                const float tan = tanf(Math::deg2rad(p.camera->getFOV()));
                const float distance = (tan > 0.F) ? (max / tan) : 1.F;
                auto cameraData = p.cameraData->get();
                cameraData.target = center;
                cameraData.distance = distance;
                setCameraData(cameraData);
                _redraw();
            }
        }

        std::shared_ptr<Core::IValueSubject<BBox3f> > SceneWidget::observeBBox() const
        {
            return _p->bbox;
        }

        std::shared_ptr<Core::IValueSubject<size_t> > SceneWidget::observePrimitivesCount() const
        {
            return _p->primitivesCount;
        }

        std::shared_ptr<Core::IValueSubject<size_t> > SceneWidget::observePointCount() const
        {
            return _p->pointCount;
        }

        void SceneWidget::_layoutEvent(Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            const BBox2f& g = getGeometry();
            p.size.w = g.w();
            p.size.h = g.h();
            auto cameraData = p.cameraData->get();
            cameraData.aspect = p.size.w / static_cast<float>(p.size.h > 0 ? p.size.h : 0);
            setCameraData(cameraData);
        }

        void SceneWidget::_paintEvent(Event::Paint&)
        {
            DJV_PRIVATE_PTR();
            if (p.offscreenBuffer2)
            {
                const BBox2f& g = getGeometry();
                auto& render = _getRender();
                render->setFillColor(AV::Image::Color(1.F, 1.F, 1.F));
                render->drawTexture(g, p.offscreenBuffer2->getColorID());
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
                    auto cameraData = p.cameraData->get();
                    cameraData.longitude += (pointerInfo.projectedPos.x - p.pointerPos.x) * rotationSensitivity;
                    while (cameraData.longitude < 0.F)
                    {
                        cameraData.longitude += 360.F;
                    }
                    while (cameraData.longitude > 360.F)
                    {
                        cameraData.longitude -= 360.F;
                    }
                    cameraData.latitude += (pointerInfo.projectedPos.y - p.pointerPos.y) * rotationSensitivity;
                    cameraData.latitude = Math::clamp(cameraData.latitude, -89.F, 89.F);
                    setCameraData(cameraData);
                }
                i = p.buttons.find(2);
                if (i != p.buttons.end())
                {
                    auto cameraData = p.cameraData->get();
                    cameraData.distance +=
                        (pointerInfo.projectedPos.x - p.pointerPos.x) * zoomSensitivity +
                        (pointerInfo.projectedPos.y - p.pointerPos.y) * zoomSensitivity;
                    cameraData.distance = std::max(cameraData.distance, 1.F);
                    setCameraData(cameraData);
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
            const auto& renderOptions = p.renderOptions->get();
            if (p.size.isValid())
            {
                bool create = !p.offscreenBuffer;
                create |= p.offscreenBuffer && p.size != p.offscreenBuffer->getSize();
                if (create)
                {
                    p.offscreenBuffer = AV::OpenGL::OffscreenBuffer::create(
                        p.size,
                        AV::Image::Type::RGBA_U8,
                        renderOptions.depthBufferType,
                        renderOptions.multiSampling);
                    p.offscreenBuffer2 = AV::OpenGL::OffscreenBuffer::create(
                        p.size,
                        AV::Image::Type::RGBA_U8);
                }
            }
            else
            {
                p.offscreenBuffer.reset();
                p.offscreenBuffer2.reset();
            }
            if (p.offscreenBuffer && p.offscreenBuffer2)
            {
                const AV::OpenGL::OffscreenBufferBinding binding(p.offscreenBuffer);
                switch (renderOptions.multiSampling)
                {
                case AV::OpenGL::OffscreenSampling::None:
                    glDisable(GL_MULTISAMPLE);
                    break;
                case AV::OpenGL::OffscreenSampling::_2:
                case AV::OpenGL::OffscreenSampling::_4:
                case AV::OpenGL::OffscreenSampling::_8:
                case AV::OpenGL::OffscreenSampling::_16:
                    glEnable(GL_MULTISAMPLE);
                    break;
                default: break;
                }
                Scene::RenderOptions options;
                options.size = p.size;
                options.camera = p.camera;
                options.clip = p.camera->getClip();
                options.shaderMode = renderOptions.shaderMode;
                options.depthBufferMode = renderOptions.depthBufferMode;
                p.render->render(p.render3D, options);
                glBindFramebuffer(GL_READ_FRAMEBUFFER, p.offscreenBuffer->getID());
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, p.offscreenBuffer2->getID());
                glBlitFramebuffer(
                    0, 0, p.size.w, p.size.h,
                    0, 0, p.size.w, p.size.h,
                    GL_COLOR_BUFFER_BIT,
                    GL_NEAREST);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
        }

        void SceneWidget::_sceneUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.scene)
            {
                glm::mat4x4 m = glm::mat4x4(1.F);
                switch (p.sceneRotate->get())
                {
                case SceneRotate::X90:
                    m = glm::rotate(m, Math::deg2rad(90.F), glm::vec3(1.F, 0.F, 0.F));
                    break;
                case SceneRotate::X_90:
                    m = glm::rotate(m, Math::deg2rad(-90.F), glm::vec3(1.F, 0.F, 0.F));
                    break;
                case SceneRotate::Y90:
                    m = glm::rotate(m, Math::deg2rad(90.F), glm::vec3(0.F, 1.F, 0.F));
                    break;
                case SceneRotate::Y_90:
                    m = glm::rotate(m, Math::deg2rad(-90.F), glm::vec3(0.F, 1.F, 0.F));
                    break;
                case SceneRotate::Z90:
                    m = glm::rotate(m, Math::deg2rad(90.F), glm::vec3(0.F, 0.F, 1.F));
                    break;
                case SceneRotate::Z_90:
                    m = glm::rotate(m, Math::deg2rad(-90.F), glm::vec3(0.F, 0.F, 1.F));
                    break;
                default: break;
                }
                p.scene->setSceneXForm(m);
                p.scene->bboxUpdate();
                p.render->setScene(p.scene);
                const float max = p.scene->getBBoxMax();
                auto cameraData = p.cameraData->get();
                cameraData.clip = Core::FloatRange(max * nearMult, max * farMult);
                setCameraData(cameraData);
                _redraw();
            }
        }

    } // namespace UI

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        SceneRotate,
        DJV_TEXT("None"),
        DJV_TEXT("X+90"),
        DJV_TEXT("X-90"),
        DJV_TEXT("Y+90"),
        DJV_TEXT("Y-90"),
        DJV_TEXT("Z+90"),
        DJV_TEXT("Z-90"));

} // namespace djv
