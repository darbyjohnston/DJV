//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvGraphics/OpenGL.h>
#include <djvGraphics/PixelData.h>

#include <djvCore/Error.h>

namespace djv
{
    namespace Graphics
    {
        //! This class proivides an OpenGL texture.
        class OpenGLTexture
        {
        public:
            ~OpenGLTexture();

            //! Initialize the texture.
            //!
            //! Throws:
            //! - Core::Error
            void init(
                const PixelDataInfo &,
                GLenum target = GL_TEXTURE_2D,
                GLenum min = GL_LINEAR,
                GLenum mag = GL_LINEAR);

            //! Initialize the texture.
            //!
            //! Throws:
            //! - Core::Error
            void init(
                const PixelData &,
                GLenum target = GL_TEXTURE_2D,
                GLenum min = GL_LINEAR,
                GLenum mag = GL_LINEAR);

            //! Get the pixel information.
            const PixelDataInfo & info() const;

            //! Get the target.
            GLenum target() const;

            //! Get the minify filter.
            GLenum min() const;

            //! Get the magnify filter.
            GLenum mag() const;

            //! Get the texture ID.
            GLuint id() const;

            //! Bind the texture.
            void bind();

            //! Copy pixel data to the texture.
            void copy(const PixelData &);

            //! Copy pixel data to the texture.
            void copy(const PixelData &, const Core::Box2i &);

            //! Copy the current read buffer to the texture.
            void copy(const glm::ivec2 &);

        private:
            void del();

            PixelDataInfo _info;
            GLenum        _target = GL_NONE;
            GLenum        _min = GL_NONE;
            GLenum        _mag = GL_NONE;
            GLuint        _id = 0;
        };

    } // namespace Graphics
} // namespace djv
