//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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
#include <djvCore/Path.h>

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
                    std::stringstream ss;
                    switch (type)
                    {
                    case ErrorType::Open:
                        ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be opened") << ". " << Core::Error::getLastError();
                        break;
                    case ErrorType::OpenTemp:
                        ss << DJV_TEXT("The temporary file cannot be opened") << ". " << Core::Error::getLastError();
                        break;
                    case ErrorType::MemoryMap:
                        ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be mapped") << ". " << Core::Error::getLastError();
                        break;
                    case ErrorType::Close:
                        ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be closed") << ". " << Core::Error::getLastError();
                        break;
                    case ErrorType::CloseMemoryMap:
                        ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be unmapped") << ". " << Core::Error::getLastError();
                        break;
                    case ErrorType::Read:
                        ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be read") << ". " << Core::Error::getLastError();
                        break;
                    case ErrorType::ReadMemoryMap:
                        ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be read") << ".";
                        break;
                    case ErrorType::Write:
                        ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be written") << ". " << Core::Error::getLastError();
                        break;
                    case ErrorType::Seek:
                        ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be seeked") << ". " << Core::Error::getLastError();
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
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16;
                _f = CreateFileW(utf16.from_bytes(fileName).c_str(), desiredAccess, shareMode, 0, disposition, flags, 0);
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
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16;
                if (GetTempFileNameW(path, L"", 0, buf))
                {
                    open(utf16.to_bytes(buf), Mode::ReadWrite);
                }
                else
                {
                    throw Error(getErrorMessage(ErrorType::OpenTemp, _fileName));
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
                    p = tmp.data();
                    Memory::endian(in, tmp.data(), size, wordSize);
                }

                /*DWORD n = 0;
                if (!::WriteFile(_f, p, static_cast<DWORD>(size * wordSize), &n, 0))
                {
                    throw Error(getErrorMessage(ErrorType::Write, _fileName));
                }*/
                size_t r = fwrite(p, 1, size * wordSize, _f);
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

