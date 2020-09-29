// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/SceneWidget.h>

#include <djvScene3D/Camera.h>
#include <djvScene3D/Render.h>
#include <djvScene3D/Scene.h>

#include <djvRender2D/Render.h>

#include <djvGL/OffscreenBuffer.h>

#if defined(DJV_GL_ES2)
#include <djvGL/Mesh.h>
#include <djvGL/Shader.h>
#endif // DJV_GL_ES2

#include <djvSystem/Context.h>
#include <djvSystem/TimerFunc.h>
#if defined(DJV_GL_ES2)
#include <djvSystem/ResourceSystem.h>
#endif // DJV_GL_ES2

#include <glm/gtc/matrix_transform.hpp>

#include <array>

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
            const float nearMult            = .001F;
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
            std::shared_ptr<Render3D::Render> render3D;
            Image::Size size;
            std::shared_ptr<Scene3D::Scene> scene;
            std::shared_ptr<ValueSubject<SceneRotate> > sceneRotate;
            std::shared_ptr<Scene3D::PolarCamera> camera;
            std::shared_ptr<ValueSubject<Scene3D::PolarCameraData> > cameraData;
            std::shared_ptr<ValueSubject<SceneRenderOptions> > renderOptions;
            std::shared_ptr<Scene3D::Render> render;
            std::shared_ptr<GL::OffscreenBuffer> offscreenBuffer;
            std::shared_ptr<GL::OffscreenBuffer> offscreenBuffer2;
#if defined(DJV_GL_ES2)
            std::shared_ptr<GL::Shader> shader;
#endif // DJV_GL_ES2
            System::Event::PointerID pressedID = System::Event::invalidID;
            std::map<int, bool> buttons;
            glm::vec2 pointerPos = glm::vec2(0.F, 0.F);
            std::shared_ptr<ValueSubject<Math::BBox3f> > bbox;
            std::shared_ptr<ValueSubject<size_t> > primitivesCount;
            std::shared_ptr<ValueSubject<size_t> > pointCount;
            std::shared_ptr<System::Timer> statsTimer;
        };

        void SceneWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::UI::SceneWidget");
            setPointerEnabled(true);

            p.sceneRotate = ValueSubject<SceneRotate>::create(SceneRotate::None);
            p.render3D = context->getSystemT<Render3D::Render>();
            p.camera = Scene3D::PolarCamera::create();
            p.cameraData = ValueSubject<Scene3D::PolarCameraData>::create(p.camera->getData());
            p.renderOptions = ValueSubject<SceneRenderOptions>::create(SceneRenderOptions());
            p.render = Scene3D::Render::create(context);
#if defined(DJV_GL_ES2)
            auto resourceSystem = context->getSystemT<System::ResourceSystem>();
            const System::File::Path shaderPath = resourceSystem->getPath(System::File::ResourcePath::Shaders);
            p.shader = GL::Shader::create(
                System::File::Path(shaderPath, "djvRender2DVertex.glsl"),
                System::File::Path(shaderPath, "djvRender2DFragment.glsl"));
