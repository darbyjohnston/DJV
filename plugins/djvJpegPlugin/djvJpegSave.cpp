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

//! \file djvJpegSave.cpp

#include <djvJpegSave.h>

#include <djvError.h>
#include <djvOpenGlImage.h>

//------------------------------------------------------------------------------
// djvJpegSave
//------------------------------------------------------------------------------

djvJpegSave::djvJpegSave(const djvJpegPlugin::Options & options) :
    _options (options),
    _f       (0),
    _jpegInit(false)
{}

djvJpegSave::~djvJpegSave()
{
    close();
}

void djvJpegSave::open(const djvFileInfo & in, const djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvJpegSave::open");
    //DJV_DEBUG_PRINT("in = " << in);

    _file = in;

    if (info.sequence.frames.count() > 1)
    {
        _file.setType(djvFileInfo::SEQUENCE);
    }

    _info = djvImageIoInfo();
    _info.size = info.size;

    djvPixel::FORMAT format = djvPixel::format(info.pixel);

    switch (format)
    {
        case djvPixel::LA:
            format = djvPixel::L;
            break;

        case djvPixel::RGBA:
            format = djvPixel::RGB;
            break;

        default: break;
    }

    _info.pixel = djvPixel::pixel(format, djvPixel::U8);

    //DJV_DEBUG_PRINT("info = " << _info);

    _image.set(_info);
}

namespace
{

bool jpegScanline(
    libjpeg::jpeg_compress_struct * jpeg,
    const quint8 *                  in,
    djvJpegErrorStruct *            error)
{
    if (::setjmp(error->jump))
    {
        return false;
    }

    libjpeg::JSAMPROW p [] = { (libjpeg::JSAMPLE *)(in) };

    if (! libjpeg::jpeg_write_scanlines(jpeg, p, 1))
    {
        return false;
    }

    return true;
}

bool jpeg_end(libjpeg::jpeg_compress_struct * jpeg, djvJpegErrorStruct * error)
{
    if (::setjmp(error->jump))
    {
        return false;
    }

    libjpeg::jpeg_finish_compress(jpeg);

    return true;
}

} // namespace

void djvJpegSave::write(const djvImage & in, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    //DJV_DEBUG("djvJpegSave::write");
    //DJV_DEBUG_PRINT("in = " << in);

    // Open the file.

    const QString fileName = _file.fileName(frame.frame);

    //DJV_DEBUG_PRINT("file name = " << fileName);

    djvImageIoInfo info(_info);
    info.tags = in.tags;

    _open(fileName, info);

    // Convert the image.

    const djvPixelData * p = &in;

    if (in.info() != _info)
    {
        //DJV_DEBUG_PRINT("convert = " << _image);

        _image.zero();

        djvOpenGlImage::copy(in, _image);

        p = &_image;
    }

    // Write the file.

    const int h = p->h();

    for (int y = 0; y < h; ++y)
    {
        if (! jpegScanline(&_jpeg, p->data(0, h - 1 - y), &_jpegError))
        {
            throw djvError(
                djvJpegPlugin::staticName,
                _jpegError.msg);
        }
    }

    if (! jpeg_end(&_jpeg, &_jpegError))
    {
        throw djvError(
            djvJpegPlugin::staticName,
            _jpegError.msg);
    }

    close();
}

void djvJpegSave::close() throw (djvError)
{
    if (_jpegInit)
    {
        libjpeg::jpeg_destroy_compress(&_jpeg);
        
        _jpegInit = false;
    }

    if (_f)
    {
        ::fclose(_f);
        
        _f = 0;
    }
}

namespace
{

bool jpegInit(libjpeg::jpeg_compress_struct * jpeg, djvJpegErrorStruct * error)
{
    if (setjmp(error->jump))
    {
        return false;
    }
    
    using libjpeg::jpeg_compress_struct;

    libjpeg::jpeg_create_compress(jpeg);

    return true;
}

bool jpegOpen(
    FILE *                          f,
    libjpeg::jpeg_compress_struct * jpeg,
    const djvImageIoInfo &          info,
    int                             quality,
    djvJpegErrorStruct *            error)
{
    if (setjmp(error->jump))
    {
        return false;
    }

    libjpeg::jpeg_stdio_dest(jpeg, f);

    jpeg->image_width = info.size.x;
    jpeg->image_height = info.size.y;

    if (djvPixel::L_U8 == info.pixel)
    {
        jpeg->input_components = 1;
        jpeg->in_color_space = libjpeg::JCS_GRAYSCALE;
    }
    else if (djvPixel::RGB_U8 == info.pixel)
    {
        jpeg->input_components = 3;
        jpeg->in_color_space = libjpeg::JCS_RGB;
    }

    libjpeg::jpeg_set_defaults(jpeg);

    libjpeg::jpeg_set_quality(jpeg, quality, static_cast<libjpeg::boolean>(1));
    libjpeg::jpeg_start_compress(jpeg, static_cast<libjpeg::boolean>(1));

    QString tag = info.tags[djvImageTags::tagLabels()[djvImageTags::DESCRIPTION]];

    if (tag.length())
    {
        libjpeg::jpeg_write_marker(
            jpeg,
            JPEG_COM,
            (libjpeg::JOCTET *)tag.toLatin1().data(),
            static_cast<uint>(tag.length()));
    }

    return true;
}

} // namespace

void djvJpegSave::_open(const QString & in, const djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvJpegSave::_open");
    //DJV_DEBUG_PRINT("in = " << in);

    close();

    // Initialize libjpeg.

    _jpeg.err = libjpeg::jpeg_std_error(&_jpegError.pub);
    _jpegError.pub.error_exit = djvJpegError;
    _jpegError.pub.emit_message = djvJpegWarning;
    _jpegError.msg[0] = 0;

    if (! jpegInit(&_jpeg, &_jpegError))
    {
        throw djvError(
            djvJpegPlugin::staticName,
            _jpegError.msg);
    }

    _jpegInit = true;

    // Open the file.

#if defined(DJV_WINDOWS)

    ::fopen_s(&_f, in.toLatin1().data(), "wb");

#else // DJV_WINDOWS

    _f = ::fopen(in.toLatin1().data(), "wb");

#endif // DJV_WINDOWS

    if (! _f)
    {
        throw djvError(
            djvJpegPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_OPEN].arg(in));
    }

    if (! jpegOpen(_f, &_jpeg, info, _options.quality, &_jpegError))
    {
        throw djvError(
            djvJpegPlugin::staticName,
            _jpegError.msg);
    }
}

