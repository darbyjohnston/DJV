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

#include <djvAV/OpenGLMesh.h>

#include <djvAV/TriangleMesh.h>

#include <djvCore/Math.h>

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

            std::vector<uint8_t> VBO::convert(const Geom::TriangleMesh& mesh, VBOType type)
            {
                const size_t size = mesh.triangles.size();
                return convert(mesh, type, SizeTRange(0, size > 0 ? size - 1 : 0));
            }

            std::vector<uint8_t> VBO::convert(const Geom::TriangleMesh& mesh, VBOType type, const SizeTRange & range)
            {
                const size_t vertexByteCount = getVertexByteCount(type);
                std::vector<uint8_t> out((range.max - range.min + 1) * 3 * vertexByteCount);
                uint8_t * p = out.data();
                switch (type)
                {
                case VBOType::Pos3_F32_UV_U16_Normal_U10:
                    for (size_t i = range.min; i <= range.max; ++i)
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
                    for (size_t i = range.min; i <= range.max; ++i)
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
                    for (size_t i = range.min; i <= range.max; ++i)
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

        } // namespace OpenGL
    } // namespace AV
} // namespace djv

