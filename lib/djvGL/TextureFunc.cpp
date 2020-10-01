// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvGL/TextureFunc.h>

#include <array>

using namespace djv::Core;

namespace djv
{
    namespace GL
    {
        GLenum getInternalFormat2D(Image::Type type)
        {
            const std::array<GLenum, static_cast<size_t>(Image::Type::Count)> data =
            {
                GL_NONE,
#if defined(DJV_GL_ES2)
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
#else // DJV_GL_ES2
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
#endif // DJV_GL_ES2
            };
            return data[static_cast<size_t>(type)];
        }

        /*GLenum getInternalFormat1D(Image::Type type)
        {
            //return Image::getGLFormat(type);
            return GL_RGBA;
        }*/

    } // namespace GL
} // namespace djv
