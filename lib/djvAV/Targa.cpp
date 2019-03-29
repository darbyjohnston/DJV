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

#include <djvAV/Targa.h>

#include <djvCore/Assert.h>
#include <djvCore/Error.h>

#include <QCoreApplication>

namespace djv
{
    namespace AV
    {
        const QString Targa::staticName = "Targa";

        const QStringList & Targa::compressionLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::Targa", "None") <<
                qApp->translate("djv::AV::Targa", "RLE");
            DJV_ASSERT(data.count() == COMPRESSION_COUNT);
            return data;
        }

        namespace
        {

            struct Header
            {
                Header();

                void load(Core::FileIO &, IOInfo &, bool * compression);
                void save(Core::FileIO &, const IOInfo &, bool compression);

                void debug() const;

            private:
                struct Data
                {
                    quint8  idSize;
                    quint8  cmapType;
                    quint8  imageType;
                    quint16 cmapStart;
                    quint16 cmapSize;
                    quint8  cmapBits;
                    quint16 x;
                    quint16 y;
                    quint16 width;
                    quint16 height;
                    quint8  pixelBits;
                    quint8  descriptor;
                }
                _data;
            };

            Header::Header()
            {
                _data.idSize = 0;
                _data.cmapType = 0;
                _data.imageType = 0;
                _data.cmapStart = 0;
                _data.cmapSize = 0;
                _data.cmapBits = 0;
                _data.x = 0;
                _data.y = 0;
                _data.width = 0;
                _data.height = 0;
                _data.pixelBits = 0;
                _data.descriptor = 0;
            }

            void Header::load(Core::FileIO & io, IOInfo & info, bool * compression)
            {
                //DJV_DEBUG("Header::load");

                // Read.
                io.getU8(&_data.idSize);
                io.getU8(&_data.cmapType);
                io.getU8(&_data.imageType);
                io.getU16(&_data.cmapStart);
                io.getU16(&_data.cmapSize);
                io.getU8(&_data.cmapBits);
                io.getU16(&_data.x);
                io.getU16(&_data.y);
                io.getU16(&_data.width);
                io.getU16(&_data.height);
                io.getU8(&_data.pixelBits);
                io.getU8(&_data.descriptor);
                io.seek(_data.idSize);

                // Information.
                info.layers[0].size = glm::ivec2(_data.width, _data.height);
                info.layers[0].mirror.x = (_data.descriptor >> 4) & 1;
                info.layers[0].mirror.y = (_data.descriptor >> 5) & 1;
                const int alphaBits = _data.descriptor & 15;
                //DJV_DEBUG_PRINT("alpha bits = " << alphaBits);
                switch (alphaBits)
                {
                case 0:
                case 8: break;
                default:
                    throw Core::Error(
                        Targa::staticName,
                        IOPlugin::errorLabels()[IOPlugin::ERROR_UNSUPPORTED]);
                }
                int pixel = -1;
                switch (_data.imageType)
                {
                case 3:
                case 11:
                    switch (_data.pixelBits)
                    {
                    case 8:  pixel = Pixel::L_U8; break;
                    case 16: pixel = Pixel::LA_U8; break;
                    }
                    break;
                case 2:
                case 10:
                    switch (_data.pixelBits)
                    {
                    case 24:
                        pixel = Pixel::RGB_U8;
                        info.layers[0].bgr = true;
                        break;
                    case 32:
                        pixel = Pixel::RGBA_U8;
                        info.layers[0].bgr = true;
                        break;
                    }
                    break;
                }
                if (-1 == pixel)
                {
                    throw Core::Error(
                        Targa::staticName,
                        IOPlugin::errorLabels()[IOPlugin::ERROR_UNSUPPORTED]);
                }
                info.layers[0].pixel = static_cast<Pixel::PIXEL>(pixel);
                //DJV_DEBUG_PRINT("pixel = " << info.pixel);
                const int bits = _data.pixelBits + alphaBits;
                if (bits < (Pixel::channels(info.layers[0].pixel) * 8) || (bits % 8) != 0)
                {
                    throw Core::Error(
                        Targa::staticName,
                        IOPlugin::errorLabels()[IOPlugin::ERROR_UNSUPPORTED]);
                }
                *compression =
                    10 == _data.imageType ||
                    11 == _data.imageType;
                //DJV_DEBUG_PRINT("compression = " << compression);
                info.layers[0].endian = Core::Memory::LSB;
            }

            void Header::save(Core::FileIO & io, const IOInfo & info, bool compression)
            {
                //DJV_DEBUG("Header::save");

                // Information.
                _data.width = info.layers[0].size.x;
                _data.height = info.layers[0].size.y;
                const int channels = Pixel::channels(info.layers[0].pixel);
                _data.pixelBits = channels * 8;
                switch (channels)
                {
                case 1:
                case 2: _data.imageType = 3; break;
                case 3:
                case 4: _data.imageType = 2; break;
                }
                if (compression)
                {
                    _data.imageType += 8;
                }
                int alphaBits = 0;
                switch (Pixel::format(info.layers[0].pixel))
                {
                case Pixel::LA:
                case Pixel::RGBA: alphaBits = 8; break;
                default: break;
                }
                const int orient = 0;
                _data.descriptor = (alphaBits & 15) | ((orient & 3) << 4);

                // Write.
                io.setU8(_data.idSize);
                io.setU8(_data.cmapType);
                io.setU8(_data.imageType);
                io.setU16(_data.cmapStart);
                io.setU16(_data.cmapSize);
                io.setU8(_data.cmapBits);
                io.setU16(_data.x);
                io.setU16(_data.y);
                io.setU16(_data.width);
                io.setU16(_data.height);
                io.setU8(_data.pixelBits);
                io.setU8(_data.descriptor);
            }

