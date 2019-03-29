//------------------------------------------------------------------------------
// Copyright (c) 2008-2009 Mikael Sundell
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <djvAV/IFFSave.h>

#include <djvAV/OpenGLImage.h>

#include <djvCore/CoreContext.h>

namespace djv
{
    namespace AV
    {
        IFFSave::IFFSave(const Core::FileInfo & fileInfo, const IOInfo & ioInfo, const IFF::Options & options, const QPointer<Core::CoreContext> & context) :
            Save(fileInfo, ioInfo, context),
            _options(options)
        {
            //DJV_DEBUG("IFFSave::IFFSave");
            //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);

            if (_ioInfo.sequence.frames.count() > 1)
            {
                _fileInfo.setType(Core::FileInfo::SEQUENCE);
            }

            _info = PixelDataInfo();
            _info.size = _ioInfo.layers[0].size;
            _info.endian = Core::Memory::MSB;

            Pixel::FORMAT format = Pixel::format(_ioInfo.layers[0].pixel);
            switch (format)
            {
            case Pixel::L:  format = Pixel::RGB;  break;
            case Pixel::LA: format = Pixel::RGBA; break;
            default: break;
            }

            Pixel::TYPE type = Pixel::type(_ioInfo.layers[0].pixel);
            switch (type)
            {
            case Pixel::U10:
            case Pixel::F16:
            case Pixel::F32: type = Pixel::U16; break;
            default: break;
            }

            _info.pixel = Pixel::pixel(format, type);
            _info.endian = Core::Memory::MSB;
            //DJV_DEBUG_PRINT("info = " << _info);

            _image.set(_info);
        }

        IFFSave::~IFFSave()
        {}

