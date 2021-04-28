// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Enum.h>

#include <sstream>
#include <vector>

#include <cstdint>

namespace djv
{
    namespace Core
    {
        //! This namespace provides memory functionality.
        namespace Memory
        {
            const uint64_t kilobyte = 1024; //!< The number of bytes in a kilobyte
            const uint64_t megabyte = kilobyte * 1024; //!< The number of bytes in a megabyte
            const uint64_t gigabyte = megabyte * 1024; //!< The number of bytes in a gigabyte
            const uint64_t terabyte = gigabyte * 1024; //!< The number of bytes in a terabyte

            //! This enumeration provides memory units.
            enum class Unit
            {
                KB, //!< Kilobytes
                MB, //!< Megabytes
                GB, //!< Gigabytes
                TB, //!< Terabytes

                Count,
                First = KB
            };

            //! This enumeration provides endian types.
            enum class Endian
            {
                MSB, //!< Most siginificant byte first
                LSB, //!< Least significant byte first

                Count,
                First = MSB
            };

            //! \name Units
            ///@{

            //! Get a memory size label (e.g., "2.1GB").
            std::string getSizeLabel(uint64_t);

            //! Get a memory unit label for the given size (e.g., "GB").
            std::string getUnitLabel(uint64_t);

            ///@}

            //! \name Endian
            ///@{

            //! Get the current machine's endian.
            Endian getEndian() noexcept;

            //! Get the opposite of the given endian.
            Endian opposite(Endian) noexcept;

            //! Convert the endianness of a block of memory in place.
            void endian(
                void*  in,
                size_t size,
                size_t wordSize) noexcept;

            //! Convert the endianness of a block of memory.
            void endian(
                const void* in,
                void*       out,
                size_t      size,
                size_t      wordSize) noexcept;

            ///@}

            //! Combine hashes.
            //!
            //! References:
            //! - http://www.boost.org/doc/libs/1_65_1/doc/html/hash/combine.html
            template <class T>
            void hashCombine(std::size_t&, const T&);
            
            DJV_ENUM_HELPERS(Unit);
            DJV_ENUM_HELPERS(Endian);

        } // namespace Memory
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS(Core::Memory::Unit);
    DJV_ENUM_SERIALIZE_HELPERS(Core::Memory::Endian);
    
} // namespace djv

#include <djvCore/MemoryInline.h>
