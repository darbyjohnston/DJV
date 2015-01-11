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

//! \file djvCineonHeader.cpp

#include <djvCineonHeader.h>

#include <djvFileIo.h>
#include <djvTime.h>

//------------------------------------------------------------------------------
// djvCineonHeader
//------------------------------------------------------------------------------

const quint32 djvCineonHeader::magic [] =
{
    0x802a5fd7,
    0xd75f2a80
};

const float djvCineonHeader::minSpeed = 0.000001f;

djvCineonHeader::djvCineonHeader()
{
    djvMemory::fill<quint8>(0xff, &file, sizeof(File));
    zero(file.version, 8);
    zero(file.name, 100);
    zero(file.time, 24);

    djvMemory::fill<quint8>(0xff, &image, sizeof(Image));

    for (uint i = 0; i < 8; ++i)
    {
        zero(&image.channel[i].lowData);
        zero(&image.channel[i].lowQuantity);
        zero(&image.channel[i].highData);
        zero(&image.channel[i].highQuantity);
    }

    djvMemory::fill<quint8>(0xff, &source, sizeof(Source));
    zero(&source.offset[0]);
    zero(&source.offset[1]);
    zero(source.file, 100);
    zero(source.time, 24);
    zero(source.inputDevice, 64);
    zero(source.inputModel, 32);
    zero(source.inputSerial, 32);
    zero(&source.inputPitch[0]);
    zero(&source.inputPitch[1]);
    zero(&source.gamma);

    djvMemory::fill<quint8>(0xff, &film, sizeof(Film));
    zero(film.format, 32);
    zero(&film.frameRate);
    zero(film.frameId, 32);
    zero(film.slate, 200);
}

