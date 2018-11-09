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

#include <djvAV/OpenEXRSave.h>

#include <djvAV/OpenGLImage.h>

#include <djvCore/CoreContext.h>
#include <djvCore/Error.h>

#include <ImfChannelList.h>
#include <ImfCompressionAttribute.h>
#include <ImfHeader.h>
#include <ImfOutputFile.h>
#include <ImfStandardAttributes.h>

namespace djv
{
    namespace AV
    {
        OpenEXRSave::OpenEXRSave(const Core::FileInfo & fileInfo, const ImageIOInfo & imageIOInfo, const OpenEXR::Options & options, const QPointer<Core::CoreContext> & context) :
            ImageSave(fileInfo, imageIOInfo, context),
            _options(options)
        {
            //DJV_DEBUG("OpenEXRSave::OpenEXRSave");
            //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);

            // File information.
            if (_imageIOInfo.sequence.frames.count() > 1)
            {
                _fileInfo.setType(Core::FileInfo::SEQUENCE);
            }

            // Image information.
            _info = PixelDataInfo();
            _info.size = _imageIOInfo.size;
            _info.mirror.y = true;
            Pixel::FORMAT format = Pixel::format(_imageIOInfo.pixel);
            Pixel::TYPE type = Pixel::TYPE(0);
            switch (Pixel::type(_imageIOInfo.pixel))
            {
            case Pixel::F32: type = Pixel::F32; break;
            default: type = Pixel::F16; break;
            }
            _info.pixel = Pixel::pixel(format, type);
            //DJV_DEBUG_PRINT("info = " << _info);
            _speed = _imageIOInfo.sequence.speed;

            // Initialize temporary image buffer.
            _tmp.set(_info);
        }

        OpenEXRSave::~OpenEXRSave()
        {
            _close();
        }

        void OpenEXRSave::write(const Image & in, const ImageIOFrameInfo & frame)
        {
            //DJV_DEBUG("OpenEXRSave::write");
            //DJV_DEBUG_PRINT("in = " << in);

            try
            {
                // Open the file.
                ImageIOInfo info(_info);
                info.tags = in.tags;
                info.sequence.speed = _speed;
                _open(_fileInfo.fileName(frame.frame), info);

                // Convert the image.
                const PixelData * p = &in;
                if (p->info() != _info)
                {
                    //DJV_DEBUG_PRINT("convert = " << _tmp);
                    _tmp.zero();
                    OpenGLImage().copy(in, _tmp);
                    p = &_tmp;
                }

                // Write the file.
                const int w = p->w();
                const int h = p->h();
                const int channels = p->channels();
                const int byteCount = Pixel::channelByteCount(p->pixel());
                Imf::FrameBuffer frameBuffer;
                for (int c = 0; c < channels; ++c)
                {
                    const QString & channel = _channels[c];
                    //DJV_DEBUG_PRINT("channel = " << channel);
                    frameBuffer.insert(
                        channel.toUtf8().data(),
                        Imf::Slice(
                            OpenEXR::pixelTypeToImf(Pixel::type(p->pixel())),
                            (char *)p->data() + c * byteCount,
                            channels * byteCount,
                            w * channels * byteCount,
                            1,
                            1,
                            0.f));
                }
                _f->setFrameBuffer(frameBuffer);
                _f->writePixels(h);

            }
            catch (const std::exception & error)
            {
                throw Core::Error(
                    OpenEXR::staticName,
                    error.what());
            }

            _close();
        }

        void OpenEXRSave::_open(const QString & in, const ImageIOInfo & info)
        {
            //DJV_DEBUG("OpenEXRSave::_open");
            //DJV_DEBUG_PRINT("in = " << in);

            try
            {
                // Set the header.
                Imf::Header header(info.size.x, info.size.y);
                switch (Pixel::channels(info.pixel))
                {
                case 1: _channels = QStringList() << "Y"; break;
                case 2: _channels = QStringList() << "Y" << "A"; break;
                case 3: _channels = QStringList() << "R" << "G" << "B"; break;
                case 4: _channels = QStringList() << "R" << "G" << "B" << "A"; break;
                }
                for (int i = 0; i < _channels.count(); ++i)
                {
                    header.channels().insert(
                        _channels[i].toUtf8().data(),
                        OpenEXR::pixelTypeToImf(Pixel::type(info.pixel)));
                }
                Imf::CompressionAttribute compression;
                switch (_options.compression)
                {
                case OpenEXR::COMPRESSION_NONE:
                    compression = Imf::NO_COMPRESSION;
                    break;
                case OpenEXR::COMPRESSION_RLE:
                    compression = Imf::RLE_COMPRESSION;
                    break;
                case OpenEXR::COMPRESSION_ZIPS:
                    compression = Imf::ZIPS_COMPRESSION;
                    break;
                case OpenEXR::COMPRESSION_ZIP:
                    compression = Imf::ZIP_COMPRESSION;
                    break;
                case OpenEXR::COMPRESSION_PIZ:
                    compression = Imf::PIZ_COMPRESSION;
                    break;
                case OpenEXR::COMPRESSION_PXR24:
                    compression = Imf::PXR24_COMPRESSION;
                    break;
                case OpenEXR::COMPRESSION_B44:
                    compression = Imf::B44_COMPRESSION;
                    break;
                case OpenEXR::COMPRESSION_B44A:
                    compression = Imf::B44A_COMPRESSION;
                    break;
#if OPENEXR_VERSION_HEX >= 0x02020000
                case OpenEXR::COMPRESSION_DWAA:
                    compression = Imf::DWAA_COMPRESSION;
                    break;
                case OpenEXR::COMPRESSION_DWAB:
                    compression = Imf::DWAB_COMPRESSION;
                    break;
#endif // OPENEXR_VERSION_HEX
                default: break;
                }
                header.insert(Imf::CompressionAttribute::staticTypeName(), compression);
#if OPENEXR_VERSION_HEX >= 0x02020000
                addDwaCompressionLevel(header, _options.dwaCompressionLevel);
#endif // OPENEXR_VERSION_HEX

                // Set image tags.
                OpenEXR::saveTags(info, header);

                // Open the file.
                _f = new Imf::OutputFile(in.toUtf8().data(), header);

            }
            catch (const std::exception & error)
            {
                throw Core::Error(
                    OpenEXR::staticName,
                    error.what());
            }
        }

        void OpenEXRSave::_close()
        {
            delete _f;
            _f = nullptr;
        }

    } // namespace AV
} // namespace djv
