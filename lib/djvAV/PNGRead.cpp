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

#include <sstream>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace PNG
            {
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

                void Read::_init(
                    const std::string & fileName,
                    const std::shared_ptr<ReadQueue> & queue,
                    const std::shared_ptr<Core::Context> & context)
                {
                    IRead::_init(fileName, queue, context);
                    _thread = std::thread(
                        [this, fileName]
                    {
                        std::shared_ptr<Image> image;
                        try
                        {
                            _open(fileName);
                            image = Image::create(_info);
                            for (int y = 0; y < _info.getHeight(); ++y)
                            {
                                if (!pngScanline(_png, image->getData(y)))
                                {
                                    std::stringstream s;
                                    s << pluginName << " cannot read: " << fileName << ": " << _pngError.msg;
                                    throw std::runtime_error(s.str());
                                }
                            }
                            pngEnd(_png, _pngInfoEnd);
                        }
                        catch (const Core::Error & error)
                        {
                            _hasError = true;
                            _error = error;
                        }
                        _close();
                        if (image)
                        {
                            std::lock_guard<std::mutex> lock(_queue->getMutex());
                            _queue->addVideo(0, image);
                        }
                    });
                    if (_hasError)
                    {
                        throw _error;
                    }
                }

                Read::Read()
                {}

                Read::~Read()
                {
                    _thread.join();
                }

                std::shared_ptr<Read> Read::create(const std::string & fileName, const std::shared_ptr<ReadQueue> & queue, const std::shared_ptr<Core::Context> & context)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_init(fileName, queue, context);
                    return out;
                }

                std::future<Info> Read::getInfo()
                {
                    return _infoPromise.get_future();
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

                void Read::_open(const std::string& fileName)
                {
                    _png = png_create_read_struct(
                        PNG_LIBPNG_VER_STRING,
                        &_pngError,
                        djvPngError,
                        djvPngWarning);
                    if (!_png)
                    {
                        std::stringstream s;
                        s << pluginName << " cannot open: " << fileName << ": " << _pngError.msg;
                        throw std::runtime_error(s.str());
                    }

                    _f = fopen(fileName.c_str(), "rb");
                    if (!_f)
                    {
                        std::stringstream s;
                        s << pluginName << " cannot open: " << fileName;
                        throw std::runtime_error(s.str());
                    }
                    if (!pngOpen(_f, _png, &_pngInfo, &_pngInfoEnd))
                    {
                        std::stringstream s;
                        s << pluginName << " cannot open: " << fileName << ": " << _pngError.msg;
                        throw std::runtime_error(s.str());
                    }

                    const glm::ivec2 size(
                        png_get_image_width(_png, _pngInfo),
                        png_get_image_height(_png, _pngInfo));
                    int channels = png_get_channels(_png, _pngInfo);
                    if (png_get_color_type(_png, _pngInfo) == PNG_COLOR_TYPE_PALETTE)
                    {
                        channels = 3;
                    }
                    if (png_get_valid(_png, _pngInfo, PNG_INFO_tRNS))
                    {
                        ++channels;
                    }
                    int bitDepth = png_get_bit_depth(_png, _pngInfo);
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
                    _info = Pixel::Info(size, pixelType);
                    _infoPromise.set_value(Info(fileName, VideoInfo(_info), AudioInfo()));

                    if (bitDepth >= 16 && Core::Memory::Endian::LSB == Core::Memory::getEndian())
                    {
                        png_set_swap(_png);
                    }
                }

                void Read::_close()
                {
                    if (_png || _pngInfo || _pngInfoEnd)
                    {
                        png_destroy_read_struct(
                            _png ? &_png : nullptr,
                            _pngInfo ? &_pngInfo : nullptr,
                            _pngInfoEnd ? &_pngInfoEnd : nullptr);
                        _png = nullptr;
                        _pngInfo = nullptr;
                        _pngInfoEnd = nullptr;
                    }
                    if (_f)
                    {
                        fclose(_f);
                        _f = nullptr;
                    }
                }

            } // namespace FFmpeg
        } // namespace IO
    } // namespace AV
} // namespace djv

