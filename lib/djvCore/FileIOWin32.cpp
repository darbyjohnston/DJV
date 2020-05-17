// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/FileIO.h>

#include <djvCore/Error.h>
#include <djvCore/FileSystem.h>
#include <djvCore/Math.h>
#include <djvCore/Memory.h>
#include <djvCore/Path.h>
#include <djvCore/StringFormat.h>

#include <codecvt>
#include <locale>

#include <io.h>
#include <errno.h>
#include <stdio.h>

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
                    OpenTemp,
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
                    //! \todo How can we translate this?
                    std::vector<std::string> out;
                    switch (type)
                    {
                    case ErrorType::Open:
                        out.push_back(String::Format("{0}: Cannot open file.").arg(fileName));
                        out.push_back(Core::Error::getLastError());
                        break;
                    case ErrorType::OpenTemp:
                        out.push_back("Cannot open temporary file.");
                        out.push_back(Core::Error::getLastError());
                        break;
                    case ErrorType::MemoryMap:
                        out.push_back(String::Format("{0}: Cannot memory map.").arg(fileName));
                        out.push_back(Core::Error::getLastError());
                        break;
                    case ErrorType::Close:
                        out.push_back(String::Format("{0}: Cannot close.").arg(fileName));
                        out.push_back(Core::Error::getLastError());
                        break;
                    case ErrorType::CloseMemoryMap:
                        out.push_back(String::Format("{0}: Cannot unmap.").arg(fileName));
                        out.push_back(Core::Error::getLastError());
                        break;
                    case ErrorType::Read:
                        out.push_back(String::Format("{0}: Cannot read.").arg(fileName));
                        out.push_back(Core::Error::getLastError());
                        break;
                    case ErrorType::ReadMemoryMap:
                        out.push_back(String::Format("{0}: Cannot read memory map.").arg(fileName));
                        out.push_back(Core::Error::getLastError());
                        break;
                    case ErrorType::Write:
                        out.push_back(String::Format("{0}: Cannot write.").arg(fileName));
                        out.push_back(Core::Error::getLastError());
                        break;
                    case ErrorType::Seek:
                        out.push_back(String::Format("{0}: Cannot seek.").arg(fileName));
                        out.push_back(Core::Error::getLastError());
                        break;
                    case ErrorType::SeekMemoryMap:
                        out.push_back(String::Format("{0}: Cannot seek memory map.").arg(fileName));
                        out.push_back(Core::Error::getLastError());
                        break;
                    default: break;
                    }
                    return String::join(out, ' ');
                }
                
            } // namespace
            
            void FileIO::open(const std::string& fileName, Mode mode)
            {
                close();

#if defined(DJV_MMAP)
                // Open the file.
                DWORD desiredAccess = 0;
                DWORD shareMode = 0;
                DWORD disposition = 0;
                DWORD flags =
                    //FILE_ATTRIBUTE_NORMAL;
                    FILE_FLAG_SEQUENTIAL_SCAN;
                switch (mode)
                {
                case Mode::Read:
                    desiredAccess = GENERIC_READ;
                    shareMode     = FILE_SHARE_READ;
                    disposition   = OPEN_EXISTING;
                    break;
                case Mode::Write:
                    desiredAccess = GENERIC_WRITE;
                    disposition   = CREATE_ALWAYS;
                    break;
                case Mode::ReadWrite:
                    desiredAccess = GENERIC_READ | GENERIC_WRITE;
                    shareMode     = FILE_SHARE_READ;
                    disposition   = CREATE_ALWAYS;
                    break;
                case Mode::Append:
                    desiredAccess = GENERIC_WRITE;
                    disposition   = OPEN_EXISTING;
                    break;
                default: break;
                }
                try
                {
                    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16;
                    _f = CreateFileW(utf16.from_bytes(fileName).c_str(), desiredAccess, shareMode, 0, disposition, flags, 0);
                }
                catch (const std::exception&)
                {
                    _f = INVALID_HANDLE_VALUE;
                }
                if (INVALID_HANDLE_VALUE == _f)
                {
                    throw Error(getErrorMessage(ErrorType::Open, fileName));
                }
                _fileName = fileName;
                _mode = mode;
                _pos = 0;
                _size = GetFileSize(_f, 0);

                // Memory mapping.
                if (Mode::Read == _mode && _size > 0)
                {
                    _mmap = CreateFileMapping(_f, 0, PAGE_READONLY, 0, 0, 0);
                    if (!_mmap)
                    {
                        throw Error(getErrorMessage(ErrorType::MemoryMap, fileName));
                    }

                    _mmapStart = reinterpret_cast<const uint8_t *>(MapViewOfFile(_mmap, FILE_MAP_READ, 0, 0, 0));
                    if (!_mmapStart)
                    {
                        throw Error(getErrorMessage(ErrorType::MemoryMap, fileName));
                    }

                    _mmapEnd = _mmapStart + _size;
                    _mmapP = _mmapStart;
                }
#else // DJV_MMAP
                std::string modeStr;
                switch (mode)
                {
                case Mode::Read:
                    modeStr = "r";
                    break;
                case Mode::Write:
                    modeStr = "w";
                    break;
                case Mode::ReadWrite:
                    modeStr = "r+";
                    break;
                case Mode::Append:
                    modeStr = "a";
                    break;
                default: break;
                }
                _f = fopen(fileName.c_str(), modeStr.c_str());
                if (!_f)
                {
                    throw Error(getErrorMessage(ErrorType::Open, fileName));
                }
                _fileName = fileName;
                _mode = mode;
                _pos = 0;
                if (fseek(_f, 0, SEEK_END) != 0)
                {
                    throw Error(getErrorMessage(ErrorType::Open, fileName));
                }
                _size = ftell(_f);
                if (fseek(_f, 0, SEEK_SET) != 0)
                {
                    throw Error(getErrorMessage(ErrorType::Open, fileName));
                }
#endif // DJV_MMAP
            }

            void FileIO::openTemp()
            {
                WCHAR path[MAX_PATH];
                DWORD r = GetTempPathW(MAX_PATH, path);
                if (!r)
                {
                    throw Error(getErrorMessage(ErrorType::OpenTemp, std::string()));
                }
                WCHAR buf[MAX_PATH];
                if (GetTempFileNameW(path, L"", 0, buf))
                {
                    std::string fileName;
                    try
                    {
                        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16;
                        fileName = utf16.to_bytes(buf);
                    }
                    catch (const std::exception&)
                    {
                        throw Error(getErrorMessage(ErrorType::OpenTemp, fileName));
                    }
                    open(fileName, Mode::ReadWrite);
                }
                else
                {
                    throw Error(getErrorMessage(ErrorType::OpenTemp, std::string()));
                }
            }

            bool FileIO::close(std::string* error)
            {
                bool out = true;

                _fileName = std::string();
                
#if defined(DJV_MMAP)
                if (_mmapStart != 0)
                {
                    if (!::UnmapViewOfFile((void *)_mmapStart))
                    {
                        out = false;
                        if (error)
                        {
                            *error = getErrorMessage(Error::CloseMemoryMap, _fileName);
                        }
                    }
                    _mmapStart = 0;
                }
                if (_mmap != 0)
                {
                    if (!::CloseHandle(_mmap))
                    {
                        out = false;
                        if (error)
                        {
                            *error = getErrorMessage(Error::Close, _fileName);
                        }
                    }
                    _mmap = 0;
                }
                _mmapEnd = 0;
                _mmapP   = 0;

                if (_f != INVALID_HANDLE_VALUE)
                {
                    CloseHandle(_f);
                    _f = INVALID_HANDLE_VALUE;
                }
#else // DJV_MMAP
                if (_f)
                {
                    fclose(_f);
                    _f = nullptr;
                }
#endif // DJV_MMAP

                _mode = Mode::First;
                _pos  = 0;
                _size = 0;

                return out;
            }

            void FileIO::read(void * in, size_t size, size_t wordSize)
            {
                switch (_mode)
                {
                case Mode::Read:
                {
#if defined(DJV_MMAP)
                    const uint8_t * p = _mmapP + size * wordSize;
                    if (p > _mmapEnd)
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
                    _mmapP = p;
#else // DJV_MMAP
                    /*DWORD n;
                    if (!::ReadFile(_f, in, static_cast<DWORD>(size * wordSize), &n, 0))
                    {
                        throw Error(getErrorMessage(ErrorType::Read, _fileName));
                    }*/
                    size_t r = fread(in, 1, size * wordSize, _f);
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
                    DWORD n;
                    /*if (!::ReadFile(_f, in, static_cast<DWORD>(size * wordSize), &n, 0))
                    {
                        throw Error(getErrorMessage(ErrorType::Read, _fileName));
                    }*/
                    size_t r = fread(in, 1, size * wordSize, _f);
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

            void FileIO::write(const void * in, size_t size, size_t wordSize)
            {
                const uint8_t* inP = reinterpret_cast<const uint8_t*>(in);

                std::vector<uint8_t> tmp;
                if (_endianConversion && wordSize > 1)
                {
                    tmp.resize(size * wordSize);
                    Memory::endian(in, tmp.data(), size, wordSize);
                    in = tmp.data();
                }

                /*DWORD n = 0;
                if (!::WriteFile(_f, p, static_cast<DWORD>(size * wordSize), &n, 0))
                {
                    throw Error(getErrorMessage(ErrorType::Write, _fileName));
                }*/
                size_t r = fwrite(in, 1, size * wordSize, _f);
                if (r != size * wordSize)
                {
                    throw Error(getErrorMessage(ErrorType::Write, _fileName));
                }

                _pos += size * wordSize;
                _size = std::max(_pos, _size);
            }

            void FileIO::_setPos(size_t value, bool seek)
            {
                switch (_mode)
                {
                case Mode::Read:
                {
#if defined(DJV_MMAP)
                    if (!seek)
                    {
                        _mmapP = reinterpret_cast<const uint8_t*>(_mmapStart) + value;
                    }
                    else
                    {
                        _mmapP += value;
                    }
                    if (_mmapP > _mmapEnd)
                    {
                        throw Error(getErrorMessage(ErrorType::SeekMemoryMap, _fileName));
                    }
#else // DJV_MMAP
                    /*LARGE_INTEGER v;
                    v.QuadPart = value;
                    if (!::SetFilePointerEx(
                        _f,
                        static_cast<LARGE_INTEGER>(v),
                        0,
                        !seek ? FILE_BEGIN : FILE_CURRENT))
                    {
                        throw Error(getErrorMessage(ErrorType::Seek, _fileName));
                    }*/
                    if (fseek(_f, value, !seek ? SEEK_SET : SEEK_CUR) != 0)
                    {
                        throw Error(getErrorMessage(ErrorType::Seek, _fileName));
                    }
#endif // DJV_MMAP
                    break;
                }
                case Mode::Write:
                case Mode::ReadWrite:
                case Mode::Append:
                {
                    /*LARGE_INTEGER v;
                    v.QuadPart = value;
                    if (!::SetFilePointerEx(
                        _f,
                        static_cast<LARGE_INTEGER>(v),
                        0,
                        !seek ? FILE_BEGIN : FILE_CURRENT))
                    {
                        throw Error(getErrorMessage(ErrorType::Seek, _fileName));
                    }*/
                    if (fseek(_f, value, !seek ? SEEK_SET : SEEK_CUR) != 0)
                    {
                        throw Error(getErrorMessage(ErrorType::Seek, _fileName));
                    }
                    break;
                }
                default: break;
                }

                if (!seek)
                {
                    _pos = value;
                }
                else
                {
                    _pos += value;
                }
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

