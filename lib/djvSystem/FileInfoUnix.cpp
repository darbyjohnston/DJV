// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2018-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/FileInfoPrivate.h>

#include <djvMath/FrameNumber.h>

#include <djvCore/String.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>

//#pragma optimize("", off)

#if defined(DJV_PLATFORM_MACOS) || defined(DJV_PLATFORM_IOS)
//! \bug OS X doesn't have stat64?
#define _STAT struct ::stat
#define _STAT_FNC    ::stat
#elif defined(DJV_PLATFORM_LINUX)
#define _STAT struct ::stat64
#define _STAT_FNC    ::stat64
#endif // DJV_PLATFORM_MACOS

using namespace djv::Core;

namespace djv
{
    namespace System
    {
        namespace File
        {
            bool Info::stat(std::string*)
            {
                _exists      = false;
                _size        = 0;
                _user        = 0;
                _permissions = 0;
                _time        = 0;
                if (Type::Sequence == _type)
                {
                    bool     exists      = false;
                    uint64_t size        = 0;
                    uid_t    user        = 0;
                    int      permissions = 0;
                    time_t   time        = 0;
                    for (auto i : Math::Frame::toFrames(_sequence))
                    {
                        _STAT info;
                        memset(&info, 0, sizeof(_STAT));
                        const std::string fileName = getFileName(i);
                        if (_STAT_FNC(fileName.c_str(), &info) != 0)
                        {
                            return false;
                        }
                        exists       = true;
                        size        += info.st_size;
                        user         = std::min(_user, static_cast<uid_t>(info.st_uid));
                        permissions |= (info.st_mode & S_IRUSR) ? static_cast<int>(Permissions::Read)  : 0;
                        permissions |= (info.st_mode & S_IWUSR) ? static_cast<int>(Permissions::Write) : 0;
                        permissions |= (info.st_mode & S_IXUSR) ? static_cast<int>(Permissions::Exec)  : 0;
                        time         = std::max(_time, info.st_mtime);
                    }
                    _exists      = exists;
                    _size        = size;
                    _user        = user;
                    _permissions = permissions;
                    _time        = time;
                }
                else
                {
                    _STAT info;
                    memset(&info, 0, sizeof(_STAT));
                    if (_STAT_FNC(_path.get().c_str(), &info) != 0)
                    {
                        return false;
                    }
                    _exists		  = true;
                    if (S_ISDIR(info.st_mode))
                    {
                        _type     = Type::Directory;
                    }
                    _size         = info.st_size;
                    _user         = info.st_uid;
                    _permissions |= (info.st_mode & S_IRUSR) ? static_cast<int>(Permissions::Read)  : 0;
                    _permissions |= (info.st_mode & S_IWUSR) ? static_cast<int>(Permissions::Write) : 0;
                    _permissions |= (info.st_mode & S_IXUSR) ? static_cast<int>(Permissions::Exec)  : 0;
                    _time         = info.st_mtime;
                }
                return true;
            }

            std::vector<Info> directoryList(const Path& value, const DirectoryListOptions& options)
            {
                std::vector<Info> out;
                
                // List the directory contents.
                if (auto dir = opendir(value.get().c_str()))
                {
                    dirent* de = nullptr;
                    while ((de = readdir(dir)))
                    {
                        Info info(Path(value, de->d_name));
                        
                        const std::string fileName = info.getFileName(-1, false);
                        
                        bool filter = false;
                        
                        // Filter hidden items.
                        if (!filter && fileName.size() > 0 &&
                            '.' == fileName[0])
                        {
                            filter = !options.showHidden;
                        }
                        
                        // Filter "." and ".." items.
                        if (!filter &&
                            fileName.size() == 1 &&
                            '.' == fileName[0])
                        {
                            filter = true;
                        }
                        if (!filter &&
                            fileName.size() == 2 &&
                            '.' == fileName[0] &&
                            '.' == fileName[1])
                        {
                            filter = true;
                        }
                        
                        // Filter string matches.
                        if (!filter &&
                            options.filter.size() &&
                            !String::match(fileName, options.filter))
                        {
                            filter = true;
                        }
                        
                        // Filter file extensions.
                        if (!filter &&
                            info.getType() != System::File::Type::Directory &&
                            options.extensions.size())
                        {
                            bool match = false;
                            for (const auto& i : options.extensions)
                            {
                                if (String::match(fileName, '\\' + i + '$'))
                                {
                                    match = true;
                                    break;
                                }
                            }
                            if (!match)
                            {
                                filter = true;
                            }
                        }

                        if (!filter)
                        {
                            sequence(info, options, out);
                        }
                    }
                    closedir(dir);
                }
                    
                // Sort the items.
                sort(options, out);
                
                return out;
            }

        } // namespace File
    } // namespace System
} // namespace djv

