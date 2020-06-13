// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Core
    {
        namespace Time
        {
            inline void secondsToTime(double in, int& hour, int& minute, double& seconds)
            {
                hour     = static_cast<int>(in / 3600);
                in      -= static_cast<int64_t>(hour) * static_cast<int64_t>(3600);
                minute   = in / 60;
                in      -= static_cast<int64_t>(minute) * static_cast<int64_t>(60);
                seconds  = in;
            }

            inline void timecodeToTime(
                uint32_t in,
                int& hour,
                int& minute,
                int& seconds,
                int& frame)
            {
                hour    = (in >> 28 & 0x0f) * 10 + (in >> 24 & 0x0f);
                minute  = (in >> 20 & 0x0f) * 10 + (in >> 16 & 0x0f);
                seconds = (in >> 12 & 0x0f) * 10 + (in >>  8 & 0x0f);
                frame   = (in >>  4 & 0x0f) * 10 + (in >>  0 & 0x0f);
            }

            inline uint32_t timeToTimecode(
                int hour,
                int minute,
                int seconds,
                int frame)
            {
                return
                    (hour    / 10 & 0x0f) << 28 | (hour    % 10 & 0x0f) << 24 |
                    (minute  / 10 & 0x0f) << 20 | (minute  % 10 & 0x0f) << 16 |
                    (seconds / 10 & 0x0f) << 12 | (seconds % 10 & 0x0f) <<  8 |
                    (frame   / 10 & 0x0f) <<  4 | (frame   % 10 & 0x0f) <<  0;
            }

            inline Frame::Number timecodeToFrame(uint32_t in, const Math::Rational& r)
            {
                if (!r.isValid())
                    return 0;
                int hour    = 0;
                int minute  = 0;
                int seconds = 0;
                int frame   = 0;
                timecodeToTime(in, hour, minute, seconds, frame);
                return (
                    static_cast<int64_t>(hour) * static_cast<int64_t>(60) * static_cast<int64_t>(60) +
                    static_cast<int64_t>(minute) * static_cast<int64_t>(60) +
                    static_cast<int64_t>(seconds)
                    ) *
                    static_cast<double>(r.toFloat()) +
                    static_cast<int64_t>(frame);
            }

            inline uint32_t frameToTimecode(Frame::Number frame, const Math::Rational& r)
            {
                if (!r.isValid())
                    return 0;
                const float f = r.toFloat();
                const int hour = static_cast<int>(frame / (f * 60 * 60));
                frame -= static_cast<Frame::Number>(hour) * static_cast<double>(f) * static_cast<Frame::Number>(60) * static_cast<Frame::Number>(60);
                const int minute = static_cast<int>(frame / (f * 60));
                frame -= static_cast<Frame::Number>(minute) * static_cast<double>(f) * static_cast<Frame::Number>(60);
                const int second = static_cast<int>(frame / f);
                frame -= static_cast<Frame::Number>(second) * static_cast<double>(f);
                return timeToTimecode(hour, minute, second, static_cast<int>(frame));
            }

        } // namespace Time
    } // namespace Core
} // namespace djv
