// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/JPEG.h>

#include <djvCore/Context.h>
#include <djvCore/FileIO.h>
#include <djvCore/FileSystemFunc.h>
#include <djvCore/LogSystem.h>
#include <djvCore/StringFormat.h>
#include <djvCore/StringFunc.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace JPEG
            {
                struct Write::Private
                {
                    Options options;
                };

                Write::Write() :
                    _p(new Private)
                {}

                Write::~Write()
                {
                    _finish();
                }

                std::shared_ptr<Write> Write::create(
                    const FileSystem::FileInfo& fileInfo,
                    const Info& info,
                    const WriteOptions& writeOptions,
                    const Options& options,
                    const std::shared_ptr<TextSystem>& textSystem,
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Write>(new Write);
                    out->_p->options = options;
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
                            memset(&jpeg, 0, sizeof(jpeg_compress_struct));
                        }

                    public:
                        ~File()
                        {
                            if (jpegInit)
                            {
                                jpeg_destroy_compress(&jpeg);
                                jpegInit = false;
                            }
                            if (f)
                            {
                                fclose(f);
                                f = nullptr;
                            }
                        }

                        static std::shared_ptr<File> create()
                        {
                            return std::shared_ptr<File>(new File);
                        }

                        FILE*                f          = nullptr;
                        jpeg_compress_struct jpeg;
                        bool                 jpegInit   = false;
                        JPEGErrorStruct      jpegError;
                    };

                    bool jpegInit(jpeg_compress_struct* jpeg, JPEGErrorStruct* error)
                    {
                        if (setjmp(error->jump))
                        {
                            return false;
                        }
                        jpeg_create_compress(jpeg);
                        return true;
                    }

                    bool jpegOpen(
                        FILE* f,
                        jpeg_compress_struct* jpeg,
                        const Image::Info& info,
                        const Tags& tags,
                        const Options& options,
                        JPEGErrorStruct* error)
                    {
                        if (setjmp(error->jump))
                        {
                            return false;
                        }
                        jpeg_stdio_dest(jpeg, f);
                        jpeg->image_width = info.size.w;
                        jpeg->image_height = info.size.h;
                        if (Image::Type::L_U8 == info.type)
                        {
                            jpeg->input_components = 1;
                            jpeg->in_color_space = JCS_GRAYSCALE;
                        }
                        else if (Image::Type::RGB_U8 == info.type)
                        {
                            jpeg->input_components = 3;
                            jpeg->in_color_space = JCS_RGB;
                        }
                        jpeg_set_defaults(jpeg);
                        jpeg_set_quality(jpeg, options.quality, static_cast<boolean>(1));
                        jpeg_start_compress(jpeg, static_cast<boolean>(1));
                        std::string tag = tags.get("Description");
                        if (tag.length())
                        {
                            jpeg_write_marker(
                                jpeg,
                                JPEG_COM,
                                (JOCTET*)tag.c_str(),
                                static_cast<unsigned int>(tag.size()));
                        }
                        return true;
                    }

                    bool jpegScanline(
                        jpeg_compress_struct* jpeg,
                        const uint8_t* in,
                        JPEGErrorStruct* error)
                    {
                        if (::setjmp(error->jump))
                        {
                            return false;
                        }
                        JSAMPROW p[] = { (JSAMPLE*)(in) };
                        if (!jpeg_write_scanlines(jpeg, p, 1))
                        {
                            return false;
                        }
                        return true;
                    }

                    bool jpeg_end(jpeg_compress_struct* jpeg, JPEGErrorStruct* error)
                    {
                        if (::setjmp(error->jump))
                        {
                            return false;
                        }
                        jpeg_finish_compress(jpeg);
                        return true;
                    }

                } // namespace

                Image::Type Write::_getImageType(Image::Type value) const
                {
                    Image::Type out = Image::Type::None;
                    switch (value)
                    {
                    case Image::Type::L_U8:
                    case Image::Type::RGB_U8:   out = value; break;
                    case Image::Type::L_U16:
                    case Image::Type::L_U32:
                    case Image::Type::L_F16:
                    case Image::Type::L_F32:
                    case Image::Type::LA_U8:
                    case Image::Type::LA_U16:
                    case Image::Type::LA_U32:
                    case Image::Type::LA_F16:
                    case Image::Type::LA_F32:   out = Image::Type::LA_U8; break;
                    case Image::Type::RGB_U16:
                    case Image::Type::RGB_U32:
                    case Image::Type::RGB_F16:
                    case Image::Type::RGB_F32:
                    case Image::Type::RGBA_U8:
                    case Image::Type::RGBA_U16:
                    case Image::Type::RGBA_U32:
                    case Image::Type::RGBA_F16:
                    case Image::Type::RGBA_F32: out = Image::Type::RGB_U8; break;
                    default: break;
                    }
                    return out;
                }

                void Write::_write(const std::string& fileName, const std::shared_ptr<Image::Image>& image)
                {
                    // Open the file.
                    auto f = File::create();
                    f->jpeg.err = jpeg_std_error(&f->jpegError.pub);
                    f->jpegError.pub.error_exit = djvJPEGError;
                    f->jpegError.pub.emit_message = djvJPEGWarning;
                    if (!jpegInit(&f->jpeg, &f->jpegError))
                    {
                        std::vector<std::string> messages;
                        messages.push_back(String::Format("{0}: {1}").
                            arg(fileName).
                            arg(_textSystem->getText(DJV_TEXT("error_file_open"))));
                        for (const auto& i : f->jpegError.messages)
                        {
                            messages.push_back(i);
                        }
                        throw FileSystem::Error(String::join(messages, ' '));
                    }
                    f->jpegInit = true;
                    f->f = FileSystem::fopen(fileName.c_str(), "wb");
                    if (!f->f)
                    {
                        throw FileSystem::Error(String::Format("{0}: {1}").
                            arg(fileName).
                            arg(_textSystem->getText(DJV_TEXT("error_file_open"))));
                    }
                    const auto& info = image->getInfo();
                    if (!jpegOpen(f->f, &f->jpeg, info, _info.tags, _p->options, &f->jpegError))
                    {
                        std::vector<std::string> messages;
                        messages.push_back(String::Format("{0}: {1}").
                            arg(fileName).
                            arg(_textSystem->getText(DJV_TEXT("error_file_open"))));
                        for (const auto& i : f->jpegError.messages)
                        {
                            messages.push_back(i);
                        }
                        throw FileSystem::Error(String::join(messages, ' '));
                    }

                    // Write the file.
                    const uint16_t h = image->getHeight();
                    for (uint16_t y = 0; y < h; ++y)
                    {
                        if (!jpegScanline(&f->jpeg, image->getData(y), &f->jpegError))
                        {
                            throw FileSystem::Error(f->jpegError.messages.size() ?
                                f->jpegError.messages.back() :
                                _textSystem->getText(DJV_TEXT("error_write_scanline")));
                        }
                    }
                    if (!jpeg_end(&f->jpeg, &f->jpegError))
                    {
                        std::vector<std::string> messages;
                        messages.push_back(String::Format("{0}: {1}").
                            arg(fileName).
                            arg(_textSystem->getText(DJV_TEXT("error_file_close"))));
                        for (const auto& i : f->jpegError.messages)
                        {
                            messages.push_back(i);
                        }
                        throw FileSystem::Error(String::join(messages, ' '));
                    }

                    // Log any warnings.
                    for (const auto& i : f->jpegError.messages)
                    {
                        _logSystem->log(
                            pluginName,
                            String::Format("{0}: {1}").arg(fileName).arg(i),
                            LogLevel::Warning);
                    }
                }

            } // namespace JPEG
        } // namespace IO
    } // namespace AV
} // namespace djv

