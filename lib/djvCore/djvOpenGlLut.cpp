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

#include <djvOpenGlLut.h>

#include <djvDebug.h>
#include <djvOpenGlImage.h>

#include <QCoreApplication>

using namespace gl;

//------------------------------------------------------------------------------
// djvOpenGlLut
//------------------------------------------------------------------------------

djvOpenGlLut::~djvOpenGlLut()
{
    del();
}

void djvOpenGlLut::init(const djvPixelDataInfo & info) throw (djvError)
{
    if (info == _info)
        return;

    //DJV_DEBUG("djvOpenGlLut::init");
    //DJV_DEBUG_PRINT("info = " << info);

    del();

    _info = info;
    _size = djvMath::toPow2(_info.size.x);
    //DJV_DEBUG_PRINT("size = " << _size);

    DJV_DEBUG_OPEN_GL(glGenTextures(1, &_id));
    if (! _id)
    {
        throw djvError(
            "djvOpenGlLut",
            qApp->translate("djvOpenGlLut", "Cannot create texture"));
    }

    DJV_DEBUG_OPEN_GL(glBindTexture(GL_TEXTURE_1D, _id));
    DJV_DEBUG_OPEN_GL(
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    DJV_DEBUG_OPEN_GL(
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    DJV_DEBUG_OPEN_GL(
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

    GLenum format = GL_RGBA;
    if (djvPixel::F16 == djvPixel::type(_info.pixel))
    {
        format = GL_RGBA16F;
    }
    else if (djvPixel::F32 == djvPixel::type(_info.pixel))
    {
        format = GL_RGBA32F;
    }

    DJV_DEBUG_OPEN_GL(
        glTexImage1D(
            GL_TEXTURE_1D,
            0,
            format,
            _size,
            0,
            djvOpenGlUtil::format(_info.pixel, _info.bgr),
            djvOpenGlUtil::type(_info.pixel),
            0));
}

void djvOpenGlLut::init(const djvPixelData & data) throw (djvError)
{
    init(data.info());
    bind();
    copy(data);
}

const djvPixelDataInfo & djvOpenGlLut::info() const
{
    return _info;
}

GLuint djvOpenGlLut::id() const
{
    return _id;
}

void djvOpenGlLut::bind()
{
    //DJV_DEBUG("djvOpenGlLut::bind");
    DJV_DEBUG_OPEN_GL(glBindTexture(GL_TEXTURE_1D, _id));
}

void djvOpenGlLut::copy(const djvPixelData & in)
{
    //DJV_DEBUG("djvOpenGlLut::copy");
    //DJV_DEBUG_PRINT("in = " << in);
    const djvPixelDataInfo & info = in.info();
    djvOpenGlImage::stateUnpack(in.info());
    DJV_DEBUG_OPEN_GL(
        glTexSubImage1D(
            GL_TEXTURE_1D,
            0,
            0,
            info.size.x,
            djvOpenGlUtil::format(info.pixel, info.bgr),
            djvOpenGlUtil::type(info.pixel),
            in.data()));
}

void djvOpenGlLut::del()
{
    if (_id)
    {
        glDeleteTextures(1, &_id);
        _id = 0;
    }
}
