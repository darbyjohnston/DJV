// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/AV.h>

#include <djvCore/BBox.h>
#include <djvCore/UID.h>

#include <vector>

namespace djv
{
    namespace AV
    {
        //! This namespace provides geometry functionality.
        namespace Geom
        {
            //! This struct provides a triangle mesh.
            class TriangleMesh
            {
            public:
                TriangleMesh();

                Core::UID getUID() const;

                //! This struct provides a vertex.
                struct Vertex
                {
                    Vertex() {}

                    explicit constexpr Vertex(size_t v, size_t t = 0, size_t n = 0) :
                        v(v),
                        t(t),
                        n(n)
                    {}

                    size_t v = 0;
                    size_t t = 0;
                    size_t n = 0;

                    bool operator == (const Vertex&) const;
                };

                //! This struct provides a face.
                struct Face
                {
                    std::vector<Vertex> v;

                    bool operator == (const Face&) const;
                };

                //! This struct provides a triangle.
                struct Triangle
                {
                    Triangle() {}

                    Vertex v0;
                    Vertex v1;
                    Vertex v2;

                    bool operator == (const Triangle&) const;
                };

                //! \name Mesh Components
                ///@{

                std::vector<glm::vec3> v;
                std::vector<glm::vec3> c;
                std::vector<glm::vec2> t;
                std::vector<glm::vec3> n;
                std::vector<Triangle> triangles;

                Core::BBox3f bbox = Core::BBox3f(0.F, 0.F, 0.F, 0.F, 0.F, 0.F);

                void clear();

                ///@}

                //! \name Mesh Utilities
                ///@{

                //! Compute the bounding-box of the mesh.
                void bboxUpdate();

                //! Convert a face into triangles.
                static void faceToTriangles(const Face&, std::vector<Triangle>&);

                //! Calculate the mesh normals.
                //! \todo Implement smoothing.
                //! \todo Add an option for CW and CCW.
                static void calcNormals(TriangleMesh&);

                //! Intersect a line with a triangle.
                static bool intersectTriangle(
                    const glm::vec3& pos,
                    const glm::vec3& dir,
                    const glm::vec3& v0,
                    const glm::vec3& v1,
                    const glm::vec3& v2,
                    glm::vec3&       hit,
                    glm::vec3&       barycentric);

                //! Intersect a line with a mesh.
                static bool intersect(
                    const glm::vec3&    pos,
                    const glm::vec3&    dir,
                    const TriangleMesh& mesh,
                    glm::vec3&          hit);
                static bool intersect(
                    const glm::vec3&    pos,
                    const glm::vec3&    dir,
                    const TriangleMesh& mesh,
                    glm::vec3&          hit,
                    glm::vec3&          hitColor,
                    glm::vec2&          hitTexture,
                    glm::vec3&          hitNormal);

                //! Create a mesh from a bounding-box.
                static void triangulateBBox(const Core::BBox3f&, TriangleMesh&);

                ///@}

            private:
                Core::UID _uid = 0;
            };

        } // namespace Geom
    } // namespace AV
} // namespace djv

#include <djvAV/TriangleMeshInline.h>
