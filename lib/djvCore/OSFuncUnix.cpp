// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCore/OSFunc.h>

#include <djvCore/ErrorFunc.h>
#include <djvCore/StringFormat.h>
#include <djvCore/StringFunc.h>

#include <sstream>

#include <sys/ioctl.h>
#if defined(DJV_PLATFORM_MACOS)
#include <sys/types.h>
#include <sys/sysctl.h>
#else // DJV_PLATFORM_MACOS
#include <sys/sysinfo.h>
#endif // DJV_PLATFORM_MACOS
#include <sys/utsname.h>
#include <pwd.h>
#include <unistd.h>

//#pragma optimize("", off)

namespace djv
{
    namespace Core
    {
        namespace OS
        {
            std::string getInformation()
            {
                std::string out;
                ::utsname info;
                uname(&info);
                std::stringstream s;
                s << info.sysname << " " << info.release << " " << info.machine;
                out = s.str();
                return out;
            }
            
            size_t getRAMSize()
            {
                size_t out = 0;
#if defined(DJV_PLATFORM_MACOS)
                int name[2] = { CTL_HW, HW_MEMSIZE };
                u_int namelen = sizeof(name) / sizeof(name[0]);
                uint64_t size = 0;
                size_t len = sizeof(size);
                if (0 == sysctl(name, namelen, &size, &len, NULL, 0))
                {
                    out = static_cast<size_t>(size);
                }
#else // DJV_PLATFORM_MACOS
                struct sysinfo info;
                if (0 == sysinfo(&info))
                {
                    out = info.totalram;
                }
#endif // DJV_PLATFORM_MACOS
                return out;
            }

            int getTerminalWidth()
            {
                int out = 80;
                struct winsize ws;
                ws.ws_col = 0;
                if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
                {
                    ws.ws_col = 80;
                }
                out = ws.ws_col;
                return out;
            }

            bool getEnv(const std::string& name, std::string& out)
            {
                if (const char* p = ::getenv(name.c_str()))
                {
                    out = std::string(p);
                    return true;
                }
                return false;
            }

            bool setEnv(const std::string& name, const std::string& value)
            {
                return ::setenv(name.c_str(), value.c_str(), 1) == 0;
            }
        
            bool clearEnv(const std::string& name)
            {
                return ::unsetenv(name.c_str()) == 0;
            }
        
            std::string getUserName()
            {
                std::string out;
                if (struct passwd* buf = ::getpwuid(::getuid()))
                {
                    out = std::string(buf->pw_name);
                }
                return out;
            }

            void openURL(const std::string& value)
            {
#if defined(DJV_PLATFORM_MACOS)
                CFURLRef url = CFURLCreateWithBytes(
                    NULL,
                    (UInt8*)value.c_str(),
                    value.size(),
                    kCFStringEncodingASCII,
                    NULL);
                LSOpenCFURLRef(url, 0);
                CFRelease(url);
#else // DJV_PLATFORM_MACOS
                std::stringstream ss;
                ss << "xdg-open" << " " << value;
                int r = system(ss.str().c_str());
                if (r != 0)
                {
                    std::vector<std::string> messages;
                    //! \todo How can we translate this?
                    messages.push_back(String::Format("{0}: {1}").
                        arg(value).
                        arg(DJV_TEXT("error_url_cannot_open")));
                    messages.push_back(String::Format(DJV_TEXT("error_url_code")).
                        arg(value));
                    throw std::runtime_error(String::join(messages, ' '));
                }
#endif // DJV_PLATFORM_MACOS
            }

        } // namespace OS
    } // namespace Core
} // namespace djv