#endif // DJV_GL_ES2
            p.bbox = ValueSubject<Math::BBox3f>::create(Math::BBox3f(0.F, 0.F, 0.F, 0.F, 0.F, 0.F));
            p.primitivesCount = ValueSubject<size_t>::create(0);
            p.pointCount = ValueSubject<size_t>::create(0);

            p.statsTimer = System::Timer::create(context);
            p.statsTimer->setRepeating(true);
            auto weak = std::weak_ptr<SceneWidget>(std::dynamic_pointer_cast<SceneWidget>(shared_from_this()));
            p.statsTimer->start(
                System::getTimerDuration(System::TimerValue::Slow),
                [weak](const std::chrono::steady_clock::time_point&, const Time::Duration&)
                {
                    if (auto widget = weak.lock())
                    {
                        Math::BBox3f bbox(0.F, 0.F, 0.F, 0.F, 0.F, 0.F);
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

        std::shared_ptr<SceneWidget> SceneWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<SceneWidget>(new SceneWidget);
            out->_init(context);
            return out;
        }

        void SceneWidget::setScene(const std::shared_ptr<Scene3D::Scene>& value)
        {
            DJV_PRIVATE_PTR();
            p.scene = value;
            p.render->setScene(p.scene);
            _sceneUpdate();
        }

        std::shared_ptr<IValueSubject<SceneRotate> > SceneWidget::observeSceneRotate() const
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

        std::shared_ptr<IValueSubject<Scene3D::PolarCameraData> > SceneWidget::observeCameraData() const
        {
            return _p->cameraData;
        }

        void SceneWidget::setCameraData(const Scene3D::PolarCameraData& value)
        {
            if (_p->cameraData->setIfChanged(value))
            {
                _p->camera->setData(value);
                _redraw();
            }
        }

        std::shared_ptr<IValueSubject<SceneRenderOptions> > SceneWidget::observeRenderOptions() const
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

        namespace
        {
            float getDistance(float fov, float max)
            {
                const float tan = tanf(Math::deg2rad(fov / 2.F));
                const float distance = (tan > 0.F) ? (max / 2.F / tan) : 1.F;
                return distance;
            }

        } // namespace

        void SceneWidget::frameView()
        {
            DJV_PRIVATE_PTR();
            if (p.scene)
            {
                const Math::BBox3f& bbox = p.scene->getBBox();
                const glm::vec3& center = bbox.getCenter();
                const float distance = getDistance(p.camera->getFOV(), p.scene->getBBoxMax());
                auto cameraData = p.cameraData->get();
                cameraData.target = center;
                cameraData.distance = distance;
                setCameraData(cameraData);
                _redraw();
            }
        }

        std::shared_ptr<IValueSubject<Math::BBox3f> > SceneWidget::observeBBox() const
        {
            return _p->bbox;
        }

        std::shared_ptr<IValueSubject<size_t> > SceneWidget::observePrimitivesCount() const
        {
            return _p->primitivesCount;
        }

        std::shared_ptr<IValueSubject<size_t> > SceneWidget::observePointCount() const
        {
            return _p->pointCount;
        }

        void SceneWidget::_layoutEvent(System::Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            const Math::BBox2f& g = getGeometry();
            p.size.w = g.w();
            p.size.h = g.h();
            auto cameraData = p.cameraData->get();
            cameraData.aspect = p.size.w / static_cast<float>(p.size.h > 0 ? p.size.h : 0);
            setCameraData(cameraData);
        }

        void SceneWidget::_paintEvent(System::Event::Paint&)
        {
            DJV_PRIVATE_PTR();
            if (p.offscreenBuffer2)
            {
                const Math::BBox2f& g = getGeometry();
                auto& render = _getRender();
                render->setFillColor(Image::Color(1.F, 1.F, 1.F));
                render->drawTexture(g, p.offscreenBuffer2->getColorID());
            }
        }

        void SceneWidget::_pointerMoveEvent(System::Event::PointerMove & event)
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

        void SceneWidget::_buttonPressEvent(System::Event::ButtonPress & event)
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

        void SceneWidget::_buttonReleaseEvent(System::Event::ButtonRelease & event)
        {
            DJV_PRIVATE_PTR();
            const auto& pointerInfo = event.getPointerInfo();
            if (pointerInfo.id == p.pressedID)
            {
                event.accept();
                p.pressedID = System::Event::invalidID;
                p.buttons.clear();
            }
        }

        void SceneWidget::_updateEvent(System::Event::Update&)
        {
            DJV_PRIVATE_PTR();
            const auto& renderOptions = p.renderOptions->get();
            if (p.size.isValid())
            {
                bool create = !p.offscreenBuffer;
                create |= p.offscreenBuffer && p.size != p.offscreenBuffer->getSize();
                if (create)
                {
                    auto textSystem = _getTextSystem();
                    p.offscreenBuffer = GL::OffscreenBuffer::create(
                        p.size,
                        Image::Type::RGBA_U8,
                        renderOptions.depthBufferType,
                        renderOptions.multiSampling,
                        textSystem);
                    p.offscreenBuffer2 = GL::OffscreenBuffer::create(
                        p.size,
                        Image::Type::RGBA_U8,
                        textSystem);
                }
            }
            else
            {
                p.offscreenBuffer.reset();
                p.offscreenBuffer2.reset();
            }
            if (p.offscreenBuffer && p.offscreenBuffer2)
            {
                const GL::OffscreenBufferBinding binding(p.offscreenBuffer);
#if !defined(DJV_GL_ES2)
                switch (renderOptions.multiSampling)
                {
                case GL::OffscreenSampling::None:
                    glDisable(GL_MULTISAMPLE);
                    break;
                case GL::OffscreenSampling::_2:
                case GL::OffscreenSampling::_4:
                case GL::OffscreenSampling::_8:
                case GL::OffscreenSampling::_16:
                    glEnable(GL_MULTISAMPLE);
                    break;
                default: break;
                }
#endif // DJV_GL_ES2
                Scene3D::RenderOptions options;
                options.size = p.size;
                options.camera = p.camera;
                options.clip = p.camera->getClip();
                options.shaderMode = renderOptions.shaderMode;
                options.depthBufferMode = renderOptions.depthBufferMode;
                p.render->render(p.render3D, options);
#if defined(DJV_GL_ES2)
                glBindFramebuffer(GL_FRAMEBUFFER, p.offscreenBuffer2->getID());

                p.shader->bind();
                const auto viewMatrix = glm::ortho(
                    0.F,
                    static_cast<float>(p.size.w),
                    0.F,
                    static_cast<float>(p.size.h),
                    -1.F,
                    1.F);
                p.shader->setUniform("transform.mvp", viewMatrix);
                p.shader->setUniform("imageFormat", 3);
                p.shader->setUniform("colorMode", 5);
                const GLfloat color[] = { 1.F, 1.F, 1.F, 1.F };
                p.shader->setUniform("color", color);
                p.shader->setUniform("textureSampler", 0);
                
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, p.offscreenBuffer->getColorID());
                                
                auto vbo = GL::VBO::create(2 * 4, GL::VBOType::Pos2_F32_UV_U16);
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
                vboP->x = p.size.w;
                vboP->y = 0.F;
                vboP->u = 65535;
                vboP->v = 0;
                ++vboP;
                vboP->x = p.size.w;
                vboP->y = p.size.h;
                vboP->u = 65535;
                vboP->v = 65535;
                ++vboP;
                vboP->x = p.size.w;
                vboP->y = p.size.h;
                vboP->u = 65535;
                vboP->v = 65535;
                ++vboP;
                vboP->x = 0.F;
                vboP->y = p.size.h;
                vboP->u = 0;
                vboP->v = 65535;
                ++vboP;
                vboP->x = 0.F;
                vboP->y = 0.F;
                vboP->u = 0;
                vboP->v = 0;
                ++vboP;
                vbo->copy(vboData);
                auto vao = GL::VAO::create(GL::VBOType::Pos2_F32_UV_U16, vbo->getID());
                vao->draw(GL_TRIANGLES, 0, 6);
#else // DJV_GL_ES2
                glBindFramebuffer(GL_READ_FRAMEBUFFER, p.offscreenBuffer->getID());
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, p.offscreenBuffer2->getID());
                glBlitFramebuffer(
                    0, 0, p.size.w, p.size.h,
                    0, 0, p.size.w, p.size.h,
                    GL_COLOR_BUFFER_BIT,
                    GL_NEAREST);
#endif // DJV_GL_ES2
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
                cameraData.clip = Math::FloatRange(max * nearMult, max * farMult);
                setCameraData(cameraData);
                _redraw();
            }
        }

        DJV_ENUM_HELPERS_IMPLEMENTATION(SceneRotate);

    } // namespace UI

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        SceneRotate,
        DJV_TEXT("scene_rotate_none"),
        DJV_TEXT("scene_rotate_x_+90"),
        DJV_TEXT("scene_rotate_x_-90"),
        DJV_TEXT("scene_rotate_y_+90"),
        DJV_TEXT("scene_rotate_y_-90"),
        DJV_TEXT("scene_rotate_z_+90"),
        DJV_TEXT("scene_rotate_z_-90"));

} // namespace djv
