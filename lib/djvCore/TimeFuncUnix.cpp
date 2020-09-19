// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCore/TimeFunc.h>

namespace djv
{
    namespace Core
    {
        namespace Time
        {
            void sleep(const Duration& value)
            {
                const auto microseconds = value.count();
                const auto seconds = microseconds / 1000000;
                struct timespec t;
                t.tv_sec  = seconds;
                t.tv_nsec = (microseconds - (seconds * 1000000)) * 1000;
                struct timespec out;
                nanosleep(&t, &out);
            }
            
        } // namespace Time
    } // namespace Core
} // namespace djv

