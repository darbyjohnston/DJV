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

#include <djvAV/Pixel.h>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            void convert(const Pixel & inPixel, const void * in, const Pixel & outPixel, void * out)
            {

            }

        } // namespace Image
    } // namespace AV

    namespace
    {
        const std::map<GLenum, std::string> formatLabel =
        {
            { GL_NONE, DJV_TEXT("None") },
            { GL_RED,  DJV_TEXT("L") },
            { GL_RG,   DJV_TEXT("LA") },
            { GL_RGB,  DJV_TEXT("RGB") },
            { GL_RGBA, DJV_TEXT("RGBA") }
        };
        const std::map<GLenum, std::string> typeLabel =
        {
            { GL_NONE,                    DJV_TEXT("None") },
            { GL_UNSIGNED_BYTE,           DJV_TEXT("U8") },
            { GL_UNSIGNED_INT_10_10_10_2, DJV_TEXT("U10") },
            { GL_UNSIGNED_SHORT,          DJV_TEXT("U16") },
            { GL_UNSIGNED_INT,            DJV_TEXT("U32") },
            { GL_HALF_FLOAT,              DJV_TEXT("F16") },
            { GL_FLOAT,                   DJV_TEXT("F32") }
        };

    } // namespace

    std::ostream & operator << (std::ostream & os, AV::Image::Pixel value)
    {
        const auto i = formatLabel.find(value.getFormat());
        if (i != formatLabel.end())
        {
            const auto j = typeLabel.find(value.getType());
            if (j != typeLabel.end())
            {
                os << i->second << " " << j->second;
            }
        }
        return os;
    }

    std::istream & operator >> (std::istream & is, AV::Image::Pixel & value)
    {
        GLenum format = GL_NONE;
        GLenum type = GL_NONE;
        bool error = true;
        std::string s;
        is >> s;
        for (auto i : formatLabel)
        {
            if (s == i.second)
            {
                format = i.first;
                is >> s;
                for (auto j : typeLabel)
                {
                    if (s == j.second)
                    {
                        type = j.first;
                        error = false;
                        break;
                    }
                }
                break;
            }
        }
        if (error)
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse pixel"));
        }
        value = AV::Image::Pixel(format, type);
        return is;
    }

} // namespace djv

