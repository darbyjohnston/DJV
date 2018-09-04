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

#include <djvOpenGlTexture.h>

#include <djvDebug.h>
#include <djvOpenGlImage.h>

#include <QCoreApplication>

using namespace gl;

//------------------------------------------------------------------------------
// djvOpenGlTexture
//------------------------------------------------------------------------------

djvOpenGlTexture::djvOpenGlTexture() :
    _target(GL_NONE),
    _min   (GL_NONE),
    _mag   (GL_NONE),
    _id    (0)
{}

djvOpenGlTexture::~djvOpenGlTexture()
{
    del();
}

void djvOpenGlTexture::init(
    const djvPixelDataInfo & info,
    GLenum                   target,
    GLenum                   min,
    GLenum                   mag) throw (djvError)
{
    if (info == _info && target == _target && min == _min && mag == _mag)
        return;

    del();

    //DJV_DEBUG("djvOpenGlTexture::init");
    //DJV_DEBUG_PRINT("info = " << info);

    _info   = info;
    _target = target;
    _min    = min;
    _mag    = mag;
    DJV_DEBUG_OPEN_GL(glGenTextures(1, &_id));
    //DJV_DEBUG_PRINT("id = " << int(_id));
    if (! _id)
    {
        throw djvError(
            "djvOpenGlTexture",
            qApp->translate("djvOpenGlTexture", "Cannot create texture"));
    }

    DJV_DEBUG_OPEN_GL(glBindTexture(_target, _id));
    DJV_DEBUG_OPEN_GL(glTexParameteri(_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    DJV_DEBUG_OPEN_GL(glTexParameteri(_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    DJV_DEBUG_OPEN_GL(glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, _min));
    DJV_DEBUG_OPEN_GL(glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, _mag));

    GLenum format = GL_RGBA;
    if (djvPixel::F16 == djvPixel::type(_info.pixel))
    {
        format = GL_RGBA16F;
    }
    else if (djvPixel::F32 == djvPixel::type(_info.pixel))
    {
        format = GL_RGBA32F;
    }

    glTexImage2D(
        _target,
        0,
        format,
        _info.size.x,
        _info.size.y,
        0,
        djvOpenGlUtil::format(_info.pixel, _info.bgr),
        djvOpenGlUtil::type(_info.pixel),
        0);
    GLenum error = glGetError();
#if ! defined(DJV_OSX)
    //! \todo On OS X this error is triggered the first time djv_view is
    //! started, though it doesn't actually seem to be a problem? If we
    //! throw here the image is not displayed (start djv_view from the
    //! command line with an image), but if we igore the error the image is
    //! displayed OK? Is this related to the "invalid drawable" message we
    //! are also getting on start up?
    if (error != GL_NO_ERROR)
    {
        throw djvError(
            "djvOpenGlTexture",
            qApp->translate("djvOpenGlTexture", "Cannot create texture: %1").
            arg(djvOpenGlUtil::errorString(error)));
    }
#endif // DJV_OSX
}

void djvOpenGlTexture::init(
    const djvPixelData & data,
    GLenum               target,
    GLenum               min,
    GLenum               mag) throw (djvError)
{
    init(data.info(), target, min, mag);
    copy(data);
}

void djvOpenGlTexture::copy(const djvPixelData & in)
{
    //DJV_DEBUG("djvOpenGlTexture::copy");
    //DJV_DEBUG_PRINT("in = " << in);
    bind();
    const djvPixelDataInfo & info = in.info();
    djvOpenGlImage::stateUnpack(in.info());
    DJV_DEBUG_OPEN_GL(
        glTexSubImage2D(
            _target,
            0,
            0,
            0,
            info.size.x,
            info.size.y,
            djvOpenGlUtil::format(info.pixel, info.bgr),
            djvOpenGlUtil::type(info.pixel),
            in.data()));
}

void djvOpenGlTexture::copy(const djvPixelData & in, const djvBox2i & area)
{
    //DJV_DEBUG("djvOpenGlTexture::copy");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("area = " << area);
    bind();
    const djvPixelDataInfo & info = in.info();
    glm::ivec2 position = area.position;
    if (info.mirror.x)
    {
        position.x = info.size.x - area.position.x - area.size.x;
    }
    if (info.mirror.y)
    {
        position.y = info.size.y - area.position.y - area.size.y;
    }
    djvOpenGlImage::stateUnpack(in.info(), position);
    DJV_DEBUG_OPEN_GL(glTexSubImage2D(
        _target,
        0,
        0,
        0,
        area.size.x,
        area.size.y,
        djvOpenGlUtil::format(info.pixel, info.bgr),
        djvOpenGlUtil::type(info.pixel),
        in.data()));
}

void djvOpenGlTexture::copy(const glm::ivec2 & in)
{
    //DJV_DEBUG("djvOpenGlTexture::copy");
    //DJV_DEBUG_PRINT("in = " << in);
    DJV_DEBUG_OPEN_GL(glBindTexture(_target, _id));
    DJV_DEBUG_OPEN_GL(
        glCopyTexSubImage2D(
            _target,
            0,
            0,
            0,
            0,
            0,
            in.x,
            in.y));
}

void djvOpenGlTexture::bind()
{
    //DJV_DEBUG("djvOpenGlTexture::bind");
    DJV_DEBUG_OPEN_GL(glBindTexture(_target, _id));
}

const djvPixelDataInfo & djvOpenGlTexture::info() const
{
    return _info;
}

GLenum djvOpenGlTexture::target() const
{
    return _target;
}

GLenum djvOpenGlTexture::min() const
{
    return _min;
}

GLenum djvOpenGlTexture::mag() const
{
    return _mag;
}

GLuint djvOpenGlTexture::id() const
{
    return _id;
}

void djvOpenGlTexture::del()
{
    if (_id)
    {
        glDeleteTextures(1, &_id);
        _id = 0;
    }
}

