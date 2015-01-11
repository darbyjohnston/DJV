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

//! \file djvSgiPlugin.cpp

#include <djvSgiPlugin.h>

#include <djvSgiLoad.h>
#include <djvSgiSave.h>
#include <djvSgiWidget.h>

#include <djvAssert.h>
#include <djvError.h>

extern "C"
{

DJV_PLUGIN_EXPORT djvPlugin * djvImageIo()
{
    return new djvSgiPlugin;
}

} // extern "C"

//------------------------------------------------------------------------------
// djvSgiPlugin::Options
//------------------------------------------------------------------------------

djvSgiPlugin::Options::Options() :
    compression(djvSgiPlugin::COMPRESSION_NONE)
{}

//------------------------------------------------------------------------------
// djvSgiPlugin
//------------------------------------------------------------------------------

const QString djvSgiPlugin::staticName = "SGI";

const QStringList & djvSgiPlugin::compressionLabels()
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
    Header();

    void load(djvFileIo &, djvImageIoInfo &, bool * compression) throw (djvError);
    void save(djvFileIo &, const djvImageIoInfo &, bool compression) throw (djvError);

    void debug() const;

private:

    struct Data
    {
        quint16 magic;
        quint8  storage;
        quint8  bytes;
        quint16 dimension;
        quint16 width;
        quint16 height;
        quint16 channels;
        quint32 pixelMin;
        quint32 pixelMax;
    }
        _data;
};

Header::Header()
{
    _data.magic = 474;
    _data.storage = 0;
    _data.bytes = 0;
    _data.dimension = 0;
    _data.width = 0;
    _data.height = 0;
    _data.channels = 0;
    _data.pixelMin = 0;
    _data.pixelMax = 0;
}

void Header::load(djvFileIo & io, djvImageIoInfo & info, bool * compression)
    throw (djvError)
{
    //DJV_DEBUG("Header::load");

    // Read.

    io.getU16(&_data.magic);

    if (_data.magic != 474)
    {
        throw djvError(
            djvSgiPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_UNRECOGNIZED].
            arg(io.fileName()));
    }

    io.getU8(&_data.storage);
    io.getU8(&_data.bytes);
    io.getU16(&_data.dimension);
    io.getU16(&_data.width);
    io.getU16(&_data.height);
    io.getU16(&_data.channels);
    io.getU32(&_data.pixelMin);
    io.getU32(&_data.pixelMax);

    io.setPos(512);

    //DJV_DEBUG_PRINT("bytes = " << _data.bytes);

    // Information.

    info.size = djvVector2i(_data.width, _data.height);
    
    if (! djvPixel::pixel(
        _data.channels,
        1 == _data.bytes ? 8 : 16,
        djvPixel::INTEGER,
        info.pixel))
    {
        throw djvError(
            djvSgiPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_UNSUPPORTED].
            arg(io.fileName()));
    }
    
    //DJV_DEBUG_PRINT("pixel = " << info.pixel);

    info.endian = djvMemory::MSB;

    *compression = _data.storage ? true : false;
}

void Header::save(djvFileIo & io, const djvImageIoInfo & info, bool compression)
    throw (djvError)
{
    //DJV_DEBUG("Header::save");

    // Information.

    const int channels = djvPixel::channels(info.pixel);
    const int bytes    = djvPixel::channelByteCount(info.pixel);

    _data.width = info.size.x;
    _data.height = info.size.y;

    _data.channels = channels;
    _data.bytes = bytes;

    _data.dimension = 1 == channels ? (1 == info.size.y ? 1 : 2) : 3;

    _data.pixelMin = 0;
    _data.pixelMax = 1 == bytes ? 255 : 65535;

    _data.storage = compression;

    // Write.

    io.setU16(_data.magic);
    io.setU8(_data.storage);
    io.setU8(_data.bytes);
    io.setU16(_data.dimension);
    io.setU16(_data.width);
    io.setU16(_data.height);
    io.setU16(_data.channels);
    io.setU32(_data.pixelMin);
    io.setU32(_data.pixelMax);

    io.setPos(512);
}

