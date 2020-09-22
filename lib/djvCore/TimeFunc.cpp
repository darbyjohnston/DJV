// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCore/TimeFunc.h>

#include <iomanip>
#include <sstream>

namespace djv
{
    namespace Core
    {
        namespace Time
        {
            std::string getLabel(double value)
            {
                int    hours   = 0;
                int    minutes = 0;
                double seconds = 0.0;
                secondsToTime(value, hours, minutes, seconds);
                std::stringstream ss;
                ss << std::setfill('0') << std::setw(2) << hours;
                ss << std::setw(0) << ":";
                ss << std::setfill('0') << std::setw(2) << minutes;
                ss << std::setw(0) << ":";
                ss << std::setfill('0') << std::setw(2) << static_cast<int>(seconds);
                return ss.str();
            }

            std::string getLabel(time_t value)
            {
                std::tm tm;
                localtime(&value, &tm);
                char buffer[32];
                std::strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", &tm);
                return buffer;
            }

            void localtime(const time_t * t, tm * tm)
            {
#if defined(DJV_PLATFORM_WINDOWS)
                localtime_s(tm, t);
#else // DJV_PLATFORM_WINDOWS
                localtime_r(t, tm);
#endif // DJV_PLATFORM_WINDOWS
            }

        } // namespace Time
    } // namespace Core
} // namespace djv

