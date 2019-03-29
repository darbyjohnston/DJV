//------------------------------------------------------------------------------
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

#include <djvAV/CineonHeader.h>

#include <djvCore/FileIO.h>
#include <djvCore/Time.h>

namespace djv
{
    namespace AV
    {
        const quint32 CineonHeader::magic[] =
        {
            0x802a5fd7,
            0xd75f2a80
        };

        const float CineonHeader::minSpeed = .000001f;

        CineonHeader::CineonHeader()
        {
            Core::Memory::fill<quint8>(0xff, &file, sizeof(File));
            zero(file.version, 8);
            zero(file.name, 100);
            zero(file.time, 24);

            Core::Memory::fill<quint8>(0xff, &image, sizeof(Image));

            for (uint i = 0; i < 8; ++i)
            {
                zero(&image.channel[i].lowData);
                zero(&image.channel[i].lowQuantity);
                zero(&image.channel[i].highData);
                zero(&image.channel[i].highQuantity);
            }

            Core::Memory::fill<quint8>(0xff, &source, sizeof(Source));
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

            Core::Memory::fill<quint8>(0xff, &film, sizeof(Film));
            zero(film.format, 32);
            zero(&film.frameRate);
            zero(film.frameId, 32);
            zero(film.slate, 200);
        }

        void CineonHeader::load(Core::FileIO & io, IOInfo & info, bool & filmPrint)
        {
            //DJV_DEBUG("CineonHeader::load");

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
                throw Core::Error(
                    Cineon::staticName,
                    IOPlugin::errorLabels()[IOPlugin::ERROR_UNRECOGNIZED]);
            }
            io.get(&image, sizeof(Image));
            io.get(&source, sizeof(Source));
            io.get(&film, sizeof(Film));
            if (endian)
            {
                //DJV_DEBUG_PRINT("endian");
                io.setEndian(true);
                this->endian();
                info.layers[0].endian = Core::Memory::endianOpposite(Core::Memory::endian());
            }

            // Information.
            if (file.imageOffset)
            {
                io.setPos(file.imageOffset);
            }
            switch (image.orient)
            {
            case ORIENT_LEFT_RIGHT_TOP_BOTTOM:
                info.layers[0].mirror.y = true;
                break;
            case ORIENT_LEFT_RIGHT_BOTTOM_TOP:
                break;
            case ORIENT_RIGHT_LEFT_TOP_BOTTOM:
                info.layers[0].mirror.x = true;
                break;
            case ORIENT_RIGHT_LEFT_BOTTOM_TOP:
                info.layers[0].mirror.x = true;
                info.layers[0].mirror.y = true;
                break;
            case ORIENT_TOP_BOTTOM_LEFT_RIGHT:
            case ORIENT_TOP_BOTTOM_RIGHT_LEFT:
            case ORIENT_BOTTOM_TOP_LEFT_RIGHT:
            case ORIENT_BOTTOM_TOP_RIGHT_LEFT:
                break;
            }
            if (!image.channels)
            {
                throw Core::Error(
                    Cineon::staticName,
                    IOPlugin::errorLabels()[IOPlugin::ERROR_UNSUPPORTED]);
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
                throw Core::Error(
                    Cineon::staticName,
                    IOPlugin::errorLabels()[IOPlugin::ERROR_UNSUPPORTED]);
            }

            int pixel = -1;
            switch (image.channels)
            {
            case 3:
                switch (image.channel[0].bitDepth)
                {
                case 10:
                    pixel = Pixel::RGB_U10;
                    break;
                }
                break;
            }
            if (-1 == pixel)
            {
                throw Core::Error(
                    Cineon::staticName,
                    IOPlugin::errorLabels()[IOPlugin::ERROR_UNSUPPORTED]);
            }

            info.layers[0].pixel = Pixel::PIXEL(pixel);
            info.layers[0].size = glm::ivec2(
                image.channel[0].size[0],
                image.channel[0].size[1]);

            if (isValid(&image.linePadding) && image.linePadding)
            {
                throw Core::Error(
                    Cineon::staticName,
                    IOPlugin::errorLabels()[IOPlugin::ERROR_UNSUPPORTED]);
            }

            if (isValid(&image.channelPadding) && image.channelPadding)
            {
                throw Core::Error(
                    Cineon::staticName,
                    IOPlugin::errorLabels()[IOPlugin::ERROR_UNSUPPORTED]);
            }

