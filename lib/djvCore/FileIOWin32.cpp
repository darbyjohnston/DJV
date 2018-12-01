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
#include <sstream>

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

        struct FileIO::Private
        {
            HANDLE         f         = INVALID_HANDLE_VALUE;
            std::string    fileName;
            Mode           mode      = Mode::First;
            size_t         pos       = 0;
            size_t         size      = 0;
            bool           endian    = false;
            void*          mmap      = nullptr;
            const uint8_t* mmapStart = nullptr;
            const uint8_t* mmapEnd   = nullptr;
            const uint8_t* mmapP     = nullptr;
        };

        namespace
        {
            std::atomic<size_t> fileIOCount;

        } // namespace

        FileIO::FileIO() :
            _p(new Private)
        {
            ++fileIOCount;
            //std::cerr << "djv::Core::FileIO::FileIO = " << fileIOCount << std::endl;
        }

        FileIO::FileIO(FileIO&& other)
        {
            ++fileIOCount;
            //std::cerr << "djv::Core::FileIO::FileIO = " << fileIOCount << std::endl;

            _p.reset(other._p.release());
            other._p.reset(new Private);
        }

        FileIO::~FileIO()
        {
            --fileIOCount;
            //std::cerr << "djv::Core::FileIO::~FileIO = " << fileIOCount << std::endl;

            try
            {
                close();
            }
            catch (const std::exception&)
            {}
        }

        FileIO& FileIO::operator = (FileIO&& other)
        {
            if (this != &other)
            {
                _p.reset(other._p.release());
                other._p.reset(new Private);
            }
            return *this;
        }

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
            _p->f = CreateFile(fileName.c_str(), desiredAccess, shareMode, 0, disposition, flags, 0);
            if (INVALID_HANDLE_VALUE == _p->f)
            {
                throw std::runtime_error(getOpenError(fileName));
            }
            _p->fileName = fileName;
            _p->mode     = mode;
            _p->pos      = 0;
            _p->size     = GetFileSize(_p->f, 0);

            // Memory mapping.
            if (Mode::Read == _p->mode && _p->size > 0)
            {
                _p->mmap = CreateFileMapping(_p->f, 0, PAGE_READONLY, 0, 0, 0);
                if (!_p->mmap)
                {
                    throw std::runtime_error(getMemoryMapError(_p->fileName));
                }

                _p->mmapStart = reinterpret_cast<const uint8_t*>(MapViewOfFile(_p->mmap, FILE_MAP_READ, 0, 0, 0));
                if (!_p->mmapStart)
                {
                    throw std::runtime_error(getMemoryMapError(_p->fileName));
                }

                _p->mmapEnd = _p->mmapStart + _p->size;
                _p->mmapP   = _p->mmapStart;
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
            if (_p->mmapStart != 0)
            {
                if (!::UnmapViewOfFile((void*)_p->mmapStart))
                {
                    throw std::runtime_error(getMemoryUnmapError(_p->fileName));
                }
                _p->mmapStart = 0;
            }
            if (_p->mmap != 0)
            {
                if (!::CloseHandle(_p->mmap))
                {
                    throw std::runtime_error(getCloseError(_p->fileName));
                }
                _p->mmap = 0;
            }
            _p->mmapEnd = 0;
            _p->mmapP   = 0;

            if (_p->f != INVALID_HANDLE_VALUE)
            {
                CloseHandle(_p->f);
                _p->f = INVALID_HANDLE_VALUE;
            }

            _p->mode = Mode::First;
            _p->pos  = 0;
            _p->size = 0;
        }

        bool FileIO::isOpen() const
        {
            return
                _p->f != INVALID_HANDLE_VALUE &&
                (_p->size ? _p->pos < _p->size : false);
        }
        
        const std::string& FileIO::getFileName() const
        {
            return _p->fileName;
        }

        size_t FileIO::getSize() const
        {
            return _p->size;
        }

        size_t FileIO::getPos() const
        {
            return _p->pos;
        }

        void FileIO::setPos(size_t in)
        {
            _setPos(in, false);
        }

        void FileIO::seek(size_t in)
        {
            _setPos(in, true);
        }

        bool FileIO::isEOF() const
        {
            return
                _p->f == INVALID_HANDLE_VALUE ||
                (_p->size ? _p->pos >= _p->size : true);
        }

        void FileIO::read(void* in, size_t size, size_t wordSize)
        {
            switch (_p->mode)
            {
            case Mode::Read:
            {
                const uint8_t* p = _p->mmapP + size * wordSize;
                if (p > _p->mmapEnd)
                {
                    throw std::runtime_error(getReadError(_p->fileName));
                }
                if (_p->endian && wordSize > 1)
                {
                    Memory::endian(_p->mmapP, in, size, wordSize);
                }
                else
                {
                    memcpy(in, _p->mmapP, size * wordSize);
                }
                _p->mmapP = p;
                break;
            }
            case Mode::ReadWrite:
            {
                DWORD n;
                if (!::ReadFile(_p->f, in, static_cast<DWORD>(size * wordSize), &n, 0))
                {
                    throw std::runtime_error(getReadError(_p->fileName));
                }
                break;
            }
            default: break;
            }
            _p->pos += size * wordSize;
        }

        void FileIO::write(const void* in, size_t size, size_t wordSize)
        {
            uint8_t* p = (uint8_t*)in;

            std::vector<uint8_t> tmp;
            if (_p->endian && wordSize > 1)
            {
                tmp.resize(size * wordSize);
                p = tmp.data();
                Memory::endian(in, p, size, wordSize);
            }

            DWORD n = 0;
            if (!::WriteFile(_p->f, p, static_cast<DWORD>(size * wordSize), &n, 0))
            {
                throw std::runtime_error(getWriteError(_p->fileName));
            }

            _p->pos += size * wordSize;
            _p->size = std::max(_p->pos, _p->size);
        }

        const uint8_t* FileIO::mmapP() const
        {
            return _p->mmapP;
        }

        const uint8_t* FileIO::mmapEnd() const
        {
            return _p->mmapEnd;
        }

        bool FileIO::getEndian() const
        {
            return _p->endian;
        }

        void FileIO::setEndian(bool in)
        {
            _p->endian = in;
        }

        void FileIO::_setPos(size_t value, bool seek)
        {
            switch (_p->mode)
            {
            case Mode::Read:
                if (!seek)
                {
                    _p->mmapP = reinterpret_cast<const uint8_t*>(_p->mmapStart) + value;
                }
                else
                {
                    _p->mmapP += value;
                }
                if (_p->mmapP > _p->mmapEnd)
                {
                    throw std::runtime_error(getReadError(_p->fileName));
                }
                break;
            case Mode::Write:
            case Mode::ReadWrite:
            {
                LARGE_INTEGER v;
                v.QuadPart = value;
                if (!::SetFilePointerEx(
                    _p->f,
                    static_cast<LARGE_INTEGER>(v),
                    0,
                    !seek ? FILE_BEGIN : FILE_CURRENT))
                {
                    throw std::runtime_error(getWriteError(_p->fileName));
                }
                break;
            }
            default: break;
            }

            if (!seek)
            {
                _p->pos = value;
            }
            else
            {
                _p->pos += value;
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