void djvCineonHeader::load(
    djvFileIo &      io,
    djvImageIoInfo & info,
    bool &           filmPrint) throw (djvError)
{

    //DJV_DEBUG("djvCineonHeader::load");

    // Read.

    io.get(&file, sizeof(File));

    bool endian = false;

    if (magic[0] == file.magic)
        ;
    else if (magic[1] == file.magic)
    {
        endian = true;
    }
    else
    {
        throw djvError(
            djvCineon::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_UNRECOGNIZED].
            arg(io.fileName()));
    }

    io.get(&image, sizeof(Image));
    io.get(&source, sizeof(Source));
    io.get(&film, sizeof(Film));

    if (endian)
    {
        //DJV_DEBUG_PRINT("endian");
        
        io.setEndian(true);
        
        this->endian();
        
        info.endian = djvMemory::endianOpposite(djvMemory::endian());
    }

    // Information.

    if (file.imageOffset)
    {
        io.setPos(file.imageOffset);
    }

    switch (image.orient)
    {
        case ORIENT_LEFT_RIGHT_TOP_BOTTOM:
            info.mirror.y = true;
            break;

        case ORIENT_LEFT_RIGHT_BOTTOM_TOP:
            break;

        case ORIENT_RIGHT_LEFT_TOP_BOTTOM:
            info.mirror.x = true;
            break;

        case ORIENT_RIGHT_LEFT_BOTTOM_TOP:
            info.mirror.x = true;
            info.mirror.y = true;
            break;

        case ORIENT_TOP_BOTTOM_LEFT_RIGHT:
        case ORIENT_TOP_BOTTOM_RIGHT_LEFT:
        case ORIENT_BOTTOM_TOP_LEFT_RIGHT:
        case ORIENT_BOTTOM_TOP_RIGHT_LEFT:
            break;
    }

    if (! image.channels)
    {
        throw djvError(
            djvCineon::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_UNSUPPORTED].
            arg(io.fileName()));
    }

    int i = 1;

    for (; i < image.channels; ++i)
    {
        if ((image.channel[i].size[0] != image.channel[0].size[0]) ||
            (image.channel[i].size[1] != image.channel[0].size[1]))
        {
            break;
        }

        if (image.channel[i].bitDepth != image.channel[0].bitDepth)
        {
            break;
        }
    }

    if (i < image.channels)
    {
        throw djvError(
            djvCineon::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_UNSUPPORTED].
            arg(io.fileName()));
    }

    int pixel = -1;

    switch (image.channels)
    {
        case 3:
            switch (image.channel[0].bitDepth)
            {
                case 10:
                    pixel = djvPixel::RGB_U10;
                    break;
            }

            break;
    }

    if (-1 == pixel)
    {
        throw djvError(
            djvCineon::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_UNSUPPORTED].
            arg(io.fileName()));
    }

    info.pixel = djvPixel::PIXEL(pixel);

    info.size = djvVector2i(
        image.channel[0].size[0],
        image.channel[0].size[1]);

    if (isValid(&image.linePadding) && image.linePadding)
    {
        throw djvError(
            djvCineon::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_UNSUPPORTED].
            arg(io.fileName()));
    }

    if (isValid(&image.channelPadding) && image.channelPadding)
    {
        throw djvError(
            djvCineon::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_UNSUPPORTED].
            arg(io.fileName()));
    }

    filmPrint = DESCRIPTOR_R_FILM_PRINT == image.channel[0].descriptor[1];

    // File image tags.

    const QStringList & tags = djvImageTags::tagLabels();
    const QStringList & cineonTags = djvCineon::tagLabels();

    if (isValid(file.time, 24))
    {
        info.tags[tags[djvImageTags::TIME]] = toString(file.time, 24);
    }

    // Source image tags.

    if (isValid(&source.offset[0]) && isValid(&source.offset[1]))
        info.tags[cineonTags[djvCineon::TAG_SOURCE_OFFSET]] = (QStringList() <<
            QString::number(source.offset[0]) <<
            QString::number(source.offset[1])).join(" ");

    if (isValid(source.file, 100))
    {
        info.tags[cineonTags[djvCineon::TAG_SOURCE_FILE]] =
            toString(source.file, 100);
    }

    if (isValid(source.time, 24))
    {
        info.tags[cineonTags[djvCineon::TAG_SOURCE_TIME]] =
            toString(source.time, 24);
    }

    if (isValid(source.inputDevice, 64))
        info.tags[cineonTags[djvCineon::TAG_SOURCE_INPUT_DEVICE]] =
            toString(source.inputDevice, 64);

    if (isValid(source.inputModel, 32))
        info.tags[cineonTags[djvCineon::TAG_SOURCE_INPUT_MODEL]] =
            toString(source.inputModel, 32);

    if (isValid(source.inputSerial, 32))
        info.tags[cineonTags[djvCineon::TAG_SOURCE_INPUT_SERIAL]] =
            toString(source.inputSerial, 32);

    if (isValid(&source.inputPitch[0]) && isValid(&source.inputPitch[1]))
        info.tags[cineonTags[djvCineon::TAG_SOURCE_INPUT_PITCH]] = (QStringList() <<
            QString::number(source.inputPitch[0]) <<
            QString::number(source.inputPitch[1])).join(" ");

    if (isValid(&source.gamma))
        info.tags[cineonTags[djvCineon::TAG_SOURCE_GAMMA]] =
            QString::number(source.gamma);

    // Film image tags.

    if (isValid(&film.id) &&
        isValid(&film.type) &&
        isValid(&film.offset) &&
        isValid(&film.prefix) &&
        isValid(&film.count))
    {
        info.tags[tags[djvImageTags::KEYCODE]] = djvTime::keycodeToString(
            film.id, film.type, film.prefix, film.count, film.offset);
    }

    if (isValid(film.format, 32))
    {
        info.tags[cineonTags[djvCineon::TAG_FILM_FORMAT]] =
            toString(film.format, 32);
    }

    if (isValid(&film.frame))
    {
        info.tags[cineonTags[djvCineon::TAG_FILM_FRAME]] =
            QString::number(film.frame);
    }
    
    if (isValid(&film.frameRate) && film.frameRate >= minSpeed)
    {
        info.sequence.speed = djvSpeed::floatToSpeed(film.frameRate);

        info.tags[cineonTags[djvCineon::TAG_FILM_FRAME_RATE]] =
            QString::number(film.frameRate);
    }

    if (isValid(film.frameId, 32))
    {
        info.tags[cineonTags[djvCineon::TAG_FILM_FRAME_ID]] =
            toString(film.frameId, 32);
    }

    if (isValid(film.slate, 200))
    {
        info.tags[cineonTags[djvCineon::TAG_FILM_SLATE]] =
            toString(film.slate, 200);
    }

    debug();
}

