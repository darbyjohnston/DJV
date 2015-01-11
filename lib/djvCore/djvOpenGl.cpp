//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

//! \file djvOpenGl.cpp

#include <djvOpenGl.h>

#include <djvColorUtil.h>
#include <djvMath.h>

//------------------------------------------------------------------------------
// djvOpenGlUtil
//------------------------------------------------------------------------------

djvOpenGlUtil::~djvOpenGlUtil()
{}

void djvOpenGlUtil::ortho(const djvVector2i & size, const djvVector2f & minMax)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, size.x, 0, size.y, minMax.x, minMax.y);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

GLenum djvOpenGlUtil::format(djvPixel::PIXEL in, bool bgr)
{
    switch (in)
    {
        case djvPixel::L_U8:
        case djvPixel::L_U16:
        case djvPixel::L_F16:
        case djvPixel::L_F32:    return GL_LUMINANCE;

        case djvPixel::LA_U8:
        case djvPixel::LA_U16:
        case djvPixel::LA_F16:
        case djvPixel::LA_F32:   return GL_LUMINANCE_ALPHA;

        case djvPixel::RGB_U8:
        case djvPixel::RGB_U16:
        case djvPixel::RGB_F16:
        case djvPixel::RGB_F32:  return bgr ? GL_BGR : GL_RGB;

        case djvPixel::RGB_U10:  return bgr ? GL_BGRA : GL_RGBA;

        case djvPixel::RGBA_U8:
        case djvPixel::RGBA_U16:
        case djvPixel::RGBA_F16:
        case djvPixel::RGBA_F32: return bgr ? GL_BGRA : GL_RGBA;

        default: break;
    }

    return 0;
}

GLenum djvOpenGlUtil::type(djvPixel::PIXEL in)
{
    switch (in)
    {
        case djvPixel::L_U8:
        case djvPixel::LA_U8:
        case djvPixel::RGB_U8:
        case djvPixel::RGBA_U8:  return GL_UNSIGNED_BYTE;

        case djvPixel::RGB_U10:  return GL_UNSIGNED_INT_10_10_10_2;

        case djvPixel::L_U16:
        case djvPixel::LA_U16:
        case djvPixel::RGB_U16:
        case djvPixel::RGBA_U16: return GL_UNSIGNED_SHORT;

        case djvPixel::L_F16:
        case djvPixel::LA_F16:
        case djvPixel::RGB_F16:
        case djvPixel::RGBA_F16: return GLEW_ARB_half_float_pixel ? GL_HALF_FLOAT_ARB : 0;

        case djvPixel::L_F32:
        case djvPixel::LA_F32:
        case djvPixel::RGB_F32:
        case djvPixel::RGBA_F32: return GL_FLOAT;

        default: break;
    }

    return 0;
}

void djvOpenGlUtil::color(const djvColor & in)
{
    djvColor tmp(djvPixel::RGBA_F32);

    djvColorUtil::convert(in, tmp);

    glColor4f(
        tmp.f32(0),
        tmp.f32(1),
        tmp.f32(2),
        tmp.f32(3));
}

void djvOpenGlUtil::drawBox(const djvBox2i & in)
{
    glVertex2i(in.x, in.y);
    glVertex2i(in.x, in.y + in.h);
    glVertex2i(in.x + in.w, in.y + in.h);
    glVertex2i(in.x + in.w, in.y);
}

void djvOpenGlUtil::drawBox(const djvBox2f & in)
{
    glVertex2d(in.x, in.y);
    glVertex2d(in.x, in.y + in.h);
    glVertex2d(in.x + in.w, in.y + in.h);
    glVertex2d(in.x + in.w, in.y);
}

void djvOpenGlUtil::drawBox(const djvBox2i & in, const djvVector2f uv[4])
{
    glTexCoord2d(uv[0].x, uv[0].y);
    glVertex2i(in.x, in.y);
    glTexCoord2d(uv[1].x, uv[1].y);
    glVertex2i(in.x, in.y + in.h);
    glTexCoord2d(uv[2].x, uv[2].y);
    glVertex2i(in.x + in.w, in.y + in.h);
    glTexCoord2d(uv[3].x, uv[3].y);
    glVertex2i(in.x + in.w, in.y);
}

void djvOpenGlUtil::drawBox(const djvBox2f & in, const djvVector2f uv[4])
{
    glTexCoord2d(uv[0].x, uv[0].y);
    glVertex2d(in.x, in.y);
    glTexCoord2d(uv[1].x, uv[1].y);
    glVertex2d(in.x, in.y + in.h);
    glTexCoord2d(uv[2].x, uv[2].y);
    glVertex2d(in.x + in.w, in.y + in.h);
    glTexCoord2d(uv[3].x, uv[3].y);
    glVertex2d(in.x + in.w, in.y);
}
