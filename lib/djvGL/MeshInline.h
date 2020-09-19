// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace GL
    {
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

    } // namespace GL
} // namespace djv

