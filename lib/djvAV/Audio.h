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

//#include <AL/al.h>
//#include <AL/alc.h>
//#include <AL/alext.h>
#include <RtAudio.h>

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
                S8,
                S16,
                S32,
                F32,
                F64,

                Count
            };
            DJV_ENUM_HELPERS(Type);

            typedef int8_t   S8_T;
            typedef int16_t S16_T;
            typedef int32_t S32_T;
            typedef float   F32_T;
            typedef double  F64_T;

            const S8_T   S8Min = std::numeric_limits<int8_t>::min();
            const S8_T   S8Max = std::numeric_limits<int8_t>::max();
            const S16_T S16Min = std::numeric_limits<int16_t>::min();
            const S16_T S16Max = std::numeric_limits<int16_t>::max();
            const S32_T S32Min = std::numeric_limits<int32_t>::min();
            const S32_T S32Max = std::numeric_limits<int32_t>::max();
            const F32_T F32Min = -1.F;
            const F32_T F32Max =  1.F;
            const F64_T F64Min = -1.0;
            const F64_T F64Max =  1.0;

            uint8_t getByteCount(Type);
            Type getIntType(uint8_t);
            Type getFloatType(uint8_t);

            //ALenum getALType(uint8_t channels, Type);
            //std::string getALErrorString(ALenum);
            RtAudioFormat toRtAudio(Type);

            void S8ToS16(S8_T, S16_T&);
            void S8ToS32(S8_T, S32_T&);
            void S8ToF32(S8_T, F32_T &);
            void S8ToF64(S8_T, F64_T &);

            void S16ToS8(S16_T, S8_T&);
            void S16ToS32(S16_T, S32_T&);
            void S16ToF32(S16_T, F32_T &);
            void S16ToF64(S16_T, F64_T &);

            void S32ToS8(S32_T, S8_T&);
            void S32ToS16(S32_T, S16_T&);
            void S32ToF32(S32_T, F32_T&);
            void S32ToF64(S32_T, F64_T&);

            void F32ToS8(F32_T, S8_T&);
            void F32ToS16(F32_T, S16_T&);
            void F32ToS32(F32_T, S32_T&);
            void F32ToS32(F32_T, F64_T&);

            void F64ToS8(F64_T, S8_T&);
            void F64ToS16(F64_T, S16_T&);
            void F64ToS32(F64_T, S32_T&);
            void F64ToS32(F64_T, F32_T&);

        } // namespace Audio
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::Audio::Type);

} // namespace djv

#include <djvAV/AudioInline.h>
