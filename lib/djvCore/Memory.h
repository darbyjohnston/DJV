// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Enum.h>

namespace djv
{
    namespace Core
    {
        //! This namespace provides memory functionality.
        namespace Memory
        {
            //! \name Units
            ///@{

            const uint64_t kilobyte = 1024; //!< The number of bytes in a kilobyte
            const uint64_t megabyte = kilobyte * 1024; //!< The number of bytes in a megabyte
            const uint64_t gigabyte = megabyte * 1024; //!< The number of bytes in a gigabyte
            const uint64_t terabyte = gigabyte * 1024; //!< The number of bytes in a terabyte

            enum class Unit
            {
                KB,
                MB,
                GB,
                TB,

                Count,
                First = KB
            };
            DJV_ENUM_HELPERS(Unit);
            std::string getSizeLabel(uint64_t);
            std::string getUnitLabel(uint64_t);

            ///@}

            //! \name Endian
            ///@{

            enum class Endian
            {
                MSB, //!< Most siginificant byte first
                LSB, //!< Least significant byte first

                Count,
                First = MSB
            };
            DJV_ENUM_HELPERS(Endian);

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

        } // namespace Memory
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS(Core::Memory::Unit);
    DJV_ENUM_SERIALIZE_HELPERS(Core::Memory::Endian);

} // namespace djv

#include <djvCore/MemoryInline.h>
