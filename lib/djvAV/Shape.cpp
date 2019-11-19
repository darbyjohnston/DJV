//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvAV/Shape.h>

#include <djvAV/TriangleMesh.h>

#include <djvCore/Math.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace Geom
        {
            IShape::~IShape()
            {}

            Square::Square(float radius) :
                _radius(radius)
            {}

            void Square::setRadius(float value)
            {
                _radius = value;
            }

            namespace
            {
                const size_t offset = 1;
            }

            void Square::triangulate(TriangleMesh & mesh) const
            {
                mesh.v.push_back(glm::vec3(-_radius, 0.F, _radius));
                mesh.v.push_back(glm::vec3(_radius, 0.F, _radius));
                mesh.v.push_back(glm::vec3(_radius, 0.F, -_radius));
                mesh.v.push_back(glm::vec3(-_radius, 0.F, -_radius));

                mesh.t.push_back(glm::vec3(0.F, 0.F, 0.F));
                mesh.t.push_back(glm::vec3(1.F, 0.F, 0.F));
                mesh.t.push_back(glm::vec3(1.F, 1.F, 0.F));
                mesh.t.push_back(glm::vec3(0.F, 1.F, 0.F));

                TriangleMesh::Triangle triangle;
                triangle.v0.v = triangle.v0.t = 0 + offset;
                triangle.v1.v = triangle.v1.t = 1 + offset;
                triangle.v2.v = triangle.v2.t = 2 + offset;
                mesh.triangles.push_back(triangle);

                triangle.v0.v = triangle.v0.t = 2 + offset;
                triangle.v1.v = triangle.v1.t = 3 + offset;
                triangle.v2.v = triangle.v2.t = 0 + offset;
                mesh.triangles.push_back(triangle);

                TriangleMesh::calcNormals(mesh);
            }

            Circle::Circle(float radius, size_t resolution) :
                _radius(radius)
            {
                setResolution(resolution);
            }

            void Circle::setRadius(float value)
            {
                _radius = value;
            }

            void Circle::setResolution(size_t value)
            {
                _resolution = std::max(value, size_t(3));
            }

            void Circle::triangulate(TriangleMesh & mesh) const
            {
                for (size_t i = 0; i < _resolution; ++i)
                {
                    const float v = i / static_cast<float>(_resolution);
                    const float c = cosf(v * Math::pi2);
                    const float s = sinf(v * Math::pi2);
                    mesh.v.push_back(glm::vec3(c * _radius, 0.F, s * _radius));
                    mesh.t.push_back(glm::vec3((c + 1.F) / 2.F, (s + 1.F) / 2.F, 0.F));
                }

                TriangleMesh::Triangle triangle;
                const size_t resolutionMinusOne = static_cast<size_t>(_resolution - 1);
                for (size_t i = 1; i < resolutionMinusOne; ++i)
                {
                    triangle.v0.v = triangle.v0.t = 0 + offset;
                    triangle.v1.v = triangle.v1.t = i + offset;
                    triangle.v2.v = triangle.v2.t = i + 1 + offset;
                    mesh.triangles.push_back(triangle);
                }

                TriangleMesh::calcNormals(mesh);
            }

            Cube::Cube(float radius) :
                _radius(radius)
            {}

            void Cube::setRadius(float value)
            {
                _radius = value;
            }

            void Cube::triangulate(TriangleMesh & mesh) const
            {
                BBox3f bbox;
                bbox.min = glm::vec3(-_radius, -_radius, -_radius);
                bbox.max = glm::vec3(_radius, _radius, _radius);
                TriangleMesh::triangulateBBox(bbox, mesh);
            }

            Sphere::Sphere(float radius, const Resolution & resolution) :
                _radius(radius)
            {
                setResolution(resolution);
            }

            void Sphere::setRadius(float value)
            {
                _radius = value;
            }

            void Sphere::setResolution(const Resolution & value)
            {
                _resolution.first = std::max(value.first, size_t(3));
                _resolution.second = std::max(value.second, size_t(3));
            }

            void Sphere::setUSpan(const Span & value)
            {
                _uSpan = value;
            }

            void Sphere::setVSpan(const Span & value)
            {
                _vSpan = value;
            }

            void Sphere::setTextureSpan(bool value)
            {
                _textureSpan = value;
            }

            void Sphere::triangulate(TriangleMesh & mesh) const
            {
                //! \bug Use only a single vertex at each pole.
                for (size_t v = 0; v <= _resolution.second; ++v)
                {
                    const float v1 = v / static_cast<float>(_resolution.second);
                    const float v2 = Math::lerp(v1, _vSpan.first, _vSpan.second);

                    for (size_t u = 0; u <= _resolution.first; ++u)
                    {
                        const float u1 = u / static_cast<float>(_resolution.first);
                        const float u2 = Math::lerp(u1, _uSpan.first, _uSpan.second);
                        const float x = _radius * sinf(v2 * Math::pi) * cosf(u2 * Math::pi2);
                        const float y = _radius * cosf(v2 * Math::pi);
                        const float z = _radius * sinf(v2 * Math::pi) * sinf(u2 * Math::pi2);
                        mesh.v.push_back(glm::vec3(x, y, z));
                        if (_textureSpan)
                        {
                            mesh.t.push_back(glm::vec3(u1, 1.F - v1, 0.F));
                        }
                        else
                        {
                            mesh.t.push_back(glm::vec3(u2, 1.F - v2, 0.F));
                        }
                    }
                }

                TriangleMesh::Triangle triangle;
                for (size_t v = 0; v < _resolution.second; ++v)
                {
                    for (size_t u = 0; u < _resolution.first; ++u)
                    {
                        const size_t i = u + v * (_resolution.first + 1);
                        const size_t j = u + (v + 1) * (_resolution.first + 1);
                        triangle.v0.v = triangle.v0.t = j + 1 + offset;
                        triangle.v1.v = triangle.v1.t = j + offset;
                        triangle.v2.v = triangle.v2.t = i + offset;
                        mesh.triangles.push_back(triangle);
                        triangle.v0.v = triangle.v0.t = i + offset;
                        triangle.v1.v = triangle.v1.t = i + 1 + offset;
                        triangle.v2.v = triangle.v2.t = j + 1 + offset;
                        mesh.triangles.push_back(triangle);
                    }
                }

                TriangleMesh::calcNormals(mesh);
            }

            Cylinder::Cylinder(float radius, float length, size_t resolution) :
                _radius(radius),
                _length(length)
            {
                setResolution(resolution);
            }

            void Cylinder::setRadius(float value)
            {
                _radius = value;
            }

            void Cylinder::setLength(float value)
            {
                _length = value;
            }

            void Cylinder::setResolution(size_t value)
            {
                _resolution = std::max(value, size_t(3));
            }

            void Cylinder::setSpan(const Span & value)
            {
                _span = value;
            }

            void Cylinder::setCapped(bool value)
            {
                _capped = value;
            }

            void Cylinder::triangulate(TriangleMesh & mesh) const
            {
                const float l = _length / 2.F;
                for (size_t u = 0; u <= _resolution; ++u)
                {
                    const float u1 = u / static_cast<float>(_resolution);
                    const float u2 = Math::lerp(u1, _span.first, _span.second);
                    const float c = cosf(u2 * Math::pi2);
                    const float s = sinf(u2 * Math::pi2);
                    glm::vec3 v(c * _radius, -l, s * _radius);
                    mesh.v.push_back(v);
                    if (_textureSpan)
                    {
                        mesh.t.push_back(glm::vec3(u1, 0.F, 0.F));
                    }
                    else
                    {
                        mesh.t.push_back(glm::vec3(u2, 0.F, 0.F));
                    }
                    v.y = l;
                    mesh.v.push_back(v);
                    if (_textureSpan)
                    {
                        mesh.t.push_back(glm::vec3(u1, 1.F, 0.F));
                    }
                    else
                    {
                        mesh.t.push_back(glm::vec3(u2, 1.F, 0.F));
                    }
                }

                TriangleMesh::Triangle triangle;
                for (size_t i = 0; i < _resolution; ++i)
                {
                    triangle.v0.v = triangle.v0.t = i * 2 + 0 + offset;
                    triangle.v1.v = triangle.v1.t = i * 2 + 1 + offset;
                    triangle.v2.v = triangle.v2.t = i * 2 + 3 + offset;
                    mesh.triangles.push_back(triangle);
                    triangle.v0.v = triangle.v0.t = i * 2 + 3 + offset;
                    triangle.v1.v = triangle.v1.t = i * 2 + 2 + offset;
                    triangle.v2.v = triangle.v2.t = i * 2 + 0 + offset;
                    mesh.triangles.push_back(triangle);
                }

                if (_capped)
                {
                    for (size_t i = 1; i < _resolution - 1; ++i)
                    {
                        triangle.v0.v = triangle.v0.t = 0 + offset;
                        triangle.v1.v = triangle.v1.t = i * 2 + 0 + offset;
                        triangle.v2.v = triangle.v2.t = i * 2 + 2 + offset;
                        mesh.triangles.push_back(triangle);
                        triangle.v0.v = triangle.v0.t = 1 + offset;
                        triangle.v1.v = triangle.v1.t = i * 2 + 3 + offset;
                        triangle.v2.v = triangle.v2.t = i * 2 + 1 + offset;
                        mesh.triangles.push_back(triangle);
                    }
                }

                TriangleMesh::calcNormals(mesh);
            }

        } // namespace Geom
    } // namespace AV
} // namespace djv
