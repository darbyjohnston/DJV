// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/PNG.h>

#include <djvAV/PNGFunc.h>

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
                struct Write::Private
                {
                };

                Write::Write() :
                    _p(new Private)
                {}

                Write::~Write()
                {
                    _finish();
                }

                std::shared_ptr<Write> Write::create(
                    const System::File::Info& fileInfo,
                    const Info& info,
                    const WriteOptions& writeOptions,
                    const std::shared_ptr<System::TextSystem>& textSystem,
                    const std::shared_ptr<System::ResourceSystem>& resourceSystem,
                    const std::shared_ptr<System::LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Write>(new Write);
                    out->_init(fileInfo, info, writeOptions, textSystem, resourceSystem, logSystem);
                    return out;
                }

                namespace
                {
                    class File
                    {
                        DJV_NON_COPYABLE(File);

                        File()
                        {
                            png = png_create_write_struct(
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
                            if (png || pngInfo)
                            {
                                png_destroy_write_struct(
                                    png     ? &png     : nullptr,
                                    pngInfo ? &pngInfo : nullptr);
                                png     = nullptr;
                                pngInfo = nullptr;
                            }
                        }

                        static std::shared_ptr<File> create()
                        {
                            return std::shared_ptr<File>(new File);
                        }

                        bool open(const std::string& fileName)
                        {
                            f = System::File::fopen(fileName.c_str(), "wb");
                            return f;
                        }

                        FILE *      f        = nullptr;
                        png_structp png      = nullptr;
                        png_infop   pngInfo  = nullptr;
                        ErrorStruct pngError;
                    };

                    bool pngOpen(
                        FILE *              f,
                        png_structp         png,
                        png_infop *         pngInfo,
                        const Image::Info & info)
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
                        png_init_io(png, f);

                        int colorType = 0;
                        switch (info.getGLFormat())
                        {
#if defined(DJV_GL_ES2)
                        case GL_LUMINANCE:       colorType = PNG_COLOR_TYPE_GRAY;       break;
                        case GL_LUMINANCE_ALPHA: colorType = PNG_COLOR_TYPE_GRAY_ALPHA; break;
#else // DJV_GL_ES2
                        case GL_RED:             colorType = PNG_COLOR_TYPE_GRAY;       break;
                        case GL_RG:              colorType = PNG_COLOR_TYPE_GRAY_ALPHA; break;
#endif // DJV_GL_ES2
                        case GL_RGB:             colorType = PNG_COLOR_TYPE_RGB;        break;
                        case GL_RGBA:            colorType = PNG_COLOR_TYPE_RGB_ALPHA;  break;
                        default: break;
                        }

                        png_set_IHDR(
                            png,
                            *pngInfo,
                            info.size.w,
                            info.size.h,
                            static_cast<int>(Image::getBitDepth(info.type)),
                            colorType,
                            PNG_INTERLACE_NONE,
                            PNG_COMPRESSION_TYPE_DEFAULT,
                            PNG_FILTER_TYPE_DEFAULT);
                        png_write_info(png, *pngInfo);

                        if (Image::getBitDepth(info.type) > 8 && Memory::Endian::LSB == Memory::getEndian())
                        {
                            png_set_swap(png);
                        }

                        return true;
                    }

                    bool pngScanline(png_structp png, const uint8_t * in)
                    {
                        if (setjmp(png_jmpbuf(png)))
                            return false;
                        png_write_row(png, reinterpret_cast<const png_byte*>(in));
                        return true;
                    }

                    bool pngEnd(png_structp png, png_infop pngInfo)
                    {
                        if (setjmp(png_jmpbuf(png)))
                            return false;
                        png_write_end(png, pngInfo);
                        return true;
                    }

                } // namespace

                Image::Type Write::_getImageType(Image::Type value) const
                {
                    Image::Type out = Image::Type::None;
                    switch (value)
                    {
                    case Image::Type::L_U8:
                    case Image::Type::L_U16:
                    case Image::Type::LA_U8:
                    case Image::Type::LA_U16:
                    case Image::Type::RGB_U8:
                    case Image::Type::RGB_U16:
                    case Image::Type::RGBA_U8:
                    case Image::Type::RGBA_U16: out = value; break;
                    case Image::Type::L_U32:
                    case Image::Type::L_F16:
                    case Image::Type::L_F32:    out = Image::Type::L_U16; break;
                    case Image::Type::LA_U32:
                    case Image::Type::LA_F16:
                    case Image::Type::LA_F32:   out = Image::Type::LA_U16; break;
                    case Image::Type::RGB_U10:
                    case Image::Type::RGB_U32:
                    case Image::Type::RGB_F16:
                    case Image::Type::RGB_F32:  out = Image::Type::RGB_U16; break;
                    case Image::Type::RGBA_U32:
                    case Image::Type::RGBA_F16:
                    case Image::Type::RGBA_F32: out = Image::Type::RGBA_U16; break;
                    default: break;
                    }
                    return out;
                }

                void Write::_write(const std::string& fileName, const std::shared_ptr<Image::Data>& image)
                {
                    // Open the file.
                    auto f = File::create();
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
                    const auto& info = image->getInfo();
                    if (!pngOpen(f->f, f->png, &f->pngInfo, info))
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

                    // Write the file.
                    for (uint16_t y = 0; y < info.size.h; ++y)
                    {
                        if (!pngScanline(f->png, image->getData(y)))
                        {
                            std::vector<std::string> messages;
                            messages.push_back(String::Format("{0}: {1}").
                                arg(fileName).
                                arg(_textSystem->getText(DJV_TEXT("error_write_scanline"))));
                            for (const auto& i : f->pngError.messages)
                            {
                                messages.push_back(i);
                            }
                            throw System::File::Error(String::join(messages, ' '));
                        }
                    }
                    if (!pngEnd(f->png, f->pngInfo))
                    {
                        std::vector<std::string> messages;
                        messages.push_back(String::Format("{0}: {1}").
                            arg(fileName).
                            arg(_textSystem->getText(DJV_TEXT("error_file_close"))));
                        for (const auto& i : f->pngError.messages)
                        {
                            messages.push_back(i);
                        }
                        throw System::File::Error(String::join(messages, ' '));
                    }

                    // Log any warnings.
                    for (const auto& i : f->pngError.messages)
                    {
                        _logSystem->log(
                            pluginName,
                            String::Format("{0}: {1}").arg(fileName).arg(i),
                            System::LogLevel::Warning);
                    }
                }

            } // namespace PNG
        } // namespace IO
    } // namespace AV
} // namespace djv