        void IFFSave::write(const Image & in, const ImageIOInfo & frame)
        {
            //DJV_DEBUG("djvIFFSave::write");
            //DJV_DEBUG_PRINT("in = " << in);

            // Open the file.
            Core::FileIO io;
            io.setEndian(Core::Memory::endian() != Core::Memory::MSB);
            io.open(_fileInfo.fileName(frame.frame), Core::FileIO::WRITE);
            IFF::saveInfo(
                io,
                _info,
                _options.compression != IFF::COMPRESSION_NONE);

            // Convert the image.
            const PixelData * p = &in;
            if (in.info() != _info)
            {
                //DJV_DEBUG_PRINT("convert = " << _image);
                _image.zero();
                OpenGLImage().copy(in, _image);
                p = &_image;
            }

            // Write the file.
            const int w = p->w(), h = p->h();
            const int channels = Pixel::channels(p->info().pixel);
            const int channelByteCount = Pixel::channelByteCount(p->info().pixel);
            const int byteCount = Pixel::byteCount(p->info().pixel);

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
            glm::ivec2 size = IFF::tileSize(w, h);

            // Y order.
            for (int y = 0; y < size.y; y++)
            {
                // X order.
                for (int x = 0; x < size.x; x++)
                {
                    // Set tile coordinates.
                    quint16 xmin, xmax, ymin, ymax;

                    // Set xmin and xmax.
                    xmin = x * IFF::tileWidth();
                    xmax = Core::Math::min(xmin + IFF::tileWidth(), w) - 1;

                    // Set ymin and ymax.
                    ymin = y * IFF::tileHeight();
                    ymax = Core::Math::min(ymin + IFF::tileHeight(), h) - 1;

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
                    length = IFF::alignSize(length, 4);

                    // Append xmin, xmax, ymin and ymax.
                    length += 8;

                    // Tile compression.
                    bool tile_compress = compress;

                    // Set bytes.
                    std::vector<quint8> tile(tileLength);
                    quint8 * outP = tile.data();

                    // Handle 8-bit data.
                    if (p->info().pixel == Pixel::RGB_U8 ||
                        p->info().pixel == Pixel::RGBA_U8)
                    {
                        if (tile_compress)
                        {
                            quint32 index = 0, size = 0;

                            // Set bytes.
                            // NOTE: prevent buffer overrun.
                            std::vector<quint8> tmp(tileLength * 2);

                            // Map: RGB(A)8 RGBA to BGRA
                            for (int c = (channels * channelByteCount) - 1; c >= 0; --c)
                            {
                                std::vector<quint8> in(tw * th);
                                quint8 * inP = in.data();

                                // Data.
                                for (quint16 py = ymin; py <= ymax; py++)
                                {
                                    const quint8 * inDy = p->data(0, py);

                                    for (quint16 px = xmin; px <= xmax; px++)
                                    {
                                        // Get pixel.
                                        quint8 pixel;
                                        const quint8 * inDx = inDy + px * byteCount + c;
                                        memcpy(&pixel, inDx, 1);
                                        // Set pixel.
                                        *inP++ = pixel;
                                    }
                                }

                                // Compress
                                size = IFF::writeRle(in.data(), tmp.data() + index, tw * th);
                                index += size;
                            }

                            // If size exceeds tile length use uncompressed.
                            if (index < tileLength)
                            {
                                memcpy(tile.data(), tmp.data(), index);

                                // Set tile length.
                                tileLength = index;

                                // Append xmin, xmax, ymin and ymax.
                                length = index + 8;

                                // Set length.
                                quint32 align = IFF::alignSize(length, 4);

                                if (align > length)
                                {
                                    outP = tile.data() + index;

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
                                        memcpy(&pixel, inDx, 1);
                                        // Set pixel.
                                        *outP++ = pixel;
                                    }
                                }
                            }
                        }
                    }
                    // Handle 16-bit data.
                    else if (
                        p->info().pixel == Pixel::RGB_U16 ||
                        p->info().pixel == Pixel::RGBA_U16)
                    {
                        if (tile_compress)
                        {
                            quint32 index = 0, size = 0;

                            // Set bytes.
                            // NOTE: prevent buffer overrun.
                            std::vector<quint8> tmp(tileLength * 2);

                            // Set map.
                            int* map = NULL;

                            if (Core::Memory::endian() == Core::Memory::LSB)
                            {
                                int rgb16[] = { 0, 2, 4, 1, 3, 5 };
                                int rgba16[] = { 0, 2, 4, 7, 1, 3, 5, 6 };

                                if (_info.pixel == Pixel::RGB_U16)
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

                                if (_info.pixel == Pixel::RGB_U16)
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
                                std::vector<quint8> in(tw * th);
                                quint8 * inP = in.data();

                                // Data.
                                for (quint16 py = ymin; py <= ymax; py++)
                                {
                                    const quint8 * inDy = p->data(0, py);

                                    for (quint16 px = xmin; px <= xmax; px++)
                                    {
                                        // Get pixel.
                                        quint8 pixel;
                                        const quint8 * inDx = inDy + px * byteCount + mc;
                                        memcpy(&pixel, inDx, 1);
                                        // Set pixel.
                                        *inP++ = pixel;
                                    }
                                }

                                // Compress
                                size = IFF::writeRle(in.data(), tmp.data() + index, tw * th);
                                index += size;
                            }

                            // If size exceeds tile length use uncompressed.
                            if (index < tileLength)
                            {
                                memcpy(tile.data(), tmp.data(), index);

                                // Set tile length.
                                tileLength = index;

                                // Append xmin, xmax, ymin and ymax.
                                length = index + 8;

                                // Set length.
                                quint32 align = IFF::alignSize(length, 4);

                                if (align > length)
                                {
                                    outP = tile.data() + index;

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

                                        if (Core::Memory::endian() == Core::Memory::LSB)
                                        {
                                            Core::Memory::convertEndian(inDx, &pixel, 1, 2);
                                        }
                                        else
                                        {
                                            memcpy(&pixel, inDx, 2);
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
                    io.set(tile.data(), tileLength);
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

    } // namespace AV
} // namespace djv
