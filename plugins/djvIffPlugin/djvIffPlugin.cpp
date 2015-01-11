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

//! \file djvIffPlugin.cpp

#include <djvIffPlugin.h>

#include <djvIffLoad.h>
#include <djvIffSave.h>
#include <djvIffWidget.h>

#include <djvAssert.h>
#include <djvError.h>

extern "C"
{

DJV_PLUGIN_EXPORT djvPlugin * djvImageIo()
{
    return new djvIffPlugin;
}

} // extern "C"

//------------------------------------------------------------------------------
// djvIffPlugin::Options
//------------------------------------------------------------------------------

djvIffPlugin::Options::Options() :
    compression(djvIffPlugin::COMPRESSION_RLE)
{}

//------------------------------------------------------------------------------
// djvIffPlugin
//------------------------------------------------------------------------------

const QString djvIffPlugin::staticName = "IFF";

const QStringList & djvIffPlugin::compressionLabels()
{
    static const QStringList data = QStringList() <<
        "None" <<
        "RLE";

    DJV_ASSERT(data.count() == COMPRESSION_COUNT);

    return data;
}

namespace
{

struct Header
{
    Header ();

    void load(djvFileIo &, djvImageIoInfo &, int * tiles, bool * compression)
        throw (djvError);
    void save(djvFileIo &, const djvImageIoInfo &, bool compression)
        throw (djvError);

    void debug() const;

private:

