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

#pragma once

#include <djvAV/Pixel.h>

#include <djvCore/PicoJSON.h>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            //! This class provides a color.
            class Color
            {
            public:
                Color();
                explicit Color(Type);
                Color(int r, int g, int b, int a = U8Range.max);
                Color(F32_T r, F32_T g, F32_T b, F32_T a = F32Range.max);

                Type getType() const;
                bool isValid() const;

                U8_T  getU8(size_t channel) const;
                U10_T getU10(size_t channel) const;
                U16_T getU16(size_t channel) const;
                U32_T getU32(size_t channel) const;
                F16_T getF16(size_t channel) const;
                F32_T getF32(size_t channel) const;
                void setU8(U8_T, size_t channel);
                void setU10(U10_T, size_t channel);
                void setU16(U16_T, size_t channel);
                void setU32(U32_T, size_t channel);
                void setF16(F16_T, size_t channel);
                void setF32(F32_T, size_t channel);

                const uint8_t * getData() const;
                uint8_t * getData();
                void setData(const uint8_t*);

                void zero();

                Color convert(Type) const;

                static void rgbToHSV(const float[3], float[3]);
                static void hsvToRGB(const float[3], float[3]);

                bool operator == (const Color &) const;
                bool operator != (const Color &) const;

            private:
                Type _type = Type::None;
                std::vector<uint8_t> _data;
            };

        } // namespace Image
    } // namespace AV

    picojson::value toJSON(const AV::Image::Color&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, AV::Image::Color&);

    std::ostream & operator << (std::ostream &, const AV::Image::Color &);
    std::istream & operator >> (std::istream &, AV::Image::Color &);

} // namespace djv

#include <djvAV/ColorInline.h>
