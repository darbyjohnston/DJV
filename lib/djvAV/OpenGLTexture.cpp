// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/OpenGLTexture.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace OpenGL
        {
            void Texture::_init(const Image::Info & info, GLenum filterMin, GLenum filterMag)
            {
                _info = info;
                _filterMin = filterMin;
                _filterMag = filterMag;
                if (_info.isValid())
                {
#if defined(DJV_OPENGL_PBO)
                    glGenBuffers(1, &_pbo);
                    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pbo);
                    glBufferData(
                        GL_PIXEL_UNPACK_BUFFER,
                        info.getDataByteCount(),
                        0,
                        GL_STREAM_DRAW);
#endif // DJV_OPENGL_PBO

                    glGenTextures(1, &_id);
                    glBindTexture(GL_TEXTURE_2D, _id);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _filterMin);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _filterMag);
                    glTexImage2D(
                        GL_TEXTURE_2D,
                        0,
                        getInternalFormat(_info.type),
                        _info.size.w,
                        _info.size.h,
                        0,
                        _info.getGLFormat(),
                        _info.getGLType(),
                        0);
                }
            }

            Texture::~Texture()
            {
                if (_id)
                {
                    glDeleteTextures(1, &_id);
                    _id = 0;
                }
#if defined(DJV_OPENGL_PBO)
                if (_pbo)
                {
                    glDeleteBuffers(1, &_pbo);
                    _pbo = 0;
                }
#endif // DJV_OPENGL_PBO
            }

            std::shared_ptr<Texture> Texture::create(const Image::Info & info, GLenum filterMin, GLenum filterMag)
            {
                auto out = std::shared_ptr<Texture>(new Texture);
                out->_init(info, filterMin, filterMag);
                return out;
            }

            void Texture::set(const Image::Info& info)
            {
                if (info == _info)
                    return;
                _info = info;
                if (_info.isValid())
                {
                    if (_id)
                    {
                        glDeleteTextures(1, &_id);
                    }
#if defined(DJV_OPENGL_PBO)
                    if (_pbo)
                    {
                        glDeleteBuffers(1, &_pbo);
                    }
                    glGenBuffers(1, &_pbo);
                    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pbo);
                    glBufferData(
                        GL_PIXEL_UNPACK_BUFFER,
                        _info.getDataByteCount(),
                        0,
                        GL_STREAM_DRAW);
#endif // DJV_OPENGL_PBO

                    glGenTextures(1, &_id);
                    glBindTexture(GL_TEXTURE_2D, _id);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _filterMin);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _filterMag);
                    glTexImage2D(
                        GL_TEXTURE_2D,
                        0,
                        getInternalFormat(_info.type),
                        _info.size.w,
                        _info.size.h,
                        0,
                        _info.getGLFormat(),
                        _info.getGLType(),
                        0);
                }
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
                    info.size.w,
                    info.size.h,
                    info.getGLFormat(),
                    info.getGLType(),
                    data.getData());
#else // DJV_OPENGL_ES2

#if defined(DJV_OPENGL_PBO)
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pbo);
                glBufferSubData(
                    GL_PIXEL_UNPACK_BUFFER,
                    0,
                    info.getDataByteCount(),
                    data.getData());
#endif // DJV_OPENGL_PBO

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
#if defined(DJV_OPENGL_PBO)
                    0
#else // DJV_OPENGL_PBO
                    data.getData()
#endif // DJV_OPENGL_PBO
                    );
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
                    info.size.w,
                    info.size.h,
                    info.getGLFormat(),
                    info.getGLType(),
                    data.getData());
#else // DJV_OPENGL_ES2

#if defined(DJV_OPENGL_PBO)
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pbo);
                glBufferSubData(
                    GL_PIXEL_UNPACK_BUFFER,
                    0,
                    info.getDataByteCount(),
                    data.getData());
#endif // DJV_OPENGL_PBO

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
#if defined(DJV_OPENGL_PBO)
                    0
#else // DJV_OPENGL_PBO
                    data.getData()
