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

#include <djvGraphics/OpenGL.h>

#include <djvGraphics/ColorUtil.h>

#include <djvCore/Math.h>

namespace djv
{
    namespace Graphics
    {
        namespace OpenGL
        {
            GLenum internalFormat(Pixel::PIXEL in)
            {
                switch (in)
                {
                case Pixel::L_U8:     return GL_R8;
                case Pixel::L_U16:    return GL_R16;
                case Pixel::L_F16:    return GL_R16F;
                case Pixel::L_F32:    return GL_R32F;
                case Pixel::LA_U8:    return GL_RG8;
                case Pixel::LA_U16:   return GL_RG16;
                case Pixel::LA_F16:   return GL_RG16F;
                case Pixel::LA_F32:   return GL_RG32F;
                case Pixel::RGB_U8:   return GL_RGB8;
                case Pixel::RGB_U16:  return GL_RGB16;
                case Pixel::RGB_F16:  return GL_RGB16F;
                case Pixel::RGB_F32:  return GL_RGB32F;
                case Pixel::RGB_U10:  return GL_RGB10;
                case Pixel::RGBA_U8:  return GL_RGBA8;
                case Pixel::RGBA_U16: return GL_RGBA16;
                case Pixel::RGBA_F16: return GL_RGBA16F;
                case Pixel::RGBA_F32: return GL_RGBA32F;
                default: break;
                }
                return GL_NONE;
            }

            GLenum format(Pixel::PIXEL in, bool bgr)
            {
                switch (in)
                {
                case Pixel::L_U8:
                case Pixel::L_U16:
                case Pixel::L_F16:
                case Pixel::L_F32:    return GL_RED;
                case Pixel::LA_U8:
                case Pixel::LA_U16:
                case Pixel::LA_F16:
                case Pixel::LA_F32:   return GL_RG;
                case Pixel::RGB_U8:
                case Pixel::RGB_U16:
                case Pixel::RGB_F16:
                case Pixel::RGB_F32:  return bgr ? GL_BGR : GL_RGB;
                case Pixel::RGB_U10:  return bgr ? GL_BGRA : GL_RGBA;
                case Pixel::RGBA_U8:
                case Pixel::RGBA_U16:
                case Pixel::RGBA_F16:
                case Pixel::RGBA_F32: return bgr ? GL_BGRA : GL_RGBA;
                default: break;
                }
                return GL_NONE;
            }

            GLenum type(Pixel::PIXEL in)
            {
                switch (in)
                {
                case Pixel::L_U8:
                case Pixel::LA_U8:
                case Pixel::RGB_U8:
                case Pixel::RGBA_U8:  return GL_UNSIGNED_BYTE;
                case Pixel::RGB_U10:  return GL_UNSIGNED_INT_10_10_10_2;
                case Pixel::L_U16:
                case Pixel::LA_U16:
                case Pixel::RGB_U16:
                case Pixel::RGBA_U16: return GL_UNSIGNED_SHORT;
                case Pixel::L_F16:
                case Pixel::LA_F16:
                case Pixel::RGB_F16:
                case Pixel::RGBA_F16: return GL_HALF_FLOAT;
                case Pixel::L_F32:
                case Pixel::LA_F32:
                case Pixel::RGB_F32:
                case Pixel::RGBA_F32: return GL_FLOAT;
                default: break;
                }
                return GL_NONE;
            }

        } // namespace OpenGL
    } // namespace Graphics
} // namespace djv
