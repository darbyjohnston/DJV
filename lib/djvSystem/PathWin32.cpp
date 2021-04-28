// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvSystem/Path.h>

#include <djvCore/StringFormat.h>
#include <djvCore/String.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <windows.h>
#include <direct.h>
#include <Shlobj.h>
#include <shellapi.h>

#include <codecvt>
#include <locale>

using namespace djv::Core;

namespace djv
{
    namespace System
    {
        namespace File
        {
            char Path::getCurrentSeparator() noexcept
            {
                return getSeparator(PathSeparator::Windows);
            }

            void mkdir(const Path& value)
            {
                if (_wmkdir(String::toWide(value.get()).c_str()) != 0)
                {
                    //! \todo How can we translate this?
                    throw std::invalid_argument(String::Format("{0}: {1}").
                        arg(value.get()).
                        arg(DJV_TEXT("error_cannot_be_created")));
                }
            }

            void rmdir(const Path& value)
            {
                if (_wrmdir(String::toWide(value.get()).c_str()) != 0)
                {
                    //! \todo How can we translate this?
                    throw std::invalid_argument(String::Format("{0}: {1}").
                        arg(value.get()).
                        arg(DJV_TEXT("error_cannot_be_removed")));
                }
            }

            Path getAbsolute(const Path& value)
            {
                wchar_t buf[MAX_PATH];
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16;
                if (!::_wfullpath(buf, utf16.from_bytes(value.get()).c_str(), MAX_PATH))
                {
                    buf[0] = 0;
                }
                return Path(utf16.to_bytes(buf));
            }

            Path getCWD()
            {
                wchar_t buf[MAX_PATH];
                if (!::_wgetcwd(buf, MAX_PATH))
                {
                    buf[0] = 0;
                }
                return Path(String::fromWide(buf));
            }

            Path getTemp()
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

            Path getPath(DirectoryShortcut value)
            {
                Path out;
                KNOWNFOLDERID id;
                memset(&id, 0, sizeof(KNOWNFOLDERID));
                switch (value)
                {
                case DirectoryShortcut::Home:      id = FOLDERID_Profile;   break;
                case DirectoryShortcut::Desktop:   id = FOLDERID_Desktop;   break;
                case DirectoryShortcut::Documents: id = FOLDERID_Documents; break;
                case DirectoryShortcut::Downloads: id = FOLDERID_Downloads; break;
                default: break;
                }
                wchar_t * path = nullptr;
                HRESULT result = SHGetKnownFolderPath(id, 0, NULL, &path);
                if (S_OK == result && path)
                {
                    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16;
                    out = Path(utf16.to_bytes(path));
                }
                CoTaskMemFree(path);
                return out;
            }

        } // namespace File
    } // namespace System
} // namespace djv

