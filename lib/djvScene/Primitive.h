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

#include <djvScene/Enum.h>
#include <djvScene/Layer.h>

#include <djvAV/TriangleMesh.h>

#include <glm/mat4x4.hpp>

#include <memory>

namespace djv
{
    namespace Scene
    {
        class IMaterial;

        //! This class provides the base functionality for primitives.
        class IPrimitive : public ILayerItem
        {
            DJV_NON_COPYABLE(IPrimitive);

        protected:
            IPrimitive();

        public:
            virtual ~IPrimitive() = 0;

            const std::string& getName() const;
            void setName(const std::string&);

            bool isVisible() const;
            void setVisible(bool);

            const glm::mat4x4& getXForm() const;
            const glm::mat4x4& getXFormFinal() const;
            void setXForm(const glm::mat4x4&);
            void setXFormFinal(const glm::mat4x4&);

            const std::shared_ptr<AV::Geom::TriangleMesh>& getMesh() const;
            void setMesh(const std::shared_ptr<AV::Geom::TriangleMesh>&);

            MaterialAssignment getMaterialAssignment() const;
            void setMaterialAssignment(MaterialAssignment);

            const std::shared_ptr<IMaterial>& getMaterial() const;
            void setMaterial(const std::shared_ptr<IMaterial>&);

            const std::weak_ptr<IPrimitive>& getParent() const;
            const std::vector<std::shared_ptr<IPrimitive> >& getChildren() const;
            void addChild(const std::shared_ptr<IPrimitive>&);
            void removeChild(const std::shared_ptr<IPrimitive>&);
            void clearChildren();

            virtual const std::vector<std::shared_ptr<IPrimitive> >& getPrimitives() const;

        private:
            std::string _name;
            bool _visible = true;
            glm::mat4x4 _xform = glm::mat4x4(1.F);
            glm::mat4x4 _xformFinal = glm::mat4x4(1.F);
            std::shared_ptr<AV::Geom::TriangleMesh> _mesh;
            MaterialAssignment _materialAssignment = MaterialAssignment::Primitive;
            std::shared_ptr<IMaterial> _material;
            std::weak_ptr<IPrimitive> _parent;
            std::vector<std::shared_ptr<IPrimitive> > _children;
        };

        //! This class provides a null primitive.
        class NullPrimitive : public IPrimitive
        {
            DJV_NON_COPYABLE(NullPrimitive);

        protected:
            NullPrimitive();

        public:
            static std::shared_ptr<NullPrimitive> create();

        private:
        };

        //! This class provides a primitive instance.
        class InstancePrimitive : public IPrimitive
        {
            DJV_NON_COPYABLE(InstancePrimitive);

        protected:
            InstancePrimitive();

        public:
            static std::shared_ptr<InstancePrimitive> create();

            const std::vector<std::shared_ptr<IPrimitive> >& getInstances() const;
            void setInstances(const std::vector<std::shared_ptr<IPrimitive> >&);
            void addInstance(const std::shared_ptr<IPrimitive>&);

            const std::vector<std::shared_ptr<IPrimitive> >& getPrimitives() const override;

        private:
            std::vector<std::shared_ptr<IPrimitive> > _instances;
        };

        //! This class provides a mesh primitive.
        class MeshPrimitive : public IPrimitive
        {
            DJV_NON_COPYABLE(MeshPrimitive);

        protected:
            MeshPrimitive();

        public:
            static std::shared_ptr<MeshPrimitive> create();
        };

    } // namespace Scene
} // namespace djv

#include <djvScene/PrimitiveInline.h>