void djvCineonHeader::save(
    djvFileIo &              io,
    const djvImageIoInfo &   info,
    djvCineon::COLOR_PROFILE colorProfile) throw (djvError)
{

    //DJV_DEBUG("djvCineonHeader::save");

    // Information.

    file.imageOffset = 2048;
    file.headerSize = 1024;
    file.industryHeaderSize = 1024;
    file.userHeaderSize = 0;

    image.orient = ORIENT_LEFT_RIGHT_TOP_BOTTOM;

    image.channels = 3;

    if (djvCineon::COLOR_PROFILE_FILM_PRINT == colorProfile)
    {
        image.channel[0].descriptor[1] = DESCRIPTOR_R_FILM_PRINT;
        image.channel[1].descriptor[1] = DESCRIPTOR_G_FILM_PRINT;
        image.channel[2].descriptor[1] = DESCRIPTOR_B_FILM_PRINT;
    }
    else
    {
        image.channel[0].descriptor[1] = DESCRIPTOR_L;
        image.channel[1].descriptor[1] = DESCRIPTOR_L;
        image.channel[2].descriptor[1] = DESCRIPTOR_L;
    }

    const int bitDepth = 10;

    for (int i = 0; i < image.channels; ++i)
    {
        image.channel[i].descriptor[0] = 0;
        image.channel[i].bitDepth = bitDepth;
        image.channel[i].size[0] = info.size.x;
        image.channel[i].size[1] = info.size.y;

        image.channel[i].lowData = 0;

        switch (bitDepth)
        {
            case 8:
                image.channel[i].highData = djvPixel::u8Max;
                break;

            case 10:
                image.channel[i].highData = djvPixel::u10Max;
                break;

            case 12:
                image.channel[i].highData = djvPixel::u12Max;
                break;

            case 16:
                image.channel[i].highData = djvPixel::u16Max;
                break;
        }
    }

    image.interleave = 0;
    image.packing = 5;
    image.dataSign = 0;
    image.dataSense = 0;
    image.linePadding = 0;
    image.channelPadding = 0;

    // File image tags.

    const QStringList & tags = djvImageTags::tagLabels();
    const QStringList & cineonTags = djvCineon::tagLabels();
    QString tmp;

    djvStringUtil::cString(info.fileName, file.name, 100, false);
    djvStringUtil::cString(info.tags[tags[djvImageTags::TIME]], file.time, 24, false);

    // Source image tags.

    tmp = info.tags[cineonTags[djvCineon::TAG_SOURCE_OFFSET]];

    if (tmp.length())
    {
        const QStringList list = tmp.split(' ', QString::SkipEmptyParts);

        if (2 == list.count())
        {
            source.offset[0] = list[0].toInt();
            source.offset[1] = list[1].toInt();
        }
    }

    tmp = info.tags[cineonTags[djvCineon::TAG_SOURCE_FILE]];

    if (tmp.length())
    {
        djvStringUtil::cString(tmp, source.file, 100, false);
    }

    tmp = info.tags[cineonTags[djvCineon::TAG_SOURCE_TIME]];

    if (tmp.length())
    {
        djvStringUtil::cString(tmp, source.time, 24, false);
    }

    tmp = info.tags[cineonTags[djvCineon::TAG_SOURCE_INPUT_DEVICE]];

    if (tmp.length())
    {
        djvStringUtil::cString(tmp, source.inputDevice, 64, false);
    }

    tmp = info.tags[cineonTags[djvCineon::TAG_SOURCE_INPUT_MODEL]];

    if (tmp.length())
    {
        djvStringUtil::cString(tmp, source.inputModel, 32, false);
    }

    tmp = info.tags[cineonTags[djvCineon::TAG_SOURCE_INPUT_SERIAL]];

    if (tmp.length())
    {
        djvStringUtil::cString(tmp, source.inputSerial, 32, false);
    }

    tmp = info.tags[cineonTags[djvCineon::TAG_SOURCE_INPUT_PITCH]];

    if (tmp.length())
    {
        const QStringList list = tmp.split(' ', QString::SkipEmptyParts);

        if (2 == list.count())
        {
            source.offset[0] = list[0].toInt();
            source.offset[1] = list[1].toInt();
        }
    }

    tmp = info.tags[cineonTags[djvCineon::TAG_SOURCE_GAMMA]];

    if (tmp.length())
    {
        source.gamma = static_cast<float>(tmp.toDouble());
    }

    // Film image tags.

    tmp = info.tags[tags[djvImageTags::KEYCODE]];

    if (tmp.length())
    {
        int id = 0, type = 0, prefix = 0, count = 0, offset = 0;
        djvTime::stringToKeycode(tmp, id, type, prefix, count, offset);
        film.id = id;
        film.type = type;
        film.offset = offset;
        film.prefix = prefix;
        film.count = count;
    }

    tmp = info.tags[cineonTags[djvCineon::TAG_FILM_FORMAT]];

    if (tmp.length())
    {
        djvStringUtil::cString(tmp, film.format, 32, false);
    }

    tmp = info.tags[cineonTags[djvCineon::TAG_FILM_FRAME]];

    if (tmp.length())
    {
        film.frame = tmp.toInt();
    }

    tmp = info.tags[cineonTags[djvCineon::TAG_FILM_FRAME_RATE]];

    if (tmp.length())
    {
        film.frameRate = static_cast<float>(tmp.toDouble());
    }

    tmp = info.tags[cineonTags[djvCineon::TAG_FILM_FRAME_ID]];

    if (tmp.length())
    {
        djvStringUtil::cString(tmp, film.frameId, 32, false);
    }

    tmp = info.tags[cineonTags[djvCineon::TAG_FILM_SLATE]];

    if (tmp.length())
    {
        djvStringUtil::cString(tmp, film.slate, 200, false);
    }

    // Write.

    debug();

    const bool endian = djvMemory::endian() != djvMemory::MSB;
    
    io.setEndian(endian);

    if (endian)
    {
        //DJV_DEBUG_PRINT("endian");
        this->endian();
        file.magic = magic[1];
    }
    else
    {
        file.magic = magic[0];
    }

    io.set(&file, sizeof(File));
    io.set(&image, sizeof(Image));
    io.set(&source, sizeof(Source));
    io.set(&film, sizeof(Film));
}