            filmPrint = DESCRIPTOR_R_FILM_PRINT == image.channel[0].descriptor[1];

            // File tags.
            const QStringList & tags = Tags::tagLabels();
            const QStringList & cineonTags = Cineon::tagLabels();
            if (isValid(file.time, 24))
            {
                info.tags[tags[Tags::TIME]] = toString(file.time, 24);
            }

            // Source tags.
            if (isValid(&source.offset[0]) && isValid(&source.offset[1]))
                info.tags[cineonTags[Cineon::TAG_SOURCE_OFFSET]] = (QStringList() <<
                    QString::number(source.offset[0]) <<
                    QString::number(source.offset[1])).join(" ");
            if (isValid(source.file, 100))
            {
                info.tags[cineonTags[Cineon::TAG_SOURCE_FILE]] =
                    toString(source.file, 100);
            }
            if (isValid(source.time, 24))
            {
                info.tags[cineonTags[Cineon::TAG_SOURCE_TIME]] =
                    toString(source.time, 24);
            }
            if (isValid(source.inputDevice, 64))
                info.tags[cineonTags[Cineon::TAG_SOURCE_INPUT_DEVICE]] =
                toString(source.inputDevice, 64);
            if (isValid(source.inputModel, 32))
                info.tags[cineonTags[Cineon::TAG_SOURCE_INPUT_MODEL]] =
                toString(source.inputModel, 32);
            if (isValid(source.inputSerial, 32))
                info.tags[cineonTags[Cineon::TAG_SOURCE_INPUT_SERIAL]] =
                toString(source.inputSerial, 32);
            if (isValid(&source.inputPitch[0]) && isValid(&source.inputPitch[1]))
                info.tags[cineonTags[Cineon::TAG_SOURCE_INPUT_PITCH]] = (QStringList() <<
                    QString::number(source.inputPitch[0]) <<
                    QString::number(source.inputPitch[1])).join(" ");
            if (isValid(&source.gamma))
                info.tags[cineonTags[Cineon::TAG_SOURCE_GAMMA]] =
                QString::number(source.gamma);

            // Film tags.
            if (isValid(&film.id) &&
                isValid(&film.type) &&
                isValid(&film.offset) &&
                isValid(&film.prefix) &&
                isValid(&film.count))
            {
                info.tags[tags[Tags::KEYCODE]] = Core::Time::keycodeToString(
                    film.id, film.type, film.prefix, film.count, film.offset);
            }
            if (isValid(film.format, 32))
            {
                info.tags[cineonTags[Cineon::TAG_FILM_FORMAT]] =
                    toString(film.format, 32);
            }
            if (isValid(&film.frame))
            {
                info.tags[cineonTags[Cineon::TAG_FILM_FRAME]] =
                    QString::number(film.frame);
            }
            if (isValid(&film.frameRate) && film.frameRate >= minSpeed)
            {
                info.sequence.speed = Core::Speed::floatToSpeed(film.frameRate);

                info.tags[cineonTags[Cineon::TAG_FILM_FRAME_RATE]] =
                    QString::number(film.frameRate);
            }
            if (isValid(film.frameId, 32))
            {
                info.tags[cineonTags[Cineon::TAG_FILM_FRAME_ID]] =
                    toString(film.frameId, 32);
            }
            if (isValid(film.slate, 200))
            {
                info.tags[cineonTags[Cineon::TAG_FILM_SLATE]] =
                    toString(film.slate, 200);
            }

            debug();
        }

        void CineonHeader::save(Core::FileIO & io, const IOInfo & info, Cineon::COLOR_PROFILE colorProfile)
        {
            //DJV_DEBUG("CineonHeader::save");

            // Information.
            file.imageOffset = 2048;
            file.headerSize = 1024;
            file.industryHeaderSize = 1024;
            file.userHeaderSize = 0;

            image.orient = ORIENT_LEFT_RIGHT_TOP_BOTTOM;

            image.channels = 3;

            if (Cineon::COLOR_PROFILE_FILM_PRINT == colorProfile)
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
                image.channel[i].size[0] = info.layers[0].size.x;
                image.channel[i].size[1] = info.layers[0].size.y;

                image.channel[i].lowData = 0;

                switch (bitDepth)
                {
                case 8:
                    image.channel[i].highData = Pixel::u8Max;
                    break;
                case 10:
                    image.channel[i].highData = Pixel::u10Max;
                    break;
                case 12:
                    image.channel[i].highData = Pixel::u12Max;
                    break;
                case 16:
                    image.channel[i].highData = Pixel::u16Max;
                    break;
                }
            }

