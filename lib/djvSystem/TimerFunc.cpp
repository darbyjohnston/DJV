// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvSystem/TimerFunc.h>

#include <algorithm>
#include <array>
#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace System
    {
        size_t getTimerValue(TimerValue value)
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

        Time::Duration getTimerDuration(TimerValue value)
        {
            return Time::Duration(std::chrono::duration_cast<Time::Duration>(
                std::chrono::milliseconds(getTimerValue(value))));
        }

        DJV_ENUM_HELPERS_IMPLEMENTATION(TimerValue);

    } // namespace System

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        System,
        TimerValue,
        DJV_TEXT("timer_very_slow"),
        DJV_TEXT("timer_slow"),
        DJV_TEXT("timer_medium"),
        DJV_TEXT("timer_fast"),
        DJV_TEXT("timer_very_fast"));

} // namespace djv