void djvCineonHeader::saveEnd(djvFileIo & io) throw (djvError)
{
    const quint32 size = static_cast<quint32>(io.pos());
    io.setPos(20);
    io.setU32(size);
}

void djvCineonHeader::endian()
{
    djvMemory::convertEndian(&file.imageOffset, 1, 4);
    djvMemory::convertEndian(&file.headerSize, 1, 4);
    djvMemory::convertEndian(&file.industryHeaderSize, 1, 4);
    djvMemory::convertEndian(&file.userHeaderSize, 1, 4);
    djvMemory::convertEndian(&file.size, 1, 4);

    for (uint i = 0; i < 8; ++i)
    {
        djvMemory::convertEndian(image.channel[i].size, 2, 4);
        djvMemory::convertEndian(&image.channel[i].lowData, 1, 4);
        djvMemory::convertEndian(&image.channel[i].lowQuantity, 1, 4);
        djvMemory::convertEndian(&image.channel[i].highData, 1, 4);
        djvMemory::convertEndian(&image.channel[i].highQuantity, 1, 4);
    }

    djvMemory::convertEndian(image.white, 2, 4);
    djvMemory::convertEndian(image.red, 2, 4);
    djvMemory::convertEndian(image.green, 2, 4);
    djvMemory::convertEndian(image.blue, 2, 4);
    djvMemory::convertEndian(&image.linePadding, 1, 4);
    djvMemory::convertEndian(&image.channelPadding, 1, 4);

    djvMemory::convertEndian(source.offset, 2, 4);
    djvMemory::convertEndian(source.inputPitch, 2, 4);
    djvMemory::convertEndian(&source.gamma, 1, 4);

    djvMemory::convertEndian(&film.prefix, 1, 4);
    djvMemory::convertEndian(&film.count, 1, 4);
    djvMemory::convertEndian(&film.frame, 1, 4);
    djvMemory::convertEndian(&film.frameRate, 1, 4);
}

