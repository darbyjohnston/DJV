// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2018-2020 Darby Johnston
// All rights reserved.

#include <djvCore/Path.h>

#include <djvCore/Error.h>
#include <djvCore/FileInfo.h>
#include <djvCore/FileSystem.h>
#include <djvCore/StringFormat.h>

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
            char getCurrentSeparator() noexcept
            {
                return getSeparator(PathSeparator::Unix);
            }

            void mkdir(const Path& value)
            {
                if (::mkdir(value.get().c_str(), S_IRWXU) != 0)
                {
                    //! \todo How can we translate this?
                    throw std::invalid_argument(String::Format("{0}: {1}").
                        arg(value.get()).
                        arg(DJV_TEXT("error_cannot_be_created")));
                }
            }
            
            void rmdir(const Path& value)
            {
                if (::rmdir(value.get().c_str()) != 0)
                {
                    //! \todo How can we translate this?
                    throw std::invalid_argument(String::Format("{0}: {1}").
                        arg(value.get()).
                        arg(DJV_TEXT("error_cannot_be_removed")));
                }
            }
            
            Path getAbsolute(const Path& value)
            {
                std::string directoryName = value.getDirectoryName();
                std::string fileName = value.getFileName();
                if (directoryName.empty())
                {
                    directoryName = ".";
                }
                if ("." == fileName)
                {
                    fileName = std::string();
                }
                char buf[PATH_MAX];
                if (!realpath(directoryName.c_str(), buf))
                {
                    buf[0] = 0;
                    //! \todo How can we translate this?
                    switch (errno)
                    {
                    case EACCES:       throw Error("Permission denied.");
                    case EINVAL:       throw Error("Invalid path.");
                    case EIO:          throw Error("File system I/O error.");
                    case ELOOP:        throw Error("Too many sumbolic links.");
                    case ENAMETOOLONG: throw Error("Path too long.");
                    case ENOENT:       throw Error("Path does not exist.");
                    case ENOTDIR:      throw Error("Path is not a directory.");
                    }
                }
                return Path(buf, fileName);
            }

            Path getCWD()
            {
                char buf[PATH_MAX];
                if (!getcwd(buf, PATH_MAX))
                {
                    buf[0] = 0;
                }
                return Path(buf);
            }

            Path getTemp()
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

