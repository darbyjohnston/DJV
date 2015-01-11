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

//! \file djvTargaSave.cpp

#include <djvTargaSave.h>

#include <djvOpenGlImage.h>

//------------------------------------------------------------------------------
// djvTargaSave
//------------------------------------------------------------------------------

djvTargaSave::djvTargaSave(const djvTargaPlugin::Options & options) :
    _options(options)
{}

djvTargaSave::~djvTargaSave()
{}

void djvTargaSave::open(const djvFileInfo & in, const djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvTargaSave::open");
    //DJV_DEBUG_PRINT("in = " << in);

    _file = in;

    if (info.sequence.frames.count() > 1)
    {
        _file.setType(djvFileInfo::SEQUENCE);
    }

    _info = djvPixelDataInfo();
    _info.size = info.size;
    _info.pixel = djvPixel::pixel(djvPixel::format(info.pixel), djvPixel::U8);

    switch (djvPixel::format(_info.pixel))
    {
        case djvPixel::RGB:
        case djvPixel::RGBA: _info.bgr = true; break;

        default: break;
    }

    _info.endian = djvMemory::LSB;

    //DJV_DEBUG_PRINT("info = " << _info);

    _image.set(_info);
}

void djvTargaSave::write(const djvImage & in, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    //DJV_DEBUG("djvTargaSave::write");
    //DJV_DEBUG_PRINT("in = " << in);

    // Open the file.
    
    const QString fileName = _file.fileName(frame.frame);
    
    djvFileIo io;

    io.setEndian(djvMemory::endian() != djvMemory::LSB);

    io.open(fileName, djvFileIo::WRITE);

    djvTargaPlugin::saveInfo(
        io,
        _info,
        _options.compression != djvTargaPlugin::COMPRESSION_NONE);

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

    if (! _options.compression)
    {
        io.set(p->data(), p->dataByteCount());
    }
    else
    {
        const int w = p->w(), h = p->h();
        const int channels = djvPixel::channels(p->info().pixel);

        djvMemoryBuffer<quint8> scanline(w * channels * 2);

        for (int y = 0; y < h; ++y)
        {
            const quint64 size =
                djvTargaPlugin::writeRle(p->data(0, y), scanline(), w, channels);
            
            io.set(scanline.data(), size);
        }
    }
}
