//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvCore/Context.h>
#include <djvCore/FileIO.h>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace PNG
            {
                struct Read::Private
                {
                    std::string fileName;
                    Pixel::Info info;
                    FILE * f = nullptr;
                    png_structp png = nullptr;
                    png_infop pngInfo = nullptr;
                    png_infop pngInfoEnd = nullptr;
                    ErrorStruct pngError;
                };

                Read::Read() :
                    _p(new Private)
                {}

                Read::~Read()
                {}

                std::shared_ptr<Read> Read::create(const std::string & fileName, const std::shared_ptr<Queue> & queue, const std::shared_ptr<Core::Context> & context)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_init(fileName, queue, context);
                    return out;
                }

                namespace
                {
                    bool pngOpen(
                        FILE*       f,
                        png_structp png,
                        png_infop*  pngInfo,
                        png_infop*  pngInfoEnd)
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
                        return true;
                    }

                } // namespace

                Info Read::_open(const std::string& fileName, const Speed & speed, Duration duration)
                {
                    DJV_PRIVATE_PTR();
                    p.fileName = fileName;

                    p.png = png_create_read_struct(
                        PNG_LIBPNG_VER_STRING,
                        &p.pngError,
                        djvPngError,
                        djvPngWarning);
                    if (!p.png)
                    {
                        std::stringstream s;
                        s << pluginName << " cannot open: " << fileName << ": " << p.pngError.msg;
                        throw std::runtime_error(s.str());
                    }

                    p.f = Core::fopen(fileName.c_str(), "rb");
                    if (!p.f)
                    {
                        std::stringstream s;
                        s << pluginName << " cannot open: " << fileName;
                        throw std::runtime_error(s.str());
                    }
                    if (!pngOpen(p.f, p.png, &p.pngInfo, &p.pngInfoEnd))
                    {
                        std::stringstream s;
                        s << pluginName << " cannot open: " << fileName << ": " << p.pngError.msg;
                        throw std::runtime_error(s.str());
                    }

                    const glm::ivec2 size(
                        png_get_image_width(p.png, p.pngInfo),
                        png_get_image_height(p.png, p.pngInfo));
                    int channels = png_get_channels(p.png, p.pngInfo);
                    if (png_get_color_type(p.png, p.pngInfo) == PNG_COLOR_TYPE_PALETTE)
                    {
                        channels = 3;
                    }
                    if (png_get_valid(p.png, p.pngInfo, PNG_INFO_tRNS))
                    {
                        ++channels;
                    }
                    int bitDepth = png_get_bit_depth(p.png, p.pngInfo);
                    if (bitDepth < 8)
                    {
                        bitDepth = 8;
                    }
                    Pixel::Type pixelType = Pixel::getIntType(channels, bitDepth);
                    if (Pixel::Type::None == pixelType)
                    {
                        std::stringstream s;
                        s << pluginName << " cannot open: " << fileName;
                        throw std::runtime_error(s.str());
                    }
                    p.info = Pixel::Info(size, pixelType);

                    if (bitDepth >= 16 && Core::Memory::Endian::LSB == Core::Memory::getEndian())
                    {
                        png_set_swap(p.png);
                    }

                    return Info(fileName, VideoInfo(p.info, speed, duration), AudioInfo());
                }

                namespace
                {
                    bool pngScanline(png_structp png, uint8_t* out)
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

                std::shared_ptr<Image> Read::_read()
                {
                    DJV_PRIVATE_PTR();
                    auto out = Image::create(p.info);
                    for (int y = 0; y < p.info.getHeight(); ++y)
                    {
                        if (!pngScanline(p.png, out->getData(y)))
                        {
                            std::stringstream s;
                            s << pluginName << " cannot read: " << p.fileName << ": " << p.pngError.msg;
                            throw std::runtime_error(s.str());
                        }
                    }
                    pngEnd(p.png, p.pngInfoEnd);
                    return out;
                }

                void Read::_close()
                {
                    DJV_PRIVATE_PTR();
                    if (p.png || p.pngInfo || p.pngInfoEnd)
                    {
                        png_destroy_read_struct(
                            p.png ? &p.png : nullptr,
                            p.pngInfo ? &p.pngInfo : nullptr,
                            p.pngInfoEnd ? &p.pngInfoEnd : nullptr);
                        p.png = nullptr;
                        p.pngInfo = nullptr;
                        p.pngInfoEnd = nullptr;
                    }
                    if (p.f)
                    {
                        fclose(p.f);
                        p.f = nullptr;
                    }
                }

            } // namespace PNG
        } // namespace IO
    } // namespace AV
} // namespace djv

