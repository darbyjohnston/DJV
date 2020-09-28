// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Enum.h>
#include <djvCore/Memory.h>

#include <sstream>
#include <vector>

namespace djv
{
    namespace Core
    {
        namespace Memory
        {
            //! \name Units
            ///@{

            std::string getSizeLabel(uint64_t);
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

#include <djvCore/MemoryFuncInline.h>

