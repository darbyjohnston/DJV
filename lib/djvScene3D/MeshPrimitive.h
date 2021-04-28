// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvScene3D/IPrimitive.h>

#include <djvGeom/TriangleMesh.h>

namespace djv
{
    namespace Scene3D
    {
        //! Mesh primitive.
        class MeshPrimitive : public IPrimitive
        {
            DJV_NON_COPYABLE(MeshPrimitive);

        protected:
            MeshPrimitive();

        public:
            static std::shared_ptr<MeshPrimitive> create();

            void addMesh(const std::shared_ptr<Geom::TriangleMesh>&);

            std::string getClassName() const override;
            const std::vector<std::shared_ptr<Geom::TriangleMesh> >& getMeshes() const override;
            size_t getPointCount() const override;

        private:
            std::vector<std::shared_ptr<Geom::TriangleMesh> > _meshes;
            size_t _pointCount = 0;
        };

    } // namespace Scene3D
} // namespace djv

#include <djvScene3D/MeshPrimitiveInline.h>
