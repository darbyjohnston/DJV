//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvCore/OS.h>

#include <djvCore/FileInfo.h>
#include <djvCore/Memory.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <windows.h>
#include <Shlobj.h>
#include <shellapi.h>
#include <stdlib.h>
#include <VersionHelpers.h>

#include <codecvt>
#include <locale>

//#pragma optimize("", off)

namespace djv
{
    namespace Core
    {
        namespace OS
        {
            namespace
            {
                enum class Windows
                {
                    Unknown,
                    _7,
                    _8,
                    _8_1,
                    _10,

                    Count
                };

                std::string getLabel(Windows value)
                {
                    const std::vector<std::string> data =
                    {
                        "Unknown",
                        "Windows 7",
                        "Windows 8",
                        "Windows 8.1",
                        "Windows 10"
                    };
                    DJV_ASSERT(data.size() == static_cast<size_t>(Windows::Count));
                    return data[static_cast<size_t>(value)];
                }

                typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);

                Windows windowsVersion()
                {
                    Windows out = Windows::Unknown;
                    if (IsWindows10OrGreater())
                    {
                        out = Windows::_10;
                    }
                    else if (IsWindows8Point1OrGreater())
                    {
                        out = Windows::_8_1;
                    }
                    else if (IsWindows8OrGreater())
                    {
                        out = Windows::_8;
                    }
                    else if (IsWindows7OrGreater())
                    {
                        out = Windows::_7;
                    }
                    return out;
                }

            } // namespace

            std::string getInformation()
            {
                return getLabel(windowsVersion());
            }

            size_t getRAMSize()
            {
                MEMORYSTATUSEX statex;
                statex.dwLength = sizeof(statex);
                GlobalMemoryStatusEx(&statex);
                return statex.ullTotalPhys;
            }

            std::string getUserName()
            {
                WCHAR tmp[String::cStringLength] = { 0 };
                DWORD size = String::cStringLength;
                GetUserNameW(tmp, &size);
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16;
                return std::string(utf16.to_bytes(tmp));
            }

            int getTerminalWidth()
            {
                int out = 80;
                HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
                if (h != INVALID_HANDLE_VALUE)
                {
                    CONSOLE_SCREEN_BUFFER_INFO info;
                    if (GetConsoleScreenBufferInfo(h, &info))
                    {
                        out = info.dwSize.X;
                    }
                }
                return out;
            }

            std::string getEnv(const std::string & name)
            {
                std::string out;
                size_t size = 0;
                wchar_t * p = 0;
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16;
                if (0 == _wdupenv_s(&p, &size, utf16.from_bytes(name).c_str()))
                {
                    if (p)
                    {
                        out = utf16.to_bytes(p);
                    }
                }
                if (p)
                {
                    free(p);
                }
                return out;
            }

            bool setEnv(const std::string & name, const std::string & value)
            {
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16;
                return _wputenv_s(utf16.from_bytes(name).c_str(), utf16.from_bytes(value).c_str()) == 0;
            }

            FileSystem::Path getPath(DirectoryShortcut value)
            {
                FileSystem::Path out;
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
                    out = FileSystem::Path(utf16.to_bytes(path));
                }
                CoTaskMemFree(path);
                return out;
            }

            void openURL(const std::string& value)
            {
                ShellExecute(0, 0, value.c_str(), 0, 0, SW_SHOW);
            }

        } // namespace OS
    } // namespace Core
} // namespace djv