            void Header::debug() const
            {
                //DJV_DEBUG("Header::debug");
                //DJV_DEBUG_PRINT("id size = " << _data.idSize);
                //DJV_DEBUG_PRINT("cmap type = " << _data.cmapType);
                //DJV_DEBUG_PRINT("image type = " << _data.imageType);
                //DJV_DEBUG_PRINT("cmap start = " << _data.cmapStart);
                //DJV_DEBUG_PRINT("cmap size = " << _data.cmapSize);
                //DJV_DEBUG_PRINT("cmap bits = " << _data.cmapBits);
                //DJV_DEBUG_PRINT("image position = " <<_data. x << " " << _data.y);
                //DJV_DEBUG_PRINT("image size = " << _data.width << " " << _data.height);
                //DJV_DEBUG_PRINT("pixel bits = " << _data.pixelBits);
            }

        } // namespace

        void Targa::loadInfo(Core::FileIO & io, IOInfo & info, bool * compression)
        {
            Header header;
            header.load(io, info, compression);
        }

        void Targa::saveInfo(Core::FileIO & io, const IOInfo & info, bool compression)
        {
            Header header;
            header.save(io, info, compression);
        }

        const quint8 * Targa::readRle(
            const quint8 * in,
            const quint8 * end,
            quint8 *       out,
            int            size,
            int            channels)
        {
            //DJV_DEBUG("readRle");
            //DJV_DEBUG_PRINT("size = " << size);
            //DJV_DEBUG_PRINT("channels = " << channels);
            const quint8 * const outEnd = out + size * channels;
            while (out < outEnd)
            {
                // Information.
                if (in > end)
                {
                    return 0;
                }
                const quint8 count = (*in & 0x7f) + 1;
                const bool   run = (*in & 0x80) ? true : false;
                const int    length = run ? 1 : count;
                //DJV_DEBUG_PRINT("length = " << length);
                ++in;

                // Unpack.
                if (in + length * channels > end)
                {
                    return 0;
                }
                if (run)
                {
                    for (int j = 0; j < count; ++j, out += channels)
                    {
                        switch (channels)
                        {
                        case 4: out[3] = in[3];
                        case 3: out[2] = in[2];
                        case 2: out[1] = in[1];
                        case 1: out[0] = in[0]; break;
                        }
                    }
                    in += channels;
                }
                else
                {
                    for (int j = 0; j < count; ++j, in += channels, out += channels)
                    {
                        switch (channels)
                        {
                        case 4: out[3] = in[3];
                        case 3: out[2] = in[2];
                        case 2: out[1] = in[1];
                        case 1: out[0] = in[0]; break;
                        }
                    }
                }
            }
            return in;
        }

        quint64 Targa::writeRle(
            const quint8 * in,
            quint8 *       out,
            int            size,
            int            channels)
        {
            //DJV_DEBUG("Targa::writeRle");
            //DJV_DEBUG_PRINT("size = " << size);
            //DJV_DEBUG_PRINT("channels = " << channels);

            const quint8 * const _out = out;
            const quint8 * const end = in + size * channels;
            while (in < end)
            {
                // Find runs.
                const int max = Core::Math::min(0x7f + 1, static_cast<int>(end - in) / channels);
                int count = 1;
                for (; count < max; ++count)
                {
                    int c = 0;
                    for (; c < channels; ++c)
                    {
                        if (in[count * channels + c] != in[(count - 1) * channels + c])
                        {
                            break;
                        }
                    }
                    if (c < channels)
                    {
                        break;
                    }
                }
                const bool run = count > 1;
                const int  length = run ? 1 : count;
                //DJV_DEBUG_PRINT("count = " << count);
                //DJV_DEBUG_PRINT("  run = " << run);
                //DJV_DEBUG_PRINT("  length = " << length);

                // Information.
                *out++ = ((count - 1) & 0x7f) | (run << 7);

                // Pack.
                for (int j = 0; j < length * channels; j += channels)
                {
                    switch (channels)
                    {
                    case 4: out[j + 3] = in[j + 3];
                    case 3: out[j + 2] = in[j + 2];
                    case 2: out[j + 1] = in[j + 1];
                    case 1: out[j] = in[j]; break;
                    }
                }
                out += length * channels;
                in += count * channels;
            }

            const quint64 r = out - _out;
            //DJV_DEBUG_PRINT("return = " << static_cast<int>(r));
            return r;
        }

        const QStringList & Targa::optionsLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::Targa", "Compression");
            DJV_ASSERT(data.count() == OPTIONS_COUNT);
            return data;
        }

    } // namespace AV

    _DJV_STRING_OPERATOR_LABEL(AV::Targa::COMPRESSION, AV::Targa::compressionLabels())

} // namespace djv
