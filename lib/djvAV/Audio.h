//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvCore/StringUtil.h>

#include <QMetaType>

#include <AL/al.h>

namespace djv
{
    namespace AV
    {
        //! This class provides audio functionality.
        class Audio
        {
            Q_GADGET

        public:

            //! This enumeration provides the audio data types.
            enum TYPE
            {
                TYPE_NONE,  //!< Invalid
                U8,         //!< 8-bit unsigned
                S16,        //!< 16-bit signed
                S32,        //!< 32-bit signed
                F32,        //!< 32-bit floating-point

                TYPE_COUNT
            };
            Q_ENUM(TYPE);

            typedef uint8_t U8_T;
            typedef int16_t S16_T;
            typedef int32_t S32_T;
            typedef float   F32_T;

            static const U8_T   u8Min;
            static const U8_T   u8Max;
            static const S16_T s16Min;
            static const S16_T s16Max;
            static const S32_T s32Min;
            static const S32_T s32Max;
            static const F32_T f32Min;
            static const F32_T f32Max;

            //! Get the audio data type labels.
            static const QStringList & typeLabels();

            //! Get the number of bytes in a data type.
            static inline int byteCount(TYPE);

            //! Get the integer type from the given number of bytes.
            static inline TYPE intType(size_t);

            //! Get the floating-point type from the given number of bytes.
            static inline TYPE floatType(size_t);

            //! Get the OpenAL format from the given channels and type.
            static inline ALenum toAL(size_t channels, TYPE);

            //! Get an OpenAL error string.
            static QString alErrorString(ALenum);

            static inline void convert( U8_T, S16_T &);
            static inline void convert( U8_T, S32_T &);
            static inline void convert( U8_T, F32_T &);
            static inline void convert(S16_T,  U8_T &);
            static inline void convert(S16_T, S32_T &);
            static inline void convert(S16_T, F32_T &);
            static inline void convert(S32_T,  U8_T &);
            static inline void convert(S32_T, S16_T &);
            static inline void convert(S32_T, F32_T &);
            static inline void convert(F32_T,  U8_T &);
            static inline void convert(F32_T, S16_T &);
            static inline void convert(F32_T, S32_T &);
        };

    } // namespace AV

    DJV_STRING_OPERATOR(AV::Audio::TYPE);

} // namespace djv

#include <djvAV/AudioInline.h>
