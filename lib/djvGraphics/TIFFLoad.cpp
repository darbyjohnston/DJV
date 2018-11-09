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

#include <djvGraphics/TIFFLoad.h>

#include <djvGraphics/Image.h>
#include <djvGraphics/PixelDataUtil.h>

#include <djvCore/CoreContext.h>

namespace djv
{
    namespace Graphics
    {
        TIFFLoad::TIFFLoad(const Core::FileInfo & fileInfo, const QPointer<Core::CoreContext> & context) :
            ImageLoad(fileInfo, context)
        {
            _open(_fileInfo.fileName(_fileInfo.sequence().start()), _imageIOInfo);
            _close();
            if (Core::FileInfo::SEQUENCE == _fileInfo.type())
            {
                _imageIOInfo.sequence.frames = _fileInfo.sequence().frames;
            }
        }

        TIFFLoad::~TIFFLoad()
        {
            _close();
        }

        void TIFFLoad::read(Image & image, const ImageIOFrameInfo & frame)
        {
            //DJV_DEBUG("TIFFLoad::read");
            //DJV_DEBUG_PRINT("frame = " << frame);

            image.colorProfile = ColorProfile();
            image.tags = ImageTags();

            // Open the file.
            const QString fileName = _fileInfo.fileName(frame.frame != -1 ? frame.frame : _fileInfo.sequence().start());
            //DJV_DEBUG_PRINT("file name = " << fileName);
            ImageIOInfo info;
            _open(fileName, info);
            image.tags = info.tags;

            // Read the file.
            auto data = frame.proxy ? &_tmp : &image;
            data->set(info);
            for (int y = 0; y < info.size.y; ++y)
            {
                if (TIFFReadScanline(_f, (tdata_t *)data->data(0, y), y) == -1)
                {
                    throw Core::Error(
                        TIFF::staticName,
                        ImageIO::errorLabels()[ImageIO::ERROR_READ]);
                }
                if (_palette)
                {
                    TIFF::paletteLoad(
                        data->data(0, y),
                        info.size.x,
                        Pixel::channelByteCount(info.pixel),
                        _colormap[0], _colormap[1], _colormap[2]);
                }
            }

            // Proxy scaling.
            if (frame.proxy)
            {
                info.size = PixelDataUtil::proxyScale(info.size, frame.proxy);
                info.proxy = frame.proxy;
                image.set(info);
                PixelDataUtil::proxyScale(_tmp, image, frame.proxy);
            }

            // Close the file.
            //DJV_DEBUG_PRINT("image = " << image);
            _close();
        }

        void TIFFLoad::_open(const QString & in, ImageIOInfo & info)
        {
            //DJV_DEBUG("TIFFLoad::_open");
            //DJV_DEBUG_PRINT("in = " << in);

            // Open the file.
#if defined(DJV_WINDOWS)
            _f = TIFFOpenW(Core::StringUtil::qToStdWString(in).data(), "r");
#else
            _f = TIFFOpen(in.toUtf8().data(), "r");
#endif // DJV_WINDOWS
            if (!_f)
            {
                throw Core::Error(
                    TIFF::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_OPEN]);
            }

            // Read the Header.
            uint32   width = 0;
            uint32   height = 0;
            uint16   photometric = 0;
            uint16   samples = 0;
            uint16   sampleDepth = 0;
            uint16   sampleFormat = 0;
            uint16 * extraSamples = nullptr;
            uint16   extraSamplesSize = 0;
            uint16   orient = 0;
            uint16   compression = 0;
            uint16   channels = 0;
            TIFFGetFieldDefaulted(_f, TIFFTAG_IMAGEWIDTH, &width);
            TIFFGetFieldDefaulted(_f, TIFFTAG_IMAGELENGTH, &height);
            TIFFGetFieldDefaulted(_f, TIFFTAG_PHOTOMETRIC, &photometric);
            TIFFGetFieldDefaulted(_f, TIFFTAG_SAMPLESPERPIXEL, &samples);
            TIFFGetFieldDefaulted(_f, TIFFTAG_BITSPERSAMPLE, &sampleDepth);
            TIFFGetFieldDefaulted(_f, TIFFTAG_SAMPLEFORMAT, &sampleFormat);
            TIFFGetFieldDefaulted(_f, TIFFTAG_EXTRASAMPLES, &extraSamplesSize,
                &extraSamples);
            TIFFGetFieldDefaulted(_f, TIFFTAG_ORIENTATION, &orient);
            TIFFGetFieldDefaulted(_f, TIFFTAG_COMPRESSION, &compression);
            TIFFGetFieldDefaulted(_f, TIFFTAG_PLANARCONFIG, &channels);
            TIFFGetFieldDefaulted(_f, TIFFTAG_COLORMAP,
                &_colormap[0], &_colormap[1], &_colormap[2]);

