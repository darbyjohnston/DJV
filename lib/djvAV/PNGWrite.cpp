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

#include <djvAV/PNG.h>

#include <djvCore/FileIO.h>

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
                    const FileSystem::FileInfo& fileInfo,
                    const Info & info,
                    const WriteOptions& writeOptions,
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Write>(new Write);
                    out->_init(fileInfo, info, writeOptions, resourceSystem, logSystem);
                    return out;
                }

                namespace
                {
                    struct File
                    {
                        File()
                        {
                            memset(&pngError, 0, sizeof(ErrorStruct));
                        }

                        File(File&& other) noexcept :
                            f(other.f),
                            png(other.png),
                            pngInfo(other.pngInfo),
                            pngError(std::move(other.pngError))
                        {}

                        ~File()
                        {
                            if (png || pngInfo)
                            {
                                png_destroy_write_struct(
                                    png     ? &png     : nullptr,
                                    pngInfo ? &pngInfo : nullptr);
                                png     = nullptr;
                                pngInfo = nullptr;
                            }
                            if (f)
                            {
                                fclose(f);
                                f = nullptr;
                            }
                        }

                        File& operator = (File&& other) noexcept
                        {
                            if (this != &other)
                            {
                                f = other.f;
                                png = other.png;
                                pngInfo = other.pngInfo;
                                pngError = std::move(other.pngError);
                            }
                            return *this;
                        }

                        bool open(const std::string& fileName)
                        {
                            f = FileSystem::fopen(fileName.c_str(), "wb");
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
#if defined(DJV_OPENGL_ES2)
                        case GL_LUMINANCE:       colorType = PNG_COLOR_TYPE_GRAY;       break;
                        case GL_LUMINANCE_ALPHA: colorType = PNG_COLOR_TYPE_GRAY_ALPHA; break;
#else // DJV_OPENGL_ES2
                        case GL_RED:             colorType = PNG_COLOR_TYPE_GRAY;       break;
                        case GL_RG:              colorType = PNG_COLOR_TYPE_GRAY_ALPHA; break;
#endif // DJV_OPENGL_ES2
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
                        return true;
                    }

                    bool pngScanline(png_structp png, const uint8_t * in)
                    {
                        if (setjmp(png_jmpbuf(png)))
                            return false;
                        png_write_row(png, (png_byte *)in);
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

                void Write::_write(const std::string & fileName, const std::shared_ptr<Image::Image> & image)
                {
                    File f;
                    f.png = png_create_write_struct(
                        PNG_LIBPNG_VER_STRING,
                        &f.pngError,
                        djvPngError,
                        djvPngWarning);
                    if (!f.png)
                    {
                        throw std::runtime_error(f.pngError.msg);
                    }
                    if (!f.open(fileName))
                    {
                        throw std::runtime_error(DJV_TEXT("Cannot open file."));
                    }
                    const auto& info = image->getInfo();
                    if (!pngOpen(f.f, f.png, &f.pngInfo, info))
                    {
                        throw std::runtime_error(f.pngError.msg);
                    }
                    if (Image::getBitDepth(info.type) > 8 && Memory::Endian::LSB == Memory::getEndian())
                    {
                        png_set_swap(f.png);
                    }

                    for (uint16_t y = 0; y < info.size.h; ++y)
                    {
                        if (!pngScanline(f.png, image->getData(y)))
                        {
                            throw std::runtime_error(f.pngError.msg);
                        }
                    }
                    if (!pngEnd(f.png, f.pngInfo))
                    {
                        throw std::runtime_error(f.pngError.msg);
                    }
                }

            } // namespace PNG
        } // namespace IO
    } // namespace AV
} // namespace djv

