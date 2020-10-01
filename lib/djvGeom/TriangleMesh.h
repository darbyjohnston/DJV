// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <djvMath/BBox.h>

#include <djvCore/UID.h>

#include <vector>

namespace djv
{
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
                Vertex();
                explicit constexpr Vertex(size_t v, size_t t = 0, size_t n = 0);

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
                Triangle();

                Vertex v0;
                Vertex v1;
                Vertex v2;

                bool operator == (const Triangle&) const;
            };

            std::vector<glm::vec3> v;
            std::vector<glm::vec3> c;
            std::vector<glm::vec2> t;
            std::vector<glm::vec3> n;
            std::vector<Triangle> triangles;

            Math::BBox3f bbox = Math::BBox3f(0.F, 0.F, 0.F, 0.F, 0.F, 0.F);

            //! \name Utility
            ///@{

            void clear();

            //! Compute the bounding-box of the mesh.
            void bboxUpdate();

            ///@}

        private:
            Core::UID _uid = 0;
        };

    } // namespace Geom
} // namespace djv

#include <djvGeom/TriangleMeshInline.h>