            //DJV_DEBUG_PRINT("tiff size = " << width << " " << height);
            //DJV_DEBUG_PRINT("tiff photometric = " << photometric);
            //DJV_DEBUG_PRINT("tiff samples = " << samples);
            //DJV_DEBUG_PRINT("tiff sample depth = " << sampleDepth);
            //DJV_DEBUG_PRINT("tiff channels = " << channels);

            // Get file information.
            info.fileName = in;
            info.size = glm::ivec2(width, height);
            if (samples > 1 && PLANARCONFIG_SEPARATE == channels)
            {
                throw Core::Error(
                    TIFF::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_UNSUPPORTED]);
            }
            Pixel::PIXEL pixel = static_cast<Pixel::PIXEL>(0);
            bool found = false;
            switch (photometric)
            {
            case PHOTOMETRIC_PALETTE:
                pixel = Pixel::RGB_U8;
                found = true;
                break;
            case PHOTOMETRIC_MINISWHITE:
            case PHOTOMETRIC_MINISBLACK:
            case PHOTOMETRIC_RGB:
                if (32 == sampleDepth && sampleFormat != SAMPLEFORMAT_IEEEFP)
                    break;
                found = Pixel::pixel(
                    samples,
                    sampleDepth,
                    SAMPLEFORMAT_IEEEFP == sampleFormat ?
                    Pixel::FLOAT :
                    Pixel::INTEGER,
                    pixel);
                break;
            }
            //DJV_DEBUG_PRINT("found = " << found);
            //DJV_DEBUG_PRINT("pixel = " << pixel);
            if (!found)
            {
                throw Core::Error(
                    TIFF::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_UNSUPPORTED]);
            }
            info.pixel = pixel;
            _compression = compression != COMPRESSION_NONE;
            _palette = PHOTOMETRIC_PALETTE == photometric;
            switch (orient)
            {
            case ORIENTATION_TOPLEFT:  info.mirror.y = true;                 break;
            case ORIENTATION_TOPRIGHT: info.mirror.x = info.mirror.y = true; break;
            case ORIENTATION_BOTRIGHT: info.mirror.x = true;                 break;
            case ORIENTATION_BOTLEFT:                                        break;
            }

            // Get image tags.
            const QStringList & tags = ImageTags::tagLabels();
            char * tag = 0;
            if (TIFFGetField(_f, TIFFTAG_ARTIST, &tag))
            {
                info.tags[tags[ImageTags::CREATOR]] = tag;
            }
            if (TIFFGetField(_f, TIFFTAG_IMAGEDESCRIPTION, &tag))
            {
                info.tags[tags[ImageTags::DESCRIPTION]] = tag;
            }
            if (TIFFGetField(_f, TIFFTAG_COPYRIGHT, &tag))
            {
                info.tags[tags[ImageTags::COPYRIGHT]] = tag;
            }
            if (TIFFGetField(_f, TIFFTAG_DATETIME, &tag))
            {
                info.tags[tags[ImageTags::TIME]] = tag;
            }
        }

        void TIFFLoad::_close()
        {
            if (_f)
            {
                TIFFClose(_f);
                _f = nullptr;
            }
        }

    } // namespace Graphics
} // namespace djv
