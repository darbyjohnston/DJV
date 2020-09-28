// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvGeom/TriangleMesh.h>

#include <djvMath/BBox.h>

#include <djvCore/UID.h>

#include <vector>

namespace djv
{
    namespace Geom
    {
        //! Convert a face into triangles.
        void faceToTriangles(
            const TriangleMesh::Face&,
            std::vector<TriangleMesh::Triangle>&);

        //! Calculate the mesh normals.
        //! \todo Implement smoothing.
        //! \todo Add an option for CW and CCW.
        void calcNormals(TriangleMesh&);

        //! Intersect a line with a triangle.
        bool intersectTriangle(
            const glm::vec3& pos,
            const glm::vec3& dir,
            const glm::vec3& v0,
            const glm::vec3& v1,
            const glm::vec3& v2,
            glm::vec3&       hit,
            glm::vec3&       barycentric);

        //! Intersect a line with a mesh.
        bool intersect(
            const glm::vec3&    pos,
            const glm::vec3&    dir,
            const TriangleMesh& mesh,
            glm::vec3&          hit);
        bool intersect(
            const glm::vec3&    pos,
            const glm::vec3&    dir,
            const TriangleMesh& mesh,
            glm::vec3&          hit,
            glm::vec3&          hitColor,
            glm::vec2&          hitTexture,
            glm::vec3&          hitNormal);

        //! Create a mesh from a bounding-box.
        void triangulateBBox(const Math::BBox3f&, TriangleMesh&);

    } // namespace Geom
} // namespace djv
