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

//! \file djvIffLoad.cpp

#include <djvIffLoad.h>

#include <djvImage.h>
#include <djvPixelDataUtil.h>

//------------------------------------------------------------------------------
// djvIffLoad
//------------------------------------------------------------------------------

djvIffLoad::djvIffLoad() :
    _tiles      (0),
    _compression(false)
{}

djvIffLoad::~djvIffLoad()
{}

void djvIffLoad::open(const djvFileInfo & in, djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvIffLoad::open");
    //DJV_DEBUG_PRINT("in = " << in);

    _file = in;
    
    djvFileIo io;
    
    _open(_file.fileName(_file.sequence().start()), info, io);

    if (djvFileInfo::SEQUENCE == _file.type())
    {
        info.sequence.frames = _file.sequence().frames;
    }
}

void djvIffLoad::read(djvImage & image, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    //DJV_DEBUG("djvIffLoad::read");
    //DJV_DEBUG_PRINT("frame = " << frame);

    image.colorProfile = djvColorProfile();
    image.tags = djvImageTags();

    quint8 type[4];
    quint8 pixels[32];

    quint32 size;
    quint32 chunkSize;
    quint32 tilesRgba;

    // Open the file.

    const QString fileName =
        _file.fileName(frame.frame != -1 ? frame.frame : _file.sequence().start());

    //DJV_DEBUG_PRINT("file name = " << fileName);

    djvImageIoInfo info;
    
    djvFileIo io;
    
    _open(fileName, info, io);
    
    image.tags = info.tags;

    // Read the file.

    const int channelByteCount = djvPixel::channelByteCount(info.pixel);
    const uint byteCount = djvPixel::byteCount(info.pixel);

    //DJV_DEBUG_PRINT("channels = " << channels);
    //DJV_DEBUG_PRINT("channelByteCount = " << channelByteCount);
    //DJV_DEBUG_PRINT("byteCount = " << byteCount);

    io.readAhead();

    djvPixelData * data = frame.proxy ? &_tmp : &image;

    data->set(info);

    tilesRgba = _tiles;

    // Read FOR4 <size> TBMP block
    for (;;)
    {
        // Get type.
        io.get(&type, 4);

        // Get length.
        io.getU32(&size, 1);
        chunkSize = djvIffPlugin::alignSize(size, 4);

        if (type[0] == 'A' &&
                type[1] == 'U' &&
                type[2] == 'T' &&
                type[3] == 'H')
        {
            //DJV_DEBUG_PRINT("type = AUTH");
            const quint8 * p = io.mmapP();

            // Get tag.
            info.tags[djvImageTags::tagLabels()[djvImageTags::CREATOR]] =
                QString((const char *)p).mid(chunkSize);

            // Skip to the next block.
            io.seek(chunkSize);

            // Set tag.
            image.tags = info.tags;
        }
        else if (type[0] == 'F' &&
                 type[1] == 'O' &&
                 type[2] == 'R' &&
                 type[3] == '4')
        {
            //DJV_DEBUG_PRINT("type = FOR4");

            // Get type.
            io.get(&type, 4);

            // Check if TBMP.
            if (type[0] == 'T' &&
                    type[1] == 'B' &&
                    type[2] == 'M' &&
                    type[3] == 'P')
            {
                //DJV_DEBUG_PRINT("type = TBMP");

                // Read RGBA and ZBUF block.
                for (;;)
                {
                    // Get type.
                    io.get(&type, 4);

                    // Get length.
                    io.getU32(&size, 1);
                    chunkSize = djvIffPlugin::alignSize(size, 4);

                    // Tiles and RGBA.
                    if (tilesRgba &&
                            type[0] == 'R' &&
                            type[1] == 'G' &&
                            type[2] == 'B' &&
                            type[3] == 'A')
                    {
                        // Set image size.
                        quint32 imageSize = size;

                        // Get tile coordinates.
                        quint16 xmin, xmax, ymin, ymax;
                        io.getU16(&xmin, 1);
                        io.getU16(&ymin, 1);
                        io.getU16(&xmax, 1);
                        io.getU16(&ymax, 1);

                        if (xmin > xmax ||
                            ymin > ymax ||
                            xmax >= info.size.x ||
                            ymax >= info.size.y)
                        {
                            throw djvError(
                                djvIffPlugin::staticName,
                                djvImageIo::errorLabels()[
                                    djvImageIo::ERROR_UNSUPPORTED].
                                arg(fileName));
                        }

                        // NOTE: tile w = xmax - xmin + 1
                        //       tile h = ymax - ymin + 1

                        quint32 tw = xmax - xmin + 1;
                        quint32 th = ymax - ymin + 1;

                        if (!tw || !th)
                        {
                            throw djvError(
                                djvIffPlugin::staticName,
                                djvImageIo::errorLabels()[
                                    djvImageIo::ERROR_UNSUPPORTED].
                                arg(fileName));
                        }

                        bool tile_compress = false;

                        // If tile compression fails to be less than
                        // image data stored uncompressed, the tile
                        // is written uncompressed.

                        // Set channels.
                        quint8 channels = djvPixel::channels(info.pixel);

                        // Set tile pixels.

                        // Append xmin, xmax, ymin and ymax.
                        quint32 tileSize =
                            tw * th * channels *
                            djvPixel::channelByteCount(info.pixel) + 8;

                        // Test compressed.
                        if (tileSize > imageSize)
                        {
                            tile_compress = true;
                        }

                        // Handle 8-bit data.
                        if (info.pixel == djvPixel::RGB_U8 ||
                            info.pixel == djvPixel::RGBA_U8)
                        {

                            // Tile compress.

                            if (tile_compress)
                            {
                                // Set bytes.
                                const quint8 * p = io.mmapP();

                                // Map: RGB(A)8 BGRA to RGBA
                                for (int c = (channels * channelByteCount) - 1;
                                    c >= 0;
                                    --c)
                                {
                                    djvMemoryBuffer<quint8> in(tw * th);
                                    quint8 * inP = in ();

                                    // Uncompress.

                                    p += djvIffPlugin::readRle (p, in(), tw * th);

                                    for (quint16 py = ymin; py <= ymax; py++)
                                    {
                                        quint8 * out_dy = data->data(0, py);

                                        for (
                                            quint16 px = xmin;
                                            px <= xmax;
                                            px++)
                                        {
                                            quint8 * outP =
                                                out_dy + px * byteCount + c;
                                            *outP++ = *inP++;
                                        }
                                    }
                                }

                                // Seek

                                io.seek(imageSize - 8);

                                // Test.

                                if (p != io.mmapP())
                                {
                                    throw djvError(
                                        djvIffPlugin::staticName,
                                        djvImageIo::errorLabels()[
                                            djvImageIo::ERROR_UNSUPPORTED].
                                        arg(fileName));
                                }
                            }
                            else
                            {
                                for (quint16 py = ymin; py <= ymax; py++)
                                {
                                    quint8 * out_dy = data->data(xmin, py);

                                    // Tile scanline.

                                    djvMemoryBuffer<quint8> scanline(tw * byteCount);
                                    quint8 * outP = scanline();

                                    // Set bytes.

                                    for (quint16 px = xmin; px <= xmax; px++)
                                    {
                                        // Get pixels.
                                        io.get(&pixels, byteCount);

                                        if (size < static_cast<quint32>(byteCount))
                                        {
                                            throw djvError(
                                                djvIffPlugin::staticName,
                                                djvImageIo::errorLabels()[
                                                    djvImageIo::ERROR_READ].
                                                arg(fileName));
                                        }

                                        size -= byteCount;

                                        // Map: RGB(A)8 ABGR to ARGB
                                        for (int c = channels - 1; c >= 0; --c)
                                        {
                                            quint8 pixel;
                                            quint8 * inP =
                                                pixels + c * channelByteCount;
                                            djvMemory::copy(inP, &pixel, 1);
                                            *outP++ = pixel;
                                        }
                                    }

                                    // Copy data.
                                    djvMemory::copy(
                                        scanline(), out_dy, tw * byteCount);
                                }
                            }
                        }
                        // Handle 16-bit data.
                        else if (
                            info.pixel == djvPixel::RGB_U16 ||
                            info.pixel == djvPixel::RGBA_U16)
                        {
                            if (tile_compress)
                            {

                                // Set bytes.
                                const quint8 * p = io.mmapP();

                                // Set map.
                                int* map = NULL;

                                if (djvMemory::endian() == djvMemory::LSB)
                                {
                                    int rgb16[] = { 0, 2, 4, 1, 3, 5 };
                                    int rgba16[] = { 0, 2, 4, 7, 1, 3, 5, 6 };

                                    if (info.pixel == djvPixel::RGB_U16)
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

                                    if (info.pixel == djvPixel::RGB_U16)
                                    {
                                        map = rgb16;
                                    }
                                    else
                                    {
                                        map = rgba16;
                                    }
                                }

                                // Map: RGB(A)8 BGRA to RGBA
                                for (int c = (channels * channelByteCount) - 1;
                                    c >= 0;
                                    --c)
                                {
                                    int mc = map[c];

                                    djvMemoryBuffer<quint8> in(tw * th);
                                    quint8 * inP = in ();

                                    // Uncompress.

                                    p += djvIffPlugin::readRle (p, in(), tw * th);

                                    for (quint16 py = ymin; py <= ymax; py++)
                                    {
                                        quint8 * out_dy = data->data(0, py);

                                        for (
                                            quint16 px = xmin;
                                            px <= xmax;
                                            px++)
                                        {
                                            quint8 * outP =
                                                out_dy + px * byteCount + mc;
                                            *outP++ = *inP++;
                                        }
                                    }
                                }

                                // Seek

                                io.seek(imageSize - 8);

                                // Test.

                                if (p != io.mmapP())
                                {
                                    throw djvError(
                                        djvIffPlugin::staticName,
                                        djvImageIo::errorLabels()[
                                            djvImageIo::ERROR_UNSUPPORTED].
                                        arg(fileName));
                                }
                            }
                            else
                            {
                                for (quint16 py = ymin; py <= ymax; py++)
                                {
                                    quint8 * out_dy = data->data(xmin, py);

                                    // Tile scanline.

                                    djvMemoryBuffer<quint16> scanline(
                                        tw * byteCount);
                                    quint16 * outP = scanline();

                                    // Set bytes.

                                    for (quint16 px = xmin; px <= xmax; px++)
                                    {
                                        // Get pixels.
                                        io.get(&pixels, byteCount);

                                        if (size < static_cast<quint32>(byteCount))
                                        {
                                            throw djvError(
                                                djvIffPlugin::staticName,
                                                djvImageIo::errorLabels()[
                                                    djvImageIo::ERROR_READ].
                                                arg(fileName));
                                        }

                                        size -= byteCount;

                                        // Map: RGB8 ABGR to ARGB
                                        for (int c = channels - 1; c >= 0; --c)
                                        {
                                            quint16 pixel;
                                            quint8 * in =
                                                pixels + c * channelByteCount;

                                            if (djvMemory::endian() == djvMemory::LSB)
                                            {
                                                djvMemory::convertEndian(in, &pixel, 1, 2);
                                            }
                                            else
                                            {
                                                djvMemory::copy(in, &pixel, 2);
                                            }

                                            // Set pixel.
                                            *outP++ = pixel;
                                        }
                                    }

                                    // Copy data.
                                    djvMemory::copy(
                                        scanline(),
                                        out_dy,
                                        tw * byteCount);
                                }
                            }
                        }
                        else
                        {
                            io.seek(chunkSize);
                        }

                        // Seek to align to chunksize.
                        size = chunkSize - imageSize;

                        if (size)
                        {
                            io.seek(size);
                        }

                        tilesRgba--;
                    }
                    else
                    {
                        io.seek(chunkSize);
                    }

                    if (tilesRgba)
                    {
                        continue;
                    }
                    else
                    {
                        break;
                    }

                }

                // TBMP done, break.
                break;
            }

        }
        else
        {
            // Skip to the next block.
            io.seek(chunkSize);
        }
    }

    if (frame.proxy)
    {
        info.size = djvPixelDataUtil::proxyScale(info.size, frame.proxy);
        info.proxy = frame.proxy;
        image.set(info);

        djvPixelDataUtil::proxyScale(_tmp, image, frame.proxy);
    }

    //DJV_DEBUG_PRINT("image = " << image);
}

void djvIffLoad::_open(const djvFileInfo & in, djvImageIoInfo & info, djvFileIo & io)
    throw (djvError)
{
    //DJV_DEBUG("djvIffLoad::_open");
    //DJV_DEBUG_PRINT("in = " << in);

    io.setEndian(djvMemory::endian() != djvMemory::MSB);
    
    io.open(in, djvFileIo::READ);

    info.fileName = in;
    
    djvIffPlugin::loadInfo(io, info, &_tiles, &_compression);
}
