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

#include <djvGraphics/TIFFSave.h>

#include <djvGraphics/OpenGLImage.h>

namespace djv
{
    namespace Graphics
    {
        TIFFSave::TIFFSave(const TIFF::Options & options, Core::CoreContext * context) :
            ImageSave(context),
            _options(options),
            _f(0)
        {}

        TIFFSave::~TIFFSave()
        {
            close();
        }

        void TIFFSave::open(const Core::FileInfo & in, const ImageIOInfo & info)
        {
            //DJV_DEBUG("TIFFSave::open");
            //DJV_DEBUG_PRINT("in = " << in);

            _file = in;

            _info = PixelDataInfo();
            _info.size = info.size;
            _info.mirror.y = true;

            Pixel::TYPE type = Pixel::type(info.pixel);
            switch (type)
            {
            case Pixel::U10: type = Pixel::U16; break;
            case Pixel::F16: type = Pixel::F32; break;

            default: break;
            }

            _info.pixel = Pixel::pixel(Pixel::format(info.pixel), type);
            //DJV_DEBUG_PRINT("info = " << _info);

            _image.set(_info);
        }

        void TIFFSave::write(const Image & in, const ImageIOFrameInfo & frame)
        {
            //DJV_DEBUG("TIFFSave::write");
            //DJV_DEBUG_PRINT("in = " << in);

            // Open the file.
            const QString fileName = _file.fileName(frame.frame);
            //DJV_DEBUG_PRINT("file name = " << fileName);
            ImageIOInfo info(_info);
            info.tags = in.tags;
            _open(fileName, info);

            // Convert the image.
            const PixelData * p = &in;
            if (in.info() != _info)
            {
                //DJV_DEBUG_PRINT("convert = " << _image);
                _image.zero();
                OpenGLImage().copy(in, _image);
                p = &_image;
            }

            //! Write the file.
            const int h = p->h();
            for (int y = 0; y < h; ++y)
            {
                if (TIFFWriteScanline(_f, (tdata_t *)p->data(0, y), y) == -1)
                {
                    throw Core::Error(
                        TIFF::staticName,
                        ImageIO::errorLabels()[ImageIO::ERROR_WRITE]);
                }
            }

            close();
        }

        void TIFFSave::close()
        {
            if (_f)
            {
                TIFFClose(_f);
                _f = 0;
            }
        }

        void TIFFSave::_open(const QString & in, const ImageIOInfo & info)
        {
            //DJV_DEBUG("TIFFSave::_open");
            //DJV_DEBUG_PRINT("in = " << in);

            close();

            // Open the file.
            _f = TIFFOpen(in.toLatin1().data(), "w");
            if (!_f)
            {
                throw Core::Error(
                    TIFF::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_OPEN]);
            }

            // Write the header.
            uint16 photometric = 0;
            uint16 samples = 0;
            uint16 sampleDepth = 0;
            uint16 sampleFormat = 0;
            uint16 extraSamples[] = { EXTRASAMPLE_ASSOCALPHA };
            uint16 extraSamplesSize = 0;
            uint16 compression = 0;
            switch (Pixel::format(_image.pixel()))
            {
            case Pixel::L:
                photometric = PHOTOMETRIC_MINISBLACK;
                samples = 1;
                break;
            case Pixel::LA:
                photometric = PHOTOMETRIC_MINISBLACK;
                samples = 2;
                extraSamplesSize = 1;
                break;
            case Pixel::RGB:
                photometric = PHOTOMETRIC_RGB;
                samples = 3;
                break;
            case Pixel::RGBA:
                photometric = PHOTOMETRIC_RGB;
                samples = 4;
                extraSamplesSize = 1;
                break;
            default: break;
            }
            switch (Pixel::type(_image.pixel()))
            {
            case Pixel::U8:
                sampleDepth = 8;
                sampleFormat = SAMPLEFORMAT_UINT;
                break;
            case Pixel::U16:
                sampleDepth = 16;
                sampleFormat = SAMPLEFORMAT_UINT;
                break;
            case Pixel::F16:
            case Pixel::F32:
                sampleDepth = 32;
                sampleFormat = SAMPLEFORMAT_IEEEFP;
                break;
            default: break;
            }
            switch (_options.compression)
            {
            case TIFF::_COMPRESSION_NONE:
                compression = COMPRESSION_NONE;
                break;
            case TIFF::_COMPRESSION_RLE:
                compression = COMPRESSION_PACKBITS;
                break;
            case TIFF::_COMPRESSION_LZW:
                compression = COMPRESSION_LZW;
                break;
            default: break;
            }
            TIFFSetField(_f, TIFFTAG_IMAGEWIDTH, _image.w());
            TIFFSetField(_f, TIFFTAG_IMAGELENGTH, _image.h());
            TIFFSetField(_f, TIFFTAG_PHOTOMETRIC, photometric);
            TIFFSetField(_f, TIFFTAG_SAMPLESPERPIXEL, samples);
            TIFFSetField(_f, TIFFTAG_BITSPERSAMPLE, sampleDepth);
            TIFFSetField(_f, TIFFTAG_SAMPLEFORMAT, sampleFormat);
            TIFFSetField(_f, TIFFTAG_EXTRASAMPLES, extraSamplesSize, extraSamples);
            TIFFSetField(_f, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
            TIFFSetField(_f, TIFFTAG_COMPRESSION, compression);
            TIFFSetField(_f, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

            // Set image tags.
            const QStringList & tags = ImageTags::tagLabels();
            QString tmp = info.tags[tags[ImageTags::CREATOR]];
            if (tmp.length())
            {
                TIFFSetField(_f, TIFFTAG_ARTIST, tmp.toLatin1().data());
            }
            tmp = info.tags[tags[ImageTags::COPYRIGHT]];
            if (tmp.length())
            {
                TIFFSetField(_f, TIFFTAG_COPYRIGHT, tmp.toLatin1().data());
            }
            tmp = info.tags[tags[ImageTags::TIME]];
            if (tmp.length())
            {
                TIFFSetField(_f, TIFFTAG_DATETIME, tmp.toLatin1().data());
            }
            tmp = info.tags[tags[ImageTags::DESCRIPTION]];
            if (tmp.length())
            {
                TIFFSetField(_f, TIFFTAG_IMAGEDESCRIPTION, tmp.toLatin1().data());
            }
        }

    } // namespace Graphics
} // namespace djv
