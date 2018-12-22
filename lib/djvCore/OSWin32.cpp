//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <Shlobj.h>
#include <stdlib.h>

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

                const std::string& getLabel(Windows value)
                {
                    static const std::vector<std::string> data =
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

                    // Get OS version information.
                    OSVERSIONINFOEX info;
                    memset(&info, 0, sizeof(OSVERSIONINFOEX));
                    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
                    if (!::GetVersionEx((OSVERSIONINFO *)&info))
                        return out;

                    // Get system information.
                    SYSTEM_INFO systemInfo;
                    memset(&systemInfo, 0, sizeof(SYSTEM_INFO));
                    if (PGNSI pGNSI = (PGNSI)::GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo"))
                    {
                        pGNSI(&systemInfo);
                    }
                    else
                    {
                        GetSystemInfo(&systemInfo);
                    }

                    // Use OS version and system information to determnine the version name.
                    switch (info.dwMajorVersion)
                    {
                    case 6:
                        switch (info.dwMinorVersion)
                        {
                        case 1: out = Windows::_7;   break;
                        case 2: out = Windows::_8;   break;
                        case 3: out = Windows::_8_1; break;
                        }
                        break;
                    case 10:
                        out = Windows::_10;
                        break;
                    }
                    return out;
                }

            } // namespace

            std::string getInformation()
            {
                return getLabel(windowsVersion());
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

            std::string getEnv(const std::string& name)
            {
                std::string out;
                size_t size = 0;
                char* p = 0;
                if (0 == _dupenv_s(&p, &size, name.c_str()))
                {
                    if (p)
                    {
                        out = p;
                    }
                }
                if (p)
                {
                    free(p);
                }
                return out;
            }

            bool setEnv(const std::string& name, const std::string& value)
            {
                return _putenv_s(name.c_str(), value.c_str()) == 0;
            }
        
            std::string getUserName()
            {
                TCHAR tmp[String::cStringLength] = { 0 };
                DWORD size = String::cStringLength;
                GetUserName(tmp, &size);
                return std::string(tmp);
            }

            FileSystem::Path getPath(DirectoryShortcut value)
            {
                FileSystem::Path out;
                KNOWNFOLDERID id;
                switch (value)
                {
                case DirectoryShortcut::Home:      id = FOLDERID_Profile;   break;
                case DirectoryShortcut::Desktop:   id = FOLDERID_Desktop;   break;
                case DirectoryShortcut::Documents: id = FOLDERID_Documents; break;
                case DirectoryShortcut::Downloads: id = FOLDERID_Downloads; break;
                default: break;
                }
                wchar_t* path = nullptr;
                HRESULT result = SHGetKnownFolderPath(id, 0, NULL, &path);
                if (S_OK == result && path)
                {
                    out = FileSystem::Path(String::fromWide(path));
                }
                CoTaskMemFree(path);
                return out;
            }

        } // namespace OS
    } // namespace Core
} // namespace djv

