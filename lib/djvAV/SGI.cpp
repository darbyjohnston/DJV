//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvAV/SGI.h>

#include <djvCore/Assert.h>
#include <djvCore/Error.h>

#include <QCoreApplication>

namespace djv
{
    namespace AV
    {
        const QString SGI::staticName = "SGI";

        const QStringList & SGI::compressionLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::SGI", "None") <<
                qApp->translate("djv::AV::SGI", "RLE");
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

            void Header::load(Core::FileIO & io, IOInfo & info, bool * compression)
            {
                //DJV_DEBUG("Header::load");

                // Read.
                io.getU16(&_data.magic);
                if (_data.magic != 474)
                {
                    throw Core::Error(
                        SGI::staticName,
                        IOPlugin::errorLabels()[IOPlugin::ERROR_UNRECOGNIZED]);
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
                info.layers[0].size = glm::ivec2(_data.width, _data.height);
                if (!Pixel::intPixel(_data.channels, 1 == _data.bytes ? 8 : 16, info.layers[0].pixel))
                {
                    throw Core::Error(
                        SGI::staticName,
                        IOPlugin::errorLabels()[IOPlugin::ERROR_UNSUPPORTED]);
                }
                //DJV_DEBUG_PRINT("pixel = " << info.pixel);
                info.layers[0].endian = Core::Memory::MSB;

                *compression = _data.storage ? true : false;
            }

            void Header::save(Core::FileIO & io, const IOInfo & info, bool compression)
            {
                //DJV_DEBUG("Header::save");

                // Information.
                const int channels = Pixel::channels(info.layers[0].pixel);
                const int bytes = Pixel::channelByteCount(info.layers[0].pixel);
                _data.width = info.layers[0].size.x;
                _data.height = info.layers[0].size.y;
                _data.channels = channels;
                _data.bytes = bytes;
                _data.dimension = 1 == channels ? (1 == info.layers[0].size.y ? 1 : 2) : 3;
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

        void SGI::loadInfo(Core::FileIO & io, IOInfo & info, bool * compression)
        {
            Header header;
            header.load(io, info, compression);
        }

        void SGI::saveInfo(Core::FileIO & io, const IOInfo & info, bool compression)
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
                    const int  count = *inP & 0x7f;
                    const bool run = !(*inP & 0x80);
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
                        if (!endian)
                        {
                            for (int j = 0; j < count; ++j, ++outP)
                            {
                                *outP = *inP;
                            }
                        }
                        else
                        {
                            Core::Memory::convertEndian(inP, outP, 1, bytes);
                            Core::Memory::convertEndian(inP, outP, 1, bytes);
                            if (count > 1)
                            {
                                Core::Memory::fill<T>(*outP, outP + 1, count - 1);
                            }
                            outP += count;
                        }
                        ++inP;
                    }
                    else
                    {
                        if (!endian)
                        {
                            for (int j = 0; j < length; ++j, ++inP, ++outP)
                            {
                                *outP = *inP;
                            }
                        }
                        else
                        {
                            Core::Memory::convertEndian(inP, outP, length, bytes);
                            inP += length;
                            outP += length;
                        }
                    }
                }
                return true;
            }

        } // namespace

        bool SGI::readRle(
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
                    const int max = Core::Math::min(0x7f, static_cast<int>(end - inP));
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
                    const bool run = match > min;
                    const int  length = run ? 1 : count;
                    //DJV_DEBUG_PRINT("count = " << count);
                    //DJV_DEBUG_PRINT("  run = " << run);
                    //DJV_DEBUG_PRINT("  length = " << length);

                    // Information.
                    *outP++ = (count & 0x7f) | ((!run) << 7);

                    // Pack.
                    if (!endian)
                    {
                        for (int i = 0; i < length; ++i)
                        {
                            outP[i] = inP[i];
                        }
                    }
                    else
                    {
                        Core::Memory::convertEndian(inP, outP, length, bytes);
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

        quint64 SGI::writeRle(
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

        const QStringList & SGI::optionsLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::SGI", "Compression");
            DJV_ASSERT(data.count() == OPTIONS_COUNT);
            return data;
        }

    } // namespace AV

    _DJV_STRING_OPERATOR_LABEL(AV::SGI::COMPRESSION, AV::SGI::compressionLabels())

} // namespace djv
