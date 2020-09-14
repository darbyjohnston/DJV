// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/FileIO.h>

#include <sstream>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            FileIO::FileIO()
            {}

            FileIO::~FileIO()
            {
                close();
            }

            std::shared_ptr<FileIO> FileIO::create()
            {
                return std::shared_ptr<FileIO>(new FileIO);
            }

            const std::string& FileIO::getFileName() const
            {
                return _fileName;
            }

            size_t FileIO::getSize() const
            {
                return _size;
            }

            size_t FileIO::getPos() const
            {
                return _pos;
            }

            void FileIO::setPos(size_t in)
            {
                _setPos(in, false);
            }

            void FileIO::seek(size_t in)
            {
                _setPos(in, true);
            }
            
            void FileIO::read8(int8_t* value, size_t size)
            {
                return read(value, size, 1);
            }

            void FileIO::readU8(uint8_t* value, size_t size)
            {
                return read(value, size, 1);
            }

            void FileIO::read16(int16_t* value, size_t size)
            {
                return read(value, size, 2);
            }

            void FileIO::readU16(uint16_t* value, size_t size)
            {
                return read(value, size, 2);
            }

            void FileIO::read32(int32_t* value, size_t size)
            {
                return read(value, size, 4);
            }

            void FileIO::readU32(uint32_t* value, size_t size)
            {
                return read(value, size, 4);
            }

            void FileIO::readF32(float* value, size_t size)
            {
                return read(value, size, 4);
            }

            void FileIO::write8(const int8_t* value, size_t size)
            {
                write(value, size, 1);
            }

            void FileIO::writeU8(const uint8_t* value, size_t size)
            {
                write(value, size, 1);
            }

            void FileIO::write16(const int16_t* value, size_t size)
            {
                write(value, size, 2);
            }

            void FileIO::writeU16(const uint16_t* value, size_t size)
            {
                write(value, size, 2);
            }

            void FileIO::write32(const int32_t* value, size_t size)
            {
                return write(value, size, 4);
            }

            void FileIO::writeU32(const uint32_t* value, size_t size)
            {
                return write(value, size, 4);
            }

            void FileIO::writeF32(const float* value, size_t size)
            {
                write(value, size, 4);
            }

            void FileIO::write8(int8_t value)
            {
                write8(&value, 1);
            }

            void FileIO::writeU8(uint8_t value)
            {
                writeU8(&value, 1);
            }

            void FileIO::write16(int16_t value)
            {
                write16(&value, 1);
            }

            void FileIO::writeU16(uint16_t value)
            {
                writeU16(&value, 1);
            }

            void FileIO::write32(int32_t value)
            {
                write32(&value, 1);
            }

            void FileIO::writeU32(uint32_t value)
            {
                writeU32(&value, 1);
            }

            void FileIO::writeF32(float value)
            {
                writeF32(&value, 1);
            }

            void FileIO::write(const std::string& value)
            {
                write8(reinterpret_cast<const int8_t*>(value.c_str()), value.size());
            }
            
#if defined(DJV_MMAP)
            const uint8_t* FileIO::mmapP() const
            {
                return _mmapP;
            }

            const uint8_t* FileIO::mmapEnd() const
            {
                return _mmapEnd;
            }
#endif // DJV_MMAP

            bool FileIO::hasEndianConversion() const
            {
                return _endianConversion;
            }
            
            void FileIO::setEndianConversion(bool in)
            {
                _endianConversion = in;
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv
