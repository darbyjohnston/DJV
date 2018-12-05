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

#include <djvCore/Core.h>

namespace djv
{
    namespace AV
    {
        struct TriangleMesh;

        namespace OpenGL
        {
            //! This enumeration provides the VBO vertex types. The components are in
            //! this order:
            //! - Position
            //! - UV
            //! - Normal
            //! - Color
            enum class VBOType
            {
                F32_U16_U10,
                F32_U16_U10_U8,
                F32_F32_F32_F32,

                Count,
                First = F32_U16_U10
            };

            size_t getVertexSize(VBOType);

            class VBO
            {
                DJV_NON_COPYABLE(VBO);
                void _init(size_t size, size_t vertexCount, VBOType);
                VBO();

            public:
                ~VBO();

                static std::shared_ptr<VBO> create(size_t size, size_t vertexCount, VBOType);

                size_t getSize() const;
                size_t getVertexCount() const;
                VBOType getType() const;
                GLuint getID() const;

                void copy(const std::vector<uint8_t>&);
                void copy(const std::vector<uint8_t>&, size_t offset);

                static std::vector<uint8_t> convert(const TriangleMesh&, VBOType);

            private:
                DJV_PRIVATE();
            };

            class VAO
            {
                DJV_NON_COPYABLE(VAO);
                void _init(VBOType, GLuint vbo);
                VAO();

            public:
                ~VAO();

                static std::shared_ptr<VAO> create(VBOType, GLuint vbo);

                GLuint getID() const;

                void bind();
                void draw(size_t offset, size_t size);

            private:
                DJV_PRIVATE();
            };

        } // namespace OpenGL
    } // namespace AV
} // namespace djv
