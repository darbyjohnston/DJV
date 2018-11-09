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

#include <djvAV/PPMLoad.h>

#include <djvAV/Image.h>
#include <djvAV/PixelDataUtil.h>

#include <djvCore/CoreContext.h>
#include <djvCore/FileIOUtil.h>

namespace djv
{
    namespace AV
    {
        PPMLoad::PPMLoad(const Core::FileInfo & fileInfo, const QPointer<Core::CoreContext> & context) :
            ImageLoad(fileInfo, context)
        {
            Core::FileIO io;
            _open(_fileInfo.fileName(_fileInfo.sequence().start()), _imageIOInfo, io);
            if (Core::FileInfo::SEQUENCE == _fileInfo.type())
            {
                _imageIOInfo.sequence.frames = _fileInfo.sequence().frames;
            }
        }

        PPMLoad::~PPMLoad()
        {}

        void PPMLoad::read(Image & image, const ImageIOFrameInfo & frame)
        {
            //DJV_DEBUG("PPMLoad::read");
            //DJV_DEBUG_PRINT("frame = " << frame);
            image.colorProfile = ColorProfile();
            image.tags = ImageTags();

            // Open the file.
            const QString fileName = _fileInfo.fileName(frame.frame != -1 ? frame.frame : _fileInfo.sequence().start());
            //DJV_DEBUG_PRINT("file name = " << fileName);
            ImageIOInfo info;
            QScopedPointer<Core::FileIO> io(new Core::FileIO);
            _open(fileName, info, *io);

            // Read the file.
            io->readAhead();
            PixelData * data = frame.proxy ? &_tmp : &image;
            if (PPM::DATA_BINARY == _data && _bitDepth != 1)
            {
                if ((io->size() - io->pos()) < PixelDataUtil::dataByteCount(info))
                {
                    throw Core::Error(
                        PPM::staticName,
                        ImageIO::errorLabels()[ImageIO::ERROR_READ]);
                }
                data->set(info, io->mmapP(), io.data());
                io.take();
            }
            else
            {
                data->set(info);
                const int channels = Pixel::channels(info.pixel);
                if (PPM::DATA_BINARY == _data && 1 == _bitDepth)
                {
                    const quint64 scanlineByteCount = PPM::scanlineByteCount(
                        info.size.x, channels, _bitDepth, _data);
                    std::vector<quint8> scanline(scanlineByteCount);
                    //DJV_DEBUG_PRINT("scanline = " <<
                    //    static_cast<int>(scanlineByteCount));
                    for (int y = 0; y < info.size.y; ++y)
                    {
                        io->get(scanline.data(), scanlineByteCount);
                        const quint8 * inP = scanline.data();
                        quint8 * outP = data->data(0, y);
                        for (int i = info.size.x - 1; i >= 0; --i)
                        {
                            outP[i] = (inP[i / 8] >> (7 - (i % 8))) & 1 ? 0 : 255;
                        }
                    }
                }
                else
                {
                    for (int y = 0; y < info.size.y; ++y)
                    {
                        PPM::asciiLoad(
                            *io,
                            data->data(0, y),
                            info.size.x * channels,
                            _bitDepth);
                    }
                }
            }

            // Proxy scale the image.
            if (frame.proxy)
            {
                info.size = PixelDataUtil::proxyScale(info.size, frame.proxy);
                info.proxy = frame.proxy;
                image.set(info);
                PixelDataUtil::proxyScale(_tmp, image, frame.proxy);
            }

            //DJV_DEBUG_PRINT("image = " << image);
        }

        void PPMLoad::_open(const QString & in, ImageIOInfo & info, Core::FileIO & io)
        {
            //DJV_DEBUG("PPMLoad::_open");
            //DJV_DEBUG_PRINT("in = " << in);

            // Open the file.
            io.setEndian(Core::Memory::endian() != Core::Memory::MSB);
            io.open(in, Core::FileIO::READ);
            char magic[] = { 0, 0, 0 };
            io.get(magic, 2);
            //DJV_DEBUG_PRINT("magic = " << magic);
            if (magic[0] != 'P')
            {
                throw Core::Error(
                    PPM::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_UNRECOGNIZED]);
            }
            switch (magic[1])
            {
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6': break;
            default:
                throw Core::Error(
                    PPM::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_UNSUPPORTED]);
            }
            const int ppmType = magic[1] - '0';
            //DJV_DEBUG_PRINT("ppm type = " << ppmType);

            // Read the header.
            char tmp[Core::StringUtil::cStringLength] = "";
            int width = 0;
            int height = 0;
            Core::FileIOUtil::word(io, tmp, Core::StringUtil::cStringLength);
            width = QString(tmp).toInt();
            Core::FileIOUtil::word(io, tmp, Core::StringUtil::cStringLength);
            height = QString(tmp).toInt();
            int maxValue = 0;
            if (ppmType != 1 && ppmType != 4)
            {
                Core::FileIOUtil::word(io, tmp, Core::StringUtil::cStringLength);
                maxValue = QString(tmp).toInt();
            }
            //DJV_DEBUG_PRINT("max value = " << maxValue);

            // Information.
            info.fileName = in;
            info.size = glm::ivec2(width, height);
            info.mirror.y = true;
            if (1 == ppmType || 4 == ppmType)
            {
                _bitDepth = 1;
            }
            else
            {
                _bitDepth = maxValue < 256 ? 8 : 16;
            }
            //DJV_DEBUG_PRINT("bit depth = " << _bitDepth);
            int channels = 0;
            switch (ppmType)
            {
            case 1:
            case 2:
            case 4:
            case 5: channels = 1; break;
            case 3:
            case 6: channels = 3; break;
            }
            if (!Pixel::pixel(
                channels,
                _bitDepth != 1 ? _bitDepth : 8,
                Pixel::INTEGER,
                info.pixel))
            {
                throw Core::Error(
                    PPM::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_UNSUPPORTED]);
            }
            _data =
                (1 == ppmType || 2 == ppmType || 3 == ppmType) ?
                PPM::DATA_ASCII :
                PPM::DATA_BINARY;

            if (PPM::DATA_BINARY == _data)
            {
                info.endian = Core::Memory::MSB;
            }
        }

    } // namespace AV
} // namespace djv
