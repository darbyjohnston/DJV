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

#include <djvGraphics/SGILoad.h>

#include <djvGraphics/Image.h>
#include <djvGraphics/PixelDataUtil.h>

namespace djv
{
    namespace Graphics
    {
        SGILoad::SGILoad(djvCoreContext * context) :
            ImageLoad(context)
        {}

        SGILoad::~SGILoad()
        {}

        void SGILoad::open(const djvFileInfo & in, ImageIOInfo & info)
            throw (djvError)
        {
            //DJV_DEBUG("SGILoad::open");
            //DJV_DEBUG_PRINT("in = " << in);
            _file = in;
            djvFileIO io;
            _open(_file.fileName(_file.sequence().start()), info, io);
            if (djvFileInfo::SEQUENCE == _file.type())
            {
                info.sequence.frames = _file.sequence().frames;
            }
        }

        void SGILoad::read(Image & image, const ImageIOFrameInfo & frame)
            throw (djvError)
        {
            //DJV_DEBUG("SGILoad::read");
            //DJV_DEBUG_PRINT("frame = " << frame);

            image.colorProfile = ColorProfile();
            image.tags = ImageTags();

            // Open the file.
            const QString fileName =
                _file.fileName(frame.frame != -1 ? frame.frame : _file.sequence().start());
            //DJV_DEBUG_PRINT("file name = " << fileName);
            ImageIOInfo info;
            djvFileIO io;
            _open(fileName, info, io);

            // Read the file.
            io.readAhead();
            const quint64 pos = io.pos();
            const quint64 size = io.size() - pos;
            const int     channels = Pixel::channels(info.pixel);
            const int     bytes = Pixel::channelByteCount(info.pixel);
            if (!_compression)
            {
                if (1 == bytes)
                {
                    const quint8 * p = io.mmapP();
                    io.seek(PixelDataUtil::dataByteCount(info));
                    _tmp.set(info, p);
                }
                else
                {
                    if (size != PixelDataUtil::dataByteCount(info))
                    {
                        throw djvError(
                            SGI::staticName,
                            ImageIO::errorLabels()[ImageIO::ERROR_READ]);
                    }
                    _tmp.set(info);
                    io.get(_tmp.data(), size / bytes, bytes);
                }
            }
            else
            {
                _tmp.set(info);
                std::vector<quint8> tmp(size);
                io.get(tmp.data(), size / bytes, bytes);
                const quint8 * inP = tmp.data();
                const quint8 * end = inP + size;
                quint8 *       outP = _tmp.data();
                for (int c = 0; c < channels; ++c)
                {
                    //DJV_DEBUG_PRINT("channel = " << c);
                    for (int y = 0; y < info.size.y; ++y, outP += info.size.x * bytes)
                    {
                        //DJV_DEBUG_PRINT("y = " << y);
                        if (!SGI::readRle(
                            inP + _rleOffset[y + info.size.y * c] - pos,
                            end,
                            outP,
                            info.size.x,
                            bytes,
                            io.endian()))
                        {
                            throw djvError(
                                SGI::staticName,
                                ImageIO::errorLabels()[ImageIO::ERROR_READ]);
                        }
                    }
                }
            }

            // Interleave the image channels.
            info.size = PixelDataUtil::proxyScale(info.size, frame.proxy);
            info.proxy = frame.proxy;
            image.set(info);
            PixelDataUtil::planarInterleave(_tmp, image, frame.proxy);

            //DJV_DEBUG_PRINT("image = " << image);
        }

        void SGILoad::_open(const QString & in, ImageIOInfo & info, djvFileIO & io)
            throw (djvError)
        {
            //DJV_DEBUG("djvSGILoad::_open");
            //DJV_DEBUG_PRINT("in = " << in);

            // Open the file.
            io.setEndian(djvMemory::endian() != djvMemory::MSB);
            io.open(in, djvFileIO::READ);
            info.fileName = in;
            SGI::loadInfo(io, info, &_compression);

            // Read the scanline tables.
            if (_compression)
            {
                const int tableSize = info.size.y * Pixel::channels(info.pixel);
                //DJV_DEBUG_PRINT("rle table size = " << tableSize);
                _rleOffset.resize(tableSize);
                _rleSize.resize(tableSize);
                io.getU32(_rleOffset.data(), tableSize);
                io.getU32(_rleSize.data(), tableSize);
            }
        }

    } // namespace Graphics
} // namespace djv