void Header::debug() const
{
    //DJV_DEBUG("Header::debug");
    //DJV_DEBUG_PRINT("magic = " << _data.magic);
    //DJV_DEBUG_PRINT("storage = " << _data.storage);
    //DJV_DEBUG_PRINT("bytes = " << _data.bytes);
    //DJV_DEBUG_PRINT("dimension = " << _data.dimension);
    //DJV_DEBUG_PRINT("width = " << _data.width);
    //DJV_DEBUG_PRINT("height = " << _data.height);
    //DJV_DEBUG_PRINT("pixel range = " << _data.pixelMin << " " <<
    //    _data.pixelMax);
}

} // namespace

void djvSgiPlugin::loadInfo(djvFileIo & io, djvImageIoInfo & info, bool * compression)
    throw (djvError)
{
    Header header;
    header.load(io, info, compression);
}

void djvSgiPlugin::saveInfo(djvFileIo & io, const djvImageIoInfo & info, bool compression)
    throw (djvError)
{
    Header header;
    header.save(io, info, compression);
}

namespace
{

template<typename T>
bool load(
    const void * in,
    const void * end,
    void *       out,
    int          size,
    bool         endian)
{
    //DJV_DEBUG("load");
    //DJV_DEBUG_PRINT("size = " << size);
    //DJV_DEBUG_PRINT("endian = " << endian);

    const quint64 bytes = sizeof(T);
    
    //DJV_DEBUG_PRINT("bytes = " << bytes);

    const T * inP = reinterpret_cast<const T *>(in);
    T * outP = reinterpret_cast<T *>(out);
    const T * const outEnd = outP + size;

    while (outP < outEnd)
    {
        // Information.

        if (inP > end)
        {
            return false;
        }

        const int  count  = *inP & 0x7f;
        const bool run    = ! (*inP & 0x80);
        const int  length = run ? 1 : count;
        
        //DJV_DEBUG_PRINT("count = " << count);
        //DJV_DEBUG_PRINT("  run = " << run);
        //DJV_DEBUG_PRINT("  length = " << length);

        ++inP;

        // Unpack.

        if (inP + length > end)
        {
            return false;
        }

        if (run)
        {
            if (! endian)
            {
                for (int j = 0; j < count; ++j, ++outP)
                {
                    *outP = *inP;
                }
            }
            else
            {
                djvMemory::convertEndian(inP, outP, 1, bytes);

                if (count > 1)
                {
                    djvMemory::fill<T>(*outP, outP + 1, count - 1);
                }

                outP += count;
            }

            ++inP;
        }
        else
        {
            if (! endian)
            {
                for (int j = 0; j < length; ++j, ++inP, ++outP)
                {
                    *outP = *inP;
                }
            }
            else
            {
                djvMemory::convertEndian(inP, outP, length, bytes);
                
                inP += length;
                outP += length;
            }
        }
    }

    return true;
}

} // namespace

bool djvSgiPlugin::readRle(
    const void * in,
    const void * end,
    void *       out,
    int          size,
    int          bytes,
    bool         endian)
{
    switch (bytes)
    {
        case 1: return load<quint8>(in, end, out, size, false);

        case 2: return load<quint16>(in, end, out, size, endian);
    }

    return false;
}

