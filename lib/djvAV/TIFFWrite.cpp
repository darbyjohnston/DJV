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

#include <djvAV/TIFF.h>

#include <djvAV/ImageConvert.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace TIFF
            {
                struct Write::Private
                {
                    Settings settings;
                };

                Write::Write(const Settings & settings) :
                    _p(new Private)
                {
                    _p->settings = settings;
                }

                Write::~Write()
                {}

                std::shared_ptr<Write> Write::create(const std::string & fileName, const Settings & settings, const Info & info, const std::shared_ptr<Queue> & queue, Context * context)
                {
                    auto out = std::shared_ptr<Write>(new Write(settings));
                    out->_init(fileName, info, queue, context);
                    return out;
                }

                namespace
                {
                    struct File
                    {
                        ~File()
                        {
                            if (f)
                            {
                                TIFFClose(f);
                                f = nullptr;
                            }
                        }

                        ::TIFF * f = nullptr;
                    };
                }
                
                void Write::_write(const std::string & fileName, const std::shared_ptr<Image::Image> & image)
                {
                    Image::Type imageType = Image::Type::None;
                    switch (image->getType())
                    {
                    case Image::Type::L_U8:
                    case Image::Type::L_U16:
                    case Image::Type::L_U32:
                    case Image::Type::L_F32:
                    case Image::Type::LA_U8:
                    case Image::Type::LA_U16:
                    case Image::Type::LA_U32:
                    case Image::Type::LA_F32:
                    case Image::Type::RGB_U8:
                    case Image::Type::RGB_U16:
                    case Image::Type::RGB_U32:
                    case Image::Type::RGB_F32:
                    case Image::Type::RGBA_U8:
                    case Image::Type::RGBA_U16:
                    case Image::Type::RGBA_U32:
                    case Image::Type::RGBA_F32: imageType = image->getType();      break;
                    case Image::Type::L_F16:    imageType = Image::Type::L_F32;    break;
                    case Image::Type::LA_F16:   imageType = Image::Type::LA_F32;   break;
                    case Image::Type::RGB_U10:  imageType = Image::Type::RGB_U16;  break;
                    case Image::Type::RGB_F16:  imageType = Image::Type::RGB_F32;  break;
                    case Image::Type::RGBA_F16: imageType = Image::Type::RGBA_F32; break;
                    default: break;
                    }
                    if (Image::Type::None == imageType)
                    {
                        std::stringstream s;
                        s << DJV_TEXT("The TIFF file") <<
                            " '" << fileName << "' " << DJV_TEXT("cannot be written") << ".";
                        throw std::runtime_error(s.str());
                    }
                    Image::Layout layout;
                    layout.mirror.y = true;
                    Image::Info info(_imageInfo.size, imageType, layout);

                    std::shared_ptr<Image::Data> imageData = image;
                    if (imageData->getInfo() != info)
                    {
                        auto tmp = Image::Data::create(info);
                        _convert->process(*imageData, info, *tmp);
                        imageData = tmp;
                    }

                    File f;
#if defined(DJV_PLATFORM_WINDOWS)
                    f.f = TIFFOpen(fileName.data(), "w");
#else
                    f.f = TIFFOpen(fileName.data(), "w");
#endif // DJV_WINDOWS
                    if (!f.f)
                    {
                        std::stringstream s;
                        s << DJV_TEXT("The TIFF file") <<
                            " '" << fileName << "' " << DJV_TEXT("cannot be opened") << ".";
                        throw std::runtime_error(s.str());
                    }

                    uint16 photometric      = 0;
                    uint16 samples          = 0;
                    uint16 sampleDepth      = 0;
                    uint16 sampleFormat     = 0;
                    uint16 extraSamples[]   = { EXTRASAMPLE_ASSOCALPHA };
                    uint16 extraSamplesSize = 0;
                    uint16 compression      = 0;
                    switch (Image::getChannelCount(info.type))
                    {
                    case 1:
                        photometric = PHOTOMETRIC_MINISBLACK;
                        samples = 1;
                        break;
                    case 2:
                        photometric = PHOTOMETRIC_MINISBLACK;
                        samples = 2;
                        extraSamplesSize = 1;
                        break;
                    case 3:
                        photometric = PHOTOMETRIC_RGB;
                        samples = 3;
                        break;
                    case 4:
                        photometric = PHOTOMETRIC_RGB;
                        samples = 4;
                        extraSamplesSize = 1;
                        break;
                    default: break;
                    }
                    switch (Image::getDataType(info.type))
                    {
                    case Image::DataType::U8:
                        sampleDepth = 8;
                        sampleFormat = SAMPLEFORMAT_UINT;
                        break;
                    case Image::DataType::U16:
                        sampleDepth = 16;
                        sampleFormat = SAMPLEFORMAT_UINT;
                        break;
                    case Image::DataType::U32:
                        sampleDepth = 32;
                        sampleFormat = SAMPLEFORMAT_UINT;
                        break;
                    case Image::DataType::F32:
                        sampleDepth = 32;
                        sampleFormat = SAMPLEFORMAT_IEEEFP;
                        break;
                    default: break;
                    }
                    switch (_p->settings.compression)
                    {
                    case Compression::None:
                        compression = COMPRESSION_NONE;
                        break;
                    case Compression::RLE:
                        compression = COMPRESSION_PACKBITS;
                        break;
                    case Compression::LZW:
                        compression = COMPRESSION_LZW;
                        break;
                    default: break;
                    }
                    TIFFSetField(f.f, TIFFTAG_IMAGEWIDTH, info.size.x);
                    TIFFSetField(f.f, TIFFTAG_IMAGELENGTH, info.size.y);
                    TIFFSetField(f.f, TIFFTAG_PHOTOMETRIC, photometric);
                    TIFFSetField(f.f, TIFFTAG_SAMPLESPERPIXEL, samples);
                    TIFFSetField(f.f, TIFFTAG_BITSPERSAMPLE, sampleDepth);
                    TIFFSetField(f.f, TIFFTAG_SAMPLEFORMAT, sampleFormat);
                    TIFFSetField(f.f, TIFFTAG_EXTRASAMPLES, extraSamplesSize, extraSamples);
                    TIFFSetField(f.f, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
                    TIFFSetField(f.f, TIFFTAG_COMPRESSION, compression);
                    TIFFSetField(f.f, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

                    std::string tag = _info.tags.getTag("Creator");
                    if (!tag.empty())
                    {
                        TIFFSetField(f.f, TIFFTAG_ARTIST, tag.data());
                    }
                    tag = _info.tags.getTag("Copyright");
                    if (!tag.empty())
                    {
                        TIFFSetField(f.f, TIFFTAG_COPYRIGHT, tag.data());
                    }
                    tag = _info.tags.getTag("Time");
                    if (!tag.empty())
                    {
                        TIFFSetField(f.f, TIFFTAG_DATETIME, tag.data());
                    }
                    tag = _info.tags.getTag("Description");;
                    if (!tag.empty())
                    {
                        TIFFSetField(f.f, TIFFTAG_IMAGEDESCRIPTION, tag.data());
                    }

                    for (int y = 0; y < info.size.y; ++y)
                    {
                        if (TIFFWriteScanline(f.f, (tdata_t *)imageData->getData(y), y) == -1)
                        {
                            std::stringstream s;
                            s << DJV_TEXT("The TIFF file") <<
                                " '" << fileName << "' " << DJV_TEXT("cannot be written") << ".";
                            throw std::runtime_error(s.str());
                        }
                    }
                }

            } // namespace TIFF
        } // namespace IO
    } // namespace AV
} // namespace djv

