// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/ErrorFunc.h>

#include <djvCore/StringFunc.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <windows.h>

namespace djv
{
    namespace Core
    {
        namespace Error
        {
            std::string getLastError()
            {
                const DWORD dw = GetLastError();
                TCHAR buf[String::cStringLength];
                FormatMessage(
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    dw,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    buf,
                    String::cStringLength,
                    NULL);
                std::string out = std::string(buf, lstrlen(buf));
                String::removeTrailingNewline(out);
                String::removeTrailingNewline(out);
                return out;
            }

        } // namespace Error
    } // namespace Core
} // namespace djv
