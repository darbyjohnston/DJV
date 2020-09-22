// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/FileIO.h>

#include <sstream>

namespace djv
{
    namespace System
    {
        namespace File
        {
            IO::IO()
            {}

            IO::~IO()
            {
                close();
            }

            std::shared_ptr<IO> IO::create()
            {
                return std::shared_ptr<IO>(new IO);
            }

            void IO::setPos(size_t in)
            {
                _setPos(in, false);
            }

            void IO::seek(size_t in)
            {
                _setPos(in, true);
            }
            
            void IO::read8(int8_t* value, size_t size)
            {
                return read(value, size, 1);
            }

            void IO::readU8(uint8_t* value, size_t size)
            {
                return read(value, size, 1);
            }

            void IO::read16(int16_t* value, size_t size)
            {
                return read(value, size, 2);
            }

            void IO::readU16(uint16_t* value, size_t size)
            {
                return read(value, size, 2);
            }

            void IO::read32(int32_t* value, size_t size)
            {
                return read(value, size, 4);
            }

            void IO::readU32(uint32_t* value, size_t size)
            {
                return read(value, size, 4);
            }

            void IO::readF32(float* value, size_t size)
            {
                return read(value, size, 4);
            }

            void IO::write8(const int8_t* value, size_t size)
            {
                write(value, size, 1);
            }

            void IO::writeU8(const uint8_t* value, size_t size)
            {
                write(value, size, 1);
            }

            void IO::write16(const int16_t* value, size_t size)
            {
                write(value, size, 2);
            }

            void IO::writeU16(const uint16_t* value, size_t size)
            {
                write(value, size, 2);
            }

            void IO::write32(const int32_t* value, size_t size)
            {
                return write(value, size, 4);
            }

            void IO::writeU32(const uint32_t* value, size_t size)
            {
                return write(value, size, 4);
            }

            void IO::writeF32(const float* value, size_t size)
            {
                write(value, size, 4);
            }

            void IO::write8(int8_t value)
            {
                write8(&value, 1);
            }

            void IO::writeU8(uint8_t value)
            {
                writeU8(&value, 1);
            }

            void IO::write16(int16_t value)
            {
                write16(&value, 1);
            }

            void IO::writeU16(uint16_t value)
            {
                writeU16(&value, 1);
            }

            void IO::write32(int32_t value)
            {
                write32(&value, 1);
            }

            void IO::writeU32(uint32_t value)
            {
                writeU32(&value, 1);
            }

            void IO::writeF32(float value)
            {
                writeF32(&value, 1);
            }

            void IO::write(const std::string& value)
            {
                write8(reinterpret_cast<const int8_t*>(value.c_str()), value.size());
            }

        } // namespace File
    } // namespace System
} // namespace djv