            image.interleave = 0;
            image.packing = 5;
            image.dataSign = 0;
            image.dataSense = 0;
            image.linePadding = 0;
            image.channelPadding = 0;

            // File tags.
            const QStringList & tags = Tags::tagLabels();
            const QStringList & cineonTags = Cineon::tagLabels();
            QString tmp;
            Core::StringUtil::cString(info.layers[0].fileName, file.name, 100, false);
            Core::StringUtil::cString(info.tags[tags[Tags::TIME]], file.time, 24, false);

            // Source tags.
            tmp = info.tags[cineonTags[Cineon::TAG_SOURCE_OFFSET]];
            if (tmp.length())
            {
                const QStringList list = tmp.split(' ', QString::SkipEmptyParts);

                if (2 == list.count())
                {
                    source.offset[0] = list[0].toInt();
                    source.offset[1] = list[1].toInt();
                }
            }
            tmp = info.tags[cineonTags[Cineon::TAG_SOURCE_FILE]];
            if (tmp.length())
            {
                Core::StringUtil::cString(tmp, source.file, 100, false);
            }
            tmp = info.tags[cineonTags[Cineon::TAG_SOURCE_TIME]];
            if (tmp.length())
            {
                Core::StringUtil::cString(tmp, source.time, 24, false);
            }
            tmp = info.tags[cineonTags[Cineon::TAG_SOURCE_INPUT_DEVICE]];
            if (tmp.length())
            {
                Core::StringUtil::cString(tmp, source.inputDevice, 64, false);
            }
            tmp = info.tags[cineonTags[Cineon::TAG_SOURCE_INPUT_MODEL]];
            if (tmp.length())
            {
                Core::StringUtil::cString(tmp, source.inputModel, 32, false);
            }
            tmp = info.tags[cineonTags[Cineon::TAG_SOURCE_INPUT_SERIAL]];
            if (tmp.length())
            {
                Core::StringUtil::cString(tmp, source.inputSerial, 32, false);
            }
            tmp = info.tags[cineonTags[Cineon::TAG_SOURCE_INPUT_PITCH]];
            if (tmp.length())
            {
                const QStringList list = tmp.split(' ', QString::SkipEmptyParts);
                if (2 == list.count())
                {
                    source.offset[0] = list[0].toInt();
                    source.offset[1] = list[1].toInt();
                }
            }
            tmp = info.tags[cineonTags[Cineon::TAG_SOURCE_GAMMA]];
            if (tmp.length())
            {
                source.gamma = static_cast<float>(tmp.toDouble());
            }

            // Film tags.
            tmp = info.tags[tags[Tags::KEYCODE]];
            if (tmp.length())
            {
                int id = 0, type = 0, prefix = 0, count = 0, offset = 0;
                Core::Time::stringToKeycode(tmp, id, type, prefix, count, offset);
                film.id = id;
                film.type = type;
                film.offset = offset;
                film.prefix = prefix;
                film.count = count;
            }
            tmp = info.tags[cineonTags[Cineon::TAG_FILM_FORMAT]];
            if (tmp.length())
            {
                Core::StringUtil::cString(tmp, film.format, 32, false);
            }
            tmp = info.tags[cineonTags[Cineon::TAG_FILM_FRAME]];
            if (tmp.length())
            {
                film.frame = tmp.toInt();
            }
            tmp = info.tags[cineonTags[Cineon::TAG_FILM_FRAME_RATE]];
            if (tmp.length())
            {
                film.frameRate = static_cast<float>(tmp.toDouble());
            }
            tmp = info.tags[cineonTags[Cineon::TAG_FILM_FRAME_ID]];
            if (tmp.length())
            {
                Core::StringUtil::cString(tmp, film.frameId, 32, false);
            }
            tmp = info.tags[cineonTags[Cineon::TAG_FILM_SLATE]];
            if (tmp.length())
            {
                Core::StringUtil::cString(tmp, film.slate, 200, false);
            }

            // Write.
            debug();
            const bool endian = Core::Memory::endian() != Core::Memory::MSB;
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

