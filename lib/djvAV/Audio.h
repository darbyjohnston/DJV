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

#include <djvAV/AV.h>

#include <djvCore/Enum.h>

#include <AL/al.h>
#include <AL/alc.h>

#include <limits>

namespace djv
{
    namespace AV
    {
        //! This namespace provides audio functionality.
        namespace Audio
        {
            //! This enumeration provides the audio data types.
            enum class Type
            {
                None,
                U8,
                S16,
                S32,
                F32,

                Count
            };
            DJV_ENUM_HELPERS(Type);

            typedef uint8_t  U8_T;
            typedef int16_t S16_T;
            typedef int32_t S32_T;
            typedef float   F32_T;

            const U8_T   U8Min = std::numeric_limits<uint8_t>::min();
            const U8_T   U8Max = std::numeric_limits<uint8_t>::max();
            const S16_T S16Min = std::numeric_limits<int16_t>::min();
            const S16_T S16Max = std::numeric_limits<int16_t>::max();
            const S32_T S32Min = std::numeric_limits<int32_t>::min();
            const S32_T S32Max = std::numeric_limits<int32_t>::max();
            const F32_T F32Min = -1.f;
            const F32_T F32Max =  1.f;

            inline size_t getByteCount(Type);
            inline Type getIntType(size_t);
            inline Type getFloatType(size_t);

            inline ALenum getALType(size_t channels, Type);
            std::string getALErrorString(ALenum);

            inline void U8ToS16(U8_T, S16_T &);
            inline void U8ToS32(U8_T, S32_T &);
            inline void U8ToF32(U8_T, F32_T &);

            inline void S16ToU8(S16_T, U8_T &);
            inline void S16ToS32(S16_T, S32_T &);
            inline void S16ToF32(S16_T, F32_T &);

            inline void S32ToU8(S32_T, U8_T &);
            inline void S32ToS16(S32_T, S16_T &);
            inline void S32ToF32(S32_T, F32_T &);

            inline void F32ToU8(F32_T, U8_T &);
            inline void F32ToS16(F32_T, S16_T &);
            inline void F32ToS32(F32_T, S32_T &);

        } // namespace Audio
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::Audio::Type);

} // namespace djv

#include <djvAV/AudioInline.h>
