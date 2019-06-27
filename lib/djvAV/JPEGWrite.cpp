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
                struct Write::Private
                {
                    Settings settings;
                };

                Write::Write() :
                    _p(new Private)
                {}

                Write::~Write()
                {
                    _finish();
                }

                std::shared_ptr<Write> Write::create(
                    const std::string& fileName,
                    const Settings& settings,
                    const Info& info,
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Write>(new Write);
                    out->_p->settings = settings;
                    out->_init(fileName, info, resourceSystem, logSystem);
                    return out;
                }

                namespace
                {
                    struct File
                    {
                        File()
                        {
                            memset(&jpeg, 0, sizeof(jpeg_compress_struct));
                        }

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

                        FILE* f = nullptr;
                        jpeg_compress_struct jpeg;
                        bool                 jpegInit = false;
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
                        const Settings& settings,
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
                        jpeg_set_quality(jpeg, settings.quality, static_cast<boolean>(1));
                        jpeg_start_compress(jpeg, static_cast<boolean>(1));
                        std::string tag = tags.getTag("Description");
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

                void Write::_write(const std::string & fileName, const std::shared_ptr<Image::Image> & image)
                {
                    File f;
                    f.jpeg.err = jpeg_std_error(&f.jpegError.pub);
                    f.jpegError.pub.error_exit = djvJPEGError;
                    f.jpegError.pub.emit_message = djvJPEGWarning;
                    f.jpegError.msg[0] = 0;
                    if (!jpegInit(&f.jpeg, &f.jpegError))
                    {
                        std::stringstream s;
                        s << pluginName << " " << DJV_TEXT("The file") <<
                            " '" << fileName << "' " << DJV_TEXT("cannot be opened") << ". " << f.jpegError.msg;
                        throw std::runtime_error(s.str());
                    }
                    f.jpegInit = true;

                    f.f = FileSystem::fopen(fileName.c_str(), "wb");
                    if (!f.f)
                    {
                        std::stringstream s;
                        s << pluginName << " " << DJV_TEXT("The file") <<
                            " '" << fileName << "' " << DJV_TEXT("cannot be opened") << ".";
                        throw std::runtime_error(s.str());
                    }

                    const auto& info = image->getInfo();
                    if (!jpegOpen(f.f, &f.jpeg, info, _info.tags, _p->settings, &f.jpegError))
                    {
                        std::stringstream s;
                        s << pluginName << " " << DJV_TEXT("The file") <<
                            " '" << fileName << "' " << DJV_TEXT("cannot be opened") << ". " << f.jpegError.msg;
                        throw std::runtime_error(s.str());
                    }

                    const uint16_t h = image->getHeight();
                    for (uint16_t y = 0; y < h; ++y)
                    {
                        if (!jpegScanline(&f.jpeg, image->getData(y), &f.jpegError))
                        {
                            std::stringstream s;
                            s << pluginName << " " << DJV_TEXT("The file") <<
                                " '" << fileName << "' " << DJV_TEXT("cannot be written") << ". " << f.jpegError.msg;
                            throw std::runtime_error(s.str());
                        }
                    }

                    if (!jpeg_end(&f.jpeg, &f.jpegError))
                    {
                        std::stringstream s;
                        s << pluginName << " " << DJV_TEXT("The file") <<
                            " '" << fileName << "' " << DJV_TEXT("cannot be written") << ". " << f.jpegError.msg;
                        throw std::runtime_error(s.str());
                    }
                }

            } // namespace JPEG
        } // namespace IO
    } // namespace AV
} // namespace djv

