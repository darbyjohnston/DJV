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

#include <djvAV/PNGLoad.h>

#include <djvAV/Image.h>
#include <djvAV/PixelDataUtil.h>

#include <djvCore/CoreContext.h>
#include <djvCore/Error.h>
#include <djvCore/StringUtil.h>

namespace djv
{
    namespace AV
    {
        PNGLoad::PNGLoad(const Core::FileInfo & fileInfo, const QPointer<Core::CoreContext> & context) :
            Load(fileInfo, context)
        {
            _pngError.context = context;
            _open(_fileInfo.fileName(_fileInfo.sequence().start()), _ioInfo);
            _close();
            if (Core::FileInfo::SEQUENCE == _fileInfo.type())
            {
                _ioInfo.sequence.frames = _fileInfo.sequence().frames;
            }
        }

        PNGLoad::~PNGLoad()
        {
            _close();
        }

        namespace
        {
            bool pngRead(png_structp png, quint8 * out[])
            {
                if (setjmp(png_jmpbuf(png)))
                {
                    return false;
                }
                png_read_image(png, out);
                return true;
            }

            bool pngEnd(png_structp png, png_infop pngInfo)
            {
                if (setjmp(png_jmpbuf(png)))
                {
                    return false;
                }
                png_read_end(png, pngInfo);
                return true;
            }

        } // namespace

        void PNGLoad::read(Image & image, const ImageIOInfo & frame)
        {
            //DJV_DEBUG("PNGLoad::read");
            //DJV_DEBUG_PRINT("frame = " << frame);

            image.colorProfile = ColorProfile();
            image.tags = Tags();

            // Open the file.
            const QString fileName = _fileInfo.fileName(frame.frame != -1 ? frame.frame : _fileInfo.sequence().start());
            //DJV_DEBUG_PRINT("file name = " << fileName);
            IOInfo info;
            _open(fileName, info);

            // Read the file.
            PixelData * data = frame.proxy ? &_tmp : &image;
            auto pixelDataInfo = info.layers[0];
            data->set(pixelDataInfo);

            quint8** rows = new quint8*[pixelDataInfo.size.y];
            for (int y = 0; y < pixelDataInfo.size.y; ++y)
            {
                rows[y] = data->data(0, pixelDataInfo.size.y - 1 - y);
            }
            if (!pngRead(_png, rows))
            {
                delete[] rows;
                throw Core::Error(PNG::staticName, _pngError.msg);
            }
            delete[] rows;

            if (!pngEnd(_png, _pngInfoEnd))
            {
                throw Core::Error(PNG::staticName, _pngError.msg);
            }

            // Proxy scale the image.
            if (frame.proxy)
            {
                pixelDataInfo.size = PixelDataUtil::proxyScale(pixelDataInfo.size, frame.proxy);
                pixelDataInfo.proxy = frame.proxy;
                image.set(pixelDataInfo);
                PixelDataUtil::proxyScale(_tmp, image, frame.proxy);
            }

            //DJV_DEBUG_PRINT("image = " << image);

            _close();
        }

        namespace
        {
            bool pngOpen(
                FILE *      f,
                png_structp png,
                png_infop * pngInfo,
                png_infop * pngInfoEnd)
            {
                if (setjmp(png_jmpbuf(png)))
                {
                    return false;
                }
                *pngInfo = png_create_info_struct(png);
                if (!*pngInfo)
                {
                    return false;
                }
                *pngInfoEnd = png_create_info_struct(png);
                if (!*pngInfoEnd)
                {
                    return false;
                }
                quint8 tmp[8];
                if (::fread(tmp, 8, 1, f) != 1)
                {
                    return false;
                }
                if (png_sig_cmp(tmp, 0, 8))
                {
                    return false;
                }
                png_init_io(png, f);
                png_set_sig_bytes(png, 8);
                png_read_info(png, *pngInfo);
                png_set_expand(png);
                //png_set_gray_1_2_4_to_8(png);
                png_set_palette_to_rgb(png);
                png_set_tRNS_to_alpha(png);
                return true;
            }

        } // namespace

        void PNGLoad::_open(const QString & in, IOInfo & info)
        {
            //DJV_DEBUG("PNGLoad::_open");
            //DJV_DEBUG_PRINT("in = " << in);

            // Initialize libpng.
            _png = png_create_read_struct(
                PNG_LIBPNG_VER_STRING,
                &_pngError,
                djvPNGError,
                djvPNGWarning);
            if (!_png)
            {
                throw Core::Error(
                    PNG::staticName,
                    IOPlugin::errorLabels()[IOPlugin::ERROR_OPEN]);
            }

            // Open the file.
#if defined(DJV_WINDOWS)
            ::_wfopen_s(&_f, Core::StringUtil::qToStdWString(in).data(), L"rb");
#else
            _f = ::fopen(in.toUtf8().data(), "rb");
#endif
            if (!_f)
            {
                throw Core::Error(
                    PNG::staticName,
                    IOPlugin::errorLabels()[IOPlugin::ERROR_OPEN]);
            }
            if (!pngOpen(_f, _png, &_pngInfo, &_pngInfoEnd))
            {
                throw Core::Error(
                    PNG::staticName,
                    IOPlugin::errorLabels()[IOPlugin::ERROR_OPEN]);
            }

            // Get file information.
            info.layers[0].fileName = in;
            info.layers[0].size = glm::ivec2(
                png_get_image_width(_png, _pngInfo),
                png_get_image_height(_png, _pngInfo));
            int channels = png_get_channels(_png, _pngInfo);
            if (png_get_color_type(_png, _pngInfo) == PNG_COLOR_TYPE_PALETTE)
            {
                channels = 3;
            }
            if (png_get_valid(_png, _pngInfo, PNG_INFO_tRNS))
            {
                ++channels;
            }
            //DJV_DEBUG_PRINT("channels = " << channels);
            int bitDepth = png_get_bit_depth(_png, _pngInfo);
            if (bitDepth < 8)
            {
                bitDepth = 8;
            }
            //DJV_DEBUG_PRINT("bit depth = " << bitDepth);
            if (!Pixel::intPixel(channels, bitDepth, info.layers[0].pixel))
            {
                throw Core::Error(
                    PNG::staticName,
                    IOPlugin::errorLabels()[IOPlugin::ERROR_UNSUPPORTED]);
            }

            // Set the endian.
            if (bitDepth >= 16 && Core::Memory::LSB == Core::Memory::endian())
            {
                png_set_swap(_png);
            }
        }

        void PNGLoad::_close()
        {
            if (_png || _pngInfo || _pngInfoEnd)
            {
                png_destroy_read_struct(
                    _png ? &_png : nullptr,
                    _pngInfo ? &_pngInfo : nullptr,
                    _pngInfoEnd ? &_pngInfoEnd : nullptr);
                _png = nullptr;
                _pngInfo = nullptr;
                _pngInfoEnd = nullptr;
            }
            if (_f)
            {
                ::fclose(_f);
                _f = nullptr;
            }
        }

    } // namespace AV
} // namespace djv
