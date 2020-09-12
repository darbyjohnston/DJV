// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/TIFF.h>

#include <djvCore/FileIO.h>
#include <djvCore/FileSystem.h>
#include <djvCore/StringFormat.h>
#include <djvCore/TextSystem.h>

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
                    const std::shared_ptr<TextSystem>& textSystem,
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_init(fileInfo, readOptions, textSystem, resourceSystem, logSystem);
                    return out;
                }

                Info Read::_readInfo(const std::string& fileName)
                {
                    File f;
                    return _open(fileName, f);
                }

                std::shared_ptr<Image::Image> Read::_readImage(const std::string& fileName)
                {
                    std::shared_ptr<Image::Image> out;
                    File f;
                    const auto info = _open(fileName, f);
                    out = Image::Image::create(info.video[0]);
                    out->setPluginName(pluginName);
                    for (uint16_t y = 0; y < info.video[0].size.h; ++y)
                    {
                        if (TIFFReadScanline(f.f, (tdata_t *)out->getData(y), y) == -1)
                        {
                            throw FileSystem::Error(String::Format("{0}: {1}").
                                arg(fileName).
                                arg(_textSystem->getText(DJV_TEXT("error_read_scanline"))));
                        }
                        if (f.palette)
                        {
                            TIFF::readPalette(
                                out->getData(y),
                                info.video[0].size.w,
                                static_cast<int>(Image::getChannelCount(info.video[0].type)),
                                f.colormap[0], f.colormap[1], f.colormap[2]);
                        }
                    }
                    return out;
                }

                Info Read::_open(const std::string& fileName, File& f)
                {
#if defined(DJV_PLATFORM_WINDOWS)
                    f.f = TIFFOpen(fileName.data(), "r");
#else
                    f.f = TIFFOpen(fileName.data(), "r");
#endif // DJV_WINDOWS
                    if (!f.f)
                    {
                        throw FileSystem::Error(String::Format("{0}: {1}").
                            arg(fileName).
                            arg(_textSystem->getText(DJV_TEXT("error_file_open"))));
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
                        throw FileSystem::Error(String::Format("{0}: {1}").
                            arg(fileName).
                            arg(_textSystem->getText(DJV_TEXT("error_unsupported_image_type"))));
                    }

                    Image::Layout layout;
                    switch (orient)
                    {
                    case ORIENTATION_TOPLEFT:  break;
                    case ORIENTATION_TOPRIGHT: layout.mirror.x = true; break;
                    case ORIENTATION_BOTRIGHT: layout.mirror.x = layout.mirror.y = true; break;
                    case ORIENTATION_BOTLEFT:  layout.mirror.y = true; break;
                    }

                    f.compression = compression != COMPRESSION_NONE;
                    f.palette = PHOTOMETRIC_PALETTE == photometric;

                    AV::Tags tags;
                    char * tag = 0;
                    if (TIFFGetField(f.f, TIFFTAG_ARTIST, &tag))
                    {
                        if (tag)
                        {
                            tags.set("Creator", tag);
                        }
                    }
                    if (TIFFGetField(f.f, TIFFTAG_IMAGEDESCRIPTION, &tag))
                    {
                        if (tag)
                        {
                            tags.set("Description", tag);
                        }
                    }
                    if (TIFFGetField(f.f, TIFFTAG_COPYRIGHT, &tag))
                    {
                        if (tag)
                        {
                            tags.set("Copyright", tag);
                        }
                    }
                    if (TIFFGetField(f.f, TIFFTAG_DATETIME, &tag))
                    {
                        if (tag)
                        {
                            tags.set("Time", tag);
                        }
                    }

                    Info info;
                    info.fileName = fileName;
                    info.videoSpeed = _speed;
                    info.videoSequence = _sequence;
                    info.video.push_back(Image::Info(width, height, imageType, layout));
                    info.tags = tags;
                    return info;
                }

            } // namespace TIFF
        } // namespace IO
    } // namespace AV
} // namespace djv

