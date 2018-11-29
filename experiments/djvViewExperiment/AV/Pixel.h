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

#include <Core/Util.h>

#include <QMetaType>
#include <QOpenGLFunctions_3_3_Core>

namespace djv
{
    namespace AV
    {
        struct U10_S_MSB
        {
            uint32_t r : 10, g : 10, b : 10, pad : 2;

            inline bool operator == (const U10_S_MSB &) const;
            inline bool operator != (const U10_S_MSB &) const;
        };
        struct U10_S_LSB
        {
            uint32_t pad : 2, b : 10, g : 10, r : 10;

            inline bool operator == (const U10_S_LSB &) const;
            inline bool operator != (const U10_S_LSB &) const;
        };
#if defined(DJV_ENDIAN_MSB)
        typedef U10_S_MSB U10_S;
#else
        typedef U10_S_LSB U10_S;
#endif

        class Pixel
        {
            Q_GADGET

        public:
            inline Pixel();
            inline Pixel(GLenum format, GLenum type);

            inline GLenum getFormat() const;
            inline GLenum getType() const;
            inline bool isValid() const;

            inline size_t getChannelCount() const;
            inline size_t getBitDepth() const;
            inline size_t getByteCount() const;

            static inline Pixel getIntPixel(int channels, int bitDepth);
            static inline Pixel getFloatPixel(int channels, int bitDepth);

            static QString getPixelLabel(const Pixel &);

            inline bool operator == (const Pixel &) const;
            inline bool operator != (const Pixel &) const;
            inline bool operator < (const Pixel &) const;

        private:
            GLenum _format = 0;
            GLenum _type   = 0;
        };

    } // namespace AV
} // namespace djv

#include <AV/PixelInline.h>