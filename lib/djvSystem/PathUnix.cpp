// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvSystem/Path.h>

#include <djvSystem/FileInfo.h>
#include <djvSystem/File.h>

#include <djvCore/Error.h>
#include <djvCore/StringFormat.h>

#if defined(DJV_PLATFORM_MACOS)
#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CFBundle.h>
#include <CoreServices/CoreServices.h>
#endif // DJV_PLATFORM_MACOS

#if defined(DJV_PLATFORM_LINUX)
#include <linux/limits.h>
#endif // DJV_PLATFORM_LINUX
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

namespace djv
{
    namespace System
    {
        namespace File
        {
            char Path::getCurrentSeparator() noexcept
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
                        if (Info(std::string(path)).doesExist())
                        {
                            out.set(path);
                            break;
                        }
                    }
                }
                return out;
            }

            Path getPath(DirectoryShortcut value)
            {
                Path out;
#if defined(DJV_PLATFORM_MACOS)
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
#else // DJV_PLATFORM_MACOS
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
#endif // DJV_PLATFORM_MACOS
                return out;
            }

        } // namespace File
    } // namespace System
} // namespace djv

