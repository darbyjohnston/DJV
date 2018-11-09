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

#include <djvAV/PPMSave.h>

#include <djvAV/OpenGLImage.h>

#include <djvCore/CoreContext.h>

#include <stdio.h>

namespace djv
{
    namespace AV
    {
        PPMSave::PPMSave(const Core::FileInfo & fileInfo, const ImageIOInfo & imageIOInfo, const PPM::Options & options, const QPointer<Core::CoreContext> & context) :
            ImageSave(fileInfo, imageIOInfo, context),
            _options(options),
            _bitDepth(0)
        {
            //DJV_DEBUG("PPMSave::PPMSave");
            //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);

            if (_imageIOInfo.sequence.frames.count() > 1)
            {
                _fileInfo.setType(Core::FileInfo::SEQUENCE);
            }

            _info = PixelDataInfo();
            _info.size = _imageIOInfo.size;
            _info.mirror.y = true;

            switch (_options.type)
            {
            case PPM::TYPE_AUTO:
            {
                Pixel::FORMAT format = Pixel::format(_imageIOInfo.pixel);
                switch (format)
                {
                case Pixel::LA:   format = Pixel::L;   break;
                case Pixel::RGBA: format = Pixel::RGB; break;
                default: break;
                }
                Pixel::TYPE type = Pixel::type(_imageIOInfo.pixel);
                switch (type)
                {
                case Pixel::U10:
                case Pixel::F16:
                case Pixel::F32: type = Pixel::U16; break;
                default: break;
                }
                _info.pixel = Pixel::pixel(format, type);
                _bitDepth = Pixel::bitDepth(_info.pixel);
            }
            break;
            case PPM::TYPE_U1:
                _info.pixel = Pixel::L_U8;
                _bitDepth = 1;
                break;
            default: break;
            }

            _info.endian = Core::Memory::MSB;

            //DJV_DEBUG_PRINT("info = " << _info);
            _image.set(_info);
        }

        PPMSave::~PPMSave()
        {}

        void PPMSave::write(const Image & in, const ImageIOFrameInfo & frame)
        {
            //DJV_DEBUG("PPMSave::write");
            //DJV_DEBUG_PRINT("in = " << in);
            //DJV_DEBUG_PRINT("type = " << _type);
            //DJV_DEBUG_PRINT("data = " << _data);

            // Open the file.
            Core::FileIO io;
            _open(_fileInfo.fileName(frame.frame), io);

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
            if (PPM::DATA_BINARY == _options.data && _bitDepth != 1)
            {
                io.set(p->data(), p->dataByteCount());
            }
            else
            {
                const int w = p->w(), h = p->h();
                const int channels = Pixel::channels(p->info().pixel);
                const quint64 scanlineByteCount = PPM::scanlineByteCount(
                    w,
                    channels,
                    _bitDepth,
                    _options.data);
                std::vector<quint8> scanline(scanlineByteCount);
                //DJV_DEBUG_PRINT("scanline = " << static_cast<int>(scanlineByteCount));
                for (int y = 0; y < h; ++y)
                {
                    if (PPM::DATA_BINARY == _options.data &&
                        1 == _bitDepth)
                    {
                        const quint8 * inP = p->data(0, y);
                        quint8 * outP = scanline.data();
                        for (int i = 0; i < w; ++i)
                        {
                            const int tmp = inP[i];
                            const int off = i % 8;
                            const int j = i / 8;
                            if (0 == off)
                            {
                                outP[j] = 0;
                            }
                            outP[j] |= ((!tmp) & 1) << (7 - off);
                        }

                        io.set(scanline.data(), scanlineByteCount);
                    }
                    else
                    {
                        const quint64 size = PPM::asciiSave(
                            p->data(0, y),
                            scanline.data(),
                            w * channels,
                            _bitDepth);
                        io.set(scanline.data(), size);
                    }
                }
            }
        }

        void PPMSave::_open(const QString & in, Core::FileIO & io)
        {
            //DJV_DEBUG("PPMSave::_open");
            //DJV_DEBUG_PRINT("in = " << in);

            // Open.
            io.setEndian(Core::Memory::endian() != Core::Memory::MSB);
            io.open(in, Core::FileIO::WRITE);

            // Header.
            int ppmType = 0;
            if (1 == _bitDepth)
            {
                ppmType = PPM::DATA_ASCII == _options.data ? 1 : 4;
            }
            else
            {
                ppmType = PPM::DATA_ASCII == _options.data ? 2 : 5;
                if (3 == _image.channels())
                {
                    ++ppmType;
                }
            }
            char magic[] = "P \n";
            magic[1] = '0' + ppmType;
            io.set(magic, 3);
            char tmp[Core::StringUtil::cStringLength] = "";
            int size = SNPRINTF(
                tmp,
                Core::StringUtil::cStringLength, "%d %d\n",
                _image.w(),
                _image.h());
            io.set(tmp, size);
            if (_bitDepth != 1)
            {
                const int maxValue = (8 == _bitDepth) ? Pixel::u8Max : Pixel::u16Max;
                size = SNPRINTF(tmp, Core::StringUtil::cStringLength, "%d\n", maxValue);
                io.set(tmp, size);
            }
        }

    } // namespace AV
} // namespace djv
