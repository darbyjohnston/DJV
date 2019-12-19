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
#include <djvScene/IPrimitive.h>
#include <djvScene/Light.h>
#include <djvScene/Material.h>
#include <djvScene/Scene.h>

#include <djvAV/PointList.h>
#include <djvAV/Render3D.h>
#include <djvAV/Render3DCamera.h>
#include <djvAV/Render3DLight.h>
#include <djvAV/Render3DMaterial.h>

#include <djvCore/Matrix.h>

#include <glm/gtc/matrix_transform.hpp>

#include <unordered_map>

using namespace djv::Core;

namespace djv
{
    namespace Scene
    {
        bool RenderOptions::operator == (const RenderOptions& other) const
        {
            return camera == other.camera &&
                size == other.size &&
                clip == other.clip &&
                shaderMode == other.shaderMode &&
                depthBufferMode == other.depthBufferMode;
        }

        struct Render::Private
        {
            std::weak_ptr<Core::Context> context;
            std::shared_ptr<Scene> scene;
            std::map<std::shared_ptr<IMaterial>, std::shared_ptr<AV::Render3D::IMaterial> > materials;
            std::shared_ptr<AV::Render3D::IMaterial> colorMaterial;
            std::shared_ptr<AV::Render3D::IMaterial> defaultMaterial;
            std::list<glm::mat4x4> transforms;
            const glm::mat4x4 identity = glm::mat4x4(1.F);
            struct Key
            {
                glm::mat4x4 transform = glm::mat4x4(1.F);
                AV::Image::Color color;
                std::shared_ptr<AV::Render3D::IMaterial> material;

                bool operator == (const Key& other) const
                {
                    return transform == other.transform &&
                        color == other.color &&
                        material == other.material;
                }
            };
            typedef std::pair<Key, std::vector<std::shared_ptr<AV::Geom::TriangleMesh> > > TriangleMeshesKeyValue;
            typedef std::pair<Key, std::vector<std::shared_ptr<AV::Geom::PointList> > > PointListsKeyValue;
            std::vector<TriangleMeshesKeyValue> triangleMeshes;
            std::vector<PointListsKeyValue> polyLines;
            std::vector<PointListsKeyValue> pointLists;
            size_t primitivesCount = 0;
            size_t pointCount = 0;
            size_t lightCount = 0;
        };

        void Render::_init(const std::shared_ptr<Core::Context>& context)
        {
            DJV_PRIVATE_PTR();
            p.context = context;
            p.colorMaterial = AV::Render3D::SolidColorMaterial::create(context);
            p.defaultMaterial = AV::Render3D::DefaultMaterial::create(context);
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
            p.transforms.clear();
            p.triangleMeshes.clear();
            p.polyLines.clear();
            p.pointLists.clear();
            p.primitivesCount = 0;
            p.pointCount = 0;
            p.lightCount = 0;

            p.scene = value;
            
            if (auto context = p.context.lock())
            {
                if (p.scene)
                {
                    glm::mat4x4 m(1.F);
                    switch (p.scene->getSceneOrient())
                    {
                    case SceneOrient::ZUp:
                    {
                        m = glm::rotate(m, Math::deg2rad(-90.F), glm::vec3(1.F, 0.F, 0.F));
                        break;
                    }
                    default: break;
                    }
                    m *= p.scene->getSceneXForm();
                    _pushTransform(m);
                    for (const auto& i : p.scene->getPrimitives())
                    {
                        _prePass(i, context);
                    }
                    _popTransform();
                }
            }
        }

        void Render::render(
            const std::shared_ptr<AV::Render3D::Render>& render,
            const RenderOptions& renderOptions)
        {
            DJV_PRIVATE_PTR();
            if (auto context = p.context.lock())
            {
                // Set the default material shader mode.
                for (const auto& i : p.materials)
                {
                    if (auto defaultMaterial = std::dynamic_pointer_cast<AV::Render3D::DefaultMaterial>(i.second))
                    {
                        defaultMaterial->setMode(renderOptions.shaderMode);
                    }
                }

                // Create the render camera.
                auto renderCamera = AV::Render3D::DefaultCamera::create();
                renderCamera->setV(renderOptions.camera->getV());
                renderCamera->setP(renderOptions.camera->getP());

                // Create the default lights.
                if (0 == p.lightCount)
                {
                    auto hemisphereLight = AV::Render3D::HemisphereLight::create();
                    render->addLight(hemisphereLight);
                    auto directionalLight = AV::Render3D::DirectionalLight::create();
                    render->addLight(directionalLight);
                }

                // Setup the render options.
                AV::Render3D::RenderOptions render3DOptions;
                render3DOptions.camera = renderCamera;
                render3DOptions.size = renderOptions.size;
                render3DOptions.clip = renderOptions.clip;
                render3DOptions.depthBufferMode = renderOptions.depthBufferMode;

                // Render the primitives.
                render->beginFrame(render3DOptions);
                for (const auto& i : p.triangleMeshes)
                {
                    render->setColor(i.first.color);
                    render->setMaterial(i.first.material);
                    render->pushTransform(i.first.transform);
                    render->drawTriangleMeshes(i.second);
                    render->popTransform();
                }
                for (const auto& i : p.polyLines)
                {
                    render->setColor(i.first.color);
                    render->setMaterial(i.first.material);
                    render->pushTransform(i.first.transform);
                    render->drawPolyLines(i.second);
                    render->popTransform();
                }
                for (const auto& i : p.pointLists)
                {
                    render->setColor(i.first.color);
                    render->setMaterial(i.first.material);
                    render->pushTransform(i.first.transform);
                    render->drawPoints(i.second);
                    render->popTransform();
                }
                render->endFrame();
            }
        }

