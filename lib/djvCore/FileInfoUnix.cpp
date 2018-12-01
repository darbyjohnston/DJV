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

#include <GpCore/FileInfo.h>

#include <GpCore/Memory.h>

#include <sys/stat.h>
#include <glob.h>
#include <stdlib.h>

//#pragma optimize("", off)

#if defined(GP_PLATFORM_OSX) || defined(GP_PLATFORM_IOS)
//! \bug OS X doesn't have stat64?
#define _STAT struct ::stat
#define _STAT_FNC    ::stat
#elif defined(GP_PLATFORM_LINUX)
#define _STAT struct ::stat64
#define _STAT_FNC    ::stat64
#endif // GP_PLATFORM_OSX

namespace Gp
{
    namespace Core
    {
        bool FileInfo::stat()
        {
            _init();

            _STAT info;
            memset(&info, 0, sizeof(_STAT));

            if (_STAT_FNC(_path.get().c_str(), &info) != 0)
            {
                std::string err;
                return false;
            }

            _exists      = true;
            _size        = info.st_size;
            _user        = info.st_uid;
            _permissions = 0;
            _time        = info.st_mtime;
            _type        = FileType::File;
            _permissions = 0;

            if (S_ISDIR(info.st_mode))
            {
                _type = FileType::Directory;
            }
            _permissions |= (info.st_mode & S_IRUSR) ? static_cast<int>(FilePermissions::Read)  : 0;
            _permissions |= (info.st_mode & S_IWUSR) ? static_cast<int>(FilePermissions::Write) : 0;
            _permissions |= (info.st_mode & S_IXUSR) ? static_cast<int>(FilePermissions::Exec)  : 0;

            return true;
        }

        std::vector<FileInfo> FileInfo::dirList(const Path& value, const DirListOptions& options)
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

            glob_t g;
            if (::glob(Path(value, options.glob).get().c_str(), 0, nullptr, &g) == 0)
            {
                for (size_t i = 0; i < g.gl_pathc; ++i)
                {
                    FileInfo fileInfo(Path(g.gl_pathv[i]));
                    
                    const std::string fileName = fileInfo.getFileName(-1, false);
                    
                    bool filter = false;
                    if (fileName.size() == 1 && '.' == fileName[0])
                    {
                        filter = true;
                    }
                    if (fileName.size() == 2 && '.' == fileName[0] && '.' == fileName[1])
                    {
                        filter = true;
                    }

                    if (!filter)
                    {
                        if (options.fileSequences)
                        {
                            fileInfo.evalSequence();
                            if (fileInfo.isSequenceValid())
                            {
                                const size_t size = out.size();
                                size_t i = 0;
                                for (; i < size; ++i)
                                {
                                    if (out[i].addToSequence(fileInfo))
                                    {
                                        break;
                                    }
                                }
                                if (size == i)
                                {
                                    out.push_back(fileInfo);
                                }
                            }
                            else
                            {
                                out.push_back(fileInfo);
                            }
                        }
                        else
                        {
                            out.push_back(fileInfo);
                        }
                    }
                }
            }
            globfree(&g);
            
            for (auto& i : out)
            {
                if (i.isSequenceValid())
                {
                    i.sortSequence();
                }
            }

            return out;
        }

        FILE* fopen(const std::string& fileName, const std::string& mode)
        {
            return ::fopen(fileName.c_str(), mode.c_str());
        }

    } // namespace Core
} // namespace Gp

