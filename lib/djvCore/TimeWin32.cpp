// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCore/Time.h>

#if defined(DJV_PLATFORM_WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // DJV_WINDOWS

namespace djv
{
    namespace Core
    {
        namespace Time
        {
            void sleep(const Duration& value)
            {
                if (HANDLE h = CreateWaitableTimer(NULL, TRUE, NULL))
                {
                    LARGE_INTEGER l;
                    l.QuadPart = -std::chrono::duration_cast<std::chrono::nanoseconds>(value).count() / 100;
                    if (SetWaitableTimer(h, &l, 0, NULL, NULL, FALSE))
                    {
                        WaitForSingleObject(h, INFINITE);
                    }
                    CloseHandle(h);
                }
            }
            
        } // namespace Time
    } // namespace Core
} // namespace djv
