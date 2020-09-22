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
            inline void secondsToTime(double in, int& hour, int& minute, double& seconds)
            {
                hour     = static_cast<int>(in / 3600);
                in      -= static_cast<int64_t>(hour) * static_cast<int64_t>(3600);
                minute   = in / 60;
                in      -= static_cast<int64_t>(minute) * static_cast<int64_t>(60);
                seconds  = in;
            }
            
        } // namespace Time
    } // namespace Core
} // namespace djv