        void CineonHeader::saveEnd(Core::FileIO & io)
        {
            const quint32 size = static_cast<quint32>(io.pos());
            io.setPos(20);
            io.setU32(size);
        }

        void CineonHeader::endian()
        {
            Core::Memory::convertEndian(&file.imageOffset, 1, 4);
            Core::Memory::convertEndian(&file.headerSize, 1, 4);
            Core::Memory::convertEndian(&file.industryHeaderSize, 1, 4);
            Core::Memory::convertEndian(&file.userHeaderSize, 1, 4);
            Core::Memory::convertEndian(&file.size, 1, 4);

            for (uint i = 0; i < 8; ++i)
            {
                Core::Memory::convertEndian(image.channel[i].size, 2, 4);
                Core::Memory::convertEndian(&image.channel[i].lowData, 1, 4);
                Core::Memory::convertEndian(&image.channel[i].lowQuantity, 1, 4);
                Core::Memory::convertEndian(&image.channel[i].highData, 1, 4);
                Core::Memory::convertEndian(&image.channel[i].highQuantity, 1, 4);
            }

            Core::Memory::convertEndian(image.white, 2, 4);
            Core::Memory::convertEndian(image.red, 2, 4);
            Core::Memory::convertEndian(image.green, 2, 4);
            Core::Memory::convertEndian(image.blue, 2, 4);
            Core::Memory::convertEndian(&image.linePadding, 1, 4);
            Core::Memory::convertEndian(&image.channelPadding, 1, 4);

            Core::Memory::convertEndian(source.offset, 2, 4);
            Core::Memory::convertEndian(source.inputPitch, 2, 4);
            Core::Memory::convertEndian(&source.gamma, 1, 4);

            Core::Memory::convertEndian(&film.prefix, 1, 4);
            Core::Memory::convertEndian(&film.count, 1, 4);
            Core::Memory::convertEndian(&film.frame, 1, 4);
            Core::Memory::convertEndian(&film.frameRate, 1, 4);
        }

        void CineonHeader::zero(qint32 * in)
        {
            *((quint32 *)in) = 0x80000000;
        }

        void CineonHeader::zero(float * in)
        {
            *((quint32 *)in) = 0x7F800000;
        }

        void CineonHeader::zero(char * in, int size)
        {
            memset(in, 0, size);
        }

        bool CineonHeader::isValid(const quint8 * in)
        {
            return *in != 0xff;
        }

        bool CineonHeader::isValid(const quint16 * in)
        {
            return *in != 0xffff;
        }

        namespace
        {
            // These constants are used to catch uninitialized values.
            const qint32 _intMax = 1000000;
            const float  _floatMax = 1000000.f;
            const char   _minChar = 32;
            const char   _maxChar = 126;

        } // namespace

        bool CineonHeader::isValid(const quint32 * in)
        {
            return
                *in != 0xffffffff &&
                *in < quint32(_intMax);
        }

        bool CineonHeader::isValid(const qint32 * in)
        {
            return
                *in != qint32(0x80000000) &&
                *in > -_intMax &&
                *in < _intMax;
        }

        bool CineonHeader::isValid(const float * in)
        {
            return
                *((quint32 *)in) != 0x7F800000 &&
                *in > -_floatMax &&
                *in < _floatMax;
        }

        bool CineonHeader::isValid(const char * in, int size)
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

        QString CineonHeader::toString(const char * in, int size)
        {
            const char * p = in;
            const char * const end = p + size;
            for (; *p && p < end; ++p)
                ;
            return QString::fromLatin1(in, p - in);
        }

        QString CineonHeader::debug(quint8 in)
        {
            return isValid(&in) ? QString::number(in) : "[]";
        }

        QString CineonHeader::debug(quint16 in)
        {
            return isValid(&in) ? QString::number(in) : "[]";
        }

        QString CineonHeader::debug(quint32 in)
        {
            return isValid(&in) ? QString::number(in) : "[]";
        }

        QString CineonHeader::debug(qint32 in)
        {
            return isValid(&in) ? QString::number(in) : "[]";
        }

        QString CineonHeader::debug(float in)
        {
            return isValid(&in) ? QString::number(in) : "[]";
        }

        QString CineonHeader::debug(const char * in, int size)
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

        QString CineonHeader::debug() const
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

    } // namespace AV
} // namespace djv