#endif // DJV_OPENGL_PBO
                    );
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
#endif // DJV_OPENGL_ES2
            }

            void Texture::bind()
            {
                glBindTexture(GL_TEXTURE_2D, _id);
            }

            GLenum Texture::getInternalFormat(Image::Type type)
            {
                const GLenum data[] =
                {
                    GL_NONE,
#if defined(DJV_OPENGL_ES2)
                    GL_LUMINANCE,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,

                    GL_LUMINANCE_ALPHA,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,

                    GL_RGB,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,

                    GL_RGBA,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE
#else // DJV_OPENGL_ES2
                    GL_R8,
                    GL_R16,
                    GL_R32I,
                    GL_R16F,
                    GL_R32F,

                    GL_RG8,
                    GL_RG16,
                    GL_RG32I,
                    GL_RG16F,
                    GL_RG32F,

                    GL_RGB8,
                    GL_RGB10,
                    GL_RGB16,
                    GL_RGB32I,
                    GL_RGB16F,
                    GL_RGB32F,

                    GL_RGBA8,
                    GL_RGBA16,
                    GL_RGBA32I,
                    GL_RGBA16F,
                    GL_RGBA32F
#endif // DJV_OPENGL_ES2
                };
                DJV_ASSERT(sizeof(data) / sizeof(data[0]) == static_cast<size_t>(Image::Type::Count));
                return data[static_cast<size_t>(type)];
            }

            /*void Texture1D::_init(const Image::Info& info, GLenum filter)
            {
                _info = info;
                _filter = filter;
                if (info.isValid())
                {
#if defined(DJV_OPENGL_PBO)
                    glGenBuffers(1, &_pbo);
                    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pbo);
                    glBufferData(
                        GL_PIXEL_UNPACK_BUFFER,
                        info.getDataByteCount(),
                        0,
                        GL_STREAM_DRAW);
#endif // DJV_OPENGL_PBO

                    glGenTextures(1, &_id);
                    glBindTexture(GL_TEXTURE_1D, _id);
                    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, _filter);
                    glTexImage1D(
                        GL_TEXTURE_1D,
                        0,
                        getInternalFormat(_info.type),
                        _info.size.w,
                        0,
                        _info.getGLFormat(),
                        _info.getGLType(),
                        0);
                }
            }

            Texture1D::~Texture1D()
            {
                if (_id)
                {
                    glDeleteTextures(1, &_id);
                    _id = 0;
                }
#if defined(DJV_OPENGL_PBO)
                if (_pbo)
                {
                    glDeleteBuffers(1, &_pbo);
                    _pbo = 0;
                }
#endif
            }

            std::shared_ptr<Texture1D> Texture1D::create(const Image::Info& info, GLenum filter)
            {
                auto out = std::shared_ptr<Texture1D>(new Texture1D);
                out->_init(info, filter);
                return out;
            }

            void Texture1D::copy(const Image::Data& data)
            {
                const auto& info = data.getInfo();

#if defined(DJV_OPENGL_ES2)
                glBindTexture(GL_TEXTURE_1D, _id);
                glPixelStorei(GL_UNPACK_ALIGNMENT, info.layout.alignment);
                glTexSubImage1D(
                    GL_TEXTURE_1D,
                    0,
                    0,
                    info.size.x,
                    info.getGLFormat(),
                    info.getGLType(),
                    data.getData());
#else // DJV_OPENGL_ES2

#if defined(DJV_OPENGL_PBO)
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pbo);
                glBufferSubData(
                    GL_PIXEL_UNPACK_BUFFER,
                    0,
                    info.getDataByteCount(),
                    data.getData());
#endif

                glBindTexture(GL_TEXTURE_1D, _id);
                glPixelStorei(GL_UNPACK_ALIGNMENT, info.layout.alignment);
                glPixelStorei(GL_UNPACK_SWAP_BYTES, info.layout.endian != Memory::getEndian());
                glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
                glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
                glTexSubImage1D(
                    GL_TEXTURE_1D,
                    0,
                    0,
                    info.size.w,
                    info.getGLFormat(),
                    info.getGLType(),
#if defined(DJV_OPENGL_PBO)
                    0
#else // DJV_OPENGL_PBO
                    data.getData()
#endif
                    );
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
#endif // DJV_OPENGL_ES2
            }

            void Texture1D::copy(const Image::Data& data, uint16_t x)
            {
                const auto& info = data.getInfo();

#if defined(DJV_OPENGL_ES2)
                glBindTexture(GL_TEXTURE_1D, _id);
                glPixelStorei(GL_UNPACK_ALIGNMENT, info.layout.alignment);
                glTexSubImage1D(
                    GL_TEXTURE_1D,
                    0,
                    x,
                    info.w,
                    info.getGLFormat(),
                    info.getGLType(),
                    data.getData());
#else // DJV_OPENGL_ES2

#if defined(DJV_OPENGL_PBO)
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pbo);
                glBufferSubData(
                    GL_PIXEL_UNPACK_BUFFER,
                    0,
                    info.getDataByteCount(),
                    data.getData());
#endif // DJV_OPENGL_ES2
                
                glBindTexture(GL_TEXTURE_1D, _id);
                glPixelStorei(GL_UNPACK_ALIGNMENT, info.layout.alignment);
                glPixelStorei(GL_UNPACK_SWAP_BYTES, info.layout.endian != Memory::getEndian());
                glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
                glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
                glTexSubImage1D(
                    GL_TEXTURE_1D,
                    0,
                    x,
                    info.size.w,
                    info.getGLFormat(),
                    info.getGLType(),
#if defined(DJV_OPENGL_PBO)
                    0
#else // DJV_OPENGL_PBO
                    data.getData()
#endif // DJV_OPENGL_PBO
                    );
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
#endif // DJV_OPENGL_ES2
            }

            void Texture1D::bind()
            {
                glBindTexture(GL_TEXTURE_1D, _id);
            }

            GLenum Texture1D::getInternalFormat(Image::Type type)
            {
                //return Image::getGLFormat(type);
                return GL_RGBA;
            }*/

        } // namespace OpenGL
    } // namespace AV
} // namespace djv
