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
                std::string getOpenError(const std::string& fileName)
                {
                    std::stringstream ss;
                    ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be opened") << ". " << Error::getLastError();
                    return ss.str();
                }

                std::string getOpenTempError()
                {
                    std::stringstream ss;
                    ss << DJV_TEXT("The temporary file cannot be opened") << ". " << Error::getLastError();
                    return ss.str();
                }

                std::string getCloseError(const std::string & fileName)
                {
                    std::stringstream ss;
                    ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be closed") << ". " << Error::getLastError();
                    return ss.str();
                }

                std::string getMemoryMapError(const std::string & fileName)
                {
                    std::stringstream ss;
                    ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be mapped") << ". " << Error::getLastError();
                    return ss.str();
                }

                std::string getMemoryUnmapError(const std::string & fileName)
                {
                    std::stringstream ss;
                    ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be unmapped") << ". " << Error::getLastError();
                    return ss.str();
                }

                std::string getReadError(const std::string & fileName)
                {
                    std::stringstream ss;
                    ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be read") << ". " << Error::getLastError();
                    return ss.str();
                }

                std::string getWriteError(const std::string & fileName)
                {
                    std::stringstream ss;
                    ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be written") << ". " << Error::getLastError();
                    return ss.str();
                }

                std::string getSeekError(const std::string& fileName)
                {
                    std::stringstream ss;
                    ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be seeked") << ". " << Error::getLastError();
                    return ss.str();
                }

            } // namespace

            void FileIO::open(const std::string & fileName, Mode mode)
            {
                close();

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
                    throw std::runtime_error(getOpenError(fileName));
                }
                _fileName = fileName;
                _mode = mode;
                _pos = 0;
                _size = GetFileSize(_f, 0);

#if defined(DJV_MMAP)
                // Memory mapping.
                if (Mode::Read == _mode && _size > 0)
                {
                    _mmap = CreateFileMapping(_f, 0, PAGE_READONLY, 0, 0, 0);
                    if (!_mmap)
                    {
                        throw std::runtime_error(getMemoryMapError(_fileName));
                    }

                    _mmapStart = reinterpret_cast<const uint8_t *>(MapViewOfFile(_mmap, FILE_MAP_READ, 0, 0, 0));
                    if (!_mmapStart)
                    {
                        throw std::runtime_error(getMemoryMapError(_fileName));
                    }

                    _mmapEnd = _mmapStart + _size;
                    _mmapP = _mmapStart;
                }
#endif // DJV_MMAP
            }

            void FileIO::openTemp()
            {
                WCHAR path[MAX_PATH];
                DWORD r = GetTempPathW(MAX_PATH, path);
                if (!r)
                {
                    throw std::runtime_error(getOpenTempError());
                }
                WCHAR buf[MAX_PATH];
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16;
                if (GetTempFileNameW(path, L"", 0, buf))
                {
                    open(utf16.to_bytes(buf), Mode::ReadWrite);
                }
                else
                {
                    throw std::runtime_error(getOpenTempError());
                }
            }

            void FileIO::close()
            {
                _fileName = std::string();
                
#if defined(DJV_MMAP)
                if (_mmapStart != 0)
                {
                    if (!::UnmapViewOfFile((void *)_mmapStart))
                    {
                        throw std::runtime_error(getMemoryUnmapError(_fileName));
                    }
                    _mmapStart = 0;
                }
                if (_mmap != 0)
                {
                    if (!::CloseHandle(_mmap))
                    {
                        throw std::runtime_error(getCloseError(_fileName));
                    }
                    _mmap = 0;
                }
                _mmapEnd = 0;
                _mmapP   = 0;
#endif // DJV_MMAP

                if (_f != INVALID_HANDLE_VALUE)
                {
                    CloseHandle(_f);
                    _f = INVALID_HANDLE_VALUE;
                }

                _mode = Mode::First;
                _pos  = 0;
                _size = 0;
            }

            void FileIO::setPos(size_t in)
            {
                _setPos(in, false);
            }

            void FileIO::seek(size_t in)
            {
                _setPos(in, true);
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
                        throw std::runtime_error(getReadError(_fileName));
                    }
                    if (_endian && wordSize > 1)
                    {
                        Memory::endian(_mmapP, in, size, wordSize);
                    }
                    else
                    {
                        memcpy(in, _mmapP, size * wordSize);
                    }
                    _mmapP = p;
#else // DJV_MMAP
                    DWORD n;
                    if (!::ReadFile(_f, in, static_cast<DWORD>(size * wordSize), &n, 0))
                    {
                        throw std::runtime_error(getReadError(_fileName));
                    }
                    if (_endian && wordSize > 1)
                    {
                        Memory::endian(in, size, wordSize);
                    }
#endif // DJV_MMAP
                    break;
                }
                case Mode::ReadWrite:
                {
                    DWORD n;
                    if (!::ReadFile(_f, in, static_cast<DWORD>(size * wordSize), &n, 0))
                    {
                        throw std::runtime_error(getReadError(_fileName));
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

            void FileIO::write(const void * in, size_t size, size_t wordSize)
            {
                uint8_t * p = (uint8_t *)in;

                std::vector<uint8_t> tmp;
                if (_endian && wordSize > 1)
                {
                    tmp.resize(size * wordSize);
                    p = tmp.data();
                    Memory::endian(in, p, size, wordSize);
                }

                DWORD n = 0;
                if (!::WriteFile(_f, p, static_cast<DWORD>(size * wordSize), &n, 0))
                {
                    throw std::runtime_error(getWriteError(_fileName));
                }

                _pos += size * wordSize;
                _size = std::max(_pos, _size);
            }

            void FileIO::setEndian(bool in)
            {
                _endian = in;
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
                        throw std::runtime_error(getSeekError(_fileName));
                    }
#else // DJV_MMAP
                    LARGE_INTEGER v;
                    v.QuadPart = value;
                    if (!::SetFilePointerEx(
                        _f,
                        static_cast<LARGE_INTEGER>(v),
                        0,
                        !seek ? FILE_BEGIN : FILE_CURRENT))
                    {
                        throw std::runtime_error(getSeekError(_fileName));
                    }
#endif // DJV_MMAP
                    break;
                }
                case Mode::Write:
                case Mode::ReadWrite:
                case Mode::Append:
                {
                    LARGE_INTEGER v;
                    v.QuadPart = value;
                    if (!::SetFilePointerEx(
                        _f,
                        static_cast<LARGE_INTEGER>(v),
                        0,
                        !seek ? FILE_BEGIN : FILE_CURRENT))
                    {
                        throw std::runtime_error(getSeekError(_fileName));
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

            FILE * fopen(const std::string & fileName, const std::string & mode)
            {
                FILE * out = nullptr;
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16;
                _wfopen_s(&out, utf16.from_bytes(fileName).c_str(), utf16.from_bytes(mode).c_str());
                return out;
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

