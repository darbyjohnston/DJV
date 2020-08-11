// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/AV.h>

#include <djvCore/Enum.h>
#include <djvCore/Range.h>

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

            const Core::Range::Range<S8_T> S8Range(
                std::numeric_limits<S8_T>::min(),
                std::numeric_limits<S8_T>::max());

            const Core::Range::Range<S16_T> S16Range(
                std::numeric_limits<S16_T>::min(),
                std::numeric_limits<S16_T>::max());

            const Core::Range::Range<S32_T> S32Range(
                std::numeric_limits<S32_T>::min(),
                std::numeric_limits<S32_T>::max());

            const Core::Range::Range<F32_T> F32Range(-1.F, 1.F);
            const Core::Range::Range<F64_T> F64Range(-1.F, 1.F);

            uint8_t getByteCount(Type);
            Type getIntType(uint8_t);
            Type getFloatType(uint8_t);

            //ALenum getALType(uint8_t channels, Type);
            //std::string getALErrorString(ALenum);
            RtAudioFormat toRtAudio(Type) noexcept;

            void S8ToS16(S8_T, S16_T&) noexcept;
            void S8ToS32(S8_T, S32_T&) noexcept;
            void S8ToF32(S8_T, F32_T&) noexcept;
            void S8ToF64(S8_T, F64_T&) noexcept;

            void S16ToS8(S16_T, S8_T&) noexcept;
            void S16ToS32(S16_T, S32_T&) noexcept;
            void S16ToF32(S16_T, F32_T&) noexcept;
            void S16ToF64(S16_T, F64_T&) noexcept;

            void S32ToS8(S32_T, S8_T&) noexcept;
            void S32ToS16(S32_T, S16_T&) noexcept;
            void S32ToF32(S32_T, F32_T&) noexcept;
            void S32ToF64(S32_T, F64_T&) noexcept;

            void F32ToS8(F32_T, S8_T&) noexcept;
            void F32ToS16(F32_T, S16_T&) noexcept;
            void F32ToS32(F32_T, S32_T&) noexcept;
            void F32ToS32(F32_T, F64_T&) noexcept;

            void F64ToS8(F64_T, S8_T&) noexcept;
            void F64ToS16(F64_T, S16_T&) noexcept;
            void F64ToS32(F64_T, S32_T&) noexcept;
            void F64ToS32(F64_T, F32_T&) noexcept;

        } // namespace Audio
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::Audio::Type);

} // namespace djv

#include <djvAV/AudioInline.h>
