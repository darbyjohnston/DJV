//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvCore/BBox.h>

namespace djv
{
    namespace AV
    {
        //! This namespace provides geometry functionality.
        namespace Geom
        {
            //! This struct provides a triangle mesh.
            struct TriangleMesh
            {
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

                    bool operator == (const Vertex &) const;
                };

                //! This struct provides a face.
                struct Face
                {
                    std::vector<Vertex> v;

                    bool operator == (const Face &) const;
                };

                //! This struct provides a triangle.
                struct Triangle
                {
                    Triangle() {}

                    Vertex v0;
                    Vertex v1;
                    Vertex v2;

                    bool operator == (const Triangle &) const;
                };

                //! \name Mesh Components
                ///@{

                std::vector<glm::vec3> v;
                std::vector<glm::vec3> c;
                std::vector<glm::vec2> t;
                std::vector<glm::vec3> n;
                std::vector<Triangle> triangles;

                ///@}

                //! \name Mesh Utilities
                ///@{

                void clear();

                //! Convert a face into triangles.
                static void faceToTriangles(const Face &, std::vector<Triangle> &);

                //! Get the bounding-box of the mesh.
                static Core::BBox3f getBBox(const TriangleMesh &);

                //! Calculate the mesh normals.
                //! \todo Implement smoothing.
                //! \todo Add an option for CW and CCW.
                static void calcNormals(TriangleMesh &);

                //! Intersect a line with a triangle.
                static bool intersectTriangle(
                    const glm::vec3 & pos,
                    const glm::vec3 & dir,
                    const glm::vec3 & v0,
                    const glm::vec3 & v1,
                    const glm::vec3 & v2,
                    glm::vec3 &       hit,
                    glm::vec3 &       barycentric);

                //! Intersect a line with a mesh.
                static bool intersect(
                    const glm::vec3 &    pos,
                    const glm::vec3 &    dir,
                    const TriangleMesh & mesh,
                    glm::vec3 &          hit);
                static bool intersect(
                    const glm::vec3 &    pos,
                    const glm::vec3 &    dir,
                    const TriangleMesh & mesh,
                    glm::vec3 &          hit,
                    glm::vec3 &          hitColor,
                    glm::vec2 &          hitTexture,
                    glm::vec3 &          hitNormal);

                //! Create a mesh from a bounding-box.
                static void triangulateBBox(const Core::BBox3f &, TriangleMesh &);

                ///@}
            };

        } // namespace Geom
    } // namespace AV
} // namespace djv
