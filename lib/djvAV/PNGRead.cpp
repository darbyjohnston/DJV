// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/PNG.h>

#include <djvSystem/File.h>
#include <djvSystem/FileFunc.h>
#include <djvSystem/FileIO.h>
#include <djvSystem/LogSystem.h>
#include <djvSystem/TextSystem.h>

#include <djvCore/StringFormat.h>
#include <djvCore/StringFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace PNG
            {
                class Read::File
                {
                    DJV_NON_COPYABLE(File);

                    File()
                    {
                        png = png_create_read_struct(
                            PNG_LIBPNG_VER_STRING,
                            &pngError,
                            djvPngError,
                            djvPngWarning);
                    }

                public:
                    ~File()
                    {
                        if (f)
                        {
                            fclose(f);
                            f = nullptr;
                        }
                        if (png || pngInfo || pngInfoEnd)
                        {
                            png_destroy_read_struct(
                                png        ? &png        : nullptr,
                                pngInfo    ? &pngInfo    : nullptr,
                                pngInfoEnd ? &pngInfoEnd : nullptr);
                            png        = nullptr;
                            pngInfo    = nullptr;
                            pngInfoEnd = nullptr;
                        }
                    }

                    static std::shared_ptr<File> create()
                    {
                        return std::shared_ptr<File>(new File);
                    }

                    bool open(const std::string& fileName)
                    {
                        f = System::File::fopen(fileName.c_str(), "rb");
                        return f;
                    }

                    FILE *      f          = nullptr;
                    png_structp png        = nullptr;
                    png_infop   pngInfo    = nullptr;
                    png_infop   pngInfoEnd = nullptr;
                    ErrorStruct pngError;
                };

                Read::Read()
                {}

                Read::~Read()
                {
                    _finish();
                }

                std::shared_ptr<Read> Read::create(
                    const System::File::Info& fileInfo,
                    const ReadOptions& readOptions,
                    const std::shared_ptr<System::TextSystem>& textSystem,
                    const std::shared_ptr<System::ResourceSystem>& resourceSystem,
                    const std::shared_ptr<System::LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_init(fileInfo, readOptions, textSystem, resourceSystem, logSystem);
                    return out;
                }

                namespace
                {
                    bool pngOpen(
                        FILE*       f,
                        png_structp png,
                        png_infop*  pngInfo,
                        png_infop*  pngInfoEnd,
                        uint16_t&   width,
                        uint16_t&   height,
                        uint8_t&    channels,
                        uint8_t&    bitDepth)
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

                        uint8_t tmp[8];
                        if (fread(tmp, 8, 1, f) != 1)
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

                        if (png_get_interlace_type(png, *pngInfo) != PNG_INTERLACE_NONE)
                        {
                            return false;
                        }

                        png_set_expand(png);
                        //png_set_gray_1_2_4_to_8(png);
                        png_set_palette_to_rgb(png);
                        png_set_tRNS_to_alpha(png);

                        width = png_get_image_width(png, *pngInfo);
                        height = png_get_image_height(png, *pngInfo);

                        channels = png_get_channels(png, *pngInfo);
                        if (png_get_color_type(png, *pngInfo) == PNG_COLOR_TYPE_PALETTE)
                        {
                            channels = 3;
                        }
                        if (png_get_valid(png, *pngInfo, PNG_INFO_tRNS))
                        {
                            ++channels;
                        }
                        bitDepth = png_get_bit_depth(png, *pngInfo);
                        if (bitDepth < 8)
                        {
                            bitDepth = 8;
                        }

                        if (bitDepth >= 16 && Memory::Endian::LSB == Memory::getEndian())
                        {
                            png_set_swap(png);
                        }

                        return true;
                    }

                } // namespace

                Info Read::_readInfo(const std::string& fileName)
                {
                    auto f = File::create();
                    return _open(fileName, f);
                }

                namespace
                {
                    bool pngScanline(png_structp png, uint8_t * out)
                    {
                        if (setjmp(png_jmpbuf(png)))
                        {
                            return false;
                        }
                        png_read_row(png, out, 0);
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

                std::shared_ptr<Image::Image> Read::_readImage(const std::string& fileName)
                {
                    // Open the file.
                    auto f = File::create();
                    const auto info = _open(fileName, f);

                    // Read the file.
                    auto out = Image::Image::create(info.video[0]);
                    out->setPluginName(pluginName);
                    for (uint16_t y = 0; y < info.video[0].size.h; ++y)
                    {
                        if (!pngScanline(f->png, out->getData(y)))
                        {
                            std::vector<std::string> messages;
                            messages.push_back(String::Format("{0}: {1}").
                                arg(fileName).
                                arg(_textSystem->getText(DJV_TEXT("error_read_scanline"))));
                            for (const auto& i : f->pngError.messages)
                            {
                                messages.push_back(i);
                            }
                            throw System::File::Error(String::join(messages, ' '));
                        }
                    }
                    pngEnd(f->png, f->pngInfoEnd);

                    // Log any warnings.
                    for (const auto& i : f->pngError.messages)
                    {
                        _logSystem->log(
                            pluginName,
                            String::Format("{0}: {1}").arg(fileName).arg(i),
                            System::LogLevel::Warning);
                    }

                    return out;
                }

                Info Read::_open(const std::string& fileName, const std::shared_ptr<File>& f)
                {
                    if (!f->png)
                    {
                        std::vector<std::string> messages;
                        messages.push_back(String::Format("{0}: {1}").
                            arg(fileName).
                            arg(_textSystem->getText(DJV_TEXT("error_file_open"))));
                        for (const auto& i : f->pngError.messages)
                        {
                            messages.push_back(i);
                        }
                        throw System::File::Error(String::join(messages, ' '));
                    }
                    if (!f->open(fileName))
                    {
                        throw System::File::Error(String::Format("{0}: {1}").
                            arg(fileName).
                            arg(_textSystem->getText(DJV_TEXT("error_file_open"))));
                    }
                    uint16_t width    = 0;
                    uint16_t height   = 0;
                    uint8_t  channels = 0;
                    uint8_t  bitDepth = 0;
                    if (!pngOpen(f->f, f->png, &f->pngInfo, &f->pngInfoEnd, width, height, channels, bitDepth))
                    {
                        std::vector<std::string> messages;
                        messages.push_back(String::Format("{0}: {1}").
                            arg(fileName).
                            arg(_textSystem->getText(DJV_TEXT("error_file_open"))));
                        for (const auto& i : f->pngError.messages)
                        {
                            messages.push_back(i);
                        }
                        throw System::File::Error(String::join(messages, ' '));
                    }

                    Image::Type imageType = Image::getIntType(channels, bitDepth);
                    if (Image::Type::None == imageType)
                    {
                        throw System::File::Error(String::Format("{0}: {1}").
                            arg(fileName).
                            arg(_textSystem->getText(DJV_TEXT("error_unsupported_image_type"))));
                    }

                    Info info;
                    info.fileName = fileName;
                    info.videoSpeed = _speed;
                    info.videoSequence = _sequence;
                    info.video.push_back(Image::Info(width, height, imageType));
                    return info;
                }

            } // namespace PNG
        } // namespace IO
    } // namespace AV
} // namespace djv