    struct Data
    {
        quint32 x;
        quint32 y;
        quint32 width;
        quint32 height;
        quint8  pixelBits;
        quint8  pixelChannels;
        quint16 tiles;
    }
        _data;
};

Header::Header()
{
    _data.x = 0;
    _data.y = 0;
    _data.width = 0;
    _data.height = 0;
    _data.pixelBits = 0;
    _data.pixelChannels = 0;
    _data.tiles = 0;
}

void Header::load(
    djvFileIo &      io,
    djvImageIoInfo & info,
    int *            tiles,
    bool *           compression) throw (djvError)
{
    //DJV_DEBUG("Header::load");

    quint8  type[4];
    quint32 size;
    quint32 chunksize;
    quint32 tbhdsize;
    quint32 flags;
    quint32 compressed;
    quint16 bytes;
    quint16 prnum;
    quint16 prden;

    // Read FOR4 <size> CIMG.
    for (;;)
    {
        // Get type.
        io.get(&type, 4);

        // Get length.
        io.getU32(&size, 1);
        chunksize = djvIffPlugin::alignSize(size, 4);

        if (type[0] == 'F' &&
                type[1] == 'O' &&
                type[2] == 'R' &&
                type[3] == '4')
        {
            //DJV_DEBUG_PRINT("type = FOR4");
            //DJV_DEBUG_PRINT("size = " << size);
            //DJV_DEBUG_PRINT("chunksize = " << chunksize);

            // Get type
            io.get(&type, 4);

            // Check if CIMG.
            if (type[0] == 'C' &&
                    type[1] == 'I' &&
                    type[2] == 'M' &&
                    type[3] == 'G')
            {
                //DJV_DEBUG_PRINT("type = CIMG");

                // Read TBHD.
                for (;;)
                {
                    // Get type
                    io.get(&type, 4);

                    // Get length
                    io.getU32(&size, 1);
                    chunksize = djvIffPlugin::alignSize(size, 4);

                    if (type[0] == 'T' &&
                            type[1] == 'B' &&
                            type[2] == 'H' &&
                            type[3] == 'D')
                    {
                        tbhdsize = size;

                        // Test if size if correct.
                        if (tbhdsize != 24 && tbhdsize != 32)
                        {
                            throw djvError(
                                djvIffPlugin::staticName,
                                djvImageIo::errorLabels()[
                                    djvImageIo::ERROR_READ].
                                arg(io.fileName()));
                        }

                        // Set data.
                        Data data;

                        // Get width and height.
                        io.getU32(&data.width, 1);
                        io.getU32(&data.height, 1);
                        info.size = djvVector2i(data.width, data.height);

                        // Get prnum and prdeb
                        io.getU16(&prnum, 1);
                        io.getU16(&prden, 1);

                        // Get flags, bytes, tiles and compressed.
                        io.getU32(&flags, 1);
                        io.getU16(&bytes, 1);

                        // Get tiles.
                        io.getU16(&data.tiles, 1);
                        *tiles = data.tiles;

                        // Get compressed.
                        io.getU32(&compressed, 1);

                        // 0 no compression
                        // 1 RLE compression
                        // 2 QRL (not supported)
                        // 3 QR4 (not supported)
                        if (compressed > 1)
                        {
                            // no compression or non-rle compression not
                            // supported

                            throw djvError(
                                djvIffPlugin::staticName,
                                djvImageIo::errorLabels()[
                                    djvImageIo::ERROR_UNSUPPORTED].
                                arg(io.fileName()));
                        }

                        // Get compressed.
                        *compression = compressed;

                        // Set XY.
                        if (tbhdsize == 32)
                        {
                            io.getU32(&data.x, 1);
                            io.getU32(&data.y, 1);
                        }
                        else
                        {
                            data.x = 0;
                            data.y = 0;
                        }

                        // Test format.
                        if (flags & 0x00000003)
                        {
                            // Test if grayscale is set, if throw assert.
                            DJV_ASSERT(!(flags & 0x00000010));
                            bool alpha = false;

                            // Test for RGB channels.
                            if (flags & 0x00000001)
                            {
                                data.pixelChannels = 3;
                            }

                            // Test for Alpha channel.
                            if (flags & 0x00000002)
                            {
                                data.pixelChannels++;
                                alpha = true;
                            }

                            // Test pixel bits.
                            if (flags & 0x00002000)
                            {
                                data.pixelBits = 16; // 12bit represented as
                                                      // 16bit

                                if (!alpha)
                                {
                                    info.pixel = djvPixel::RGB_U16;
                                }
                                else
                                {
                                    info.pixel = djvPixel::RGBA_U16;
                                }
                            }
                            else
                            {
                                if (!bytes)
                                {
                                    data.pixelBits = 8; // 8bit

                                    if (!alpha)
                                    {
                                        info.pixel = djvPixel::RGB_U8;
                                    }
                                    else
                                    {
                                        info.pixel = djvPixel::RGBA_U8;
                                    }
                                }
                                else
                                {
                                    data.pixelBits = 16; // 16bit

                                    if (!alpha)
                                    {
                                        info.pixel = djvPixel::RGB_U16;
                                    }
                                    else
                                    {
                                        info.pixel = djvPixel::RGBA_U16;
                                    }
                                }
                            }

                            // Test bits.
                            const int bits =
                                data.pixelChannels * data.pixelBits;
                            DJV_ASSERT (
                                bits == (djvPixel::channels(info.pixel) *
                                    data.pixelBits) &&
                                (bits % data.pixelBits) == 0);
                        }

                        // Z format.
                        else if (flags & 0x00000004)
                        {
                            data.pixelChannels = 1;
                            data.pixelBits = 32; // 32bit

                            // NOTE: Z_F32 support - not supported
                            // info->pixel = PIXEL(djvPixel::Z_F32);

                            DJV_ASSERT(bytes == 0);
                        }

                        // Set data.
                        _data = data;

                        // TBHD done, break.
                        break;
                    }

                    // Skip to the next block.
                    io.seek(chunksize);
                }

                // Test if supported else skip to next block.
                if (_data.width > 0 &&
                        _data.height > 0 &&
                        _data.pixelBits > 0 &&
                        _data.pixelChannels > 0)
                {
                    debug();
                    break;
                }

            }
        }

        // Skip to the next block.
        io.seek(chunksize);
    }
}


void Header::save(djvFileIo & io, const djvImageIoInfo & info, bool compression)
    throw (djvError)
{
    //DJV_DEBUG("Header::save");

    // FOR4 <size> CIMG    - 4 + 4 + 4 = 16 bytes
    // TBHD <size> <data>  - 4 + 4 + 32 = 40 bytes

    quint32 length = 0;
    quint32 flags  = 0;

    // Information.

    const int channels = djvPixel::channels(info.pixel);
    int bytes = 0;

    _data.width = info.size.x;
    _data.height = info.size.y;
    _data.pixelChannels = channels;
    _data.pixelBits = 1 == channels ? (1 == info.size.y ? 1 : 2) : 3;

    // Write.

    // 'FOR4' type
    io.setU8('F');
    io.setU8('O');
    io.setU8('R');
    io.setU8('4');

    // 'FOR4' length
    // NOTE: only reserved for now.
    io.setU32(length);

    // 'CIMG' type
    io.setU8('C');
    io.setU8('I');
    io.setU8('M');
    io.setU8('G');

    // 'TBHD' type
    io.setU8('T');
    io.setU8('B');
    io.setU8('H');
    io.setU8('D');

    // 'TBHD' length
    length = 32;
    io.setU32(length);

    // Set data.
    // NOTE: 32 bytes.

    // Set width and height.
    io.setU32(_data.width);
    io.setU32(_data.height);

    // Set prnum and prden.
    io.setU16(1);
    io.setU16(1);

    // Set flags.
    switch (info.pixel)
    {
        case djvPixel::RGB_U8:
            // RGB 8.
            flags = 0x00000001;
            bytes = 0;
            break;

        case djvPixel::RGBA_U8:
            // RGBA 8.
            flags = 0x00000003;
            bytes = 0;
            break;

        case djvPixel::RGB_U16:
            // RGB 16.
            flags = 0x00000001;
            bytes = 1;
            break;

        case djvPixel::RGBA_U16:
            // RGBA 16.
            flags = 0x00000003;
            bytes = 1;
            break;

        default:
            break;
    }

    io.setU32(flags);
    io.setU16(bytes);

    // Set tiles.
    djvVector2i size = djvIffPlugin::tileSize(_data.width, _data.height);
    io.setU16(size.x * size.y);

    // Set compressed.
    // 0 no compression
    // 1 RLE compression
    // 2 QRL (not supported)
    // 3 QR4 (not supported)
    if (!compression)
    {
        io.setU32(0);
    }
    else
    {
        io.setU32(1);
    }

    // Set x and y.
    io.setU32(0);
    io.setU32(0);
}

void Header::debug() const
{
    //DJV_DEBUG("Header::debug");
    //DJV_DEBUG_PRINT("x = " << _data.x);
    //DJV_DEBUG_PRINT("y = " << _data.y);
    //DJV_DEBUG_PRINT("width = " << _data.width);
    //DJV_DEBUG_PRINT("height = " << _data.height);
    //DJV_DEBUG_PRINT("pixelBits = " << _data.pixelBits);
    //DJV_DEBUG_PRINT("pixelChannels = " << _data.pixelChannels);
    //DJV_DEBUG_PRINT("tiles = " << _data.tiles);
}

} // namespace

