//------------------------------------------------------------------------------
// Copyright (c) 2008-2009 Mikael Sundell
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

//! \file djvIffSave.cpp

#include <djvIffSave.h>

#include <djvOpenGlImage.h>

//------------------------------------------------------------------------------
// djvIffSave
//------------------------------------------------------------------------------

djvIffSave::djvIffSave(const djvIffPlugin::Options & options) :
    _options(options)
{}

djvIffSave::~djvIffSave()
{}

void djvIffSave::open(const djvFileInfo & in, const djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvIffSave::open");
    //DJV_DEBUG_PRINT("in = " << in);

    _file = in;

    if (info.sequence.frames.count() > 1)
    {
        _file.setType(djvFileInfo::SEQUENCE);
    }

    _info = djvPixelDataInfo();
    _info.size = info.size;
    _info.endian = djvMemory::MSB;

    djvPixel::FORMAT format = djvPixel::format(info.pixel);

    switch (format)
    {
        case djvPixel::L:  format = djvPixel::RGB;  break;
        case djvPixel::LA: format = djvPixel::RGBA; break;

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
    _info.endian = djvMemory::MSB;

    //DJV_DEBUG_PRINT("info = " << _info);

    _image.set(_info);
}

void djvIffSave::write(const djvImage & in, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    //DJV_DEBUG("djvIffSave::write");
    //DJV_DEBUG_PRINT("in = " << in);

    // Open the file.

    djvFileIo io;

    io.setEndian(djvMemory::endian() != djvMemory::MSB);

    io.open(_file.fileName(frame.frame), djvFileIo::WRITE);

    djvIffPlugin::saveInfo(
        io,
        _info,
        _options.compression != djvIffPlugin::COMPRESSION_NONE);

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

    const int w = p->w(), h = p->h();
    const int channels = djvPixel::channels(p->info().pixel);
    const int channelByteCount = djvPixel::channelByteCount(p->info().pixel);
    const int byteCount = djvPixel::byteCount(p->info().pixel);

    const bool compress = _options.compression ? true : false;

    quint32 length = 0;

    //DJV_DEBUG_PRINT("channels = " << channels);
    //DJV_DEBUG_PRINT("channelByteCount = " << channelByteCount);
    //DJV_DEBUG_PRINT("byteCount = " << byteCount);

    quint64 pos = 0;
    pos = io.pos();

    //DJV_DEBUG_PRINT("pos = " << static_cast<int>(pos));

    // 'FOR4' type
    io.setU8('F');
    io.setU8('O');
    io.setU8('R');
    io.setU8('4');

    // 'FOR4' length
    // NOTE: only reserved for now.
    io.setU32(length);

    // 'TBMP' type
    io.setU8('T');
    io.setU8('B');
    io.setU8('M');
    io.setU8('P');

    // Write tiles.

    djvVector2i size = djvIffPlugin::tileSize(w, h);

    // Y order.

    for (int y = 0; y < size.y; y++)
    {
        // X order.

        for (int x = 0; x < size.x; x++)
        {
            // Set tile coordinates.
            quint16 xmin, xmax, ymin, ymax;

            // Set xmin and xmax.
            xmin = x * djvIffPlugin::tileWidth();
            xmax = djvMath::min(xmin + djvIffPlugin::tileWidth(), w) - 1;

            // Set ymin and ymax.
            ymin = y * djvIffPlugin::tileHeight();
            ymax = djvMath::min(ymin + djvIffPlugin::tileHeight(), h) - 1;

            // Set width and height.
            quint32 tw = xmax - xmin + 1;
            quint32 th = ymax - ymin + 1;

            // Set type.
            io.setU8('R');
            io.setU8('G');
            io.setU8('B');
            io.setU8('A');

            // Length.
            quint32 length = tw * th * byteCount;

            // Tile length.
            quint32 tileLength = length;

            // Align.
            length = djvIffPlugin::alignSize(length, 4);

            // Append xmin, xmax, ymin and ymax.
            length += 8;

            // Tile compression.
            bool tile_compress = compress;

            // Set bytes.
            djvMemoryBuffer<quint8> tile(tileLength);
            quint8 * outP = tile();

            // Handle 8-bit data.
            if (p->info().pixel == djvPixel::RGB_U8 ||
                p->info().pixel == djvPixel::RGBA_U8)
            {
                if (tile_compress)
                {
                    quint32 index = 0, size = 0;

                    // Set bytes.
                    // NOTE: prevent buffer overrun.
                    djvMemoryBuffer<quint8> tmp(tileLength * 2);

                    // Map: RGB(A)8 RGBA to BGRA
                    for (int c = (channels * channelByteCount) - 1; c >= 0; --c)
                    {
                        djvMemoryBuffer<quint8> in(tw * th);
                        quint8 * inP = in();

                        // Data.

                        for (quint16 py = ymin; py <= ymax; py++)
                        {
                            const quint8 * inDy = p->data(0, py);

                            for (quint16 px = xmin; px <= xmax; px++)
                            {
                                // Get pixel.
                                quint8 pixel;
                                const quint8 * inDx = inDy + px * byteCount + c;
                                djvMemory::copy(inDx, &pixel, 1);
                                // Set pixel.
                                *inP++ = pixel;
                            }
                        }

                        // Compress

                        size = djvIffPlugin::writeRle(in(), tmp() + index, tw * th);
                        index += size;
                    }

                    // If size exceeds tile length use uncompressed.

                    if (index < tileLength)
                    {
                        djvMemory::copy(tmp(), tile(), index);

                        // Set tile length.
                        tileLength = index;

                        // Append xmin, xmax, ymin and ymax.
                        length = index + 8;

                        // Set length.
                        quint32 align = djvIffPlugin::alignSize(length, 4);

                        if (align > length)
                        {
                            outP = tile() + index;

                            // Pad.
                            for (int i = 0;
                                i < static_cast<int>(align - length);
                                i++)
                            {
                                *outP++ = '\0';
                                tileLength++;
                            }
                        }
                    }
                    else
                    {
                        tile_compress = false;
                    }
                }

                if (!tile_compress)
                {
                    for (quint16 py = ymin; py <= ymax; py++)
                    {
                        const quint8 * inDy = p->data(0, py);

                        for (quint16 px = xmin; px <= xmax; px++)
                        {
                            // Map: RGB(A)8 RGBA to BGRA
                            for (int c = channels - 1; c >= 0; --c)
                            {
                                // Get pixel.
                                quint8 pixel;
                                const quint8 * inDx =
                                    inDy + px * byteCount + c * channelByteCount;
                                djvMemory::copy(inDx, &pixel, 1);
                                // Set pixel.
                                *outP++ = pixel;
                            }
                        }
                    }
                }
            }
            // Handle 16-bit data.
            else if (
                p->info().pixel == djvPixel::RGB_U16 ||
                p->info().pixel == djvPixel::RGBA_U16)
            {
                if (tile_compress)
                {
                    quint32 index = 0, size = 0;

                    // Set bytes.
                    // NOTE: prevent buffer overrun.
                    djvMemoryBuffer<quint8> tmp(tileLength * 2);

                    // Set map.
                    int* map = NULL;

                    if (djvMemory::endian() == djvMemory::LSB)
                    {
                        int rgb16[] = { 0, 2, 4, 1, 3, 5 };
                        int rgba16[] = { 0, 2, 4, 7, 1, 3, 5, 6 };

                        if (_info.pixel == djvPixel::RGB_U16)
                        {
                            map = rgb16;
                        }
                        else
                        {
                            map = rgba16;
                        }
                    }
                    else
                    {
                        int rgb16[] = { 1, 3, 5, 0, 2, 4 };
                        int rgba16[] = { 1, 3, 5, 7, 0, 2, 4, 6 };

                        if (_info.pixel == djvPixel::RGB_U16)
                        {
                            map = rgb16;
                        }
                        else
                        {
                            map = rgba16;
                        }
                    }

                    // Map: RGB(A)16 RGBA to BGRA
                    for (int c = (channels * channelByteCount) - 1; c >= 0; --c)
                    {
                        int mc = map[c];
                        djvMemoryBuffer<quint8> in(tw * th);
                        quint8 * inP = in();

                        // Data.

                        for (quint16 py = ymin; py <= ymax; py++)
                        {
                            const quint8 * inDy = p->data(0, py);

                            for (quint16 px = xmin; px <= xmax; px++)
                            {
                                // Get pixel.
                                quint8 pixel;
                                const quint8 * inDx = inDy + px * byteCount + mc;
                                djvMemory::copy(inDx, &pixel, 1);
                                // Set pixel.
                                *inP++ = pixel;
                            }
                        }

                        // Compress

                        size = djvIffPlugin::writeRle(in(), tmp() + index, tw * th);
                        index += size;
                    }

                    // If size exceeds tile length use uncompressed.

                    if (index < tileLength)
                    {
                        djvMemory::copy(tmp(), tile(), index);

                        // Set tile length.
                        tileLength = index;

                        // Append xmin, xmax, ymin and ymax.
                        length = index + 8;

                        // Set length.
                        quint32 align = djvIffPlugin::alignSize(length, 4);

                        if (align > length)
                        {
                            outP = tile() + index;

                            // Pad.
                            for (
                                int i = 0;
                                i < static_cast<int>(align - length);
                                i++)
                            {
                                *outP++ = '\0';
                                tileLength++;
                            }
                        }
                    }
                    else
                    {
                        tile_compress = false;
                    }
                }

                if (!tile_compress)
                {
                    for (quint16 py = ymin; py <= ymax; py++)
                    {
                        const quint8 * inDy = p->data(0, py);

                        for (quint16 px = xmin; px <= xmax; px++)
                        {
                            // Map: RGB(A)16 RGBA to BGRA
                            for (int c = channels - 1; c >= 0; --c)
                            {
                                quint16 pixel;
                                const quint8 * inDx =
                                    inDy + px * byteCount + c * channelByteCount;

                                if (djvMemory::endian() == djvMemory::LSB)
                                {
                                    djvMemory::convertEndian(inDx, &pixel, 1, 2);
                                }
                                else
                                {
                                    djvMemory::copy(inDx, &pixel, 2);
                                }

                                // Set pixel.
                                *outP++ = pixel;
                                outP++;
                            }
                        }
                    }
                }
            }

            // Set length.
            io.setU32(length);

            // Set xmin, xmax, ymin and ymax.
            io.setU16(xmin);
            io.setU16(ymin);
            io.setU16(xmax);
            io.setU16(ymax);

            // Write.
            io.set(tile(), tileLength);
        }
    }

    // Set FOR4 CIMG and FOR4 TBMP size
    quint32 p0 = io.pos() - 8;
    quint32 p1 = p0 - pos;

    // NOTE: FOR4 <size> CIMG
    io.setPos(4);
    io.setU32(p0);

    // NOTE: FOR4 <size> TBMP
    io.setPos(pos + 4);
    io.setU32(p1);
}
