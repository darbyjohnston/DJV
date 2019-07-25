//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#pragma once

#include <djvCore/Enum.h>

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

            //! Convert a byte count to a human readable string.
            std::string getSizeLabel(uint64_t);

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
            inline Endian getEndian();

            //! Get the opposite of the given endian.
            inline Endian opposite(Endian);

            //! Convert the endianness of a block of memory in place.
            inline void endian(
                void*  in,
                size_t size,
                size_t wordSize);

            //! Convert the endianness of a block of memory.
            inline void endian(
                const void* in,
                void*       out,
                size_t      size,
                size_t      wordSize);

            ///@}

            //! Combine hashes.
            //!
            //! References:
            //! - http://www.boost.org/doc/libs/1_65_1/doc/html/hash/combine.html
            template <class T>
            inline void hashCombine(std::size_t &, const T &);

        } // namespace Memory
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS(Core::Memory::Endian);

} // namespace djv

#include <djvCore/MemoryInline.h>
