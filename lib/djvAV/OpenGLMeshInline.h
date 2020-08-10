// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace AV
    {
        namespace OpenGL
        {
            inline size_t getVertexByteCount(VBOType value) noexcept
            {
                const size_t data[] =
                {
                    12, // 2 * sizeof(float) + 2 * sizeof(uint16_t)
                    12, // 3 * sizeof(float)
                    16, // 3 * sizeof(float) + 2 * sizeof(uint16_t)
                    20, // 3 * sizeof(float) + 2 * sizeof(uint16_t) + sizeof(PackedNormal)
                    24, // 3 * sizeof(float) + 2 * sizeof(uint16_t) + sizeof(PackedNormal) + sizeof(PackedColor)
                    44, // 3 * sizeof(float) + 2 * sizeof(float) + 3 * sizeof(float) + 3 * sizeof(float)
                    16  // 3 * sizeof(float) + sizeof(PackedColor)
                };
                return data[static_cast<size_t>(value)];
            }

            inline VBO::VBO()
            {}

            inline size_t VBO::getSize() const
            {
                return _size;
            }

            inline VBOType VBO::getType() const
            {
                return _type;
            }

            inline GLuint VBO::getID() const
            {
                return _vbo;
            }

            inline VAO::VAO()
            {}

            inline GLuint VAO::getID() const
            {
                return _vao;
            }

        } // namespace OpenGL
    } // namespace AV
} // namespace djv

