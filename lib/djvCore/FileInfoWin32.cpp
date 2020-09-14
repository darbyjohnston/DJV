// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/FileInfoPrivate.h>

#include <djvCore/Memory.h>
#include <djvCore/StringFunc.h>

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
#define _STAT_FNC _wstati64

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
                else if (_path.isServer())
                {
                    //! \todo What about servers?
                    _exists = true;
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

        } // namespace FileSystem
    } // namespace Core
} // namespace djv
