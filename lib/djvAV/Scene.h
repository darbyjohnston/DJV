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

#pragma once

#include <djvAV/ImageData.h>
#include <djvAV/TriangleMesh.h>

#include <djvCore/Enum.h>

#include <glm/mat4x4.hpp>

#include <memory>

namespace djv
{
    namespace AV
    {
        namespace Render
        {
            class Render3D;

        } // namespace Render

        //! This namespace provides scene functionality.
        namespace Scene
        {
            class IMaterial : public std::enable_shared_from_this<IMaterial>
            {
            public:
                virtual ~IMaterial() = 0;

            private:
            };

            class BasicMaterial : public IMaterial
            {
            public:

            private:
            };

            enum class MaterialAssignment
            {
                Layer,
                Parent,
                Object,

                Count,
                First = Layer
            };

            class IObject : public std::enable_shared_from_this<IObject>
            {
            public:
                virtual ~IObject() = 0;

                virtual void render(const std::shared_ptr<Render::Render3D>&) = 0;

            private:
                std::string _name;
                glm::mat4x4 _xform;
                MaterialAssignment _materialAssignment = MaterialAssignment::First;
                std::shared_ptr<IMaterial> _material;
            };

            class MeshObject : public IObject
            {
            public:
                void render(const std::shared_ptr<Render::Render3D>&) override;

            private:
                Geom::TriangleMesh _mesh;
            };

            class ObjectInstance : public IObject
            {
            private:
                std::shared_ptr<IObject> _object;
            };

            class Group : public IObject
            {
            private:
                std::vector<std::shared_ptr<IObject> > _objects;
            };

            class ICamera : public std::enable_shared_from_this<ICamera>
            {
            public:
                virtual ~ICamera() = 0;

            private:
                glm::mat4x4 _xform;
            };

            class Camera : public ICamera
            {
            private:
                float _fov = 45.F;
            };

            class ILight : public std::enable_shared_from_this<ILight>
            {
            public:
                virtual ~ILight() = 0;

            private:
            };

            class PointLight : public ILight
            {
            private:
                glm::vec3 _position = glm::vec3(0.F, 0.F, 0.F);
                float _radius = 100.F;
            };

            class Layer : public std::enable_shared_from_this<Layer>
            {
            private:
                std::vector<std::shared_ptr<IObject> > _objects;
                std::vector<std::shared_ptr<Layer> > _layers;
                std::shared_ptr<IMaterial> _material;
            };

            struct RenderOptions
            {
                std::shared_ptr<ICamera> camera;
                Image::Size size;
            };

            class Scene
            {
            public:
                void render(
                    const std::shared_ptr<Render::Render3D>&,
                    const RenderOptions&);

            private:
                std::vector<std::shared_ptr<IObject> > _objects;
                std::vector<std::shared_ptr<ICamera> > _cameras;
                std::vector<std::shared_ptr<ILight> > _lights;
                std::vector< std::shared_ptr<Layer> > _layers;
            };

        } // namespace Render
    } // namespace AV
} // namespace djv
