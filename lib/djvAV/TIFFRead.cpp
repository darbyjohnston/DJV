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

#include <djvCore/FileIO.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace TIFF
            {
                struct Read::File
                {
                    ~File()
                    {
                        if (f)
                        {
                            TIFFClose(f);
                            f = nullptr;
                        }
                    }

                    ::TIFF * f           = nullptr;
                    bool     compression = false;
                    bool     palette     = false;
                    uint16 * colormap[3] = { nullptr, nullptr, nullptr };
                };

                Read::Read()
                {}

                Read::~Read()
                {
                    _finish();
                }

                std::shared_ptr<Read> Read::create(
                    const FileSystem::FileInfo& fileInfo,
                    const ReadOptions& readOptions,
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_init(fileInfo, readOptions, resourceSystem, logSystem);
                    return out;
                }

                Info Read::_readInfo(const std::string & fileName)
                {
                    File f;
                    return _open(fileName, f);
                }

                std::shared_ptr<Image::Image> Read::_readImage(const std::string & fileName)
                {
                    std::shared_ptr<Image::Image> out;
                    File f;
                    const auto info = _open(fileName, f);
                    out = Image::Image::create(info.video[0].info);
                    for (uint16_t y = 0; y < info.video[0].info.size.h; ++y)
                    {
                        if (TIFFReadScanline(f.f, (tdata_t *)out->getData(y), y) == -1)
                        {
                            std::stringstream s;
                            s << DJV_TEXT("The TIFF file") <<
                                " '" << fileName << "' " << DJV_TEXT("cannot be read") << ".";
                            throw std::runtime_error(s.str());
                        }
                        if (f.palette)
                        {
                            TIFF::paletteLoad(
                                out->getData(y),
                                info.video[0].info.size.w,
                                static_cast<int>(Image::getChannelCount(info.video[0].info.type)),
                                f.colormap[0], f.colormap[1], f.colormap[2]);
                        }
                    }
                    return out;
                }

                Info Read::_open(const std::string & fileName, File & f)
                {
#if defined(DJV_PLATFORM_WINDOWS)
                    f.f = TIFFOpen(fileName.data(), "r");
#else
                    f.f = TIFFOpen(fileName.data(), "r");
#endif // DJV_WINDOWS
                    if (!f.f)
                    {
                        std::stringstream s;
                        s << DJV_TEXT("The TIFF file") <<
                            " '" << fileName << "' " << DJV_TEXT("cannot be opened") << ".";
                        throw std::runtime_error(s.str());
                    }

                    uint32   width            = 0;
                    uint32   height           = 0;
                    uint16   photometric      = 0;
                    uint16   samples          = 0;
                    uint16   sampleDepth      = 0;
                    uint16   sampleFormat     = 0;
                    uint16 * extraSamples     = nullptr;
                    uint16   extraSamplesSize = 0;
                    uint16   orient           = 0;
                    uint16   compression      = 0;
                    uint16   channels         = 0;
                    TIFFGetFieldDefaulted(f.f, TIFFTAG_IMAGEWIDTH, &width);
                    TIFFGetFieldDefaulted(f.f, TIFFTAG_IMAGELENGTH, &height);
                    TIFFGetFieldDefaulted(f.f, TIFFTAG_PHOTOMETRIC, &photometric);
                    TIFFGetFieldDefaulted(f.f, TIFFTAG_SAMPLESPERPIXEL, &samples);
                    TIFFGetFieldDefaulted(f.f, TIFFTAG_BITSPERSAMPLE, &sampleDepth);
                    TIFFGetFieldDefaulted(f.f, TIFFTAG_SAMPLEFORMAT, &sampleFormat);
                    TIFFGetFieldDefaulted(f.f, TIFFTAG_EXTRASAMPLES, &extraSamplesSize, &extraSamples);
                    TIFFGetFieldDefaulted(f.f, TIFFTAG_ORIENTATION, &orient);
                    TIFFGetFieldDefaulted(f.f, TIFFTAG_COMPRESSION, &compression);
                    TIFFGetFieldDefaulted(f.f, TIFFTAG_PLANARCONFIG, &channels);
                    TIFFGetFieldDefaulted(f.f, TIFFTAG_COLORMAP, &f.colormap[0], &f.colormap[1], &f.colormap[2]);

                    Image::Type imageType = Image::Type::None;
                    switch (photometric)
                    {
                    case PHOTOMETRIC_PALETTE:
                        imageType = Image::Type::RGB_U8;
                        break;
                    case PHOTOMETRIC_MINISWHITE:
                    case PHOTOMETRIC_MINISBLACK:
                    case PHOTOMETRIC_RGB:
                        if (32 == sampleDepth && sampleFormat != SAMPLEFORMAT_IEEEFP)
                            break;
                        if (SAMPLEFORMAT_IEEEFP == sampleFormat)
                        {
                            imageType = Image::getFloatType(samples, sampleDepth);
                        }
                        else
                        {
                            imageType = Image::getIntType(samples, sampleDepth);
                        }
                        break;
                    }
                    if (Image::Type::None == imageType)
                    {
                        std::stringstream s;
                        s << DJV_TEXT("The TIFF file") <<
                            " '" << fileName << "' " << DJV_TEXT("cannot be opened") << ".";
                        throw std::runtime_error(s.str());
                    }

                    Image::Layout layout;
                    switch (orient)
                    {
                    case ORIENTATION_TOPLEFT:  layout.mirror.y = true; break;
                    case ORIENTATION_TOPRIGHT: layout.mirror.x = layout.mirror.y = true; break;
                    case ORIENTATION_BOTRIGHT: layout.mirror.x = true; break;
                    case ORIENTATION_BOTLEFT: break;
                    }

                    f.compression = compression != COMPRESSION_NONE;
                    f.palette = PHOTOMETRIC_PALETTE == photometric;

                    AV::Tags tags;
                    char * tag = 0;
                    if (TIFFGetField(f.f, TIFFTAG_ARTIST, &tag))
                    {
                        if (tag)
                        {
                            tags.setTag("Creator", tag);
                        }
                    }
                    if (TIFFGetField(f.f, TIFFTAG_IMAGEDESCRIPTION, &tag))
                    {
                        if (tag)
                        {
                            tags.setTag("Description", tag);
                        }
                    }
                    if (TIFFGetField(f.f, TIFFTAG_COPYRIGHT, &tag))
                    {
                        if (tag)
                        {
                            tags.setTag("Copyright", tag);
                        }
                    }
                    if (TIFFGetField(f.f, TIFFTAG_DATETIME, &tag))
                    {
                        if (tag)
                        {
                            tags.setTag("Time", tag);
                        }
                    }

                    auto imageInfo = Image::Info(width, height, imageType, layout);
                    auto info = Info(fileName, VideoInfo(imageInfo, _speed, _duration));
                    info.tags = tags;
                    return info;
                }

            } // namespace TIFF
        } // namespace IO
    } // namespace AV
} // namespace djv

