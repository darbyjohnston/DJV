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

//! \file djvImagePlay2TestImage.cpp

#include <djvImagePlay2TestImage.h>

#include <djvError.h>
#include <djvFileIoUtil.h>
#include <djvOpenGlImage.h>

djvImagePlay2TestImage::djvImagePlay2TestImage() :
    _id(0)
{}

djvImagePlay2TestImage::~djvImagePlay2TestImage()
{
    del();
}

const djvPixelDataInfo & djvImagePlay2TestImage::info() const
{
    return _info;
}

void djvImagePlay2TestImage::del()
{
    if (_id)
    {
        glDeleteTextures(1, &_id);
        _id = 0;
    }
}

djvImagePlay2TestImagePpm::djvImagePlay2TestImagePpm() :
    _init(false),
    _load(false)
{}

void djvImagePlay2TestImagePpm::load(const QString & in) throw (djvError)
{
    //DJV_DEBUG("djvImagePlay2TestImagePpm::load");
    //DJV_DEBUG_PRINT("in = " << in);

    _io.open(in, djvFileIo::READ);
    _io.readAhead();

    char magic [] = { 0, 0, 0 };
    _io.get(magic, 2);

    //DJV_DEBUG_PRINT("magic = " << magic);

    if (magic[0] != 'P')
    {
        throw djvError("File is not recognized as a PPM file");
    }

    if (magic[1] != '6')
    {
        throw djvError("Only PPM files of type 6 are supported");
    }

    char tmp[djvStringUtil::cStringLength] = "";

    djvVector2i size;
    djvFileIoUtil::word(_io, tmp, djvStringUtil::cStringLength);
    size.x = QString(tmp).toInt();
    djvFileIoUtil::word(_io, tmp, djvStringUtil::cStringLength);
    size.y = QString(tmp).toInt();

    int maxValue = 0;
    djvFileIoUtil::word(_io, tmp, djvStringUtil::cStringLength);
    maxValue = QString(tmp).toInt();

    if (maxValue >= 256)
    {
        throw djvError("Only 8-bit PPM files are supported");
    }

    djvPixelDataInfo info(size, djvPixel::RGB_U8);
    info.mirror.y = true;
    info.endian = djvMemory::MSB;

    //DJV_DEBUG_PRINT("info = " << info.size);

    if (info != _info)
    {
        _init = true;

        _info = info;
    }

    _load = true;
}

void djvImagePlay2TestImagePpm::bind() const throw (djvError)
{
    djvImagePlay2TestImagePpm * that =
        const_cast<djvImagePlay2TestImagePpm *>(this);

    //DJV_DEBUG("djvImagePlay2TestImagePpm::bind");
    //DJV_DEBUG_PRINT("info = " << _info.size);

    const GLenum target = GL_TEXTURE_2D;
    const GLenum format = djvOpenGlUtil::format(_info.pixel, _info.bgr);
    const GLenum type   = djvOpenGlUtil::type(_info.pixel);

    if (_init)
    {
        //DJV_DEBUG_PRINT("init");

        that->del();

        DJV_DEBUG_OPEN_GL(glGenTextures(1, &that->_id));

        if (! _id)
        {
            throw djvError("glGenTextures");
        }

        DJV_DEBUG_OPEN_GL(glEnable(target));
        DJV_DEBUG_OPEN_GL(glBindTexture(target, _id));

        DJV_DEBUG_OPEN_GL(
            glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        DJV_DEBUG_OPEN_GL(
            glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        DJV_DEBUG_OPEN_GL(
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        DJV_DEBUG_OPEN_GL(
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

        DJV_DEBUG_OPEN_GL(
            glTexImage2D(
                target,
                0,
                GL_RGBA,
                _info.size.x,
                _info.size.y,
                0,
                format,
                type,
                0));

        that->_init = false;
    }

    DJV_DEBUG_OPEN_GL(glEnable(target));
    DJV_DEBUG_OPEN_GL(glBindTexture(target, _id));

    if (_load)
    {
        //DJV_DEBUG_PRINT("load");

        djvOpenGlImage::stateUnpack(_info);

        const quint8 * p = _io.mmapP();

        DJV_DEBUG_OPEN_GL(
            glTexSubImage2D(
                target,
                0,
                0,
                0,
                _info.size.x,
                _info.size.y,
                format,
                type,
                p));

        that->_load = false;
    }
}
