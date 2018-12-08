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

namespace djv
{
    namespace Core
    {
        inline FileIO::FileIO()
        {}

        inline bool FileIO::isOpen() const
        {
#if defined(DJV_PLATFORM_WINDOWS)
            return
                _f != INVALID_HANDLE_VALUE &&
                (_size ? _pos < _size : false);
#else // DJV_PLATFORM_WINDOWS
            return
                _f != -1 &&
                (_size ? _pos < _size : false);
#endif //DJV_PLATFORM_WINDOWS
        }
        
        inline const std::string& FileIO::getFileName() const
        {
            return _fileName;
        }

        inline size_t FileIO::getSize() const
        {
            return _size;
        }

        inline size_t FileIO::getPos() const
        {
            return _pos;
        }

        inline bool FileIO::isEOF() const
        {
#if defined(DJV_PLATFORM_WINDOWS)
            return
                _f == INVALID_HANDLE_VALUE ||
                (_size ? _pos >= _size : true);
#else // DJV_PLATFORM_WINDOWS
            return
                -1 == _f ||
                (_size ? _pos >= _size : true);
#endif //DJV_PLATFORM_WINDOWS
        }

        inline const uint8_t* FileIO::mmapP() const
        {
            return _mmapP;
        }

        inline const uint8_t* FileIO::mmapEnd() const
        {
            return _mmapEnd;
        }

        inline bool FileIO::getEndian() const
        {
            return _endian;
        }

        inline void FileIO::read8(int8_t* value, size_t size)
        {
            return read(value, size, 1);
        }

        inline void FileIO::readU8(uint8_t* value, size_t size)
        {
            return read(value, size, 1);
        }

        inline void FileIO::read16(int16_t* value, size_t size)
        {
            return read(value, size, 2);
        }

        inline void FileIO::readU16(uint16_t* value, size_t size)
        {
            return read(value, size, 2);
        }

        inline void FileIO::read32(int32_t* value, size_t size)
        {
            return read(value, size, 4);
        }

        inline void FileIO::readU32(uint32_t* value, size_t size)
        {
            return read(value, size, 4);
        }

        inline void FileIO::readF32(float* value, size_t size)
        {
            return read(value, size, 4);
        }

        inline void FileIO::write8(const int8_t* value, size_t size)
        {
            write(value, size, 1);
        }

        inline void FileIO::writeU8(const uint8_t* value, size_t size)
        {
            write(value, size, 1);
        }

        inline void FileIO::write16(const int16_t* value, size_t size)
        {
            write(value, size, 2);
        }

        inline void FileIO::writeU16(const uint16_t* value, size_t size)
        {
            write(value, size, 2);
        }

        inline void FileIO::write32(const int32_t* value, size_t size)
        {
            return write(value, size, 4);
        }

        inline void FileIO::writeU32(const uint32_t* value, size_t size)
        {
            return write(value, size, 4);
        }

        inline void FileIO::writeF32(const float* value, size_t size)
        {
            write(value, size, 4);
        }

        inline void FileIO::write8(int8_t value)
        {
            write8(&value, 1);
        }

        inline void FileIO::writeU8(uint8_t value)
        {
            writeU8(&value, 1);
        }

        inline void FileIO::write16(int16_t value)
        {
            write16(&value, 1);
        }

        inline void FileIO::writeU16(uint16_t value)
        {
            writeU16(&value, 1);
        }

        inline void FileIO::write32(int32_t value)
        {
            write32(&value, 1);
        }

        inline void FileIO::writeU32(uint32_t value)
        {
            writeU32(&value, 1);
        }

        inline void FileIO::writeF32(float value)
        {
            writeF32(&value, 1);
        }

        inline void FileIO::write(const std::string& value)
        {
            write8(reinterpret_cast<const int8_t*>(value.c_str()), value.size());
        }

    } // namespace Core
} // namespace djv

