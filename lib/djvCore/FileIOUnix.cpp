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

#include <djvCore/FileIO.h>

#include <djvCore/Error.h>
#include <djvCore/FileSystem.h>
#include <djvCore/Math.h>
#include <djvCore/Memory.h>

#include <iostream>
#include <sstream>

#if defined(DJV_PLATFORM_LINUX)
#include <linux/limits.h>
#endif // DJV_PLATFORM_LINUX
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define _STAT     struct stat
#define _STAT_FNC stat

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            namespace
            {
                enum class ErrorType
                {
                    Open,
                    Stat,
                    MemoryMap,
                    Close,
                    CloseMemoryMap,
                    Read,
                    ReadMemoryMap,
                    Write,
                    Seek,
                    SeekMemoryMap
                };
                
                std::string getErrorMessage(ErrorType type, const std::string& fileName)
                {
                    std::stringstream ss;
                    char buf[String::cStringLength] = "";
                    switch (type)
                    {
                    case ErrorType::Open:
                        ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be opened") << ". ";
#if defined(DJV_PLATFORM_LINUX)
                        ss << strerror_r(errno, buf, String::cStringLength);
#else // DJV_PLATFORM_LINUX
                        strerror_r(errno, buf, String::cStringLength);
                        ss << buf;
#endif // DJV_PLATFORM_LINUX
                        break;
                    case ErrorType::Stat:
                        ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be queried") << ". ";
#if defined(DJV_PLATFORM_LINUX)
                        ss << strerror_r(errno, buf, String::cStringLength);
#else // DJV_PLATFORM_LINUX
                        strerror_r(errno, buf, String::cStringLength);
                        ss << buf;
#endif // DJV_PLATFORM_LINUX
                        break;
                    case ErrorType::MemoryMap:
                        ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be mapped") << ". ";
#if defined(DJV_PLATFORM_LINUX)
                        ss << strerror_r(errno, buf, String::cStringLength);
#else // DJV_PLATFORM_LINUX
                        strerror_r(errno, buf, String::cStringLength);
                        ss << buf;
#endif // DJV_PLATFORM_LINUX
                        break;
                    case ErrorType::Close:
                        ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be unmapped") << ". ";
#if defined(DJV_PLATFORM_LINUX)
                        ss << strerror_r(errno, buf, String::cStringLength);
#else // DJV_PLATFORM_LINUX
                        strerror_r(errno, buf, String::cStringLength);
                        ss << buf;
#endif // DJV_PLATFORM_LINUX
                        break;
                    case ErrorType::CloseMemoryMap:
                        ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be closed") << ". ";
#if defined(DJV_PLATFORM_LINUX)
                        ss << strerror_r(errno, buf, String::cStringLength);
#else // DJV_PLATFORM_LINUX
                        strerror_r(errno, buf, String::cStringLength);
                        ss << buf;
#endif // DJV_PLATFORM_LINUX
                        break;
                    case ErrorType::Read:
                        ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be read") << ". ";
#if defined(DJV_PLATFORM_LINUX)
                        ss << strerror_r(errno, buf, String::cStringLength);
#else // DJV_PLATFORM_LINUX
                        strerror_r(errno, buf, String::cStringLength);
                        ss << buf;
#endif // DJV_PLATFORM_LINUX
                        break;
                    case ErrorType::ReadMemoryMap:
                        ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be read") << ".";
                        break;
                    case ErrorType::Write:
                        ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be written") << ". ";
#if defined(DJV_PLATFORM_LINUX)
                        ss << strerror_r(errno, buf, String::cStringLength);
#else // DJV_PLATFORM_LINUX
                        strerror_r(errno, buf, String::cStringLength);
                        ss << buf;
#endif // DJV_PLATFORM_LINUX
                        break;
                    case ErrorType::Seek:
                        ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be seeked") << ". ";
#if defined(DJV_PLATFORM_LINUX)
                        ss << strerror_r(errno, buf, String::cStringLength);
#else // DJV_PLATFORM_LINUX
                        strerror_r(errno, buf, String::cStringLength);
                        ss << buf;
#endif // DJV_PLATFORM_LINUX
                        break;
                    case ErrorType::SeekMemoryMap:
                        ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be seeked") << ".";
                        break;
                    default: break;
                    }
                    return ss.str();
                }
            
            } // namespace
                        
            void FileIO::open(const std::string& fileName, Mode mode)
            {
                close();

                // Open the file.
                int openFlags = 0;
                int openMode  = 0;
                switch (mode)
                {
                case Mode::Read:
                    openFlags = O_RDONLY;
                    break;
                case Mode::Write:
                    openFlags = O_WRONLY | O_CREAT | O_TRUNC;
                    openMode  = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
                    break;
                case Mode::ReadWrite:
                    openFlags = O_RDWR | O_CREAT | O_TRUNC;
                    openMode  = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
                    break;
                case Mode::Append:
                    openFlags = O_WRONLY | O_CREAT | O_APPEND;
                    openMode  = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
                    break;
                default: break;
                }
                _f = ::open(fileName.c_str(), openFlags, openMode);
                if (-1 == _f)
                {
                    throw Error(getErrorMessage(ErrorType::Open, fileName));
                }

                // Stat the file.
                _STAT info;
                memset(&info, 0, sizeof(_STAT));
                if (_STAT_FNC(fileName.c_str(), &info) != 0)
                {
                    throw Error(getErrorMessage(ErrorType::Stat, fileName));
                }
                _fileName = fileName;
                _mode     = mode;
                _pos      = 0;
                _size     = info.st_size;

#if defined(DJV_MMAP)
                // Memory mapping.
                if (Mode::Read == _mode && _size > 0)
                {
                    _mmap = mmap(0, _size, PROT_READ, MAP_SHARED, _f, 0);
                    madvise(_mmap, _size, MADV_SEQUENTIAL | MADV_SEQUENTIAL);
                    if (_mmap == (void *) - 1)
                    {
                        throw Error(getErrorMessage(ErrorType::MemoryMap, fileName));
                    }
                    _mmapStart = reinterpret_cast<const uint8_t *>(_mmap);
                    _mmapEnd   = _mmapStart + _size;
                    _mmapP     = _mmapStart;
                }
#endif // DJV_MMAP
            }
            
            void FileIO::openTemp()
            {
                close();

                // Open the file.
                const std::string fileName = "XXXXXX";
                const size_t size = fileName.size();
                std::vector<char> buf(size + 1);
                memcpy(buf.data(), fileName.c_str(), size);
                buf[size] = 0;
                _f = mkstemp(buf.data());
                if (-1 == _f)
                {
                    throw Error(getErrorMessage(ErrorType::Open, fileName));
                }

                // Stat the file.
                _STAT info;
                memset(&info, 0, sizeof(_STAT));
                if (_STAT_FNC(buf.data(), &info) != 0)
                {
                    throw Error(getErrorMessage(ErrorType::Stat, fileName));
                }
                _fileName = std::string(buf.data());
                _mode     = Mode::ReadWrite;
                _pos      = 0;
                _size     = info.st_size;
            }

            bool FileIO::close(std::string* error)
            {
                bool out = true;
                
                _fileName = std::string();
#if defined(DJV_MMAP)
                if (_mmap != (void *) - 1)
                {
                    int r = munmap(_mmap, _size);
                    if (-1 == r)
                    {
                        out = false;
                        if (error)
                        {
                            *error = getErrorMessage(ErrorType::CloseMemoryMap, _fileName);
                        }
                    }
                    _mmap = (void *)-1;
                }
                _mmapStart = 0;
                _mmapEnd   = 0;
#endif // DJV_MMAP
                if (_f != -1)
                {
                    int r = ::close(_f);
                    if (-1 == r)
                    {
                        out = false;
                        if (error)
                        {
                            *error = getErrorMessage(ErrorType::Close, _fileName);
                        }
                    }
                    _f = -1;
                }

                _mode = static_cast<Mode>(0);
                _pos  = 0;
                _size = 0;
                
                return out;
            }
            
            void FileIO::read(void* in, size_t size, size_t wordSize)
            {
                switch (_mode)
                {
                case Mode::Read:
                {
#if defined(DJV_MMAP)
                    const uint8_t* mmapP = _mmapP + size * wordSize;
                    if (mmapP > _mmapEnd)
                    {
                        throw Error(getErrorMessage(ErrorType::ReadMemoryMap, _fileName));
                    }
                    if (_endian && wordSize > 1)
                    {
                        Memory::endian(_mmapP, in, size, wordSize);
                    }
                    else
                    {
                        memcpy(in, _mmapP, size * wordSize);
                    }
                    _mmapP = mmapP;
#else // DJV_MMAP
                    const size_t r = ::read(_f, in, size * wordSize);
                    if (r != size * wordSize)
                    {
                        throw Error(getErrorMessage(ErrorType::Read, _fileName));
                    }
#endif // DJV_MMAP
                    break;
                }
                case Mode::ReadWrite:
                {
                    const size_t r = ::read(_f, in, size * wordSize);
                    if (r != size * wordSize)
                    {
                        throw Error(getErrorMessage(ErrorType::Read, _fileName));
                    }
                    if (_endian && wordSize > 1)
                    {
                        Memory::endian(in, size, wordSize);
                    }
                    break;
                }
                default: break;
                }
                _pos += size * wordSize;
            }

            void FileIO::write(const void* in, size_t size, size_t wordSize)
            {
                const uint8_t* inP = reinterpret_cast<const uint8_t*>(in);
                std::vector<uint8_t> tmp;
                if (_endian && wordSize > 1)
                {
                    tmp.resize(size * wordSize);
                    inP = tmp.data();
                    Memory::endian(in, tmp.data(), size, wordSize);
                }
                if (::write(_f, inP, size * wordSize) == -1)
                {
                    throw Error(getErrorMessage(ErrorType::Write, _fileName));
                }
                _pos += size * wordSize;
                _size = std::max(_pos, _size);
            }

            void FileIO::setPos(size_t in)
            {
                _setPos(in, false);
            }

            void FileIO::seek(size_t in)
            {
                _setPos(in, true);
            }

            void FileIO::setEndian(bool in)
            {
                _endian = in;
            }

            void FileIO::_setPos(size_t in, bool seek)
            {
                switch (_mode)
                {
                case Mode::Read:
                {
#if defined(DJV_MMAP)
                    if (!seek)
                    {
                        _mmapP = reinterpret_cast<const uint8_t*>(_mmapStart) + in;
                    }
                    else
                    {
                        _mmapP += in;
                    }
                    if (_mmapP > _mmapEnd)
                    {
                        throw Error(getErrorMessage(ErrorType::SeekMemoryMap, _fileName));
                    }
#else // DJV_MMAP
                    if (::lseek(_f, in, ! seek ? SEEK_SET : SEEK_CUR) == (off_t) - 1)
                    {
                        throw Error(getErrorMessage(ErrorType::Seek, _fileName));
                    }
#endif // DJV_MMAP
                    break;
                }
                case Mode::Write:
                case Mode::ReadWrite:
                {
                    if (::lseek(_f, in, ! seek ? SEEK_SET : SEEK_CUR) == (off_t) - 1)
                    {
                        throw Error(getErrorMessage(ErrorType::Seek, _fileName));
                    }
                    break;
                }
                default: break;
                }
                if (!seek)
                {
                    _pos = in;
                }
                else
                {
                    _pos += in;
                }
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

