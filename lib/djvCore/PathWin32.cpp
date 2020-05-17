// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/Path.h>

#include <djvCore/FileInfo.h>
#include <djvCore/String.h>
#include <djvCore/StringFormat.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <windows.h>
#include <direct.h>

#include <codecvt>
#include <locale>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            void Path::mkdir(const Path & value)
            {
                if (_wmkdir(String::toWide(value.get()).c_str()) != 0)
                {
                    //! \todo How can we translate this?
                    throw std::invalid_argument(String::Format("{0}: {1}").
                        arg(value.get()).
                        arg(DJV_TEXT("error_cannot_be_created")));
                }
            }

            void Path::rmdir(const Path & value)
            {
                if (_wrmdir(String::toWide(value.get()).c_str()) != 0)
                {
                    //! \todo How can we translate this?
                    throw std::invalid_argument(String::Format("{0}: {1}").
                        arg(value.get()).
                        arg(DJV_TEXT("error_cannot_be_removed")));
                }
            }

            Path Path::getAbsolute(const Path & value)
            {
                wchar_t buf[MAX_PATH];
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16;
                if (!::_wfullpath(buf, utf16.from_bytes(value.get()).c_str(), MAX_PATH))
                {
                    buf[0] = 0;
                }
                return Path(utf16.to_bytes(buf));
            }

            Path Path::getCWD()
            {
                wchar_t buf[MAX_PATH];
                if (!::_wgetcwd(buf, MAX_PATH))
                {
                    buf[0] = 0;
                }
                return Path(String::fromWide(buf));
            }

            Path Path::getTemp()
            {
                Path out;
                WCHAR buf[MAX_PATH];
                DWORD r = GetTempPathW(MAX_PATH, buf);
                if (r && r < MAX_PATH)
                {
                    out.set(String::fromWide(buf));
                }
                return out;
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

