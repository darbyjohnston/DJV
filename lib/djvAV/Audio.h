// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/AV.h>

#include <djvCore/Range.h>

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

            typedef int8_t   S8_T;
            typedef int16_t S16_T;
            typedef int32_t S32_T;
            typedef float   F32_T;
            typedef double  F64_T;

            const Core::Math::Range<S8_T> S8Range(
                std::numeric_limits<S8_T>::min(),
                std::numeric_limits<S8_T>::max());

            const Core::Math::Range<S16_T> S16Range(
                std::numeric_limits<S16_T>::min(),
                std::numeric_limits<S16_T>::max());

            const Core::Math::Range<S32_T> S32Range(
                std::numeric_limits<S32_T>::min(),
                std::numeric_limits<S32_T>::max());

            const Core::Math::Range<F32_T> F32Range(-1.F, 1.F);
            const Core::Math::Range<F64_T> F64Range(-1.F, 1.F);

        } // namespace Audio
    } // namespace AV
} // namespace djv
