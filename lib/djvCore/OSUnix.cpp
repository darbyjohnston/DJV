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

#include <GpCore/OS.h>

#include <GpCore/Path.h>

#if defined(GP_PLATFORM_OSX)
#include <CoreServices/CoreServices.h>
#endif // GP_PLATFORM_OSX

#include <sstream>

#include <sys/ioctl.h>
#include <sys/termios.h>
#include <sys/utsname.h>
#include <pwd.h>
#include <unistd.h>

//#pragma optimize("", off)

namespace Gp
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

            std::string getEnv(const std::string& name)
            {
                std::string out;
                if (const char * p = ::getenv(name.c_str()))
                {
                    out = p;
                }
                return out;
            }

            bool setEnv(const std::string& name, const std::string& value)
            {
                return ::setenv(name.c_str(), value.c_str(), 1) == 0;
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

            Path getPath(DirectoryShortcut value)
            {
                Path out;
#if defined(GP_PLATFORM_OSX)
                OSType folderType = kDesktopFolderType;
                switch (value)
                {
                case DirectoryShortcut::Home:      folderType = kCurrentUserFolderType; break;
                case DirectoryShortcut::Desktop:   folderType = kDesktopFolderType;     break;
                case DirectoryShortcut::Documents: folderType = kDocumentsFolderType;   break;
                case DirectoryShortcut::Downloads: folderType = kCurrentUserFolderType; break;
                default: break;
                }
                FSRef ref;
                FSFindFolder(kUserDomain, folderType, kCreateFolder, &ref);
                char path[PATH_MAX];
                FSRefMakePath(&ref, (UInt8*)&path, PATH_MAX);
                if (DirectoryShortcut::Downloads == value)
                {
                    out = Path(path, "Downloads");
                }
                else
                {
                    out = Path(path);
                }
#elif defined(GP_PLATFORM_LINUX)
                if (struct passwd* buf = ::getpwuid(::getuid()))
                {
                    const std::string dir(buf->pw_dir);
                    switch (value)
                    {
                    case DirectoryShortcut::Home:      out = Path(dir);              break;
                    case DirectoryShortcut::Desktop:   out = Path(dir, "Desktop");   break;
                    case DirectoryShortcut::Documents: out = Path(dir, "Documents"); break;
                    case DirectoryShortcut::Downloads: out = Path(dir, "Downloads"); break;
                    default: break;
                    }
                }
#endif // GP_PLATFORM_OSX
                return out;
            }

        } // namespace OS
    } // namespace Core
} // namespace Gp
