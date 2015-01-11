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

//! \file djvPpmSave.cpp

#include <djvPpmSave.h>

#include <djvOpenGlImage.h>

#include <stdio.h>

//------------------------------------------------------------------------------
// djvPpmSave
//------------------------------------------------------------------------------

djvPpmSave::djvPpmSave(const djvPpmPlugin::Options & options) :
    _options (options),
    _bitDepth(0)
{}

djvPpmSave::~djvPpmSave()
{}

void djvPpmSave::open(const djvFileInfo & in, const djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvPpmSave::open");
    //DJV_DEBUG_PRINT("in = " << in);

    _file = in;

    if (info.sequence.frames.count() > 1)
    {
        _file.setType(djvFileInfo::SEQUENCE);
    }

    _info          = djvPixelDataInfo();
    _info.size     = info.size;
    _info.mirror.y = true;

    switch (_options.type)
    {
        case djvPpmPlugin::TYPE_AUTO:
        {
            djvPixel::FORMAT format = djvPixel::format(info.pixel);

            switch (format)
            {
                case djvPixel::LA:   format = djvPixel::L;   break;
                case djvPixel::RGBA: format = djvPixel::RGB; break;

                default: break;
            }

            djvPixel::TYPE type = djvPixel::type(info.pixel);

            switch (type)
            {
                case djvPixel::U10:
                case djvPixel::F16:
                case djvPixel::F32: type = djvPixel::U16; break;

                default: break;
            }

            _info.pixel = djvPixel::pixel(format, type);
            _bitDepth = djvPixel::bitDepth(_info.pixel);
        }
        break;

        case djvPpmPlugin::TYPE_U1:
        
            _info.pixel = djvPixel::L_U8;
            _bitDepth   = 1;
            
            break;

        default: break;
    }

    _info.endian = djvMemory::MSB;

    //DJV_DEBUG_PRINT("info = " << _info);

    _image.set(_info);
}

void djvPpmSave::write(const djvImage & in, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    //DJV_DEBUG("djvPpmSave::write");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("type = " << _type);
    //DJV_DEBUG_PRINT("data = " << _data);

    // Open the file.

    djvFileIo io;

    _open(_file.fileName(frame.frame), io);

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

    if (djvPpmPlugin::DATA_BINARY == _options.data && _bitDepth != 1)
    {
        io.set(p->data(), p->dataByteCount());
    }
    else
    {
        const int w = p->w(), h = p->h();
        
        const int channels = djvPixel::channels(p->info().pixel);
        
        const quint64 scanlineByteCount = djvPpmPlugin::scanlineByteCount(
            w,
            channels,
            _bitDepth,
            _options.data);

        djvMemoryBuffer<quint8> scanline(scanlineByteCount);

        //DJV_DEBUG_PRINT("scanline = " << static_cast<int>(scanlineByteCount));

        for (int y = 0; y < h; ++y)
        {
            if (djvPpmPlugin::DATA_BINARY == _options.data &&
                1 == _bitDepth)
            {
                const quint8 * inP = p->data(0, y);
                quint8 * outP = scanline();

                for (int i = 0; i < w; ++i)
                {
                    const int tmp = inP[i];
                    const int off = i % 8;
                    const int j = i / 8;

                    if (0 == off)
                    {
                        outP[j] = 0;
                    }

                    outP[j] |= ((! tmp) & 1) << (7 - off);
                }

                io.set(scanline(), scanlineByteCount);
            }
            else
            {
                const quint64 size = djvPpmPlugin::asciiSave(
                    p->data(0, y),
                    scanline(),
                    w * channels,
                    _bitDepth);
                
                io.set(scanline(), size);
            }
        }
    }
}

void djvPpmSave::_open(const QString & in, djvFileIo & io) throw (djvError)
{
    //DJV_DEBUG("djvPpmSave::_open");
    //DJV_DEBUG_PRINT("in = " << in);

    // Open.

    io.setEndian(djvMemory::endian() != djvMemory::MSB);
    
    io.open(in, djvFileIo::WRITE);

    // Header.

    int ppmType = 0;

    if (1 == _bitDepth)
    {
        ppmType = djvPpmPlugin::DATA_ASCII == _options.data ? 1 : 4;
    }
    else
    {
        ppmType = djvPpmPlugin::DATA_ASCII == _options.data ? 2 : 5;

        if (3 == _image.channels())
        {
            ++ppmType;
        }
    }

    char magic [] = "P \n";
    magic[1] = '0' + ppmType;
    io.set(magic, 3);

    char tmp[djvStringUtil::cStringLength] = "";
    
    int size = SNPRINTF(
        tmp,
        djvStringUtil::cStringLength, "%d %d\n",
        _image.w(),
        _image.h());
    
    io.set(tmp, size);

    if (_bitDepth != 1)
    {
        //! \todo The symbol djvPixel::u16Max is undefined on Mac OS 10.6?

        //const int maxValue =
        //    (8 == _bitDepth) ? djvPixel::u8Max : djvPixel::u16Max;
        
        const int max_value = (8 == _bitDepth) ? djvPixel::u8Max : 65535;
        
        size = SNPRINTF(tmp, djvStringUtil::cStringLength, "%d\n", max_value);
        
        io.set(tmp, size);
    }
}

