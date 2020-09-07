// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/OpenGLMesh.h>

#include <djvAV/TriangleMesh.h>

#include <djvCore/Math.h>

#include <array>
#include <iostream>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace OpenGL
        {
            namespace
            {
                struct PackedNormal
                {
                    unsigned int x : 10;
                    unsigned int y : 10;
                    unsigned int z : 10;
                    unsigned int unused : 2;
                };

                struct PackedColor
                {
                    unsigned int r : 8;
                    unsigned int g : 8;
                    unsigned int b : 8;
                    unsigned int a : 8;
                };

            } // namespace

            void VBO::_init(size_t size, VBOType type)
            {
                _size = size;
                _type = type;
                glGenBuffers(1, &_vbo);
                glBindBuffer(GL_ARRAY_BUFFER, _vbo);
                glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(_size * getVertexByteCount(type)), NULL, GL_DYNAMIC_DRAW);
            }

            VBO::~VBO()
            {
                if (_vbo)
                {
                    glDeleteBuffers(1, &_vbo);
                    _vbo = 0;
                }
            }

            std::shared_ptr<VBO> VBO::create(size_t size, VBOType type)
            {
                auto out = std::shared_ptr<VBO>(new VBO);
                out->_init(size, type);
                return out;
            }

            void VBO::copy(const std::vector<uint8_t>& data)
            {
                glBindBuffer(GL_ARRAY_BUFFER, _vbo);
                glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizei>(data.size()), (void*)data.data());
            }

            void VBO::copy(const std::vector<uint8_t>& data, size_t offset)
            {
                glBindBuffer(GL_ARRAY_BUFFER, _vbo);
                glBufferSubData(GL_ARRAY_BUFFER, offset, static_cast<GLsizei>(data.size()), (void*)data.data());
            }

            void VBO::copy(const std::vector<uint8_t>& data, size_t offset, size_t size)
            {
                glBindBuffer(GL_ARRAY_BUFFER, _vbo);
                glBufferSubData(GL_ARRAY_BUFFER, offset, static_cast<GLsizei>(size), (void*)data.data());
            }

            std::vector<uint8_t> VBO::convert(const Geom::PointList& pointList, VBOType type)
            {
                const size_t size = pointList.v.size();
                const size_t vertexByteCount = getVertexByteCount(type);
                std::vector<uint8_t> out(size * vertexByteCount);
                uint8_t* p = out.data();
                switch (type)
                {
                case VBOType::Pos3_F32:
                    for (size_t i = 0; i < size; ++i)
                    {
                        float* pf = reinterpret_cast<float*>(p);
                        pf[0] = pointList.v[i].x;
                        pf[1] = pointList.v[i].y;
                        pf[2] = pointList.v[i].z;
                        p += 3 * sizeof(float);
                    }
                    break;
                case VBOType::Pos3_F32_Color_U8:
                    for (size_t i = 0; i < size; ++i)
                    {
                        float* pf = reinterpret_cast<float*>(p);
                        pf[0] = pointList.v[i].x;
                        pf[1] = pointList.v[i].y;
                        pf[2] = pointList.v[i].z;
                        p += 3 * sizeof(float);

                        auto packedColor = reinterpret_cast<PackedColor*>(p);
                        packedColor->r = Math::clamp(static_cast<int>(pointList.c[i][0] * 255.F), 0, 255);
                        packedColor->g = Math::clamp(static_cast<int>(pointList.c[i][1] * 255.F), 0, 255);
                        packedColor->b = Math::clamp(static_cast<int>(pointList.c[i][2] * 255.F), 0, 255);
                        packedColor->a = 255;
                        p += sizeof(PackedColor);
                    }
                    break;
                default: break;
                }
                return out;
            }

            std::vector<uint8_t> VBO::convert(const Geom::TriangleMesh& mesh, VBOType type)
            {
                const size_t size = mesh.triangles.size();
                return convert(mesh, type, SizeTRange(0, size > 0 ? size - 1 : 0));
            }

            std::vector<uint8_t> VBO::convert(const Geom::TriangleMesh& mesh, VBOType type, const SizeTRange& range)
            {
                const size_t vertexByteCount = getVertexByteCount(type);
                std::vector<uint8_t> out((range.getMax() - range.getMin() + 1) * 3 * vertexByteCount);
                uint8_t * p = out.data();
                switch (type)
                {
                case VBOType::Pos3_F32_UV_U16:
                    for (size_t i = range.getMin(); i <= range.getMax(); ++i)
                    {
                        const Geom::TriangleMesh::Vertex* vertices[] =
                        {
                            &mesh.triangles[i].v0,
                            &mesh.triangles[i].v1,
                            &mesh.triangles[i].v2
                        };
                        for (size_t k = 0; k < 3; ++k)
                        {
                            const size_t v = vertices[k]->v;
                            float* pf = reinterpret_cast<float*>(p);
                            pf[0] = v ? mesh.v[v - 1][0] : 0.F;
                            pf[1] = v ? mesh.v[v - 1][1] : 0.F;
                            pf[2] = v ? mesh.v[v - 1][2] : 0.F;
                            p += 3 * sizeof(float);

                            const size_t t = vertices[k]->t;
                            uint16_t* pu16 = reinterpret_cast<uint16_t*>(p);
                            pu16[0] = t ? Math::clamp(static_cast<int>(mesh.t[t - 1][0] * 65535.F), 0, 65535) : 0;
                            pu16[1] = t ? Math::clamp(static_cast<int>(mesh.t[t - 1][1] * 65535.F), 0, 65535) : 0;
                            p += 2 * sizeof(uint16_t);
                        }
                    }
                    break;
                case VBOType::Pos3_F32_UV_U16_Normal_U10:
                    for (size_t i = range.getMin(); i <= range.getMax(); ++i)
                    {
                        const Geom::TriangleMesh::Vertex* vertices[] =
                        {
                            &mesh.triangles[i].v0,
                            &mesh.triangles[i].v1,
                            &mesh.triangles[i].v2
                        };
                        for (size_t k = 0; k < 3; ++k)
                        {
                            const size_t v = vertices[k]->v;
                            float* pf = reinterpret_cast<float*>(p);
                            pf[0] = v ? mesh.v[v - 1][0] : 0.F;
                            pf[1] = v ? mesh.v[v - 1][1] : 0.F;
                            pf[2] = v ? mesh.v[v - 1][2] : 0.F;
                            p += 3 * sizeof(float);

                            const size_t t = vertices[k]->t;
                            uint16_t* pu16 = reinterpret_cast<uint16_t*>(p);
                            pu16[0] = t ? Math::clamp(static_cast<int>(mesh.t[t - 1][0] * 65535.F), 0, 65535) : 0;
                            pu16[1] = t ? Math::clamp(static_cast<int>(mesh.t[t - 1][1] * 65535.F), 0, 65535) : 0;
                            p += 2 * sizeof(uint16_t);

                            const size_t n = vertices[k]->n;
                            auto packedNormal = reinterpret_cast<PackedNormal*>(p);
                            packedNormal->x = n ? Math::clamp(static_cast<int>(mesh.n[n - 1][0] * 511.F), -512, 511) : 0;
                            packedNormal->y = n ? Math::clamp(static_cast<int>(mesh.n[n - 1][1] * 511.F), -512, 511) : 0;
                            packedNormal->z = n ? Math::clamp(static_cast<int>(mesh.n[n - 1][2] * 511.F), -512, 511) : 0;
                            p += sizeof(PackedNormal);
                        }
                    }
                    break;
                case VBOType::Pos3_F32_UV_U16_Normal_U10_Color_U8:
                    for (size_t i = range.getMin(); i <= range.getMax(); ++i)
                    {
                        const Geom::TriangleMesh::Vertex* vertices[] =
                        {
                            &mesh.triangles[i].v0,
                            &mesh.triangles[i].v1,
                            &mesh.triangles[i].v2
                        };
                        for (size_t k = 0; k < 3; ++k)
                        {
                            const size_t v = vertices[k]->v;
                            float* pf = reinterpret_cast<float*>(p);
                            pf[0] = v ? mesh.v[v - 1][0] : 0.F;
                            pf[1] = v ? mesh.v[v - 1][1] : 0.F;
                            pf[2] = v ? mesh.v[v - 1][2] : 0.F;
                            p += 3 * sizeof(float);

                            const size_t t = vertices[k]->t;
                            uint16_t* pu16 = reinterpret_cast<uint16_t*>(p);
                            pu16[0] = t ? Math::clamp(static_cast<int>(mesh.t[t - 1][0] * 65535.F), 0, 65535) : 0;
                            pu16[1] = t ? Math::clamp(static_cast<int>(mesh.t[t - 1][1] * 65535.F), 0, 65535) : 0;
                            p += 2 * sizeof(uint16_t);

                            const size_t n = vertices[k]->n;
                            auto packedNormal = reinterpret_cast<PackedNormal*>(p);
                            packedNormal->x = n ? Math::clamp(static_cast<int>(mesh.n[n - 1][0] * 511.F), -512, 511) : 0;
                            packedNormal->y = n ? Math::clamp(static_cast<int>(mesh.n[n - 1][1] * 511.F), -512, 511) : 0;
                            packedNormal->z = n ? Math::clamp(static_cast<int>(mesh.n[n - 1][2] * 511.F), -512, 511) : 0;
                            p += sizeof(PackedNormal);

                            auto packedColor = reinterpret_cast<PackedColor*>(p);
                            packedColor->r = v ? Math::clamp(static_cast<int>(mesh.c[v - 1][0] * 255.F), 0, 255) : 0;
                            packedColor->g = v ? Math::clamp(static_cast<int>(mesh.c[v - 1][1] * 255.F), 0, 255) : 0;
                            packedColor->b = v ? Math::clamp(static_cast<int>(mesh.c[v - 1][2] * 255.F), 0, 255) : 0;
                            packedColor->a = 255;
                            p += sizeof(PackedColor);
                        }
                    }
                    break;
                case VBOType::Pos3_F32_UV_F32_Normal_F32_Color_F32:
                    for (size_t i = range.getMin(); i <= range.getMax(); ++i)
                    {
                        const Geom::TriangleMesh::Vertex* vertices[] =
                        {
                            &mesh.triangles[i].v0,
                            &mesh.triangles[i].v1,
                            &mesh.triangles[i].v2
                        };
                        for (size_t k = 0; k < 3; ++k)
                        {
                            const size_t v = vertices[k]->v;
                            float* pf = reinterpret_cast<float*>(p);
                            pf[0] = v ? mesh.v[v - 1][0] : 0.F;
                            pf[1] = v ? mesh.v[v - 1][1] : 0.F;
                            pf[2] = v ? mesh.v[v - 1][2] : 0.F;
                            p += 3 * sizeof(float);

                            const size_t t = vertices[k]->t;
                            pf = reinterpret_cast<float*>(p);
                            pf[0] = t ? mesh.t[t - 1][0] : 0.F;
                            pf[1] = t ? mesh.t[t - 1][1] : 0.F;
                            p += 2 * sizeof(float);

                            const size_t n = vertices[k]->n;
                            pf = reinterpret_cast<float*>(p);
                            pf[0] = n ? mesh.n[n - 1][0] : 0.F;
                            pf[1] = n ? mesh.n[n - 1][1] : 0.F;
                            pf[2] = n ? mesh.n[n - 1][2] : 0.F;
                            p += 3 * sizeof(float);

                            pf = reinterpret_cast<float*>(p);
                            pf[0] = v ? mesh.c[v - 1][0] : 1.F;
                            pf[1] = v ? mesh.c[v - 1][1] : 1.F;
                            pf[2] = v ? mesh.c[v - 1][2] : 1.F;
                            p += 3 * sizeof(float);
                        }
                    }
                    break;
                    default: break;
                }
                return out;
            }

            void VAO::_init(VBOType type, GLuint vbo)
            {
#if defined(DJV_OPENGL_ES2)
                glGenVertexArraysOES(1, &_vao);
                glBindVertexArrayOES(_vao);
#else // DJV_OPENGL_ES2
                glGenVertexArrays(1, &_vao);
                glBindVertexArray(_vao);
#endif // DJV_OPENGL_ES2
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                const size_t vertexByteCount = getVertexByteCount(type);
                switch (type)
                {
                case VBOType::Pos2_F32_UV_U16:
                    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(vertexByteCount), (GLvoid*)0);
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(1, 2, GL_UNSIGNED_SHORT, GL_TRUE, static_cast<GLsizei>(vertexByteCount), (GLvoid*)8);
                    glEnableVertexAttribArray(1);
                    break;
                case VBOType::Pos3_F32:
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(vertexByteCount), (GLvoid*)0);
                    glEnableVertexAttribArray(0);
                    break;
                case VBOType::Pos3_F32_UV_U16:
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(vertexByteCount), (GLvoid*)0);
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(1, 2, GL_UNSIGNED_SHORT, GL_TRUE, static_cast<GLsizei>(vertexByteCount), (GLvoid*)12);
                    glEnableVertexAttribArray(1);
                    break;
