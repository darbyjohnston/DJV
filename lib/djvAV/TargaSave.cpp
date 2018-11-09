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

#include <djvAV/TargaSave.h>

#include <djvAV/OpenGLImage.h>

#include <djvCore/CoreContext.h>

namespace djv
{
    namespace AV
    {
        TargaSave::TargaSave(const Core::FileInfo & fileInfo, const ImageIOInfo & imageIOInfo, const Targa::Options & options, const QPointer<Core::CoreContext> & context) :
            ImageSave(fileInfo, imageIOInfo, context),
            _options(options)
        {
            if (_imageIOInfo.sequence.frames.count() > 1)
            {
                _fileInfo.setType(Core::FileInfo::SEQUENCE);
            }
            _info = PixelDataInfo();
            _info.size = _imageIOInfo.size;
            _info.pixel = Pixel::pixel(Pixel::format(_imageIOInfo.pixel), Pixel::U8);
            switch (Pixel::format(_info.pixel))
            {
            case Pixel::RGB:
            case Pixel::RGBA: _info.bgr = true; break;
            default: break;
            }
            _info.endian = Core::Memory::LSB;
            //DJV_DEBUG_PRINT("info = " << _info);
            _image.set(_info);
        }

        TargaSave::~TargaSave()
        {}

        void TargaSave::write(const Image & in, const ImageIOFrameInfo & frame)
        {
            //DJV_DEBUG("TargaSave::write");
            //DJV_DEBUG_PRINT("in = " << in);

            // Open the file.
            const QString fileName = _fileInfo.fileName(frame.frame);
            Core::FileIO io;
            io.setEndian(Core::Memory::endian() != Core::Memory::LSB);
            io.open(fileName, Core::FileIO::WRITE);
            Targa::saveInfo(
                io,
                _info,
                _options.compression != Targa::COMPRESSION_NONE);

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
            if (!_options.compression)
            {
                io.set(p->data(), p->dataByteCount());
            }
            else
            {
                const int w = p->w(), h = p->h();
                const int channels = Pixel::channels(p->info().pixel);
                std::vector<quint8> scanline(w * channels * 2);
                for (int y = 0; y < h; ++y)
                {
                    const quint64 size = Targa::writeRle(p->data(0, y), scanline.data(), w, channels);
                    io.set(scanline.data(), size);
                }
            }
        }

    } // namespace AV
} // namespace djv
