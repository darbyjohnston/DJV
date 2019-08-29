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

#pragma once

#include <djvAV/ImageData.h>
#include <djvAV/OpenGL.h>

namespace djv
{
    namespace AV
    {
        namespace OpenGL
        {
            //! This class provides an OpenGL texture.
            class Texture
            {
                DJV_NON_COPYABLE(Texture);
                void _init(
                    const Image::Info&,
                    GLenum filterMin = GL_LINEAR,
                    GLenum filterMax = GL_LINEAR);
                inline Texture();

            public:
                ~Texture();

                static std::shared_ptr<Texture> create(
                    const Image::Info&,
                    GLenum filterMin = GL_LINEAR,
                    GLenum filterMax = GL_LINEAR);

                inline const Image::Info& getInfo() const;
                inline GLuint getID() const;

                void set(const Image::Info&);
                void copy(const Image::Data&);
                void copy(const Image::Data&, uint16_t x, uint16_t y);

                void bind();

                static GLenum getInternalFormat(Image::Type);

            private:
                Image::Info _info;
                GLuint _id = 0;
#if defined(DJV_OPENGL_PBO)
                GLuint _pbo = 0;
#endif // DJV_OPENGL_PBO
            };

            //! This class provides a 1D OpenGL texture.
            /*class Texture1D
            {
                DJV_NON_COPYABLE(Texture1D);
                void _init(
                    const Image::Info&,
                    GLenum filter = GL_LINEAR);
                inline Texture1D();

            public:
                ~Texture1D();

                static std::shared_ptr<Texture1D> create(
                    const Image::Info&,
                    GLenum filter = GL_LINEAR);

                inline const Image::Info& getInfo() const;
                inline GLuint getID() const;

                void copy(const Image::Data&);
                void copy(const Image::Data&, uint16_t);

                void bind();

                static GLenum getInternalFormat(Image::Type);

            private:
                Image::Info _info;
                GLuint _id = 0;
#if defined(DJV_OPENGL_PBO)
                GLuint _pbo = 0;
#endif // DJV_OPENGL_PBO
            };*/

        } // namespace OpenGL
    } // namespace AV
} // namespace djv

#include <djvAV/OpenGLTextureInline.h>