void djvIffPlugin::loadInfo(
    djvFileIo &      io,
    djvImageIoInfo & info,
    int *            tiles,
    bool *           compression) throw (djvError)
{
    Header header;
    header.load(io, info, tiles, compression);
}

void djvIffPlugin::saveInfo(djvFileIo & io, const djvImageIoInfo & info, bool compression)
    throw (djvError)
{
    Header header;
    header.save(io, info, compression);
}

int djvIffPlugin::readRle(
    const quint8 * in,
    quint8 *       out,
    int            size)
{
    //DJV_DEBUG("djvIffPlugin::readRle");
    //DJV_DEBUG_PRINT("size = " << size);

    const quint8 * const _in = in;
    const quint8 * const end = out + size;

    while (out < end)
    {
        // Information.

        const quint8 count = (*in & 0x7f) + 1;
        const bool run = (*in & 0x80) ? true : false;
        ++in;

        // Find runs.

        if (!run)
        {
            // Verbatim.

            for (int i = 0; i < count; i++)
            {
                *out++ = *in++;
            }
        }
        else
        {
            // Duplicate.

            const quint8 p = *in++;

            for (int i = 0; i < count; i++)
            {
                *out++ = p;
            }
        }
    }

    const int r = in - _in;
    //DJV_DEBUG_PRINT("tile r = " << r);
    return r;
}

namespace
{

void saveVerbatim (
    const quint8 *& in,
    quint8 *&       out,
    int             size)
{
    int count = 1;
    unsigned char byte = 0;

    // Two in a row or count.

    for (; count < size; ++count)
    {
        if (in[count - 1] == in[count])
        {
            if (byte == in[count - 1])
            {
                count -= 2;
                break;
            }
        }

        byte = in[count - 1];
    }

    // Information.

    *out++ = count - 1;
    djvMemory::copy(in, out, count);

    out += count;
    in += count;
}

void saveDuplicate (
    const quint8 *& in,
    quint8 *&       out,
    int             size)
{
    int count = 1;

    for (; count < size; ++count)
    {
        if (in[count - 1] != in[count])
        {
            break;
        }
    }

    const bool run = count > 1;
    const int length = run ? 1 : count;

    // Information.

    *out++ = ((count - 1) & 0x7f) | (run << 7);
    *out = *in;

    out += length;
    in += count;
}

} // namespace

