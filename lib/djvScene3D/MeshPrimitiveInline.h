// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Scene3D
    {
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

    } // namespace Scene3D
} // namespace djv
