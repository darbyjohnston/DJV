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

#include <djvAV/SGISave.h>

#include <djvAV/OpenGLImage.h>
#include <djvAV/PixelDataUtil.h>

#include <djvCore/CoreContext.h>

namespace djv
{
    namespace AV
    {
        SGISave::SGISave(const Core::FileInfo & fileInfo, const IOInfo & ioInfo, const SGI::Options & options, const QPointer<Core::CoreContext> & context) :
            Save(fileInfo, ioInfo, context),
            _options(options)
        {
            if (_ioInfo.sequence.frames.count() > 1)
            {
                _fileInfo.setType(Core::FileInfo::SEQUENCE);
            }
            _info = PixelDataInfo();
            _info.size = _ioInfo.layers[0].size;
            Pixel::TYPE type = Pixel::type(_ioInfo.layers[0].pixel);
            switch (type)
            {
            case Pixel::U10:
            case Pixel::F16:
            case Pixel::F32: type = Pixel::U16; break;
            default: break;
            }
            _info.pixel = Pixel::pixel(Pixel::format(_ioInfo.layers[0].pixel), type);
            _info.endian = Core::Memory::MSB;
            //DJV_DEBUG_PRINT("info = " << _info);
            _image.set(_info);
        }

        SGISave::~SGISave()
        {}

        void SGISave::write(const Image & in, const ImageIOInfo & frame)
        {
            //DJV_DEBUG("SGISave::write");
            //DJV_DEBUG_PRINT("in = " << in);
            //DJV_DEBUG_PRINT("compression = " << _options.compression);

            // Open the file.
            const QString fileName = _fileInfo.fileName(frame.frame);
            Core::FileIO io;
            io.setEndian(Core::Memory::endian() != Core::Memory::MSB);
            io.open(fileName, Core::FileIO::WRITE);
            SGI::saveInfo(
                io,
                _info,
                _options.compression != SGI::COMPRESSION_NONE);

            // Setup the scanline tables.
            if (_options.compression)
            {
                const int tableSize = _info.size.y * Pixel::channels(_info.pixel);
                //DJV_DEBUG_PRINT("rle table size = " << tableSize);
                _rleOffset.resize(tableSize);
                _rleSize.resize(tableSize);
                io.seek(tableSize * 2 * 4);
            }

            // Convert the image.
            const PixelData * p = &in;
            if (in.info() != _info)
            {
                //DJV_DEBUG_PRINT("convert = " << _image);
                _image.zero();
                OpenGLImage().copy(in, _image);
                p = &_image;
            }
            _tmp.set(p->info());
            const int w = _tmp.w();
            const int h = _tmp.h();
            const int channels = Pixel::channels(_tmp.pixel());
            const int bytes = Pixel::channelByteCount(_tmp.pixel());

            // Deinterleave the image channels.
            PixelDataUtil::planarDeinterleave(*p, _tmp);

            // Write the file.
            if (!_options.compression)
            {
                io.set(_tmp.data(), _tmp.dataByteCount() / bytes, bytes);
            }
            else
            {
                std::vector<quint8> scanline(w * bytes * 2);
                for (int c = 0; c < channels; ++c)
                {
                    for (int y = 0; y < h; ++y)
                    {
                        const quint64 size = SGI::writeRle(
                            _tmp.data() + (c * h + y) * w * bytes,
                            scanline.data(),
                            w,
                            bytes,
                            io.endian());
                        _rleOffset[y + c * h] = quint32(io.pos());
                        _rleSize[y + c * h] = quint32(size);
                        io.set(scanline.data(), size / bytes, bytes);
                    }
                }
                io.setPos(512);
                io.setU32(_rleOffset.data(), h * channels);
                io.setU32(_rleSize.data(), h * channels);
            }
        }

    } // namespace AV
} // namespace djv