int djvIffPlugin::writeRle (
    const quint8 * in,
    quint8 *       out,
    int            size)
{
    //DJV_DEBUG("djvIffPlugin::aveRle");
    //DJV_DEBUG_PRINT("size = " << size);

    const quint8 * const _out = out;
    const quint8 * const end = in + size;

    while (in < end)
    {
        // Find runs.

        const int max = djvMath::min(0x7f + 1, static_cast<int>(end - in));

        if (max > 0)
        {
            if (in[0] != in[1])
            {
                // Verbatim.

                saveVerbatim (in, out, max);
            }
            else
            {
                // Duplicate.

                saveDuplicate (in, out, max);
            }
        }
    }

    const int r = out - _out;
    //DJV_DEBUG_PRINT("tile r = " << r);
    return r;
}

quint32 djvIffPlugin::alignSize (quint32 size, quint32 alignment)
{
    quint32 mod = size % alignment;

    if (mod)
    {
        mod = alignment - mod;
        size += mod;
    }

    return size;
}

djvVector2i djvIffPlugin::tileSize(quint32 width, quint32 height)
{
    quint32 tw = djvIffPlugin::tileWidth(), th = djvIffPlugin::tileHeight();
    djvVector2i size = djvVector2i(
        (width + tw - 1) / tw,
        (height + th - 1) / th);
    return (size);
}

int djvIffPlugin::tileWidth()
{
    static int tile_w = 64;
    return tile_w;
}

int djvIffPlugin::tileHeight()
{
    static int tile_h = 64;
    return tile_h;
}

const QStringList & djvIffPlugin::optionsLabels()
{
    static const QStringList data = QStringList() <<
        "Compression";

    DJV_ASSERT(data.count() == OPTIONS_COUNT);

    return data;
}

djvPlugin * djvIffPlugin::copyPlugin() const
{
    djvIffPlugin * plugin = new djvIffPlugin;
    
    plugin->_options = _options;
    
    return plugin;
}

QString djvIffPlugin::pluginName() const
{
    return staticName;
}

QStringList djvIffPlugin::extensions() const
{
    return QStringList() <<
        ".iff" <<
        ".z";
}

QStringList djvIffPlugin::option(const QString & in) const
{
    QStringList out;

    if (0 == in.compare(options()[COMPRESSION_OPTION], Qt::CaseInsensitive))
    {
        out << _options.compression;
    }

    return out;
}

bool djvIffPlugin::setOption(const QString & in, QStringList & data)
{
    try
    {
        if (0 == in.compare(options()[COMPRESSION_OPTION], Qt::CaseInsensitive))
        {
            COMPRESSION compression = static_cast<COMPRESSION>(0);
            
            data >> compression;
            
            if (compression != _options.compression)
            {
                _options.compression = compression;
                
                Q_EMIT optionChanged(in);
            }
        }
    }
    catch (const QString &)
    {
        return false;
    }

    return true;
}

QStringList djvIffPlugin::options() const
{
    return optionsLabels();
}

void djvIffPlugin::commandLine(QStringList & in) throw (QString)
{
    QStringList tmp;
    QString     arg;

    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            if ("-iff_compression" == arg)
            {
                in >> _options.compression;
            }
            else
            {
                tmp << arg;
            }
        }
    }
    catch (const QString &)
    {
        throw arg;
    }

    in = tmp;
}

QString djvIffPlugin::commandLineHelp() const
{
    return QString(
"\n"
"IFF Options\n"
"\n"
"    -iff_compression (value)\n"
"        Set the file compression used when saving IFF images. Options = %1. "
"Default = %2.\n").
    arg(djvIffPlugin::compressionLabels().join(", ")).
    arg(djvStringUtil::label(_options.compression).join(", "));
}

djvImageLoad * djvIffPlugin::createLoad() const
{
    return new djvIffLoad;
}

djvImageSave * djvIffPlugin::createSave() const
{
    return new djvIffSave(_options);
}

djvAbstractPrefsWidget * djvIffPlugin::createWidget()
{
    return new djvIffWidget(this);
}

//------------------------------------------------------------------------------

_DJV_STRING_OPERATOR_LABEL(djvIffPlugin::COMPRESSION,
    djvIffPlugin::compressionLabels())
