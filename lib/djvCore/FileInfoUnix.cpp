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
                /*if (auto dir = opendir(path.c_str()))
                {
                    dirent* de = nullptr;
                    while ((de = readdir(dir)))
                    {
                        if (1 == de->d_namlen && '.' == de->d_name[0])
                            ;
                        else if (2 == de->d_namlen && '.' == de->d_name[0] && '.' == de->d_name[1])
                            ;
                        else
                        {
                            out.emplace_back(Path(value, std::string(de->d_name, de->d_namlen)));
                        }
                    }
                    closedir(dir);
                }*/

                //glob_t g;
                //if (::glob(Path(value, options.glob).get().c_str(), 0, nullptr, &g) == 0)
                //{
                //    for (size_t i = 0; i < g.gl_pathc; ++i)
                if (auto dir = opendir(value.get().c_str()))
                {
                    dirent* de = nullptr;
                    while ((de = readdir(dir)))
                    {
                        //FileInfo fileInfo(Path(g.gl_pathv[i]));
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
                //globfree(&g);
                    
                // Sort the items.
                _sort(options, out);
                
                return out;
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

