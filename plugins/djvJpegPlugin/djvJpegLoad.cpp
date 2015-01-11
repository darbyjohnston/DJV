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

//! \file djvJpegLoad.cpp

#include <djvJpegLoad.h>

#include <djvError.h>
#include <djvImage.h>
#include <djvPixelDataUtil.h>

//------------------------------------------------------------------------------
// djvJpegLoad
//------------------------------------------------------------------------------

djvJpegLoad::djvJpegLoad() :
    _f       (0),
    _jpegInit(false)
{}

djvJpegLoad::~djvJpegLoad()
{
    close();
}

void djvJpegLoad::open(const djvFileInfo & in, djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvJpegLoad::open");
    //DJV_DEBUG_PRINT("in = " << in);

    _file = in;
    
    _open(_file.fileName(_file.sequence().start()), info);

    if (djvFileInfo::SEQUENCE == _file.type())
    {
        info.sequence.frames = _file.sequence().frames;
    }
    
    close();
}

namespace
{

bool jpegScanline(
    libjpeg::jpeg_decompress_struct * jpeg,
    quint8 *                          out,
    djvJpegErrorStruct *              error)
{
    if (::setjmp(error->jump))
    {
        return false;
    }

    libjpeg::JSAMPROW p [] = { (libjpeg::JSAMPLE *)(out) };

    if (! libjpeg::jpeg_read_scanlines(jpeg, p, 1))
    {
        return false;
    }

    return true;
}

bool jpegEnd(
    libjpeg::jpeg_decompress_struct * jpeg,
    djvJpegErrorStruct *              error)
{
    if (::setjmp(error->jump))
    {
        return false;
    }

    libjpeg::jpeg_finish_decompress(jpeg);

    return true;
}

} // namespace

void djvJpegLoad::read(djvImage & image, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    //DJV_DEBUG("djvJpegLoad::read");
    //DJV_DEBUG_PRINT("frame = " << frame);

    image.colorProfile = djvColorProfile();
    image.tags = djvImageTags();

    // Open the file.

    const QString fileName =
        _file.fileName(frame.frame != -1 ? frame.frame : _file.sequence().start());

    //DJV_DEBUG_PRINT("file name = " << fileName);

    djvImageIoInfo info;
    
    _open(fileName, info);
    
    image.tags = info.tags;

    // Read the file.

    djvPixelData * data = frame.proxy ? &_tmp : &image;
    
    data->set(info);

    for (int y = 0; y < info.size.y; ++y)
    {
        if (! jpegScanline(
            &_jpeg,
            data->data(0, data->h() - 1 - y),
            &_jpegError))
        {
            throw djvError(
                djvJpegPlugin::staticName,
                _jpegError.msg);
        }
    }

    if (! jpegEnd(&_jpeg, &_jpegError))
    {
        throw djvError(
            djvJpegPlugin::staticName,
            _jpegError.msg);
    }

    if (frame.proxy)
    {
        info.size = djvPixelDataUtil::proxyScale(info.size, frame.proxy);
        info.proxy = frame.proxy;
        image.set(info);

        djvPixelDataUtil::proxyScale(_tmp, image, frame.proxy);
    }

    //DJV_DEBUG_PRINT("image = " << image);
    
    close();
}

void djvJpegLoad::close() throw (djvError)
{
    if (_jpegInit)
    {
        libjpeg::jpeg_destroy_decompress(&_jpeg);
        
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

bool jpegInit(
    libjpeg::jpeg_decompress_struct * jpeg,
    djvJpegErrorStruct *              error)
{
    if (::setjmp(error->jump))
    {
        return false;
    }
    
    using libjpeg::jpeg_decompress_struct;

    libjpeg::jpeg_create_decompress(jpeg);

    return true;
}

bool jpegOpen(
    FILE *                            f,
    libjpeg::jpeg_decompress_struct * jpeg,
    djvJpegErrorStruct *              error)
{
    if (::setjmp(error->jump))
    {
        return false;
    }

    libjpeg::jpeg_stdio_src(jpeg, f);

    libjpeg::jpeg_save_markers(jpeg, JPEG_COM, 0xFFFF);

    if (! libjpeg::jpeg_read_header(jpeg, static_cast<libjpeg::boolean>(1)))
    {
        return false;
    }

    if (! libjpeg::jpeg_start_decompress(jpeg))
    {
        return false;
    }

    return true;
}

} // namespace

void djvJpegLoad::_open(const QString & in, djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvJpegLoad::_open");
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

    // Open.

#if defined(DJV_WINDOWS)

    ::fopen_s(&_f, in.toLatin1().data(), "rb");

#else // DJV_WINDOWS

    _f = ::fopen(in.toLatin1().data(), "rb");

#endif // DJV_WINDOWS

    if (! _f)
    {
        throw djvError(
            djvJpegPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_OPEN].arg(in));
    }

    if (! jpegOpen(_f, &_jpeg, &_jpegError))
    {
        throw djvError(
            djvJpegPlugin::staticName,
            _jpegError.msg);
    }

    // Information.

    info.fileName = in;

    info.size = djvVector2i(_jpeg.output_width, _jpeg.output_height);

    if (! djvPixel::pixel(
        _jpeg.out_color_components, 8, djvPixel::INTEGER, info.pixel))
    {
        throw djvError(
            djvJpegPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_UNSUPPORTED].arg(in));
    }

    // Image tags.

    const libjpeg::jpeg_saved_marker_ptr marker = _jpeg.marker_list;

    if (marker)
        info.tags[djvImageTags::tagLabels()[djvImageTags::DESCRIPTION]] =
            QString((const char *)marker->data).mid(0, marker->data_length);

    //DJV_DEBUG_PRINT("info = " << info);
}

