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

#include <djvAV/JPEG.h>

#include <djvCore/Context.h>
#include <djvCore/FileIO.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace JPEG
            {
                struct Read::File
                {
                    File()
                    {
                        memset(&jpeg, 0, sizeof(jpeg_decompress_struct));
                    }

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

                    FILE *                 f         = nullptr;
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
                    const std::string & fileName,
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_init(fileName, resourceSystem, logSystem);
                    return out;
                }

                Info Read::_readInfo(const std::string & fileName)
                {
                    File f;
                    return _open(fileName, f);
                }

                namespace
                {
                    bool jpegScanline(
                        jpeg_decompress_struct * jpeg,
                        uint8_t *                out,
                        JPEGErrorStruct *        error)
                    {
                        if (::setjmp(error->jump))
                        {
                            return false;
                        }
                        JSAMPROW p[] = { (JSAMPLE *)(out) };
                        if (!jpeg_read_scanlines(jpeg, p, 1))
                        {
                            return false;
                        }
                        return true;
                    }

                    bool jpegEnd(
                        jpeg_decompress_struct * jpeg,
                        JPEGErrorStruct *        error)
                    {
                        if (::setjmp(error->jump))
                        {
                            return false;
                        }
                        jpeg_finish_decompress(jpeg);
                        return true;
                    }

                } // namespace

                std::shared_ptr<Image::Image> Read::_readImage(const std::string & fileName)
                {
                    std::shared_ptr<Image::Image> out;
                    File f;
                    const auto info = _open(fileName, f);
                    if (info.video.size())
                    {
                        out = Image::Image::create(info.video[0].info);
                        for (int y = 0; y < info.video[0].info.size.y; ++y)
                        {
                            if (!jpegScanline(&f.jpeg, out->getData(y), &f.jpegError))
                            {
                                std::stringstream s;
                                s << DJV_TEXT("The JPEG file") <<
                                    " '" << fileName << "' " << DJV_TEXT("cannot be opened") << ". " << f.jpegError.msg;
                                throw std::runtime_error(s.str());
                            }
                        }
                        if (!jpegEnd(&f.jpeg, &f.jpegError))
                        {
                            std::stringstream s;
                            s << DJV_TEXT("The JPEG file") <<
                                " '" << fileName << "' " << DJV_TEXT("cannot be read") << ". " << f.jpegError.msg;
                            throw std::runtime_error(s.str());
                        }
                    }
                    return out;
                }

                namespace
                {
                    bool jpegInit(
                        jpeg_decompress_struct * jpeg,
                        JPEGErrorStruct *        error)
                    {
                        if (::setjmp(error->jump))
                        {
                            return false;
                        }
                        jpeg_create_decompress(jpeg);
                        return true;
                    }

                    bool jpegOpen(
                        FILE *                   f,
                        jpeg_decompress_struct * jpeg,
                        JPEGErrorStruct *        error)
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

                Info Read::_open(const std::string & fileName, File & f)
                {
                    f.jpeg.err = jpeg_std_error(&f.jpegError.pub);
                    f.jpegError.pub.error_exit = djvJPEGError;
                    f.jpegError.pub.emit_message = djvJPEGWarning;
                    f.jpegError.msg[0] = 0;
                    if (!jpegInit(&f.jpeg, &f.jpegError))
                    {
                        std::stringstream s;
                        s << DJV_TEXT("The JPEG file") <<
                            " '" << fileName << "' " << DJV_TEXT("cannot be opened") << ". " << f.jpegError.msg;
                        throw std::runtime_error(s.str());
                    }
                    f.jpegInit = true;

                    f.f = FileSystem::fopen(fileName, "rb");
                    if (!f.f)
                    {
                        std::stringstream s;
                        s << DJV_TEXT("The JPEG file") <<
                            " '" << fileName << "' " << DJV_TEXT("cannot be opened") << ".";
                        throw std::runtime_error(s.str());
                    }
                    if (!jpegOpen(f.f, &f.jpeg, &f.jpegError))
                    {
                        std::stringstream s;
                        s << DJV_TEXT("The JPEG file") <<
                            " '" << fileName << "' " << DJV_TEXT("cannot be opened") << ". " << f.jpegError.msg;
                        throw std::runtime_error(s.str());
                    }

                    const glm::ivec2 size = glm::ivec2(f.jpeg.output_width, f.jpeg.output_height);
                    Image::Type imageType = Image::getIntType(f.jpeg.out_color_components, 8);
                    if (Image::Type::None == imageType)
                    {
                        std::stringstream s;
                        s << DJV_TEXT("The JPEG file") <<
                            " '" << fileName << "' " << DJV_TEXT("cannot be opened") << ".";
                        throw std::runtime_error(s.str());
                    }
                    auto info = Info(fileName, VideoInfo(Image::Info(size, imageType), _speed, _duration));

                    const jpeg_saved_marker_ptr marker = f.jpeg.marker_list;
                    if (marker)
                    {
                        info.tags.setTag("Description", std::string((const char *)marker->data, marker->data_length));
                    }

                    return info;
                }

            } // namespace JPEG
        } // namespace IO
    } // namespace AV
} // namespace djv

