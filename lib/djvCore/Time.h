// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <chrono>

namespace djv
{
    namespace Core
    {
        //! This namespace provides time functionality.
        namespace Time
        {
            //! This typedef provides the default time duration.
            typedef std::chrono::microseconds Duration;

            //! This typedef provides the default time point.
            typedef std::chrono::steady_clock::time_point TimePoint;

            // This is the same timebase as FFmpeg (libavutil.h/avutil.h).
            const int64_t timebase = 1000000;

            //! This enumeration provides time units.
            enum class Units
            {
                Timecode,
                Frames,

                Count,
                First = Timecode
            };

        } // namespace Time
    } // namespace Core
} // namespace djv

