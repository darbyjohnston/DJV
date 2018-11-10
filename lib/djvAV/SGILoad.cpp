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

#include <djvAV/SGILoad.h>

#include <djvAV/Image.h>
#include <djvAV/PixelDataUtil.h>

#include <djvCore/CoreContext.h>

namespace djv
{
    namespace AV
    {
        SGILoad::SGILoad(const Core::FileInfo & fileInfo, const QPointer<Core::CoreContext> & context) :
            Load(fileInfo, context)
        {
            Core::FileIO io;
            _open(_fileInfo.fileName(_fileInfo.sequence().start()), _ioInfo, io);
            if (Core::FileInfo::SEQUENCE == _fileInfo.type())
            {
                _ioInfo.sequence.frames = _fileInfo.sequence().frames;
            }
        }

        SGILoad::~SGILoad()
        {}

        void SGILoad::read(Image & image, const ImageIOInfo & frame)
        {
            //DJV_DEBUG("SGILoad::read");
            //DJV_DEBUG_PRINT("frame = " << frame);

            image.colorProfile = ColorProfile();
            image.tags = Tags();

            // Open the file.
            const QString fileName = _fileInfo.fileName(frame.frame != -1 ? frame.frame : _fileInfo.sequence().start());
            //DJV_DEBUG_PRINT("file name = " << fileName);
            IOInfo info;
            Core::FileIO io;
            _open(fileName, info, io);

            // Read the file.
            io.readAhead();
            const quint64 pos = io.pos();
            const quint64 size = io.size() - pos;
            auto pixelDataInfo = info.layers[0];
            const int channels = Pixel::channels(pixelDataInfo.pixel);
            const int bytes = Pixel::channelByteCount(pixelDataInfo.pixel);
            if (!_compression)
            {
                if (1 == bytes)
                {
                    const quint8 * p = io.mmapP();
                    io.seek(PixelDataUtil::dataByteCount(pixelDataInfo));
                    _tmp.set(pixelDataInfo, p);
                }
                else
                {
                    if (size != PixelDataUtil::dataByteCount(pixelDataInfo))
                    {
                        throw Core::Error(
                            SGI::staticName,
                            IOPlugin::errorLabels()[IOPlugin::ERROR_READ]);
                    }
                    _tmp.set(pixelDataInfo);
                    io.get(_tmp.data(), size / bytes, bytes);
                }
            }
            else
            {
                _tmp.set(pixelDataInfo);
                std::vector<quint8> tmp(size);
                io.get(tmp.data(), size / bytes, bytes);
                const quint8 * inP = tmp.data();
                const quint8 * end = inP + size;
                quint8 *       outP = _tmp.data();
                for (int c = 0; c < channels; ++c)
                {
                    //DJV_DEBUG_PRINT("channel = " << c);
                    for (int y = 0; y < pixelDataInfo.size.y; ++y, outP += pixelDataInfo.size.x * bytes)
                    {
                        //DJV_DEBUG_PRINT("y = " << y);
                        if (!SGI::readRle(
                            inP + _rleOffset[y + pixelDataInfo.size.y * c] - pos,
                            end,
                            outP,
                            pixelDataInfo.size.x,
                            bytes,
                            io.endian()))
                        {
                            throw Core::Error(
                                SGI::staticName,
                                IOPlugin::errorLabels()[IOPlugin::ERROR_READ]);
                        }
                    }
                }
            }

            // Interleave the image channels.
            pixelDataInfo.size = PixelDataUtil::proxyScale(pixelDataInfo.size, frame.proxy);
            pixelDataInfo.proxy = frame.proxy;
            image.set(pixelDataInfo);
            PixelDataUtil::planarInterleave(_tmp, image, frame.proxy);

            //DJV_DEBUG_PRINT("image = " << image);
        }

        void SGILoad::_open(const QString & in, IOInfo & info, Core::FileIO & io)
        {
            //DJV_DEBUG("SGILoad::_open");
            //DJV_DEBUG_PRINT("in = " << in);

            // Open the file.
            io.setEndian(Core::Memory::endian() != Core::Memory::MSB);
            io.open(in, Core::FileIO::READ);
            info.layers[0].fileName = in;
            SGI::loadInfo(io, info, &_compression);

            // Read the scanline tables.
            if (_compression)
            {
                const int tableSize = info.layers[0].size.y * Pixel::channels(info.layers[0].pixel);
                //DJV_DEBUG_PRINT("rle table size = " << tableSize);
                _rleOffset.resize(tableSize);
                _rleSize.resize(tableSize);
                io.getU32(_rleOffset.data(), tableSize);
                io.getU32(_rleSize.data(), tableSize);
            }
        }

    } // namespace AV
} // namespace djv
