// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/JPEG.h>

#include <djvCore/FileIO.h>
#include <djvCore/LogSystem.h>
#include <djvCore/FileSystemFunc.h>
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
                class Read::File
                {
                    DJV_NON_COPYABLE(File);

                    File()
                    {
                        memset(&jpeg, 0, sizeof(jpeg_decompress_struct));
                    }

                public:
                    ~File()
                    {
                        if (jpegInit)
                        {
                            jpeg_destroy_decompress(&jpeg);
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

                    FILE*                  f         = nullptr;
                    jpeg_decompress_struct jpeg;
                    bool                   jpegInit  = false;
                    JPEGErrorStruct        jpegError;
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
                    auto f = File::create();
                    return _open(fileName, f);
                }

                namespace
                {
                    bool jpegScanline(
                        jpeg_decompress_struct* jpeg,
                        uint8_t*                out,
                        JPEGErrorStruct*        error)
                    {
                        if (::setjmp(error->jump))
                        {
                            return false;
                        }
                        JSAMPROW p[] = { (JSAMPLE*)(out) };
                        if (!jpeg_read_scanlines(jpeg, p, 1))
                        {
                            return false;
                        }
                        return true;
                    }

                    bool jpegEnd(
                        jpeg_decompress_struct* jpeg,
                        JPEGErrorStruct*        error)
                    {
                        if (::setjmp(error->jump))
                        {
                            return false;
                        }
                        jpeg_finish_decompress(jpeg);
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
                        if (!jpegScanline(&f->jpeg, out->getData(y), &f->jpegError))
                        {
                            std::vector<std::string> messages;
                            messages.push_back(String::Format("{0}: {1}").
                                arg(fileName).
                                arg(_textSystem->getText(DJV_TEXT("error_read_scanline"))));
                            for (const auto& i : f->jpegError.messages)
                            {
                                messages.push_back(i);
                            }
                            throw FileSystem::Error(String::join(messages, ' '));
                        }
                    }
                    if (!jpegEnd(&f->jpeg, &f->jpegError))
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

                    return out;
                }

                namespace
                {
                    bool jpegInit(
                        jpeg_decompress_struct* jpeg,
                        JPEGErrorStruct*        error)
                    {
                        if (::setjmp(error->jump))
                        {
                            return false;
                        }
                        jpeg_create_decompress(jpeg);
                        return true;
                    }

                    bool jpegOpen(
                        FILE*                   f,
                        jpeg_decompress_struct* jpeg,
                        JPEGErrorStruct*        error)
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

                Info Read::_open(const std::string& fileName, const std::shared_ptr<File>& f)
                {
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
                    f->f = FileSystem::fopen(fileName, "rb");
                    if (!f->f)
                    {
                        throw FileSystem::Error(String::Format("{0}: {1}").
                            arg(fileName).
                            arg(_textSystem->getText(DJV_TEXT("error_file_open"))));
                    }
                    if (!jpegOpen(f->f, &f->jpeg, &f->jpegError))
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

                    Image::Type imageType = Image::getIntType(f->jpeg.out_color_components, 8);
                    if (Image::Type::None == imageType)
                    {
                        throw FileSystem::Error(String::Format("{0}: {1}").
                            arg(fileName).
                            arg(_textSystem->getText(DJV_TEXT("error_unsupported_color_components"))));
                    }
                    Info info;
                    info.fileName = fileName;
                    info.videoSpeed = _speed;
                    info.videoSequence = _sequence;
                    info.video.push_back(Image::Info(f->jpeg.output_width, f->jpeg.output_height, imageType));

                    const jpeg_saved_marker_ptr marker = f->jpeg.marker_list;
                    if (marker)
                    {
                        info.tags.set("Description", std::string((const char*)marker->data, marker->data_length));
                    }

                    return info;
                }

            } // namespace JPEG
        } // namespace IO
    } // namespace AV
} // namespace djv

