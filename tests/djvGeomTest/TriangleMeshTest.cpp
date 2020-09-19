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
                Geom::TriangleMesh mesh;
                std::stringstream ss;
                ss << mesh.getUID();
                _print("UID: " + ss.str());
            }
            
            {
                Geom::TriangleMesh::Vertex v;
                DJV_ASSERT(0 == v.v);
                DJV_ASSERT(0 == v.t);
                DJV_ASSERT(0 == v.n);
            }
            
            {
                Geom::TriangleMesh::Vertex v(1, 2, 3);
                DJV_ASSERT(1 == v.v);
                DJV_ASSERT(2 == v.t);
                DJV_ASSERT(3 == v.n);
            }
            
            {
                Geom::TriangleMesh::Vertex v(1, 2, 3);
                DJV_ASSERT(v == v);
            }
            
            {
                Geom::TriangleMesh::Face f;
                f.v.push_back(Geom::TriangleMesh::Vertex(1, 2, 3));
                DJV_ASSERT(f == f);
            }
            
            {
                Geom::TriangleMesh::Triangle t;
                t.v0 = Geom::TriangleMesh::Vertex(1, 2, 3);
                t.v1 = Geom::TriangleMesh::Vertex(4, 5, 6);
                t.v2 = Geom::TriangleMesh::Vertex(7, 8, 9);
                DJV_ASSERT(t == t);
            }
            
            {
                Geom::TriangleMesh mesh;
                DJV_ASSERT(0 == mesh.v.size());
                DJV_ASSERT(0 == mesh.c.size());
                DJV_ASSERT(0 == mesh.t.size());
                DJV_ASSERT(0 == mesh.n.size());
                DJV_ASSERT(0 == mesh.triangles.size());
                mesh.v.push_back(glm::vec3());
                mesh.c.push_back(glm::vec3());
                mesh.t.push_back(glm::vec2());
                mesh.n.push_back(glm::vec3());
                mesh.triangles.push_back(Geom::TriangleMesh::Triangle());
                mesh.clear();
                DJV_ASSERT(0 == mesh.v.size());
                DJV_ASSERT(0 == mesh.c.size());
                DJV_ASSERT(0 == mesh.t.size());
                DJV_ASSERT(0 == mesh.n.size());
                DJV_ASSERT(0 == mesh.triangles.size());
            }
            
            {
                Geom::TriangleMesh mesh;
                const glm::vec3 min(-1.F, -1.F, -1.F);
                const glm::vec3 max( 1.F,  1.F,  1.F);
                mesh.v.push_back(min);
                mesh.v.push_back(max);
                mesh.bboxUpdate();
                DJV_ASSERT(min == mesh.bbox.min);
                DJV_ASSERT(max == mesh.bbox.max);
            }
            
            {
                Geom::TriangleMesh::Face f;
                std::vector<Geom::TriangleMesh::Vertex> v;
                v.push_back(Geom::TriangleMesh::Vertex(1, 2, 3));
                v.push_back(Geom::TriangleMesh::Vertex(4, 5, 6));
                v.push_back(Geom::TriangleMesh::Vertex(7, 8, 9));
                v.push_back(Geom::TriangleMesh::Vertex(10, 11, 12));
                for (const auto& i : v)
                {
                    f.v.push_back(i);
                }
                std::vector<Geom::TriangleMesh::Triangle> t;
                Geom::TriangleMesh::faceToTriangles(f, t);
                DJV_ASSERT(2 == t.size());
                DJV_ASSERT(v[0] == t[0].v0);
                DJV_ASSERT(v[1] == t[0].v1);
                DJV_ASSERT(v[2] == t[0].v2);
                DJV_ASSERT(v[0] == t[1].v0);
                DJV_ASSERT(v[2] == t[1].v1);
                DJV_ASSERT(v[3] == t[1].v2);
            }
            
            {
                Geom::TriangleMesh mesh;
                mesh.v.push_back(glm::vec3(0.F, 0.F, 0.F));
                mesh.v.push_back(glm::vec3(1.F, 0.F, 0.F));
                mesh.v.push_back(glm::vec3(0.F, 0.F, 1.F));
                Geom::TriangleMesh::Triangle t;
                t.v0.v = 1;
                t.v1.v = 2;
                t.v2.v = 3;
                mesh.triangles.push_back(t);
                Geom::TriangleMesh::calcNormals(mesh);
                DJV_ASSERT(1 == mesh.n.size());
            }
            
            {
                glm::vec3 hit(0.F, 0.F, 0.F);
                glm::vec3 barycentric(0.F, 0.F, 0.F);
                DJV_ASSERT(Geom::TriangleMesh::intersectTriangle(
                    glm::vec3(0.F, 0.F, 1.F),
                    glm::vec3(0.F, 0.F, -1.F),
                    glm::vec3(-1.F, -1.F, 0.F),
                    glm::vec3(1.F, -1.F, 0.F),
                    glm::vec3(0.F, 1.F, 0.F),
                    hit,
                    barycentric));
                {
                    std::stringstream ss;
                    ss << hit;
                    _print("Hit: " + ss.str());
                }
                {
                    std::stringstream ss;
                    ss << barycentric;
                    _print("Barycentric: " + ss.str());
                }
                DJV_ASSERT(!Geom::TriangleMesh::intersectTriangle(
                    glm::vec3(0.F, 0.F, 1.F),
                    glm::vec3(0.F, 0.F, 1.F),
                    glm::vec3(-1.F, -1.F, 0.F),
                    glm::vec3(1.F, -1.F, 0.F),
                    glm::vec3(0.F, 1.F, 0.F),
                    hit,
                    barycentric));
            }
            
            {
                Geom::TriangleMesh mesh;
                mesh.v.push_back(glm::vec3(-1.F, -1.F, 0.F));
                mesh.v.push_back(glm::vec3(1.F, -1.F, 0.F));
                mesh.v.push_back(glm::vec3(0.F, 1.F, 0.F));
                Geom::TriangleMesh::Triangle t;
                t.v0.v = 1;
                t.v1.v = 2;
                t.v2.v = 3;
                mesh.triangles.push_back(t);
                glm::vec3 hit(0.F, 0.F, 0.F);
                DJV_ASSERT(Geom::TriangleMesh::intersect(
                    glm::vec3(0.F, 0.F, 1.F),
                    glm::vec3(0.F, 0.F, -1.F),
                    mesh,
                    hit));
                {
                    std::stringstream ss;
                    ss << hit;
                    _print("Hit: " + ss.str());
                }
                DJV_ASSERT(!Geom::TriangleMesh::intersect(
                    glm::vec3(0.F, 0.F, 1.F),
                    glm::vec3(0.F, 0.F, 1.F),
                    mesh,
                    hit));
            }
            
            {
                Geom::TriangleMesh mesh;
                mesh.v.push_back(glm::vec3(-1.F, -1.F, 0.F));
                mesh.v.push_back(glm::vec3(1.F, -1.F, 0.F));
                mesh.v.push_back(glm::vec3(0.F, 1.F, 0.F));
                mesh.c.push_back(glm::vec3(1.F, 0.F, 0.F));
                mesh.c.push_back(glm::vec3(0.F, 1.F, 0.F));
                mesh.c.push_back(glm::vec3(0.F, 0.F, 1.F));
                mesh.t.push_back(glm::vec2(0.F, 0.F));
                mesh.t.push_back(glm::vec2(1.F, 0.F));
                mesh.t.push_back(glm::vec2(1.F, 1.F));
                mesh.n.push_back(glm::vec3(0.F, 0.F, -1.F));
                mesh.n.push_back(glm::vec3(0.F, 0.F, -1.F));
                mesh.n.push_back(glm::vec3(0.F, 0.F, -1.F));
                Geom::TriangleMesh::Triangle t;
                t.v0.v = 1;
                t.v1.v = 2;
                t.v2.v = 3;
                t.v0.t = 1;
                t.v1.t = 2;
                t.v2.t = 3;
                t.v0.n = 1;
                t.v1.n = 2;
                t.v2.n = 3;
                mesh.triangles.push_back(t);
                glm::vec3 hit(0.F, 0.F, 0.F);
                glm::vec3 hitColor(0.F, 0.F, 0.F);
                glm::vec2 hitTexture(0.F, 0.F);
                glm::vec3 hitNormal(0.F, 0.F, 0.F);
                DJV_ASSERT(Geom::TriangleMesh::intersect(
                    glm::vec3(0.F, 0.F, 1.F),
                    glm::vec3(0.F, 0.F, -1.F),
                    mesh,
                    hit,
                    hitColor,
                    hitTexture,
                    hitNormal));
                {
                    std::stringstream ss;
                    ss << hit;
                    _print("Hit: " + ss.str());
                }
                {
                    std::stringstream ss;
                    ss << hit;
                    _print("Hit color: " + ss.str());
                }
                {
                    std::stringstream ss;
                    ss << hit;
                    _print("Hit texture: " + ss.str());
                }
                {
                    std::stringstream ss;
                    ss << hit;
                    _print("Hit normal: " + ss.str());
                }
                DJV_ASSERT(!Geom::TriangleMesh::intersect(
                    glm::vec3(0.F, 0.F, 1.F),
                    glm::vec3(0.F, 0.F, 1.F),
                    mesh,
                    hit,
                    hitColor,
                    hitTexture,
                    hitNormal));
            }
            
            {
                Math::BBox3f bbox(-1.F, -1.F, -1.F, 1.F, 1.F, 1.F);
                Geom::TriangleMesh mesh;
                Geom::TriangleMesh::triangulateBBox(bbox, mesh);
                DJV_ASSERT(12 == mesh.triangles.size());
            }
        }

    } // namespace GeomTest
} // namespace djv

