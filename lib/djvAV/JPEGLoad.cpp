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

#include <djvAV/JPEGLoad.h>

#include <djvAV/Image.h>
#include <djvAV/PixelDataUtil.h>

#include <djvCore/CoreContext.h>
#include <djvCore/Error.h>

namespace djv
{
    namespace AV
    {
        JPEGLoad::JPEGLoad(const Core::FileInfo & fileInfo, const QPointer<Core::CoreContext> & context) :
            Load(fileInfo, context)
        {
            _open(_fileInfo.fileName(_fileInfo.sequence().start()), _ioInfo);
            _close();
            if (Core::FileInfo::SEQUENCE == _fileInfo.type())
            {
                _ioInfo.sequence.frames = _fileInfo.sequence().frames;
            }
        }

        JPEGLoad::~JPEGLoad()
        {
            _close();
        }

        namespace
        {
            bool jpegScanline(
                jpeg_decompress_struct * jpeg,
                quint8 *                 out,
                JPEGErrorStruct *        error)
            {
                if (::setjmp(error->jump))
                {
                    return false;
                }
                JSAMPROW p[] = { (JSAMPLE *)(out) };
                if (!jpeg_read_scanlines(jpeg, p, 1))
                {
                    return false;
                }
                return true;
            }

            bool jpegEnd(
                jpeg_decompress_struct * jpeg,
                JPEGErrorStruct *        error)
            {
                if (::setjmp(error->jump))
                {
                    return false;
                }
                jpeg_finish_decompress(jpeg);
                return true;
            }

        } // namespace

        void JPEGLoad::read(Image & image, const ImageIOInfo & frame)
        {
            //DJV_DEBUG("JPEGLoad::read");
            //DJV_DEBUG_PRINT("frame = " << frame);

            image.colorProfile = ColorProfile();
            image.tags = Tags();

            // Open the file.
            const QString fileName = _fileInfo.fileName(frame.frame != -1 ? frame.frame : _fileInfo.sequence().start());
            //DJV_DEBUG_PRINT("file name = " << fileName);
            IOInfo info;
            _open(fileName, info);
            image.tags = info.tags;

            // Read the file.
            PixelData * data = frame.proxy ? &_tmp : &image;
            auto pixelDataInfo = info.layers[0];
            data->set(pixelDataInfo);
            for (int y = 0; y < pixelDataInfo.size.y; ++y)
            {
                if (!jpegScanline(
                    &_jpeg,
                    data->data(0, data->h() - 1 - y),
                    &_jpegError))
                {
                    throw Core::Error(JPEG::staticName, _jpegError.msg);
                }
            }
            if (!jpegEnd(&_jpeg, &_jpegError))
            {
                throw Core::Error(JPEG::staticName, _jpegError.msg);
            }

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
            bool jpegInit(
                jpeg_decompress_struct * jpeg,
                JPEGErrorStruct *        error)
            {
                if (::setjmp(error->jump))
                {
                    return false;
                }
                jpeg_create_decompress(jpeg);
                return true;
            }

            bool jpegOpen(
                FILE *                   f,
                jpeg_decompress_struct * jpeg,
                JPEGErrorStruct *        error)
            {
                if (::setjmp(error->jump))
                {
                    return false;
                }
                jpeg_stdio_src(jpeg, f);
                jpeg_save_markers(jpeg, JPEG_COM, 0xFFFF);
                if (!jpeg_read_header(jpeg, static_cast<boolean>(1)))
                {
                    return false;
                }
                if (!jpeg_start_decompress(jpeg))
                {
                    return false;
                }
                return true;
            }

        } // namespace

        void JPEGLoad::_open(const QString & in, IOInfo & info)
        {
            //DJV_DEBUG("JPEGLoad::_open");
            //DJV_DEBUG_PRINT("in = " << in);

            // Initialize libjpeg.
            _jpeg.err = jpeg_std_error(&_jpegError.pub);
            _jpegError.pub.error_exit = djvJPEGError;
            _jpegError.pub.emit_message = djvJPEGWarning;
            _jpegError.msg[0] = 0;
            if (!jpegInit(&_jpeg, &_jpegError))
            {
                throw Core::Error(JPEG::staticName, _jpegError.msg);
            }
            _jpegInit = true;

            // Open.
#if defined(DJV_WINDOWS)
            ::_wfopen_s(&_f, Core::StringUtil::qToStdWString(in).data(), L"rb");
#else // DJV_WINDOWS
            _f = ::fopen(in.toUtf8().data(), "rb");
#endif // DJV_WINDOWS
            if (!_f)
            {
                throw Core::Error(
                    JPEG::staticName,
                    IOPlugin::errorLabels()[IOPlugin::ERROR_OPEN]);
            }
            if (!jpegOpen(_f, &_jpeg, &_jpegError))
            {
                throw Core::Error(JPEG::staticName, _jpegError.msg);
            }

            // Information.
            info.layers[0].fileName = in;
            info.layers[0].size = glm::ivec2(_jpeg.output_width, _jpeg.output_height);
            if (!Pixel::intPixel(_jpeg.out_color_components, 8, info.layers[0].pixel))
            {
                throw Core::Error(
                    JPEG::staticName,
                    IOPlugin::errorLabels()[IOPlugin::ERROR_UNSUPPORTED]);
            }

            // Tags.
            const jpeg_saved_marker_ptr marker = _jpeg.marker_list;
            if (marker)
                info.tags[Tags::tagLabels()[Tags::DESCRIPTION]] =
                QString((const char *)marker->data).mid(0, marker->data_length);

            //DJV_DEBUG_PRINT("info = " << info);
        }

        void JPEGLoad::_close()
        {
            if (_jpegInit)
            {
                jpeg_destroy_decompress(&_jpeg);
                _jpegInit = false;
            }
            if (_f)
            {
                ::fclose(_f);
                _f = nullptr;
            }
        }

    } // namespace AV
} // namespace djv
