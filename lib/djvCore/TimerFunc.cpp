// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCore/Timer.h>

#include <algorithm>
#include <array>
#include <sstream>

namespace djv
{
    namespace Core
    {
        namespace Time
        {
            size_t getValue(TimerValue value)
            {
                const std::array<size_t, static_cast<size_t>(TimerValue::Count)> data =
                {
                    10000,
                    1000,
                    100,
                    10,
                    1
                };
                return data[static_cast<size_t>(value)];
            }

            Duration getTime(TimerValue value)
            {
                return Duration(std::chrono::duration_cast<Duration>(std::chrono::milliseconds(getValue(value))));
            }

            DJV_ENUM_HELPERS_IMPLEMENTATION(TimerValue);

        } // namespace Time
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::Time,
        TimerValue,
        DJV_TEXT("timer_very_slow"),
        DJV_TEXT("timer_slow"),
        DJV_TEXT("timer_medium"),
        DJV_TEXT("timer_fast"),
        DJV_TEXT("timer_very_fast"));

} // namespace djv

