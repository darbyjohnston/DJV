//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvCore/StringUtil.h>

#include <QMetaType>

namespace djv
{
    namespace Core
    {
        class Memory
        {
            Q_GADGET
            Q_ENUMS(ENDIAN)

        public:
            virtual ~Memory() = 0;

            static const quint64 kilobyte; //!< The number of bytes in a kilobyte.
            static const quint64 megabyte; //!< The number of bytes in a megabyte.
            static const quint64 gigabyte; //!< The number of bytes in a gigabyte.
            static const quint64 terabyte; //!< The number of bytes in a terabyte.

            //! Convert a byte count to a human readable string.
            static QString sizeLabel(quint64);

            //! This enumeration provides the machine endian.
            enum ENDIAN
            {
                MSB, //!< Most siginificant byte first
                LSB, //!< Least significant byte first

                ENDIAN_COUNT
            };

            //! Get the machine endian labels.
            static const QStringList & endianLabels();

            //! Fill a block of memory with a value.
            template<typename T>
            static inline void fill(T value, void * in, quint64 size);

            //! Get the current machine's endian.
            static ENDIAN endian();

            //! Get the opposite of the given endian.
            static inline ENDIAN endianOpposite(ENDIAN);

            //! Convert the endianness of a block of memory in place.
            static inline void convertEndian(
                void *  in,
                quint64 size,
                int     wordSize);

            //! Convert the endianness of a block of memory.
            static inline void convertEndian(
                const void * in,
                void *       out,
                quint64      size,
                int          wordSize);
        };

    } // namespace Core

    DJV_STRING_OPERATOR(Core::Memory::ENDIAN);

} // namespace djv

#include <djvCore/MemoryInline.h>