        size_t Render::getPrimitivesCount() const
        {
            return _p->primitivesCount;
        }

        size_t Render::getPointCount() const
        {
            return _p->pointCount;
        }

        AV::Image::Color Render::_getColor(const std::shared_ptr<IPrimitive>& primitive) const
        {
            AV::Image::Color out(0.F, 0.F, 0.F);
            switch (primitive->getColorAssignment())
            {
            case ColorAssignment::Layer:
                if (auto layer = primitive->getLayer().lock())
                {
                    out = layer->getColor();
                }
                break;
            case ColorAssignment::Parent:
                if (auto parent = primitive->getParent().lock())
                {
                    out = _getColor(parent);
                }
                break;
            case ColorAssignment::Primitive:
                out = primitive->getColor();
                break;
            }
            return out;
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

        const glm::mat4x4& Render::_getCurrentTransform() const
        {
            DJV_PRIVATE_PTR();
            return p.transforms.size() ? p.transforms.back() : p.identity;
        }

        void Render::_pushTransform(const glm::mat4x4& value)
        {
            DJV_PRIVATE_PTR();
            p.transforms.push_back(_getCurrentTransform() * value);
        }

        void Render::_popTransform()
        {
            DJV_PRIVATE_PTR();
            if (p.transforms.size())
            {
                p.transforms.pop_back();
            }
        }

        void Render::_prePass(
            const std::shared_ptr<IPrimitive>& primitive,
            const std::shared_ptr<Core::Context>& context)
        {
            DJV_PRIVATE_PTR();

            if (bool visible = primitive->isVisible())
            {
                // Check whether the primitive's layer is visible.
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
                    // Get the material.
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

                    // Setup the transform.
                    if (!primitive->isXFormIdentity())
                    {
                        _pushTransform(primitive->getXForm());
                    }
                    const auto& currentTransform = _getCurrentTransform();

                    // Get the primitive's triangle meshes.
                    Private::Key key;
                    key.color = _getColor(primitive);
                    key.material = renderMaterial ? renderMaterial : (primitive->isShaded() ? p.defaultMaterial : p.colorMaterial);
                    key.transform = currentTransform;
                    {
                        const auto j = std::find_if(
                            p.triangleMeshes.begin(),
                            p.triangleMeshes.end(),
                            [key](const Private::TriangleMeshesKeyValue& value)
                            {
                                return key == value.first;
                            });
                        if (j != p.triangleMeshes.end())
                        {
                            const auto& meshes = primitive->getMeshes();
                            j->second.insert(j->second.end(), meshes.begin(), meshes.end());
                        }
                        else
                        {
                            p.triangleMeshes.push_back(std::make_pair(key, primitive->getMeshes()));
                        }
                    }

                    // Get the primitive's poly-lines.
                    {
                        const auto j = std::find_if(
                            p.polyLines.begin(),
                            p.polyLines.end(),
                            [key](const Private::PointListsKeyValue& value)
                            {
                                return key == value.first;
                            });
                        if (j != p.polyLines.end())
                        {
                            const auto& pointLists = primitive->getPolyLines();
                            j->second.insert(j->second.end(), pointLists.begin(), pointLists.end());
                        }
                        else
                        {
                            p.polyLines.push_back(std::make_pair(key, primitive->getPolyLines()));
                        }
                    }

                    // Get the primitive's points.
                    {
                        const auto j = std::find_if(
                            p.pointLists.begin(),
                            p.pointLists.end(),
                            [key](const Private::PointListsKeyValue& value)
                            {
                                return key == value.first;
                            });
                        if (j != p.pointLists.end())
                        {
                            j->second.push_back(primitive->getPointList());
                        }
                        else
                        {
                            p.pointLists.push_back(Private::PointListsKeyValue(key, { primitive->getPointList() }));
                        }
                    }

                    // Recurse.
                    for (const auto& i : primitive->getPrimitives())
                    {
                        _prePass(i, context);
                    }

                    // Restore the transform.
                    if (!primitive->isXFormIdentity())
                    {
                        _popTransform();
                    }

                    p.primitivesCount += 1;
                    p.pointCount += primitive->getPointCount();
                }
            }
        }

    } // namespace Scene
} // namespace djv