void djvCineonHeader::zero(qint32 * in)
{
    *((quint32 *)in) = 0x80000000;
}

void djvCineonHeader::zero(float * in)
{
    *((quint32 *)in) = 0x7F800000;
}

void djvCineonHeader::zero(char * in, int size)
{
    djvMemory::zero(in, size);
}

bool djvCineonHeader::isValid(const quint8 * in)
{
    return *in != 0xff;
}

bool djvCineonHeader::isValid(const quint16 * in)
{
    return *in != 0xffff;
}

namespace
{

// These constants are used to catch uninitialized values.

const qint32 _intMax   = 1000000;
const float  _floatMax = 1000000.0;
const char   _minChar  = 32;
const char   _maxChar  = 126;

} // namespace

bool djvCineonHeader::isValid(const quint32 * in)
{
    return
        *in != 0xffffffff &&
        *in < quint32(_intMax);
}

bool djvCineonHeader::isValid(const qint32 * in)
{
    return
        *in != qint32(0x80000000) &&
        *in > -_intMax &&
        *in < _intMax;
}

bool djvCineonHeader::isValid(const float * in)
{
    return
        *((quint32 *)in) != 0x7F800000 &&
        *in > -_floatMax &&
        *in < _floatMax;
}

bool djvCineonHeader::isValid(const char * in, int size)
{
    const char * p = in;
    const char * const end = p + size;

    for (; *p && p < end; ++p)
    {
        if (*p < _minChar || *p > _maxChar)
        {
            return false;
        }
    }

    return size ? (in[0] != 0) : false;
}

QString djvCineonHeader::toString(const char * in, int size)
{
    const char * p = in;
    const char * const end = p + size;

    for (; *p && p < end; ++p)
        ;

    return QString::fromLatin1(in, p - in);
}

QString djvCineonHeader::debug(quint8 in)
{
    return isValid(&in) ? QString::number(in) : "[]";
}

QString djvCineonHeader::debug(quint16 in)
{
    return isValid(&in) ? QString::number(in) : "[]";
}

QString djvCineonHeader::debug(quint32 in)
{
    return isValid(&in) ? QString::number(in) : "[]";
}

QString djvCineonHeader::debug(qint32 in)
{
    return isValid(&in) ? QString::number(in) : "[]";
}

QString djvCineonHeader::debug(float in)
{
    return isValid(&in) ? QString::number(in) : "[]";
}

QString djvCineonHeader::debug(const char * in, int size)
{
    return isValid(in, size) ? toString(in, size) : "[]";
}

