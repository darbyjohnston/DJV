// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvGeom/TriangleMesh.h>

#include <djvCore/UIDFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace Geom
    {
        TriangleMesh::TriangleMesh() :
            _uid(createUID())
        {}

        TriangleMesh::Vertex::Vertex()
        {}

        TriangleMesh::Triangle::Triangle()
        {}

        void TriangleMesh::clear()
        {
            v.clear();
            c.clear();
            t.clear();
            n.clear();
            triangles.clear();
        }

        void TriangleMesh::bboxUpdate()
        {
            bbox.zero();
            if (v.size())
            {
                bbox = Math::BBox3f(v[0]);
                for (const auto& i : v)
                {
                    bbox.expand(i);
                }
            }
        }

    } // namespace Geom
} // namespace djv
