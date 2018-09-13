//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

#include <djvGraphics/Pixel.h>

#include <QMetaType>

namespace djv
{
    namespace Graphics
    {
        //! \class Color
        //!
        //! This class provides a color.
        class Color
        {
        public:
            Color();
            Color(const Color &);
            Color(Pixel::PIXEL);
            Color(Pixel::F32_T l);
            Color(Pixel::F32_T l, Pixel::F32_T a);
            Color(Pixel::F32_T r, Pixel::F32_T g, Pixel::F32_T b);
            Color(Pixel::F32_T r, Pixel::F32_T g, Pixel::F32_T b, Pixel::F32_T a);
            Color(const quint8 *, Pixel::PIXEL);

            //! Get the pixel type.
            inline Pixel::PIXEL pixel() const;

            //! Set the pixel type.
            void setPixel(Pixel::PIXEL);

            //! Get the channel count.
            inline int channels() const;

            //! Get a channel.
            inline Pixel::U8_T u8(int) const;

            //! Get a channel.
            inline Pixel::U10_T u10(int) const;

            //! Get a channel.
            inline Pixel::U16_T u16(int) const;

            //! Get a channel.
            inline Pixel::F16_T f16(int) const;

            //! Get a channel.
            inline Pixel::F32_T f32(int) const;

            //! Set a channel.
            inline void setU8(Pixel::U8_T, int);

            //! Set a channel.
            inline void setU10(Pixel::U10_T, int);

            //! Set a channel.
            inline void setU16(Pixel::U16_T, int);

            //! Set a channel.
            inline void setF16(Pixel::F16_T, int);

            //! Set a channel.
            inline void setF32(Pixel::F32_T, int);

            //! Zero the channels.
            void zero();

            //! Get a pointer to the color data.
            inline quint8 * data();

            //! Get a const pointer to the color data.
            inline const quint8 * data() const;

            Color & operator = (const Color &);

        private:
            void init();

            Pixel::PIXEL _pixel = static_cast<Pixel::PIXEL>(0);
            int          _channels = 0;
            quint8       _data[Pixel::channelsMax * Pixel::bytesMax];
        };

        DJV_COMPARISON_OPERATOR(Color);

    } // namespace Graphics
} // namespace djv

Q_DECLARE_METATYPE(djv::Graphics::Color)

DJV_STRING_OPERATOR(djv::Graphics::Color);

DJV_DEBUG_OPERATOR(djv::Graphics::Color);

#include <djvGraphics/ColorInline.h>

