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

//! \file djvPngSave.cpp

#include <djvPngSave.h>

#include <djvError.h>
#include <djvMemory.h>
#include <djvOpenGlImage.h>

//------------------------------------------------------------------------------
// djvPngSave
//------------------------------------------------------------------------------

djvPngSave::djvPngSave() :
    _f      (0),
    _png    (0),
    _pngInfo(0)
{}

djvPngSave::~djvPngSave()
{
    close();
}

void djvPngSave::open(const djvFileInfo & in, const djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvPngSave::open");
    //DJV_DEBUG_PRINT("in = " << in);

    _file = in;

    if (info.sequence.frames.count() > 1)
    {
        _file.setType(djvFileInfo::SEQUENCE);
    }

    _info = djvPixelDataInfo();
    _info.size = info.size;

    djvPixel::TYPE type = djvPixel::type(info.pixel);

    switch (type)
    {
        case djvPixel::U10:
        case djvPixel::F16:
        case djvPixel::F32: type = djvPixel::U16; break;

        default: break;
    }

    _info.pixel = djvPixel::pixel(djvPixel::format(info.pixel), type);

    //DJV_DEBUG_PRINT("info = " << _info);

    _image.set(_info);
}

namespace
{

bool pngScanline(png_structp png, const quint8 * in)
{
    if (setjmp(png_jmpbuf(png)))
        return false;

    png_write_row(png, (png_byte *)in);

    return true;
}

bool pngEnd(png_structp png, png_infop pngInfo)
{
    if (setjmp(png_jmpbuf(png)))
        return false;

    png_write_end(png, pngInfo);

    return true;
}

} // namespace

void djvPngSave::write(const djvImage & in, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    //DJV_DEBUG("djvPngSave::write");
    //DJV_DEBUG_PRINT("in = " << in);

    // Open the file.

    const QString fileName = _file.fileName(frame.frame);

    //DJV_DEBUG_PRINT("file name = " << fileName);

    djvImageIoInfo info(_info);
    info.tags = in.tags;

    _open(fileName, info);

    // Convert the image.

    const djvPixelData * p = &in;

    if (in.info() != _image.info())
    {
        _image.zero();
        
        djvOpenGlImage::copy(in, _image);
        
        p = &_image;
    }

    // Write the file.

    const int h = p->h();

    for (int y = 0; y < h; ++y)
    {
        if (! pngScanline(_png, p->data(0, h - 1 - y)))
        {
            throw djvError(
                djvPngPlugin::staticName,
                _pngError.msg);
        }
    }

    if (! pngEnd(_png, _pngInfo))
    {
        throw djvError(
            djvPngPlugin::staticName,
            _pngError.msg);
    }

    close();
}

void djvPngSave::close() throw (djvError)
{
    if (_png || _pngInfo)
    {
        png_destroy_write_struct(
            _png ? &_png : 0,
            _pngInfo ? &_pngInfo : 0);
        
        _png     = 0;
        _pngInfo = 0;
    }

    if (_f)
    {
        ::fclose(_f);
        
        _f = 0;
    }
}

namespace
{

bool pngOpen(
    FILE *                 f,
    png_structp            png,
    png_infop *            pngInfo,
    const djvImageIoInfo & info)
{
    if (setjmp(png_jmpbuf(png)))
    {
        return false;
    }

    *pngInfo = png_create_info_struct(png);

    if (! *pngInfo)
    {
        return false;
    }

    png_init_io(png, f);

    int colorType = 0;

    switch (info.pixel)
    {
        case djvPixel::L_U8:
        case djvPixel::L_U16:
            colorType = PNG_COLOR_TYPE_GRAY;
            break;

        case djvPixel::LA_U8:
        case djvPixel::LA_U16:
            colorType = PNG_COLOR_TYPE_GRAY_ALPHA;
            break;

        case djvPixel::RGB_U8:
        case djvPixel::RGB_U16:
            colorType = PNG_COLOR_TYPE_RGB;
            break;

        case djvPixel::RGBA_U8:
        case djvPixel::RGBA_U16:
            colorType = PNG_COLOR_TYPE_RGB_ALPHA;
            break;

        default: break;
    }

    png_set_IHDR(
        png,
        *pngInfo,
        info.size.x,
        info.size.y,
        djvPixel::bitDepth(info.pixel),
        colorType,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png, *pngInfo);

    return true;
}

} // namespace

void djvPngSave::_open(const QString & in, const djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvPngSave::_open");
    //DJV_DEBUG_PRINT("in = " << in);

    close();

    // Initialize libpng.

    _png = png_create_write_struct(
        PNG_LIBPNG_VER_STRING,
        &_pngError,
        djvPngError,
        djvPngWarning);

    if (! _png)
    {
        throw djvError(
            djvPngPlugin::staticName,
            _pngError.msg);
    }

    SNPRINTF(
        _pngError.msg,
        djvStringUtil::cStringLength,
        QString("Error opening: %1").arg(in).toLatin1().data());

    // Open the file.

#if defined(DJV_WINDOWS)

    ::fopen_s(&_f, in.toLatin1().data(), "wb");

#else

    _f = ::fopen(in.toLatin1().data(), "wb");

#endif

    if (! _f)
    {
        throw djvError(
            djvPngPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_OPEN].arg(in));
    }

    if (! pngOpen(_f, _png, &_pngInfo, info))
    {
        throw djvError(
            djvPngPlugin::staticName,
            _pngError.msg);
    }

    // Set the endian.
    
    if (djvPixel::bitDepth(info.pixel) >= 16 &&
        djvMemory::LSB == djvMemory::endian())
    {
        png_set_swap(_png);
    }
}
