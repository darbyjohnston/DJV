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

#include <GpCore/FileIO.h>

#include <GpCore/Error.h>
#include <GpCore/Math.h>
#include <GpCore/Memory.h>

#include <sstream>

#if defined(GP_PLATFORM_LINUX)
#include <linux/limits.h>
#endif // GP_PLATFORM_LINUX
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define _STAT struct stat
#define _STAT_FNC    stat

namespace Gp
{
    namespace Core
    {
        struct FileIO::Private
        {
            int            f         = -1;
            std::string    fileName;
            Mode           mode      = Mode::First;
            size_t         pos       = 0;
            size_t         size      = 0;
            bool           endian    = false;
            void*          mmap      = (void *) - 1;
            const uint8_t* mmapStart = nullptr;
            const uint8_t* mmapEnd   = nullptr;
            const uint8_t* mmapP     = nullptr;
        };

        FileIO::FileIO() :
            _p(new Private)
        {}

        FileIO::FileIO(FileIO&& other)
        {
            _p.reset(other._p.release());
            other._p.reset(new Private);
        }

        FileIO::~FileIO()
        {
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
            default: break;
            }
            _p->f = ::open(fileName.c_str(), openFlags, openMode);
            if (-1 == _p->f)
            {
                std::stringstream s;
                s << "Cannot open file: " << fileName;
                throw std::runtime_error(s.str());
            }

            // Stat the file.
            _STAT info;
            memset(&info, 0, sizeof(_STAT));
            if (_STAT_FNC(fileName.c_str(), &info) != 0)
            {
                std::stringstream s;
                s << "Cannot stat file: " << fileName;
                throw std::runtime_error(s.str());
            }

            _p->fileName = fileName;
            _p->mode     = mode;
            _p->pos      = 0;
            _p->size     = info.st_size;

            // Memory mapping.
            if (Mode::Read == _p->mode && _p->size > 0)
            {
                _p->mmap = mmap(0, _p->size, PROT_READ, MAP_SHARED, _p->f, 0);
                if (_p->mmap == (void *) - 1)
                {
                    std::stringstream s;
                    s << "Cannot map file: " << _p->fileName;
                    throw std::runtime_error(s.str());
                }

                _p->mmapStart = reinterpret_cast<const uint8_t *>(_p->mmap);
                _p->mmapEnd   = _p->mmapStart + _p->size;
                _p->mmapP     = _p->mmapStart;
            }
        }
        
        void FileIO::openTempFile(const std::string& fileName)
        {
            close();

            // Open the file.
            const std::string s = fileName + ".XXXXXX";
            const size_t size = s.size();
            std::vector<char> buf(size + 1);
            memcpy(buf.data(), s.c_str(), size);
            buf[size] = 0;
            _p->f = mkstemp(buf.data());
            if (-1 == _p->f)
            {
                std::stringstream s;
                s << "Cannot open file: " << fileName;
                throw std::runtime_error(s.str());
            }

            // Stat the file.
            _STAT info;
            memset(&info, 0, sizeof(_STAT));
            if (_STAT_FNC(buf.data(), &info) != 0)
            {
                std::stringstream s;
                s << "Cannot stat file: " << fileName;
                throw std::runtime_error(s.str());
            }
            _p->fileName = std::string(buf.data());
            _p->mode     = Mode::ReadWrite;
            _p->pos      = 0;
            _p->size     = info.st_size;
        }

        void FileIO::close()
        {
            if (_p->mmap != (void *) - 1)
            {
                int r = munmap(_p->mmap, _p->size);

                if (-1 == r)
                {
                    //! \bug [1.0 S] Add error code.
                }

                _p->mmap = (void *)-1;
            }
            _p->mmapStart = 0;
            _p->mmapEnd   = 0;

            if (_p->f != -1)
            {
                int r = ::close(_p->f);

                if (-1 == r)
                {
                    //! \bug [1.0 S] Add error code.
                }

                _p->f = -1;
            }

            _p->pos  = 0;
            _p->size = 0;
            _p->mode = static_cast<Mode>(0);
        }

        bool FileIO::isOpen() const
        {
            return
                _p->f != -1 &&
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

        bool FileIO::isEOF() const
        {
            return
                -1 == _p->f ||
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
                    std::stringstream s;
                    s << "Cannot read file: " << _p->fileName;
                    throw std::runtime_error(s.str());
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
                const size_t r = ::read(_p->f, in, size * wordSize);
                if (r != size * wordSize)
                {
                    std::stringstream s;
                    s << "Cannot read file: " << _p->fileName;
                    throw std::runtime_error(s.str());
                }
                if (_p->endian && wordSize > 1)
                {
                    Memory::endian(in, size, wordSize);
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

            if (::write(_p->f, p, size * wordSize) == -1)
            {
                std::stringstream s;
                s << "Cannot write file: " << _p->fileName;
                throw std::runtime_error(s.str());
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

        bool FileIO::getEndian() const
        {
            return _p->endian;
        }

        void FileIO::setEndian(bool in)
        {
            _p->endian = in;
        }

        void FileIO::_setPos(size_t in, bool seek)
        {
            switch (_p->mode)
            {
            case Mode::Read:
                if (!seek)
                {
                    _p->mmapP = reinterpret_cast<const uint8_t*>(_p->mmapStart) + in;
                }
                else
                {
                    _p->mmapP += in;
                }
                if (_p->mmapP > _p->mmapEnd)
                {
                    std::stringstream s;
                    s << "Cannot read file: " << _p->fileName;
                    throw std::runtime_error(s.str());
                }
                break;
            case Mode::Write:
            case Mode::ReadWrite:
                if (::lseek(_p->f, in, ! seek ? SEEK_SET : SEEK_CUR) == (off_t) - 1)
                {
                    std::stringstream s;
                    s << "Cannot write file: " << _p->fileName;
                    throw std::runtime_error(s.str());
                }
                break;
            default: break;
            }

            if (!seek)
            {
                _p->pos = in;
            }
            else
            {
                _p->pos += in;
            }
        }

    } // namespace Core
} // namespace Gp
