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

#include <djvCore/Path.h>

#include <djvCore/Error.h>
#include <djvCore/FileInfo.h>
#include <djvCore/FileSystem.h>

#include <sstream>

#if defined(DJV_PLATFORM_LINUX)
#include <linux/limits.h>
#endif // DJV_PLATFORM_LINUX
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            void Path::mkdir(const Path& value)
            {
                if (::mkdir(value.get().c_str(), S_IRWXU) != 0)
                {
                    std::stringstream s;
                    s << DJV_TEXT("The directory") << " '" << value << "' " << DJV_TEXT("cannot be created") << ".";
                    throw Error(s.str());
                }
            }
            
            void Path::rmdir(const Path& value)
            {
                if (::rmdir(value.get().c_str()) != 0)
                {
                    std::stringstream s;
                    s << DJV_TEXT("The directory") << " '" << value << "' " << DJV_TEXT("cannot be removed") << ".";
                    throw Error(s.str());
                }
            }
            
            Path Path::getAbsolute(const Path& value)
            {
                char buf[PATH_MAX];
                if (!realpath(value.get().c_str(), buf))
                {
                    buf[0] = 0;
                    switch (errno)
                    {
                    case EACCES:       throw Error("Permission denied");
                    case EINVAL:       throw Error("Invalid path");
                    case EIO:          throw Error("File system I/O error");
                    case ELOOP:        throw Error("Too many sumbolic links");
                    case ENAMETOOLONG: throw Error("Path too long");
                    case ENOENT:       throw Error("Path does not exist");
                    case ENOTDIR:      throw Error("Path is not a directory");
                    default: break;
                    }
                }
                return Path(buf);
            }

            Path Path::getCWD()
            {
                char buf[PATH_MAX];
                if (!getcwd(buf, PATH_MAX))
                {
                    buf[0] = 0;
                }
                return Path(buf);
            }

            Path Path::getTemp()
            {
                Path out;
                char* env = nullptr;
                if ((env = getenv("TEMP")))
                {
                    out.set(env);
                }
                else if ((env = getenv("TMP")))
                {
                    out.set(env);
                }
                else if ((env = getenv("TMPDIR")))
                {
                    out.set(env);
                }
                else
                {
                    for (const auto& path : { "/tmp", "/var/tmp", "/usr/tmp" })
                    {
                        if (FileInfo(std::string(path)).doesExist())
                        {
                            out.set(path);
                            break;
                        }
                    }
                }
                return out;
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

