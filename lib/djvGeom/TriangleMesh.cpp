// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvGeom/TriangleMesh.h>

#include <djvCore/UID.h>

#include <glm/geometric.hpp>

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

        void TriangleMesh::calcNormals(TriangleMesh& mesh)
        {
            const size_t trianglesSize = mesh.triangles.size();
            mesh.n.resize(trianglesSize);
            for (size_t i = 0; i < trianglesSize; ++i)
            {
                auto& tri = mesh.triangles[i];
                const size_t p0 = tri.v0.v;
                const size_t p1 = tri.v1.v;
                const size_t p2 = tri.v2.v;
                if (p0 && p1 && p2)
                {
                    const auto& v0 = mesh.v[p0 - 1];
                    const auto& v1 = mesh.v[p1 - 1];
                    const auto& v2 = mesh.v[p2 - 1];
                    mesh.n[i] = glm::normalize(glm::cross(v1 - v0, v2 - v0));
                    tri.v0.n = i + 1;
                    tri.v1.n = i + 1;
                    tri.v2.n = i + 1;
                }
            }
        }

        void TriangleMesh::triangulateBBox(const Math::BBox3f& value, TriangleMesh& mesh)
        {
            mesh.clear();

            mesh.v.push_back(glm::vec3(value.min.x, value.min.y, value.max.z));
            mesh.v.push_back(glm::vec3(value.max.x, value.min.y, value.max.z));
            mesh.v.push_back(glm::vec3(value.max.x, value.min.y, value.min.z));
            mesh.v.push_back(glm::vec3(value.min.x, value.min.y, value.min.z));
            mesh.v.push_back(glm::vec3(value.min.x, value.max.y, value.max.z));
            mesh.v.push_back(glm::vec3(value.max.x, value.max.y, value.max.z));
            mesh.v.push_back(glm::vec3(value.max.x, value.max.y, value.min.z));
            mesh.v.push_back(glm::vec3(value.min.x, value.max.y, value.min.z));

            mesh.t.push_back(glm::vec3(0.F, 1.F, 0.F));
            mesh.t.push_back(glm::vec3(1.F, 1.F, 0.F));
            mesh.t.push_back(glm::vec3(1.F, 0.F, 0.F));
            mesh.t.push_back(glm::vec3(0.F, 0.F, 0.F));

            // Back
            const size_t offset = 1;
            TriangleMesh::Triangle a;
            TriangleMesh::Triangle b;
            a.v0.v = 0 + offset;
            a.v1.v = 3 + offset;
            a.v2.v = 2 + offset;
            a.v0.t = 0 + offset;
            a.v1.t = 3 + offset;
            a.v2.t = 2 + offset;
            mesh.triangles.push_back(a);
            b.v0.v = 2 + offset;
            b.v1.v = 1 + offset;
            b.v2.v = 0 + offset;
            b.v0.t = 2 + offset;
            b.v1.t = 1 + offset;
            b.v2.t = 0 + offset;
            mesh.triangles.push_back(b);

            // Front
            a.v0.v = 4 + offset;
            a.v1.v = 5 + offset;
            a.v2.v = 6 + offset;
            mesh.triangles.push_back(a);
            b.v0.v = 6 + offset;
            b.v1.v = 7 + offset;
            b.v2.v = 4 + offset;
            mesh.triangles.push_back(b);

            // Top
            a.v0.v = 0 + offset;
            a.v1.v = 1 + offset;
            a.v2.v = 5 + offset;
            mesh.triangles.push_back(a);
            b.v0.v = 5 + offset;
            b.v1.v = 4 + offset;
            b.v2.v = 0 + offset;
            mesh.triangles.push_back(b);

            // Bottom
            a.v0.v = 2 + offset;
            a.v1.v = 3 + offset;
            a.v2.v = 7 + offset;
            mesh.triangles.push_back(a);
            b.v0.v = 7 + offset;
            b.v1.v = 6 + offset;
            b.v2.v = 2 + offset;
            mesh.triangles.push_back(b);

            // Left
            a.v0.v = 0 + offset;
            a.v1.v = 4 + offset;
            a.v2.v = 7 + offset;
            mesh.triangles.push_back(a);
            b.v0.v = 7 + offset;
            b.v1.v = 3 + offset;
            b.v2.v = 0 + offset;
            mesh.triangles.push_back(b);

            // Right
            a.v0.v = 1 + offset;
            a.v1.v = 2 + offset;
            a.v2.v = 6 + offset;
            mesh.triangles.push_back(a);
            b.v0.v = 6 + offset;
            b.v1.v = 5 + offset;
            b.v2.v = 1 + offset;
            mesh.triangles.push_back(b);

            calcNormals(mesh);
        }

        bool TriangleMesh::intersectTriangle(
            const glm::vec3& pos,
            const glm::vec3& dir,
            const glm::vec3& v0,
            const glm::vec3& v1,
            const glm::vec3& v2,
            glm::vec3&       out,
            glm::vec3&       barycentric)
        {
            const float epsilon = .1e-6F;

            const glm::vec3 edge1 = v1 - v0;
            const glm::vec3 edge2 = v2 - v0;

            const glm::vec3 h = glm::cross(dir, edge2);
            const float a = glm::dot(edge1, h);
            if (a > -epsilon && a < epsilon)
                return false;

            const float f = 1.F / a;
            const glm::vec3 s = pos - v0;
            const float u = f * glm::dot(s, h);
            if (u < 0.F || u > 1.F)
                return false;

            const glm::vec3 q = glm::cross(s, edge1);
            const float v = f * glm::dot(dir, q);
            if (v < 0.F || u + v > 1.F)
                return false;

            const float t = f * glm::dot(edge2, q);
            if (t > epsilon)
            {
                out = pos + dir * t;
                barycentric.x = 1.F - u - v;
                barycentric.y = u;
                barycentric.z = v;
                return true;
            }
            return false;
        }

        bool TriangleMesh::intersect(
            const glm::vec3&    pos,
            const glm::vec3&    dir,
            const TriangleMesh& mesh,
            glm::vec3 &         hit)
        {
            bool out = false;
            float closest = 0.F;
            glm::vec3 barycentric;
            bool first = true;
            size_t i = 0;
            for (const auto& triangle : mesh.triangles)
            {
                const TriangleMesh::Vertex& vert0 = triangle.v0;
                const TriangleMesh::Vertex& vert1 = triangle.v1;
                const TriangleMesh::Vertex& vert2 = triangle.v2;

                const glm::vec3& v0 = mesh.v[vert0.v - 1];
                const glm::vec3& v1 = mesh.v[vert1.v - 1];
                const glm::vec3& v2 = mesh.v[vert2.v - 1];
                glm::vec3 hitTemp;
                glm::vec3 barycentricTemp;
                if (intersectTriangle(pos, dir, v0, v1, v2, hitTemp, barycentricTemp))
                {
                    const float distance = glm::distance(pos, hitTemp);
                    if (distance < closest || first)
                    {
                        hit = hitTemp;
                        out = true;
                        closest = distance;
                        barycentric = barycentricTemp;
                        first = false;
                    }
                }

                ++i;
            }

            return out;
        }

        bool TriangleMesh::intersect(
            const glm::vec3&    pos,
            const glm::vec3&    dir,
            const TriangleMesh& mesh,
            glm::vec3&          hit,
            glm::vec3&          hitColor,
            glm::vec2&          hitTexture,
            glm::vec3&          hitNormal)
        {
            bool out = false;
            float closest = 0.F;
            glm::vec3 barycentric;
            size_t index = 0;
            bool first = true;
            size_t i = 0;
            for (const auto& triangle : mesh.triangles)
            {
                const TriangleMesh::Vertex& vert0 = triangle.v0;
                const TriangleMesh::Vertex& vert1 = triangle.v1;
                const TriangleMesh::Vertex& vert2 = triangle.v2;

                const glm::vec3& v0 = mesh.v[vert0.v - 1];
                const glm::vec3& v1 = mesh.v[vert1.v - 1];
                const glm::vec3& v2 = mesh.v[vert2.v - 1];
                glm::vec3 hitTemp;
                glm::vec3 barycentricTemp;
                if (intersectTriangle(pos, dir, v0, v1, v2, hitTemp, barycentricTemp))
                {
                    const float distance = glm::distance(pos, hitTemp);
                    if (distance < closest || first)
                    {
                        hit = hitTemp;
                        out = true;
                        closest = distance;
                        barycentric = barycentricTemp;
                        index = i;
                        first = false;
                    }
                }

                ++i;
            }

            if (out)
            {
                const TriangleMesh::Vertex& vert0 = mesh.triangles[index].v0;
                const TriangleMesh::Vertex& vert1 = mesh.triangles[index].v1;
                const TriangleMesh::Vertex& vert2 = mesh.triangles[index].v2;

                if (vert0.v <= mesh.c.size() && vert1.v <= mesh.c.size() && vert2.v <= mesh.c.size())
                {
                    const glm::vec3& c0 = mesh.c[vert0.v - 1];
                    const glm::vec3& c1 = mesh.c[vert1.v - 1];
                    const glm::vec3& c2 = mesh.c[vert2.v - 1];
                    hitColor.x = c0.x * barycentric.x + c1.x * barycentric.y + c2.x * barycentric.z;
                    hitColor.y = c0.y * barycentric.x + c1.y * barycentric.y + c2.y * barycentric.z;
                    hitColor.z = c0.z * barycentric.x + c1.z * barycentric.y + c2.z * barycentric.z;
                }

                if (vert0.t && vert1.t && vert2.t)
                {
                    const glm::vec2& t0 = mesh.t[vert0.t - 1];
                    const glm::vec2& t1 = mesh.t[vert1.t - 1];
                    const glm::vec2& t2 = mesh.t[vert2.t - 1];
                    hitTexture.x = t0.x * barycentric.x + t1.x * barycentric.y + t2.x * barycentric.z;
                    hitTexture.y = t0.y * barycentric.x + t1.y * barycentric.y + t2.y * barycentric.z;
                }

                if (vert0.n && vert1.n && vert2.n)
                {
                    const glm::vec3& n0 = mesh.n[vert0.n - 1];
                    const glm::vec3& n1 = mesh.n[vert1.n - 1];
                    const glm::vec3& n2 = mesh.n[vert2.n - 1];
                    hitNormal.x = n0.x * barycentric.x + n1.x * barycentric.y + n2.x * barycentric.z;
                    hitNormal.y = n0.y * barycentric.x + n1.y * barycentric.y + n2.y * barycentric.z;
                    hitNormal.z = n0.z * barycentric.x + n1.z * barycentric.y + n2.z * barycentric.z;
                }
            }

            return out;
        }

        void TriangleMesh::faceToTriangles(
            const TriangleMesh::Face& face,
            std::vector<TriangleMesh::Triangle>& triangles)
        {
            const size_t size = face.v.size();
            for (size_t i = 1; i < size - 1; ++i)
            {
                TriangleMesh::Triangle t;
                t.v0.v = face.v[0].v;
                t.v0.t = face.v[0].t;
                t.v0.n = face.v[0].n;
                t.v1.v = face.v[i].v;
                t.v1.t = face.v[i].t;
                t.v1.n = face.v[i].n;
                t.v2.v = face.v[i + 1].v;
                t.v2.t = face.v[i + 1].t;
                t.v2.n = face.v[i + 1].n;
                triangles.push_back(t);
            }
        }

    } // namespace Geom
} // namespace djv