#if defined(DJV_OPENGL_ES2)
#else // DJV_OPENGL_ES2
                case VBOType::Pos3_F32_UV_U16_Normal_U10:
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(vertexByteCount), (GLvoid*)0);
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(1, 2, GL_UNSIGNED_SHORT, GL_TRUE, static_cast<GLsizei>(vertexByteCount), (GLvoid*)12);
                    glEnableVertexAttribArray(1);
                    glVertexAttribPointer(2, 4, GL_INT_2_10_10_10_REV, GL_TRUE, static_cast<GLsizei>(vertexByteCount), (GLvoid*)16);
                    glEnableVertexAttribArray(2);
                    break;
                case VBOType::Pos3_F32_UV_U16_Normal_U10_Color_U8:
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(vertexByteCount), (GLvoid*)0);
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(1, 2, GL_UNSIGNED_SHORT, GL_TRUE, static_cast<GLsizei>(vertexByteCount), (GLvoid*)12);
                    glEnableVertexAttribArray(1);
                    glVertexAttribPointer(2, 4, GL_INT_2_10_10_10_REV, GL_TRUE, static_cast<GLsizei>(vertexByteCount), (GLvoid*)16);
                    glEnableVertexAttribArray(2);
                    glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, static_cast<GLsizei>(vertexByteCount), (GLvoid*)20);
                    glEnableVertexAttribArray(3);
                    break;
