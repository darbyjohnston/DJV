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
#include <djvCore/Math.h>
#include <djvCore/Memory.h>
#include <djvCore/Path.h>

#include <atomic>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <io.h>
#include <errno.h>
#include <stdio.h>

namespace djv
{
    namespace Core
    {
        namespace
        {
            std::string getOpenError(const std::string& fileName)
            {
                std::stringstream s;
                s << "Cannot open file: " << fileName << ": " << getLastError();
                return s.str();
            }

            std::string getCloseError(const std::string& fileName)
            {
                std::stringstream s;
                s << "Cannot close file: " << fileName << ": " << getLastError();
                return s.str();
            }
            
            std::string getMemoryMapError(const std::string& fileName)
            {
                std::stringstream s;
                s << "Cannot map file: " << fileName << ": " << getLastError();
                return s.str();
            }

            std::string getMemoryUnmapError(const std::string& fileName)
            {
                std::stringstream s;
                s << "Cannot unmap file: " << fileName << ": " << getLastError();
                return s.str();
            }

            std::string getReadError(const std::string& fileName)
            {
                std::stringstream s;
                s << "Cannot read file: " << fileName << ": " << getLastError();
                return s.str();
            }

            std::string getWriteError(const std::string& fileName)
            {
                std::stringstream s;
                s << "Cannot write file: " << fileName << ": " << getLastError();
                return s.str();
            }

        } // namespace

        void FileIO::open(const std::string& fileName, Mode mode)
        {
            close();

            // Open the file.
            DWORD desiredAccess = 0;
            DWORD shareMode     = 0;
            DWORD disposition   = 0;
            DWORD flags         =
                //FILE_ATTRIBUTE_NORMAL,
                //FILE_FLAG_NO_BUFFERING |
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
            default: break;
            }
            _f = CreateFile(fileName.c_str(), desiredAccess, shareMode, 0, disposition, flags, 0);
            if (INVALID_HANDLE_VALUE == _f)
            {
                throw std::runtime_error(getOpenError(fileName));
            }
            _fileName = fileName;
            _mode     = mode;
            _pos      = 0;
            _size     = GetFileSize(_f, 0);

            // Memory mapping.
            if (Mode::Read == _mode && _size > 0)
            {
                _mmap = CreateFileMapping(_f, 0, PAGE_READONLY, 0, 0, 0);
                if (!_mmap)
                {
                    throw std::runtime_error(getMemoryMapError(_fileName));
                }

                _mmapStart = reinterpret_cast<const uint8_t*>(MapViewOfFile(_mmap, FILE_MAP_READ, 0, 0, 0));
                if (!_mmapStart)
                {
                    throw std::runtime_error(getMemoryMapError(_fileName));
                }

                _mmapEnd = _mmapStart + _size;
                _mmapP   = _mmapStart;
            }
        }

        void FileIO::openTempFile(const std::string& fileName)
        {
            const Path path(fileName);
            TCHAR buf[MAX_PATH];
            if (GetTempFileName(path.getDirectoryName().c_str(), path.getBaseName().c_str(), 0, buf))
            {
                open(buf, Mode::ReadWrite);
            }
            else
            {
                throw std::runtime_error(getOpenError(fileName));
            }
        }

        void FileIO::close()
        {
            if (_mmapStart != 0)
            {
                if (!::UnmapViewOfFile((void*)_mmapStart))
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

        void FileIO::read(void* in, size_t size, size_t wordSize)
        {
            switch (_mode)
            {
            case Mode::Read:
            {
                const uint8_t* p = _mmapP + size * wordSize;
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
                break;
            }
            case Mode::ReadWrite:
            {
                DWORD n;
                if (!::ReadFile(_f, in, static_cast<DWORD>(size * wordSize), &n, 0))
                {
                    throw std::runtime_error(getReadError(_fileName));
                }
                break;
            }
            default: break;
            }
            _pos += size * wordSize;
        }

        void FileIO::write(const void* in, size_t size, size_t wordSize)
        {
            uint8_t* p = (uint8_t*)in;

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
                    throw std::runtime_error(getReadError(_fileName));
                }
                break;
            case Mode::Write:
            case Mode::ReadWrite:
            {
                LARGE_INTEGER v;
                v.QuadPart = value;
                if (!::SetFilePointerEx(
                    _f,
                    static_cast<LARGE_INTEGER>(v),
                    0,
                    !seek ? FILE_BEGIN : FILE_CURRENT))
                {
                    throw std::runtime_error(getWriteError(_fileName));
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

        FILE* fopen(const std::string& fileName, const std::string& mode)
        {
            FILE* out = nullptr;
            fopen_s(&out, fileName.c_str(), mode.c_str());
            return out;
        }

    } // namespace Core
} // namespace djv

