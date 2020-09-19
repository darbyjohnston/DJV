// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <stdint.h>

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

            enum class Unit
            {
                KB,
                MB,
                GB,
                TB,

                Count,
                First = KB
            };

            enum class Endian
            {
                MSB, //!< Most siginificant byte first
                LSB, //!< Least significant byte first

                Count,
                First = MSB
            };

        } // namespace Memory
    } // namespace Core
} // namespace djv

