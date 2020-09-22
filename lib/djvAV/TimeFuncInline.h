// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace AV
    {
        namespace Time
        {
            inline Math::Rational getTimebaseRational()
            {
                return Math::Rational(1, timebase);
            }
            
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
            
        } // namespace Time
    } // timespace AV
} // namespace djv