#endif // DJV_OPENGL_ES2
                case VBOType::Pos3_F32_UV_F32_Normal_F32_Color_F32:
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(vertexByteCount), (GLvoid*)0);
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(vertexByteCount), (GLvoid*)12);
                    glEnableVertexAttribArray(1);
                    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(vertexByteCount), (GLvoid*)20);
                    glEnableVertexAttribArray(2);
                    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(vertexByteCount), (GLvoid*)32);
                    glEnableVertexAttribArray(3);
                    break;
                case VBOType::Pos3_F32_Color_U8:
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(vertexByteCount), (GLvoid*)0);
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, static_cast<GLsizei>(vertexByteCount), (GLvoid*)12);
                    glEnableVertexAttribArray(1);
                    break;
                default: break;
                }
            }

            VAO::~VAO()
            {
                if (_vao)
                {
#if defined(DJV_OPENGL_ES2)
                    glDeleteVertexArraysOES(1, &_vao);
#else // DJV_OPENGL_ES2
                    glDeleteVertexArrays(1, &_vao);
#endif // DJV_OPENGL_ES2
                    _vao = 0;
                }
            }

            std::shared_ptr<VAO> VAO::create(VBOType type, GLuint vbo)
            {
                auto out = std::shared_ptr<VAO>(new VAO);
                out->_init(type, vbo);
                return out;
            }

            void VAO::bind()
            {
#if defined(DJV_OPENGL_ES2)
                glBindVertexArrayOES(_vao);
#else // DJV_OPENGL_ES2
                glBindVertexArray(_vao);
#endif // DJV_OPENGL_ES2
            }

            void VAO::draw(GLenum mode, size_t offset, size_t size)
            {
                glDrawArrays(mode, static_cast<GLsizei>(offset), static_cast<GLsizei>(size));
            }

            DJV_ENUM_HELPERS_IMPLEMENTATION(VBOType);

        } // namespace OpenGL
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::OpenGL,
        VBOType,
        DJV_TEXT("vbo_type_pos2_f32_uv_u16"),
        DJV_TEXT("vbo_type_pos3_f32"),
        DJV_TEXT("vbo_type_pos3_f32_uv_u16"),
        DJV_TEXT("vbo_type_pos3_f32_uv_u16_normal_u10"),
        DJV_TEXT("vbo_type_pos3_f32_uv_u16_normal_u10_color_u8"),
        DJV_TEXT("vbo_type_pos3_f32_uv_f32_normal_f32_color_f32"),
        DJV_TEXT("vbo_type_pos3_f32_u8"));

} // namespace djv

