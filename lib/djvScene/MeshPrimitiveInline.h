// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Scene
    {
        inline MeshPrimitive::MeshPrimitive()
        {}

        inline std::shared_ptr<MeshPrimitive> MeshPrimitive::create()
        {
            return std::shared_ptr<MeshPrimitive>(new MeshPrimitive);
        }

        inline void MeshPrimitive::addMesh(const std::shared_ptr<Geom::TriangleMesh>& value)
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

        inline std::string MeshPrimitive::getClassName() const
        {
            return "MeshPrimitive";
        }

        inline const std::vector<std::shared_ptr<Geom::TriangleMesh> >& MeshPrimitive::getMeshes() const
        {
            return _meshes;
        }

        inline size_t MeshPrimitive::getPointCount() const
        {
            return _pointCount;
        }

    } // namespace Scene
} // namespace djv

