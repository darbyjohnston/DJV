// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/FileInfo.h>

#include <djvCore/Memory.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <sys/stat.h>
#include <windows.h>

#include <codecvt>
#include <locale>

//#pragma optimize("", off)

#define _STAT     struct _stati64
#define _STAT_FNC        _wstati64

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            bool FileInfo::stat(std::string* error)
            {
                _exists      = false;
                _size        = 0;
                _user        = 0;
                _permissions = 0;
                _time        = 0;
                _permissions = 0;
                if (FileType::Sequence == _type)
                {
                    bool     exists      = false;
                    uint64_t size        = 0;
                    uid_t    user        = 0;
                    int      permissions = 0;
                    time_t   time        = 0;
                    for (auto i : Frame::toFrames(_sequence))
                    {
                        _STAT info;
                        memset(&info, 0, sizeof(_STAT));
                        if (_STAT_FNC(String::toWide(getFileName(i)).c_str(), &info) != 0)
                        {
                            if (error)
                            {
                                char tmp[String::cStringLength] = "";
                                strerror_s(tmp, String::cStringLength, errno);
                                *error = tmp;
                            }
                            return false;
                        }
                        exists       = true;
                        size        += info.st_size;
                        user         = std::min(_user, static_cast<uid_t>(info.st_uid));
                        permissions |= (info.st_mode & _S_IREAD)  ? static_cast<int>(FilePermissions::Read)  : 0;
                        permissions |= (info.st_mode & _S_IWRITE) ? static_cast<int>(FilePermissions::Write) : 0;
                        permissions |= (info.st_mode & _S_IEXEC)  ? static_cast<int>(FilePermissions::Exec)  : 0;
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
                    if (_STAT_FNC(String::toWide(_path.get()).c_str(), &info) != 0)
                    {
                        if (error)
                        {
                            char tmp[String::cStringLength] = "";
                            strerror_s(tmp, String::cStringLength, errno);
                            *error = tmp;
                        }
                        return false;
                    }
                    _exists       = true;
                    if (info.st_mode & _S_IFDIR)
                    {
                        _type     = FileType::Directory;
                    }
                    _size         = info.st_size;
                    _user         = info.st_uid;
                    _permissions |= (info.st_mode & _S_IREAD)  ? static_cast<int>(FilePermissions::Read)  : 0;
                    _permissions |= (info.st_mode & _S_IWRITE) ? static_cast<int>(FilePermissions::Write) : 0;
                    _permissions |= (info.st_mode & _S_IEXEC)  ? static_cast<int>(FilePermissions::Exec)  : 0;
                    _time         = info.st_mtime;
                }
                return true;
            }

            std::vector<FileInfo> FileInfo::directoryList(const Path & value, const DirectoryListOptions & options)
            {
                std::vector<FileInfo> out;
                if (!value.isEmpty())
                {
                    // Prepare the path.
                    const std::wstring path = String::toWide(value.get() + Path::getSeparator(PathSeparator::Windows) + '*');
                    WCHAR pathBuf[MAX_PATH];
                    size_t size = std::min(path.size(), static_cast<size_t>(MAX_PATH - 1));
                    memcpy(pathBuf, path.c_str(), size * sizeof(WCHAR));
                    pathBuf[size++] = 0;

                    // List the directory contents.
                    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16;
                    WIN32_FIND_DATAW ffd;
                    HANDLE hFind = FindFirstFileW(pathBuf, &ffd);
                    if (hFind != INVALID_HANDLE_VALUE)
                    {
                        do
                        {
                            const std::string fileName = utf16.to_bytes(ffd.cFileName);

                            bool filter = false;
                            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
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
                            if (!filter &&!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && options.fileExtensions.size())
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
                                FileInfo fileInfo(Path(value, fileName));
                                _fileSequence(fileInfo, options, out);
                            }
                        } while (FindNextFileW(hFind, &ffd) != 0);
                        FindClose(hFind);
                    }
                    
                    // Sort the items.
                    _sort(options, out);
                }
                return out;
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv
