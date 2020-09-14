// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2018-2020 Darby Johnston
// All rights reserved.

#include <djvCore/FileIO.h>

#include <djvCore/ErrorFunc.h>
#include <djvCore/FileSystem.h>
#include <djvCore/Math.h>
#include <djvCore/MemoryFunc.h>
#include <djvCore/PathFunc.h>
#include <djvCore/StringFormat.h>
#include <djvCore/StringFunc.h>

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

                std::string getErrorString()
                {
                    std::string out;
                    char buf[String::cStringLength] = "";
#if defined(_GNU_SOURCE)
                    out = strerror_r(errno, buf, String::cStringLength);
#else // _GNU_SOURCE
                    strerror_r(errno, buf, String::cStringLength);
                    out = buf;
#endif // _GNU_SOURCE
                    return out;
                }
                
                std::string getErrorMessage(ErrorType type, const std::string& fileName)
                {
                    //! \todo How can we translate this?
                    std::vector<std::string> out;
                    switch (type)
                    {
                    case ErrorType::Open:
                        out.push_back(String::Format("{0}: Cannot open file:").arg(fileName));
                        out.push_back(getErrorString());
                        break;
                    case ErrorType::Stat:
                        out.push_back(String::Format("{0}: Cannot stat file:").arg(fileName));
                        out.push_back(getErrorString());
                        break;
                    case ErrorType::MemoryMap:
                        out.push_back(String::Format("{0}: Cannot memory map:").arg(fileName));
                        out.push_back(getErrorString());
                        break;
                    case ErrorType::Close:
                        out.push_back(String::Format("{0}: Cannot close:").arg(fileName));
                        out.push_back(getErrorString());
                        break;
                    case ErrorType::CloseMemoryMap:
                        out.push_back(String::Format("{0}: Cannot unmap:").arg(fileName));
                        out.push_back(getErrorString());
                        break;
                    case ErrorType::Read:
                        out.push_back(String::Format("{0}: Cannot read.").arg(fileName));
                        break;
                    case ErrorType::ReadMemoryMap:
                        out.push_back(String::Format("{0}: Cannot read memory map.").arg(fileName));
                        break;
                    case ErrorType::Write:
                        out.push_back(String::Format("{0}: Cannot write:").arg(fileName));
                        out.push_back(getErrorString());
                        break;
                    case ErrorType::Seek:
                        out.push_back(String::Format("{0}: Cannot seek:").arg(fileName));
                        out.push_back(getErrorString());
                        break;
                    case ErrorType::SeekMemoryMap:
                        out.push_back(String::Format("{0}: Cannot seek memory map.").arg(fileName));
                        break;
                    default: break;
                    }
                    return String::join(out, ' ');
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
                    openFlags = O_RDWR | O_CREAT;
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
                const Path path(getTemp(), "XXXXXX");
                const std::string fileName = path.get();
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

                _mode = Mode::First;
                _pos  = 0;
                _size = 0;
                
                return out;
            }
            
            bool FileIO::isOpen() const
            {
                return _f != -1;
            }

            bool FileIO::isEOF() const
            {
                return
                    -1 == _f ||
                    (_size ? _pos >= _size : true);
            }
            
            void FileIO::read(void* in, size_t size, size_t wordSize)
            {
                if (-1 == _f)
                {
                    throw Error(getErrorMessage(ErrorType::Read, _fileName));
                }
                    
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
                    if (_endianConversion && wordSize > 1)
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
                    if (_endianConversion && wordSize > 1)
                    {
                        Memory::endian(in, size, wordSize);
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
                    if (_endianConversion && wordSize > 1)
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
                if (-1 == _f)
                {
                    throw Error(getErrorMessage(ErrorType::Write, _fileName));
                }

                const uint8_t* inP = reinterpret_cast<const uint8_t*>(in);
                std::vector<uint8_t> tmp;
                if (_endianConversion && wordSize > 1)
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

