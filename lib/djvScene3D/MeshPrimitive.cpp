// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvScene3D/MeshPrimitive.h>

#include <djvScene3D/Render.h>

using namespace djv::Core;

namespace djv
{
    namespace Scene3D
    {
        MeshPrimitive::MeshPrimitive()
        {}

        std::shared_ptr<MeshPrimitive> MeshPrimitive::create()
        {
            return std::shared_ptr<MeshPrimitive>(new MeshPrimitive);
        }

        void MeshPrimitive::addMesh(const std::shared_ptr<Geom::TriangleMesh>& value)
        {
            _meshes.push_back(value);
            Math::BBox3f bbox = getBBox();
            if (!bbox.isValid())
            {
                bbox = value->bbox;
            }
            else
            {
                bbox.expand(value->bbox);
            }
            setBBox(bbox);
            _pointCount += value->v.size();
        }

    } // namespace Scene3D
} // namespace djv

