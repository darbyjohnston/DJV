// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            inline FileIO::FileIO()
            {}

            inline bool FileIO::isOpen() const
            {
#if defined(DJV_PLATFORM_WINDOWS)
#if defined(DJV_MMAP)
                return _f != INVALID_HANDLE_VALUE;
#else // DJV_MMAP
                return _f != nullptr;
#endif // DJV_MMAP
#else // DJV_PLATFORM_WINDOWS
                return _f != -1;
#endif //DJV_PLATFORM_WINDOWS
            }

            inline const std::string & FileIO::getFileName() const
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
#if defined(DJV_MMAP)
                return
                    _f == INVALID_HANDLE_VALUE ||
                    (_size ? _pos >= _size : true);
#else // DJV_MMAP
                return
                    !_f ||
                    (_size ? _pos >= _size : true);
#endif // DJV_MMAP
#else // DJV_PLATFORM_WINDOWS
                return
                    !_f ||
                    (_size ? _pos >= _size : true);
#endif //DJV_PLATFORM_WINDOWS
            }

#if defined(DJV_MMAP)
            inline const uint8_t * FileIO::mmapP() const
            {
                return _mmapP;
            }

            inline const uint8_t * FileIO::mmapEnd() const
            {
                return _mmapEnd;
            }
#endif // DJV_MMAP

            inline bool FileIO::hasEndianConversion() const
            {
                return _endianConversion;
            }

            inline void FileIO::read8(int8_t * value, size_t size)
            {
                return read(value, size, 1);
            }

            inline void FileIO::readU8(uint8_t * value, size_t size)
            {
                return read(value, size, 1);
            }

            inline void FileIO::read16(int16_t * value, size_t size)
            {
                return read(value, size, 2);
            }

            inline void FileIO::readU16(uint16_t * value, size_t size)
            {
                return read(value, size, 2);
            }

            inline void FileIO::read32(int32_t * value, size_t size)
            {
                return read(value, size, 4);
            }

            inline void FileIO::readU32(uint32_t * value, size_t size)
            {
                return read(value, size, 4);
            }

            inline void FileIO::readF32(float * value, size_t size)
            {
                return read(value, size, 4);
            }

            inline void FileIO::write8(const int8_t * value, size_t size)
            {
                write(value, size, 1);
            }

            inline void FileIO::writeU8(const uint8_t * value, size_t size)
            {
                write(value, size, 1);
            }

            inline void FileIO::write16(const int16_t * value, size_t size)
            {
                write(value, size, 2);
            }

            inline void FileIO::writeU16(const uint16_t * value, size_t size)
            {
                write(value, size, 2);
            }

            inline void FileIO::write32(const int32_t * value, size_t size)
            {
                return write(value, size, 4);
            }

            inline void FileIO::writeU32(const uint32_t * value, size_t size)
            {
                return write(value, size, 4);
            }

            inline void FileIO::writeF32(const float * value, size_t size)
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

            inline void FileIO::write(const std::string & value)
            {
                write8(reinterpret_cast<const int8_t *>(value.c_str()), value.size());
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

