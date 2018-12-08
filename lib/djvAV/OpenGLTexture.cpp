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

#include <djvAV/OpenGLTexture.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace OpenGL
        {
            void Texture::_init(const Pixel::Info& info, GLint filter)
            {
                _info = info;
                auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                glFuncs->glGenTextures(1, &_id);
                glFuncs->glBindTexture(GL_TEXTURE_2D, _id);
                glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
                glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
                GLenum internalFormat = GL_RGBA;
                const glm::ivec2 & size = _info.getSize();
                glFuncs->glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    internalFormat,
                    size.x,
                    size.y,
                    0,
                    _info.getGLFormat(),
                    _info.getGLType(),
                    0);
                glFuncs->glGenBuffers(1, &_pbo);
            }

            Texture::~Texture()
            {
                auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                if (_id)
                {
                    glFuncs->glDeleteTextures(1, &_id);
                    _id = 0;
                }
                if (_pbo)
                {
                    glFuncs->glDeleteBuffers(1, &_pbo);
                    _pbo = 0;
                }
            }

            std::shared_ptr<Texture> Texture::create(const Pixel::Info& info, GLint filter)
            {
                auto out = std::shared_ptr<Texture>(new Texture);
                out->_init(info, filter);
                return out;
            }

            void Texture::copy(const Pixel::Data& data)
            {
                const auto& info = data.getInfo();

                auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                glFuncs->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pbo);
                glFuncs->glBufferData(GL_PIXEL_UNPACK_BUFFER, info.getDataByteCount(), data.getData(), GL_STREAM_DRAW);

                glFuncs->glBindTexture(GL_TEXTURE_2D, _id);
                glFuncs->glPixelStorei(GL_UNPACK_ALIGNMENT, info.getLayout().getAlignment());
                glFuncs->glPixelStorei(GL_UNPACK_SWAP_BYTES, info.getLayout().getEndian() != Memory::getEndian());
#if !defined(DJV_PLATFORM_IOS)
                glFuncs->glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
                glFuncs->glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
#endif // DJV_PLATFORM_IOS
                glFuncs->glTexSubImage2D(
                    GL_TEXTURE_2D,
                    0,
                    0,
                    0,
                    info.getWidth(),
                    info.getHeight(),
                    info.getGLFormat(),
                    info.getGLType(),
                    0);

                glFuncs->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
            }

            void Texture::copy(const Pixel::Data& data, const glm::ivec2& pos)
            {
                const auto& info = data.getInfo();

                auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                glFuncs->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pbo);
                glFuncs->glBufferData(GL_PIXEL_UNPACK_BUFFER, info.getDataByteCount(), data.getData(), GL_STREAM_DRAW);

                glFuncs->glBindTexture(GL_TEXTURE_2D, _id);
                glFuncs->glPixelStorei(GL_UNPACK_ALIGNMENT, info.getLayout().getAlignment());
                glFuncs->glPixelStorei(GL_UNPACK_SWAP_BYTES, info.getLayout().getEndian() != Memory::getEndian());
#if !defined(DJV_PLATFORM_IOS)
                glFuncs->glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
                glFuncs->glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
#endif // DJV_PLATFORM_IOS
                glFuncs->glTexSubImage2D(
                    GL_TEXTURE_2D,
                    0,
                    pos.x,
                    pos.y,
                    info.getWidth(),
                    info.getHeight(),
                    info.getGLFormat(),
                    info.getGLType(),
                    0);

                glFuncs->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
            }

            void Texture::bind()
            {
                glBindTexture(GL_TEXTURE_2D, _id);
            }

            GLenum Texture::getInternalFormat(Pixel::Type type)
            {
                switch (Pixel::getGLFormat(type))
                {
                case GL_RED:
                    switch (Pixel::getGLType(type))
                    {
                    case GL_UNSIGNED_BYTE: return GL_R8;
                    case GL_UNSIGNED_SHORT: return GL_R16;
#if !defined(DJV_PLATFORM_IOS)
                    case GL_HALF_FLOAT: return  GL_R16F;
#endif // DJV_PLATFORM_IOS
                    case GL_FLOAT: return GL_R32F;
                    }
                    break;
                case GL_RG:
                    switch (Pixel::getGLType(type))
                    {
                    case GL_UNSIGNED_BYTE: return GL_RG8;
                    case GL_UNSIGNED_SHORT: return GL_RG16;
#if !defined(DJV_PLATFORM_IOS)
                    case GL_HALF_FLOAT: return  GL_RG16F;
#endif // DJV_PLATFORM_IOS
                    case GL_FLOAT: return GL_RG32F;
                    }
                    break;
                case GL_RGB:
                    switch (Pixel::getGLType(type))
                    {
                    case GL_UNSIGNED_BYTE: return GL_RGB8;
                    case GL_UNSIGNED_INT_10_10_10_2: return GL_RGB10;
                    case GL_UNSIGNED_SHORT: return GL_RGB16;
#if !defined(DJV_PLATFORM_IOS)
                    case GL_HALF_FLOAT: return  GL_RGB16F;
#endif // DJV_PLATFORM_IOS
                    case GL_FLOAT: return GL_RGB32F;
                    }
                    break;
                case GL_RGBA:
                    switch (Pixel::getGLType(type))
                    {
                    case GL_UNSIGNED_BYTE: return GL_RGBA8;
                    case GL_UNSIGNED_SHORT: return GL_RGBA16;
#if !defined(DJV_PLATFORM_IOS)
                    case GL_HALF_FLOAT: return  GL_RGBA16F;
#endif // DJV_PLATFORM_IOS
                    case GL_FLOAT: return GL_RGBA32F;
                    }
                    break;
                }
                return 0;
            }

        } // namespace OpenGL
    } // namespace AV
} // namespace djv