namespace
{

const QString debugFile =
    "File\n"
    "  Image offset = %1\n"
    "  Header size = %2\n"
    "  Industry header size = %3\n"
    "  User header size = %4\n"
    "  Size = %5\n"
    "  Version = %6\n"
    "  Name = %7\n"
    "  Time = %8\n";

const QString debugImage =
    "Image\n"
    "  Orient = %1\n"
    "  Channels = %2\n"
    "  White = %3 %4\n"
    "  Red = %5 %6\n"
    "  Green = %7 %8\n"
    "  Blue = %9 %10\n"
    "  Label = %11\n"
    "  Interleave = %12\n"
    "  Packing = %13\n"
    "  Data sign = %14\n"
    "  Data sense = %15\n"
    "  Line padding = %16\n"
    "  Channel padding = %17\n";

const QString debugImageChannel =
    "Image Channel #%1\n"
    "  Descriptor = %2 %3\n"
    "  Bit depth = %4\n"
    "  Size = %5 %6\n"
    "  Low data = %7\n"
    "  Low quantity = %8\n"
    "  High data = %9\n"
    "  High quantity = %10\n";

const QString debugSource =
    "Source\n"
    "  Offset = %1 %2\n"
    "  File = %3\n"
    "  Time = %4\n"
    "  Input device = %5\n"
    "  Input model = %6\n"
    "  Input serial = %7\n"
    "  Input pitch = %8 %9\n"
    "  Gamma = %10\n";

const QString debugFilm =
    "Film\n"
    "  Id = %1\n"
    "  Type = %2\n"
    "  Offset = %3\n"
    "  Prefix = %4\n"
    "  Count = %5\n"
    "  Format = %6\n"
    "  Frame = %7\n"
    "  Frame rate = %8\n"
    "  Frame ID = %9\n"
    "  Slate = %10\n";

} // namespace

QString djvCineonHeader::debug() const
{
    QString out;

    out += QString(debugFile).
        arg(debug(file.imageOffset)).
        arg(debug(file.headerSize)).
        arg(debug(file.industryHeaderSize)).
        arg(debug(file.userHeaderSize)).
        arg(debug(file.size)).
        arg(debug(file.version, 8)).
        arg(debug(file.name, 100)).
        arg(debug(file.time, 24));

    out += QString(debugImage).
        arg(debug(image.orient)).
        arg(debug(image.channels)).
        arg(debug(image.white[0])).
        arg(debug(image.white[1])).
        arg(debug(image.red[0])).
        arg(debug(image.red[1])).
        arg(debug(image.green[0])).
        arg(debug(image.green[1])).
        arg(debug(image.blue[0])).
        arg(debug(image.blue[1])).
        arg(debug(image.label, 200)).
        arg(debug(image.interleave)).
        arg(debug(image.packing)).
        arg(debug(image.dataSign)).
        arg(debug(image.dataSense)).
        arg(debug(image.linePadding)).
        arg(debug(image.channelPadding));

    for (int i = 0; i < image.channels; ++i)
        out += QString(debugImageChannel).
            arg(i).
            arg(debug(image.channel[i].descriptor[0])).
            arg(debug(image.channel[i].descriptor[1])).
            arg(debug(image.channel[i].bitDepth)).
            arg(debug(image.channel[i].size[0])).
            arg(debug(image.channel[i].size[1])).
            arg(debug(image.channel[i].lowData)).
            arg(debug(image.channel[i].lowQuantity)).
            arg(debug(image.channel[i].highData)).
            arg(debug(image.channel[i].highQuantity));

    out += QString(debugSource).
        arg(debug(source.offset[0])).
        arg(debug(source.offset[1])).
        arg(debug(source.file, 100)).
        arg(debug(source.time, 24)).
        arg(debug(source.inputDevice, 64)).
        arg(debug(source.inputModel, 32)).
        arg(debug(source.inputSerial, 32)).
        arg(debug(source.inputPitch[0])).
        arg(debug(source.inputPitch[1])).
        arg(debug(source.gamma));

    out += QString(debugFilm).
        arg(debug(film.id)).
        arg(debug(film.type)).
        arg(debug(film.offset)).
        arg(debug(film.prefix)).
        arg(debug(film.count)).
        arg(debug(film.format, 32)).
        arg(debug(film.frame)).
        arg(debug(film.frameRate)).
        arg(debug(film.frameId, 32)).
        arg(debug(film.slate, 200));

    return out;
}
