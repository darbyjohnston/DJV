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

//! \file djvSgiSave.cpp

#include <djvSgiSave.h>

#include <djvOpenGlImage.h>
#include <djvPixelDataUtil.h>

//------------------------------------------------------------------------------
// djvSgiSave
//------------------------------------------------------------------------------

djvSgiSave::djvSgiSave(const djvSgiPlugin::Options & options) :
    _options(options)
{}

djvSgiSave::~djvSgiSave()
{}

void djvSgiSave::open(const djvFileInfo & file, const djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvSgiSave::open");
    //DJV_DEBUG_PRINT("file = " << file);

    _file = file;

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
    _info.endian = djvMemory::MSB;

    //DJV_DEBUG_PRINT("info = " << _info);

    _image.set(_info);
}

void djvSgiSave::write(const djvImage & in, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    //DJV_DEBUG("djvSgiSave::write");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("compression = " << _options.compression);

    // Open the file.
    
    const QString fileName = _file.fileName(frame.frame);

    djvFileIo io;

    io.setEndian(djvMemory::endian() != djvMemory::MSB);

    io.open(fileName, djvFileIo::WRITE);

    djvSgiPlugin::saveInfo(
        io,
        _info,
        _options.compression != djvSgiPlugin::COMPRESSION_NONE);

    // Setup the scanline tables.

    if (_options.compression)
    {
        const int tableSize = _info.size.y * djvPixel::channels(_info.pixel);
        
        //DJV_DEBUG_PRINT("rle table size = " << tableSize);

        _rleOffset.setSize(tableSize);
        _rleSize.setSize  (tableSize);

        io.seek(tableSize * 2 * 4);
    }

    // Convert the image.

    const djvPixelData * p = &in;

    if (in.info() != _info)
    {
        //DJV_DEBUG_PRINT("convert = " << _image);

        _image.zero();

        djvOpenGlImage::copy(in, _image);

        p = &_image;
    }

    _tmp.set(p->info());

    const int w        = _tmp.w();
    const int h        = _tmp.h();
    const int channels = djvPixel::channels(_tmp.pixel());
    const int bytes    = djvPixel::channelByteCount(_tmp.pixel());

    // Deinterleave the image channels.

    djvPixelDataUtil::planarDeinterleave(*p, _tmp);

    // Write the file.

    if (! _options.compression)
    {
        io.set(_tmp.data(), _tmp.dataByteCount() / bytes, bytes);
    }
    else
    {
        djvMemoryBuffer<quint8> scanline(w * bytes * 2);

        for (int c = 0; c < channels; ++c)
        {
            for (int y = 0; y < h; ++y)
            {
                const quint64 size = djvSgiPlugin::writeRle(
                    _tmp.data() + (c * h + y) * w * bytes,
                    scanline(),
                    w,
                    bytes,
                    io.endian());

                _rleOffset()[y + c * h] = quint32(io.pos());
                _rleSize  ()[y + c * h] = quint32(size);

                io.set(scanline.data(), size / bytes, bytes);
            }
        }
        
        io.setPos(512);
        io.setU32(_rleOffset(), h * channels);
        io.setU32(_rleSize(), h * channels);
    }
}

