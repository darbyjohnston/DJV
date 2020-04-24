// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2018-2020 Darby Johnston
// All rights reserved.

#include <djvCore/FileInfo.h>

#include <djvCore/Memory.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
//#include <glob.h>
#include <stdlib.h>

//#pragma optimize("", off)

#if defined(DJV_PLATFORM_OSX) || defined(DJV_PLATFORM_IOS)
//! \bug OS X doesn't have stat64?
#define _STAT struct ::stat
#define _STAT_FNC    ::stat
#elif defined(DJV_PLATFORM_LINUX)
#define _STAT struct ::stat64
#define _STAT_FNC    ::stat64
#endif // DJV_PLATFORM_OSX

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            bool FileInfo::stat(std::string*)
            {
                _exists      = false;
                _size        = 0;
                _user        = 0;
                _permissions = 0;
                _time        = 0;
                _permissions = 0;
                if (FileType::Sequence == _type)
                {
                    for (auto i : Frame::toFrames(_sequence))
                    {
                        _STAT info;
                        memset(&info, 0, sizeof(_STAT));
                        if (_STAT_FNC(getFileName(i).c_str(), &info) != 0)
                        {
                            return false;
                        }
                        _exists = true;
                        _size   += info.st_size;
                        _user   = std::min(_user, static_cast<uid_t>(info.st_uid));
                        _time   = std::max(_time, info.st_mtime);
                        _permissions |= (info.st_mode & S_IRUSR) ? static_cast<int>(FilePermissions::Read)  : 0;
                        _permissions |= (info.st_mode & S_IWUSR) ? static_cast<int>(FilePermissions::Write) : 0;
                        _permissions |= (info.st_mode & S_IXUSR) ? static_cast<int>(FilePermissions::Exec)  : 0;
                    }
                }
                else
                {
                    _STAT info;
                    memset(&info, 0, sizeof(_STAT));
                    if (_STAT_FNC(_path.get().c_str(), &info) != 0)
                    {
                        return false;
                    }
                    _exists = true;
                    _size   = info.st_size;
                    _user   = info.st_uid;
                    _time   = info.st_mtime;
                    if (S_ISDIR(info.st_mode))
                    {
                        _type = FileType::Directory;
                    }
                    _permissions |= (info.st_mode & S_IRUSR) ? static_cast<int>(FilePermissions::Read)  : 0;
                    _permissions |= (info.st_mode & S_IWUSR) ? static_cast<int>(FilePermissions::Write) : 0;
                    _permissions |= (info.st_mode & S_IXUSR) ? static_cast<int>(FilePermissions::Exec)  : 0;
                }
                return true;
            }

            std::vector<FileInfo> FileInfo::directoryList(const Path& value, const DirectoryListOptions& options)
            {
                std::vector<FileInfo> out;
                
                // List the directory contents.
                if (auto dir = opendir(value.get().c_str()))
                {
                    dirent* de = nullptr;
                    while ((de = readdir(dir)))
                    {
                        FileInfo fileInfo(Path(value, de->d_name));
                        
                        const std::string fileName = fileInfo.getFileName(-1, false);
                        
                        bool filter = false;
                        if (fileName.size() > 0 && '.' == fileName[0])
                        {
                            filter = !options.showHidden;
                        }
                        if (fileName.size() == 1 && '.' == fileName[0])
                        {
                            filter = true;
                        }
                        if (fileName.size() == 2 && '.' == fileName[0] && '.' == fileName[1])
                        {
                            filter = true;
                        }
                        if (options.filter.size() && !String::match(fileName, options.filter))
                        {
                            filter = true;
                        }
                        if (!filter && !(de->d_type & DT_DIR) && options.fileExtensions.size())
                        {
                            bool match = false;
                            for (const auto& i : options.fileExtensions)
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
                            _fileSequence(fileInfo, options, out);
                        }
                    }
                    closedir(dir);
                }
                    
                // Sort the items.
                _sort(options, out);
                
                return out;
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

