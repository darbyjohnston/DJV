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
            struct Texture::Private
            {
                Image::PixelDataInfo info;
                GLuint id = 0;
                GLuint pbo = 0;
            };

            Texture::Texture() :
                _p(new Private)
            {}

            Texture::~Texture()
            {
                del();
            }

            void Texture::init(const Image::PixelDataInfo& info, GLint filter)
            {
                if (info == _p->info)
                    return;
                del();
                _p->info = info;
                if (_p->info.isValid())
                {
                    auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                    glFuncs->glGenTextures(1, &_p->id);
                    glFuncs->glBindTexture(GL_TEXTURE_2D, _p->id);
                    glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
                    glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
                    GLenum internalFormat = GL_RGBA;
                    const glm::ivec2 & size = _p->info.getSize();
                    glFuncs->glTexImage2D(
                        GL_TEXTURE_2D,
                        0,
                        internalFormat,
                        size.x,
                        size.y,
                        0,
                        _p->info.getFormat(),
                        _p->info.getType(),
                        0);
                    glFuncs->glGenBuffers(1, &_p->pbo);
                }
            }

            void Texture::del()
            {
                _p->info = Image::PixelDataInfo();
                auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                if (_p->id)
                {
                    glFuncs->glDeleteTextures(1, &_p->id);
                    _p->id = 0;
                }
                if (_p->pbo)
                {
                    glFuncs->glDeleteBuffers(1, &_p->pbo);
                    _p->pbo = 0;
                }
            }

            void Texture::copy(const std::shared_ptr<Image::PixelData>& data)
            {
                if (_p->info.isValid())
                {
                    const auto& info = data->getInfo();

                    auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                    glFuncs->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _p->pbo);
                    glFuncs->glBufferData(GL_PIXEL_UNPACK_BUFFER, info.getDataByteCount(), data->getData(), GL_STREAM_DRAW);

                    glFuncs->glBindTexture(GL_TEXTURE_2D, _p->id);
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
                        info.getFormat(),
                        info.getType(),
                        0);

                    glFuncs->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
                }
            }

            void Texture::copy(const std::shared_ptr<Image::PixelData>& data, const glm::ivec2& pos)
            {
                if (_p->info.isValid())
                {
                    const auto& info = data->getInfo();

                    auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                    glFuncs->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _p->pbo);
                    glFuncs->glBufferData(GL_PIXEL_UNPACK_BUFFER, info.getDataByteCount(), data->getData(), GL_STREAM_DRAW);

                    glFuncs->glBindTexture(GL_TEXTURE_2D, _p->id);
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
                        info.getFormat(),
                        info.getType(),
                        0);

                    glFuncs->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
                }
            }

            void Texture::bind()
            {
                glBindTexture(GL_TEXTURE_2D, _p->id);
            }

            const Image::PixelDataInfo & Texture::getInfo() const
            {
                return _p->info;
            }

            GLuint Texture::getId() const
            {
                return _p->id;
            }

            GLenum Texture::getInternalFormat(Image::Pixel pixel)
            {
                switch (Image::getFormat(pixel))
                {
                case GL_RED:
                    switch (Image::getType(pixel))
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
                    switch (Image::getType(pixel))
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
                    switch (Image::getType(pixel))
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
                    switch (Image::getType(pixel))
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
