//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvAV/OpenGL.h>

namespace djv
{
    namespace AV
    {
        namespace Mesh
        {
            struct TriangleMesh;

        } // namespace Mesh

        namespace OpenGL
        {
            //! This enumeration provides the VBO vertex types.
            enum class VBOType
            {
                Pos3_F32_UV_U16_Normal_U10,
                Pos3_F32_UV_U16_Normal_U10_Color_U8,
                Pos3_F32_UV_F32_Normal_F32_Color_F32,

                Count,
                First = Pos3_F32_UV_U16_Normal_U10
            };

            //! Get the VBO type byte count.
            size_t getVertexByteCount(VBOType);

            //! This class provides an OpenGL vertex buffer object.
            class VBO
            {
                DJV_NON_COPYABLE(VBO);
                void _init(size_t size, size_t vertexCount, VBOType);
                inline VBO();

            public:
                ~VBO();

                static std::shared_ptr<VBO> create(size_t size, size_t vertexCount, VBOType);

                inline size_t getSize() const;
                inline size_t getVertexCount() const;
                inline VBOType getType() const;
                inline gl::GLuint getID() const;

                void copy(const std::vector<uint8_t>&);
                void copy(const std::vector<uint8_t>&, size_t offset);

                static std::vector<uint8_t> convert(const Mesh::TriangleMesh&, VBOType);

            private:
                size_t _size = 0;
                size_t _vertexCount = 0;
                VBOType _type = VBOType::First;
                gl::GLuint _vbo = 0;
            };

            //! This class provides an OpenGL vertex array object.
            class VAO
            {
                DJV_NON_COPYABLE(VAO);
                void _init(VBOType, gl::GLuint vbo);
                inline VAO();

            public:
                ~VAO();

                static std::shared_ptr<VAO> create(VBOType, gl::GLuint vbo);

                inline gl::GLuint getID() const;

                void bind();
                void draw(size_t offset, size_t size);

            private:
                gl::GLuint _vao = 0;
            };

        } // namespace OpenGL
    } // namespace AV
} // namespace djv

#include <djvAV/OpenGLMeshInline.h>

