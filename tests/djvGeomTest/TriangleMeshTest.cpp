// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvGeomTest/TriangleMeshTest.h>

#include <djvGeom/TriangleMesh.h>

#include <djvMath/VectorFunc.h>

using namespace djv::Core;
using namespace djv::Geom;

namespace djv
{
    namespace GeomTest
    {
        TriangleMeshTest::TriangleMeshTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::GeomTest::TriangleMeshTest", tempPath, context)
        {}
        
        void TriangleMeshTest::run()
        {
            {
                TriangleMesh mesh;
                std::stringstream ss;
                ss << mesh.getUID();
                _print("UID: " + ss.str());
            }
            
            {
                TriangleMesh::Vertex v;
                DJV_ASSERT(0 == v.v);
                DJV_ASSERT(0 == v.t);
                DJV_ASSERT(0 == v.n);
            }
            
            {
                TriangleMesh::Vertex v(1, 2, 3);
                DJV_ASSERT(1 == v.v);
                DJV_ASSERT(2 == v.t);
                DJV_ASSERT(3 == v.n);
            }
            
            {
                TriangleMesh::Vertex v(1, 2, 3);
                DJV_ASSERT(v == v);
            }
            
            {
                TriangleMesh::Face f;
                f.v.push_back(TriangleMesh::Vertex(1, 2, 3));
                DJV_ASSERT(f == f);
            }
            
            {
                TriangleMesh::Triangle t;
                t.v0 = TriangleMesh::Vertex(1, 2, 3);
                t.v1 = TriangleMesh::Vertex(4, 5, 6);
                t.v2 = TriangleMesh::Vertex(7, 8, 9);
                DJV_ASSERT(t == t);
            }
            
            {
                TriangleMesh mesh;
                DJV_ASSERT(0 == mesh.v.size());
                DJV_ASSERT(0 == mesh.c.size());
                DJV_ASSERT(0 == mesh.t.size());
                DJV_ASSERT(0 == mesh.n.size());
                DJV_ASSERT(0 == mesh.triangles.size());
                mesh.v.push_back(glm::vec3());
                mesh.c.push_back(glm::vec3());
                mesh.t.push_back(glm::vec2());
                mesh.n.push_back(glm::vec3());
                mesh.triangles.push_back(TriangleMesh::Triangle());
                mesh.clear();
                DJV_ASSERT(0 == mesh.v.size());
                DJV_ASSERT(0 == mesh.c.size());
                DJV_ASSERT(0 == mesh.t.size());
                DJV_ASSERT(0 == mesh.n.size());
                DJV_ASSERT(0 == mesh.triangles.size());
            }
            
            {
                TriangleMesh mesh;
                const glm::vec3 min(-1.F, -1.F, -1.F);
                const glm::vec3 max( 1.F,  1.F,  1.F);
                mesh.v.push_back(min);
                mesh.v.push_back(max);
                mesh.bboxUpdate();
                DJV_ASSERT(min == mesh.bbox.min);
                DJV_ASSERT(max == mesh.bbox.max);
            }
        }

    } // namespace GeomTest
} // namespace djv

