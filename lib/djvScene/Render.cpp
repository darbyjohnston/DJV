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

#include <djvScene/Render.h>

#include <djvScene/Camera.h>
#include <djvScene/Light.h>
#include <djvScene/Material.h>
#include <djvScene/Primitive.h>
#include <djvScene/Scene.h>

#include <djvAV/Render3D.h>
#include <djvAV/Render3DCamera.h>
#include <djvAV/Render3DLight.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace djv::Core;

namespace djv
{
    namespace Scene
    {
        struct Render::Private
        {
            std::weak_ptr<Core::Context> context;
            std::shared_ptr<Scene> scene;
            std::map<std::shared_ptr<IMaterial>, std::shared_ptr<AV::Render3D::IMaterial> > materials;
            size_t primitivesCount = 0;
            size_t triangleCount = 0;
        };

        void Render::_init(const std::shared_ptr<Core::Context>& context)
        {
            DJV_PRIVATE_PTR();
            p.context = context;
        }

        Render::Render() :
            _p(new Private)
        {}

        std::shared_ptr<Render> Render::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<Render>(new Render);
            out->_init(context);
            return out;
        }

        void Render::setScene(const std::shared_ptr<Scene>& value)
        {
            DJV_PRIVATE_PTR();
            p.materials.clear();
            p.scene = value;
        }

        void Render::render(
            const std::shared_ptr<AV::Render3D::Render>& render,
            const RenderOptions& renderOptions)
        {
            DJV_PRIVATE_PTR();
            if (auto context = p.context.lock())
            {
                p.primitivesCount = 0;
                p.triangleCount = 0;

                AV::Render3D::RenderOptions render3DOptions;
                auto renderCamera = AV::Render3D::DefaultCamera::create();
                renderCamera->setV(renderOptions.camera->getV());
                renderCamera->setP(renderOptions.camera->getP());
                render3DOptions.camera = renderCamera;
                render3DOptions.size = renderOptions.size;
                render->beginFrame(render3DOptions);
                if (p.scene)
                {
                    glm::mat4x4 m(1.F);
                    switch (p.scene->getSceneOrient())
                    {
                    case SceneOrient::ZUp:
                    {
                        m = glm::rotate(glm::mat4x4(1.F), Math::deg2rad(-90.F), glm::vec3(1.F, 0.F, 0.F));
                        break;
                    }
                    default: break;
                    }
                    m = p.scene->getSceneXForm() * m;
                    render->pushTransform(m);
                    for (const auto& i : p.scene->getPrimitives())
                    {
                        _render(i, render, context);
                    }
                    render->popTransform();
                }
                render->endFrame();
            }
        }

        size_t Render::getPrimitivesCount() const
        {
            return _p->primitivesCount;
        }

        size_t Render::getTriangleCount() const
        {
            return _p->triangleCount;
        }

        std::shared_ptr<IMaterial> Render::_getMaterial(const std::shared_ptr<IPrimitive>& primitive) const
        {
            std::shared_ptr<IMaterial> out;
            switch (primitive->getMaterialAssignment())
            {
            case MaterialAssignment::Layer:
                if (auto layer = primitive->getLayer().lock())
                {
                    out = layer->getMaterial();
                }
                break;
            case MaterialAssignment::Parent:
                if (auto parent = primitive->getParent().lock())
                {
                    out = _getMaterial(parent);
                }
                break;
            case MaterialAssignment::Primitive:
                out = primitive->getMaterial();
                break;
            }
            return out;
        }

        void Render::_render(
            const std::shared_ptr<IPrimitive>& primitive,
            const std::shared_ptr<AV::Render3D::Render>& render,
            const std::shared_ptr<Core::Context>& context)
        {
            DJV_PRIVATE_PTR();

            if (bool visible = primitive->isVisible())
            {
                auto layer = primitive->getLayer().lock();
                while (layer)
                {
                    visible &= layer->isVisible();
                    if (visible)
                    {
                        layer = layer->getLayer().lock();
                    }
                    else
                    {
                        layer.reset();
                    }
                }

                if (visible)
                {
                    p.primitivesCount = p.primitivesCount + 1;

                    if (!primitive->isXFormIdentity())
                    {
                        render->pushTransform(primitive->getXForm());
                    }

                    std::shared_ptr<AV::Render3D::IMaterial> renderMaterial;
                    if (auto material = _getMaterial(primitive))
                    {
                        auto j = p.materials.find(material);
                        if (j != p.materials.end())
                        {
                            renderMaterial = j->second;
                        }
                        else
                        {
                            renderMaterial = material->createMaterial(context);
                            p.materials[material] = renderMaterial;
                        }
                    }
                    if (renderMaterial)
                    {
                        render->setMaterial(renderMaterial);
                        render->drawTriangleMeshes(primitive->getMeshes());
                        for (const auto& j : primitive->getMeshes())
                        {
                            p.triangleCount += j->triangles.size();
                        }
                    }

                    if (auto directionalLight = std::dynamic_pointer_cast<DirectionalLight>(primitive))
                    {
                        if (directionalLight->isEnabled())
                        {
                            auto renderLight = AV::Render3D::DirectionalLight::create();
                            const auto& direction = directionalLight->getDirection();
                            glm::vec3 renderDirection = render->getCurrentTransform() * glm::vec4(direction.x, direction.y, direction.z, 1.F);
                            glm::vec3 renderPosition = render->getCurrentTransform() * glm::vec4(0.F, 0.F, 0.F, 1.F);
                            renderLight->setIntensity(directionalLight->getIntensity());
                            renderLight->setDirection(renderDirection - renderPosition);
                            render->addLight(renderLight);
                        }
                    }
                    else if (auto pointLight = std::dynamic_pointer_cast<PointLight>(primitive))
                    {
                        if (pointLight->isEnabled())
                        {
                            auto renderLight = AV::Render3D::PointLight::create();
                            glm::vec3 renderPosition = render->getCurrentTransform() * glm::vec4(0.F, 0.F, 0.F, 1.F);
                            renderLight->setIntensity(pointLight->getIntensity());
                            renderLight->setPosition(renderPosition);
                            render->addLight(renderLight);
                        }
                    }
                    else if (auto spotLight = std::dynamic_pointer_cast<SpotLight>(primitive))
                    {
                        if (spotLight->isEnabled())
                        {
                            auto renderLight = AV::Render3D::SpotLight::create();
                            const auto& direction = directionalLight->getDirection();
                            glm::vec3 renderDirection = render->getCurrentTransform() * glm::vec4(direction.x, direction.y, direction.z, 1.F);
                            glm::vec3 renderPosition = render->getCurrentTransform() * glm::vec4(0.F, 0.F, 0.F, 1.F);
                            renderLight->setIntensity(spotLight->getIntensity());
                            renderLight->setConeAngle(spotLight->getConeAngle());
                            renderLight->setPosition(renderPosition);
                            renderLight->setDirection(renderDirection - renderPosition);
                            render->addLight(renderLight);
                        }
                    }

                    for (const auto& i : primitive->getPrimitives())
                    {
                        _render(i, render, context);
                    }

                    if (!primitive->isXFormIdentity())
                    {
                        render->popTransform();
                    }
                }
            }
        }

    } // namespace Scene
} // namespace djv
