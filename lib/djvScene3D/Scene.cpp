// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvScene3D/Scene.h>

#include <djvScene3D/Camera.h>
#include <djvScene3D/IPrimitive.h>

#include <djvMath/BBoxFunc.h>
#include <djvMath/MatrixFunc.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace djv::Core;

namespace djv
{
    namespace Scene3D
    {
        Scene::Scene()
        {}

        std::shared_ptr<Scene> Scene::create()
        {
            auto out = std::shared_ptr<Scene>(new Scene);
            return out;
        }

        void Scene::addPrimitive(const std::shared_ptr<IPrimitive>& value)
        {
            _primitives.push_back(value);
        }

        void Scene::addDefinition(const std::shared_ptr<IPrimitive>& value)
        {
            _definitions.push_back(value);
        }

        void Scene::addLayer(const std::shared_ptr<Layer>& value)
        {
            _layers.push_back(value);
        }

        void Scene::bboxUpdate()
        {
            _bbox = Math::BBox3f();
            _bboxInit = true;
            _xforms.clear();
            glm::mat4x4 m(1.F);
            switch (_orient)
            {
            case SceneOrient::ZUp:
            {
                m = glm::rotate(m, Math::deg2rad(-90.F), glm::vec3(1.F, 0.F, 0.F));
                break;
            }
            default: break;
            }
            m *= _xform;
            _pushXForm(m);
            for (const auto& i : _primitives)
            {
                _bboxUpdate(i);
            }
            _popXForm();
        }

        float Scene::getBBoxMax() const
        {
            return std::max(_bbox.w(), std::max(_bbox.h(), _bbox.d()));
        }

        void Scene::printPrimitives()
        {
            std::cout << "Primitives" << std::endl;
            for (const auto& i : _primitives)
            {
                _print(i, "    ");
            }
            std::cout << "Definitions" << std::endl;
            for (const auto& i : _definitions)
            {
                _print(i, "    ");
            }
        }

        void Scene::printLayers()
        {
            std::cout << "Layers" << std::endl;
            for (const auto& i : _layers)
            {
                _print(i, "    ");
            }
        }

        void Scene::_bboxUpdate(const std::shared_ptr<IPrimitive>& primitive)
        {
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
                    if (!primitive->isXFormIdentity())
                    {
                        _pushXForm(primitive->getXForm());
                    }
                    const glm::mat4x4& xform = _getCurrentXForm();
                    const Math::BBox3f& bbox = primitive->getBBox();
                    if (_bboxInit)
                    {
                        _bboxInit = false;
                        _bbox = bbox * xform;
                    }
                    else
                    {
                        _bbox.expand(bbox * xform);
                    }
                    for (const auto& i : primitive->getPrimitives())
                    {
                        _bboxUpdate(i);
                    }
                    if (!primitive->isXFormIdentity())
                    {
                        _popXForm();
                    }
                }
            }
        }

        void Scene::_print(const std::shared_ptr<IPrimitive>& primitive, const std::string& indent)
        {
            std::cout << indent << primitive->getClassName() << ": " << primitive->getName() << std::endl;
            if (!primitive->isXFormIdentity())
            {
                std::cout << indent << primitive->getXForm() << std::endl;
            }
            std::cout << indent << "point count: " << primitive->getPointCount() << std::endl;
            for (const auto& i : primitive->getPrimitives())
            {
                _print(i, indent + "    ");
            }
        }

        void Scene::_print(const std::shared_ptr<Layer>& layer, const std::string& indent)
        {
            std::cout << indent << layer->getName() << ": " << layer->getItems().size() << std::endl;
            for (const auto& i : layer->getItems())
            {
                if (auto subLayer = std::dynamic_pointer_cast<Layer>(i))
                {
                    _print(subLayer, indent + "    ");
                }
            }
        }

    } // namespace Scene3D
} // namespace djv
