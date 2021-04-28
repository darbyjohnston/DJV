// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvScene3D/Enum.h>

#include <djvMath/BBox.h>

#include <glm/mat4x4.hpp>

#include <list>
#include <memory>
#include <string>
#include <vector>

namespace djv
{
    namespace Scene3D
    {
        class IPrimitive;
        class Layer;

        //! Scene.
        class Scene : public std::enable_shared_from_this<Scene>
        {
            DJV_NON_COPYABLE(Scene);
            Scene();

        public:
            static std::shared_ptr<Scene> create();

            const std::vector<std::shared_ptr<IPrimitive> >& getPrimitives() const;
            const std::vector<std::shared_ptr<IPrimitive> >& getDefinitions() const;
            const std::vector<std::shared_ptr<Layer> >& getLayers() const;

            void addPrimitive(const std::shared_ptr<IPrimitive>&);
            void addDefinition(const std::shared_ptr<IPrimitive>&);
            void addLayer(const std::shared_ptr<Layer>&);

            SceneOrient getSceneOrient() const;
            const glm::mat4x4& getSceneXForm() const;

            void setSceneOrient(SceneOrient);
            void setSceneXForm(const glm::mat4x4&);

            void bboxUpdate();
            const Math::BBox3f& getBBox() const;
            float getBBoxMax() const;

            void printPrimitives();
            void printLayers();

        private:
            const glm::mat4x4& _getCurrentXForm() const;
            void _pushXForm(const glm::mat4x4&);
            void _popXForm();
            void _bboxUpdate(const std::shared_ptr<IPrimitive>&);

            static void _print(const std::shared_ptr<IPrimitive>&, const std::string& indent);
            static void _print(const std::shared_ptr<Layer>&, const std::string& indent);

            std::vector<std::shared_ptr<IPrimitive> > _primitives;
            std::vector<std::shared_ptr<IPrimitive> > _definitions;
            std::vector<std::shared_ptr<Layer> > _layers;
            SceneOrient _orient = SceneOrient::YUp;
            glm::mat4x4 _xform = glm::mat4x4(1.F);
            Math::BBox3f _bbox = Math::BBox3f(0.F, 0.F, 0.F, 0.F, 0.F, 0.F);
            bool _bboxInit = true;
            std::list<glm::mat4x4> _xforms;
            const glm::mat4x4 _identity = glm::mat4x4(1.F);
        };

    } // namespace Scene3D
} // namespace djv

#include <djvScene3D/SceneInline.h>
