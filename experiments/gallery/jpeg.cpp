//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
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

#include "jpeg.h"

#include <jpeglib.h>

#include <setjmp.h>

using namespace djv;

struct JPEGErrorStruct
{
    struct jpeg_error_mgr pub;
    char msg[JMSG_LENGTH_MAX] = "";
    jmp_buf jump;
};

bool jpegInit(
    jpeg_decompress_struct* jpeg,
    JPEGErrorStruct* error)
{
    if (::setjmp(error->jump))
    {
        return false;
    }
    jpeg_create_decompress(jpeg);
    return true;
}

bool jpegOpen(
    const std::vector<uint8_t>& data,
    jpeg_decompress_struct* jpeg,
    JPEGErrorStruct* error)
{
    if (::setjmp(error->jump))
    {
        return false;
    }
    jpeg_mem_src(jpeg, data.data(), data.size());
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

bool jpegScanline(
    jpeg_decompress_struct* jpeg,
    uint8_t* out,
    JPEGErrorStruct* error)
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
    JPEGErrorStruct* error)
{
    if (::setjmp(error->jump))
    {
        return false;
    }
    jpeg_finish_decompress(jpeg);
    return true;
}

extern "C"
{
    void jpegError(j_common_ptr in)
    {
        auto error = reinterpret_cast<JPEGErrorStruct*>(in->err);
        in->err->format_message(in, error->msg);
        ::longjmp(error->jump, 1);
    }

    void jpegWarning(j_common_ptr in, int level)
    {
        if (level > 0)
        {
            //! \todo Should we send "trace" messages to the debug log?
            return;
        }
        auto error = reinterpret_cast<JPEGErrorStruct*>(in->err);
        in->err->format_message(in, error->msg);
        ::longjmp(error->jump, 1);
    }

} // extern "C"

std::shared_ptr<AV::Image::Image> jpegRead(const std::vector<uint8_t>& data)
{
    std::shared_ptr<AV::Image::Image> out;

    jpeg_decompress_struct jpeg;
    bool _jpegInit   = false;
    JPEGErrorStruct _jpegError;
    jpeg.err = jpeg_std_error(&_jpegError.pub);
    _jpegError.pub.error_exit = jpegError;
    _jpegError.pub.emit_message = jpegWarning;
    _jpegError.msg[0] = 0;

    std::string error;
    try
    {
        if (!jpegInit(&jpeg, &_jpegError))
        {
            std::stringstream s;
            s << "The JPEG file cannot be opened. " << _jpegError.msg;
            throw std::runtime_error(s.str());
        }
        _jpegInit = true;

        if (!jpegOpen(data, &jpeg, &_jpegError))
        {
            std::stringstream s;
            s << "The JPEG file cannot be opened. " << _jpegError.msg;
            throw std::runtime_error(s.str());
        }

        AV::Image::Type imageType = AV::Image::getIntType(jpeg.out_color_components, 8);
        if (AV::Image::Type::None == imageType)
        {
            std::stringstream s;
            s << "The JPEG file cannot be opened.";
            throw std::runtime_error(s.str());
        }
        const AV::Image::Info info(jpeg.output_width, jpeg.output_height, imageType);
        out = AV::Image::Image::create(info);

        for (uint16_t y = 0; y < info.size.h; ++y)
        {
            if (!jpegScanline(&jpeg, out->getData(y), &_jpegError))
            {
                std::stringstream s;
                s << "The JPEG file cannot be read. " << _jpegError.msg;
                throw std::runtime_error(s.str());
            }
        }
        if (!jpegEnd(&jpeg, &_jpegError))
        {
            std::stringstream s;
            s << "The JPEG file cannot be read. " << _jpegError.msg;
            throw std::runtime_error(s.str());
        }
    }
    catch (const std::exception& e)
    {
        error = e.what();
    }

    if (_jpegInit)
    {
        jpeg_destroy_decompress(&jpeg);
        _jpegInit = false;
    }

    if (!error.empty())
    {
        throw std::runtime_error(error);
    }

    return out;
}