namespace
{

template<typename T>
quint64 save(
    const void * in,
    void *       out,
    int          size,
    bool         endian)
{
    //DJV_DEBUG("save");
    //DJV_DEBUG_PRINT("size = " << size);
    //DJV_DEBUG_PRINT("endian = " << endian);

    const quint64 bytes = sizeof(T);
    
    //DJV_DEBUG_PRINT("bytes = " << bytes);

    const T * inP = reinterpret_cast<const T *>(in);
    T * outP = reinterpret_cast<T *>(out);
    const T * const end = inP + size;

    while (inP < end)
    {
        // Pixel runs.

        const int min = 3;
        const int max = djvMath::min(0x7f, static_cast<int>(end - inP));
        int count = 1, match = 1;

        for (; count < max; ++count)
        {
            if (inP[count] == inP[count - 1])
            {
                ++match;

                if (min == match && count >= min)
                {
                    count -= min - 1;
                    match = 1;
                    break;
                }
            }
            else
            {
                if (match >= min)
                {
                    break;
                }

                match = 1;
            }
        }

        const bool run    = match > min;
        const int  length = run ? 1 : count;
        
        //DJV_DEBUG_PRINT("count = " << count);
        //DJV_DEBUG_PRINT("  run = " << run);
        //DJV_DEBUG_PRINT("  length = " << length);

        // Information.

        *outP++ = (count & 0x7f) | ((! run) << 7);

        // Pack.

        if (! endian)
        {
            for (int i = 0; i < length; ++i)
            {
                outP[i] = inP[i];
            }
        }
        else
        {
            djvMemory::convertEndian(inP, outP, length, bytes);
        }

        outP += length;
        inP += count;
    }

    // Cap the end.

    *outP++ = 0;

    const quint64 r = (outP - reinterpret_cast<T *>(out)) * bytes;
    
    //DJV_DEBUG_PRINT("r = " << r);
    
    return r;
}

} // namespace

quint64 djvSgiPlugin::writeRle(
    const void * in,
    void *       out,
    int          size,
    int          bytes,
    bool         endian)
{
    switch (bytes)
    {
        case 1: return save<quint8>(in, out, size, false);

        case 2: return save<quint16>(in, out, size, endian);

        default: break;
    }

    return 0;
}

const QStringList & djvSgiPlugin::optionsLabels()
{
    static const QStringList data = QStringList() <<
        "Compression";

    DJV_ASSERT(data.count() == OPTIONS_COUNT);

    return data;
}

djvPlugin * djvSgiPlugin::copyPlugin() const
{
    djvSgiPlugin * plugin = new djvSgiPlugin;
    
    plugin->_options = _options;
    
    return plugin;
}

QString djvSgiPlugin::pluginName() const
{
    return staticName;
}

QStringList djvSgiPlugin::extensions() const
{
    return QStringList() <<
        ".sgi"  <<
        ".rgba" <<
        ".rgb"  <<
        ".bw";
}

QStringList djvSgiPlugin::option(const QString & in) const
{
    QStringList out;

    if (0 == in.compare(options()[COMPRESSION_OPTION], Qt::CaseInsensitive))
    {
        out << _options.compression;
    }

    return out;
}

bool djvSgiPlugin::setOption(const QString & in, QStringList & data)
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

QStringList djvSgiPlugin::options() const
{
    return optionsLabels();
}

void djvSgiPlugin::commandLine(QStringList & in) throw (QString)
{
    QStringList tmp;
    QString     arg;

    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            if ("-sgi_compression" == arg)
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

QString djvSgiPlugin::commandLineHelp() const
{
    return QString(
"\n"
"SGI Options\n"
"\n"
"    -sgi_compression (value)\n"
"        Set the file compression used when saving SGI images. Options = %1. "
"Default = %2.\n").
    arg(djvSgiPlugin::compressionLabels().join(", ")).
    arg(djvStringUtil::label(_options.compression).join(", "));
}

djvImageLoad * djvSgiPlugin::createLoad() const
{
    return new djvSgiLoad;
}

djvImageSave * djvSgiPlugin::createSave() const
{
    return new djvSgiSave(_options);
}

djvAbstractPrefsWidget * djvSgiPlugin::createWidget()
{
    return new djvSgiWidget(this);
}

//------------------------------------------------------------------------------

_DJV_STRING_OPERATOR_LABEL(djvSgiPlugin::COMPRESSION,
    djvSgiPlugin::compressionLabels())
