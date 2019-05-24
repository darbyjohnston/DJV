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

#include <djvAV/OpenGLTexture.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace OpenGL
        {
            void Texture::_init(const Image::Info & info, GLenum filter)
            {
                _info = info;
                glGenTextures(1, &_id);
                glBindTexture(GL_TEXTURE_2D, _id);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
                GLenum internalFormat = GL_RGBA;
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    internalFormat,
                    _info.size.w,
                    _info.size.h,
                    0,
                    _info.getGLFormat(),
                    _info.getGLType(),
                    0);
                glGenBuffers(1, &_pbo);
            }

            Texture::~Texture()
            {
                if (_id)
                {
                    glDeleteTextures(1, &_id);
                    _id = 0;
                }
                if (_pbo)
                {
                    glDeleteBuffers(1, &_pbo);
                    _pbo = 0;
                }
            }

            std::shared_ptr<Texture> Texture::create(const Image::Info & info, GLenum filter)
            {
                auto out = std::shared_ptr<Texture>(new Texture);
                out->_init(info, filter);
                return out;
            }

            void Texture::copy(const Image::Data & data)
            {
                const auto & info = data.getInfo();

#if defined(DJV_OPENGL_ES2)
                glBindTexture(GL_TEXTURE_2D, _id);
                glPixelStorei(GL_UNPACK_ALIGNMENT, info.layout.alignment);
                glTexSubImage2D(
                    GL_TEXTURE_2D,
                    0,
                    0,
                    0,
                    info.size.x,
                    info.size.y,
                    info.getGLFormat(),
                    info.getGLType(),
                    data.getData());
#else // DJV_OPENGL_ES2
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pbo);
                glBufferData(GL_PIXEL_UNPACK_BUFFER, info.getDataByteCount(), data.getData(), GL_DYNAMIC_DRAW);
                glBindTexture(GL_TEXTURE_2D, _id);
                glPixelStorei(GL_UNPACK_ALIGNMENT, info.layout.alignment);
                glPixelStorei(GL_UNPACK_SWAP_BYTES, info.layout.endian != Memory::getEndian());
                glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
                glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
                glTexSubImage2D(
                    GL_TEXTURE_2D,
                    0,
                    0,
                    0,
                    info.size.w,
                    info.size.h,
                    info.getGLFormat(),
                    info.getGLType(),
                    0);
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
#endif // DJV_OPENGL_ES2
            }

            void Texture::copy(const Image::Data & data, uint16_t x, uint16_t y)
            {
                const auto & info = data.getInfo();

#if defined(DJV_OPENGL_ES2)
                glBindTexture(GL_TEXTURE_2D, _id);
                glPixelStorei(GL_UNPACK_ALIGNMENT, info.layout.alignment);
                glTexSubImage2D(
                    GL_TEXTURE_2D,
                    0,
                    x,
                    y,
                    info.w,
                    info.h,
                    info.getGLFormat(),
                    info.getGLType(),
                    data.getData());
#else // DJV_OPENGL_ES2
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pbo);
                glBufferData(GL_PIXEL_UNPACK_BUFFER, info.getDataByteCount(), data.getData(), GL_STREAM_DRAW);
                glBindTexture(GL_TEXTURE_2D, _id);
                glPixelStorei(GL_UNPACK_ALIGNMENT, info.layout.alignment);
                glPixelStorei(GL_UNPACK_SWAP_BYTES, info.layout.endian != Memory::getEndian());
                glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
                glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
                glTexSubImage2D(
                    GL_TEXTURE_2D,
                    0,
                    x,
                    y,
                    info.size.w,
                    info.size.h,
                    info.getGLFormat(),
                    info.getGLType(),
                    0);
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
#endif // DJV_OPENGL_ES2
            }

            void Texture::bind()
            {
                glBindTexture(GL_TEXTURE_2D, _id);
            }

            GLenum Texture::getInternalFormat(Image::Type type)
            {
#if defined(DJV_OPENGL_ES2)
                return Image::getGLFormat(type);
#else // DJV_OPENGL_ES2
                return Image::getGLFormat(type);
                /*switch (Image::getGLFormat(type))
                {
                case GL_RED:
                    switch (Image::getGLType(type))
                    {
                    case GL_UNSIGNED_BYTE: return GL_R8;
                    case GL_UNSIGNED_SHORT: return GL_R16;
                    case GL_HALF_FLOAT: return  GL_R16F;
                    case GL_FLOAT: return GL_R32F;
                    default: break;
                    }
                    break;
                case GL_RG:
                    switch (Image::getGLType(type))
                    {
                    case GL_UNSIGNED_BYTE: return GL_RG8;
                    case GL_UNSIGNED_SHORT: return GL_RG16;
                    case GL_HALF_FLOAT: return  GL_RG16F;
                    case GL_FLOAT: return GL_RG32F;
                    default: break;
                    }
                    break;
                case GL_RGB:
                    switch (Image::getGLType(type))
                    {
                    case GL_UNSIGNED_BYTE: return GL_RGB8;
                    case GL_UNSIGNED_INT_10_10_10_2: return GL_RGB10;
                    case GL_UNSIGNED_SHORT: return GL_RGB16;
                    case GL_HALF_FLOAT: return  GL_RGB16F;
                    case GL_FLOAT: return GL_RGB32F;
                    default: break;
                    }
                    break;
                case GL_RGBA:
                    switch (Image::getGLType(type))
                    {
                    case GL_UNSIGNED_BYTE: return GL_RGBA8;
                    case GL_UNSIGNED_SHORT: return GL_RGBA16;
                    case GL_HALF_FLOAT: return  GL_RGBA16F;
                    case GL_FLOAT: return GL_RGBA32F;
                    default: break;
                    }
                    break;
                default: break;
                }
                return GL_NONE;*/
#endif // DJV_OPENGL_ES2
            }

        } // namespace OpenGL
    } // namespace AV
} // namespace djv
