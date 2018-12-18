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

#pragma once

#include <djvAV/Pixel.h>

namespace djv
{
    namespace AV
    {
        class Color
        {
        public:
            inline Color();
            inline Color(Pixel::Type);
            inline Color(int r, int g, int b, int a = Pixel::U8Max);
            inline Color(Pixel::F32_T r, Pixel::F32_T g, Pixel::F32_T b, Pixel::F32_T a = Pixel::F32Max);

            inline Pixel::Type getType() const;
            inline bool isValid() const;

            inline Pixel::U8_T  getU8 (size_t channel) const;
            inline Pixel::U10_T getU10(size_t channel) const;
            inline Pixel::U16_T getU16(size_t channel) const;
            inline Pixel::U32_T getU32(size_t channel) const;
            inline Pixel::F16_T getF16(size_t channel) const;
            inline Pixel::F32_T getF32(size_t channel) const;
            inline void setU8 (Pixel::U8_T,  size_t channel);
            inline void setU10(Pixel::U10_T, size_t channel);
            inline void setU16(Pixel::U16_T, size_t channel);
            inline void setU32(Pixel::U32_T, size_t channel);
            inline void setF16(Pixel::F16_T, size_t channel);
            inline void setF32(Pixel::F32_T, size_t channel);

            inline const uint8_t * getData() const;
            inline uint8_t * getData();

            void zero();
            Color convert(Pixel::Type) const;

            bool operator == (const Color &) const;
            bool operator != (const Color &) const;

        private:
            Pixel::Type _type = Pixel::Type::None;
            std::vector<uint8_t> _data;
        };

    } // namespace AV

    std::ostream & operator << (std::ostream &, const AV::Color &);
    std::istream & operator >> (std::istream &, AV::Color &);

} // namespace djv

#include <djvAV/ColorInline.h>
