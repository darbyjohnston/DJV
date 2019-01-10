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

#pragma once

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace djv
{
    //! This namespace provides core functionality.
    namespace Core
    {
        //! This enumeration provides the log levels.
        enum class LogLevel
        {
            Information,
            Warning,
            Error
        };

        //! This function provides an assert (use the DJV_ASSERT macro instead).
        void _assert(const char * file, int line);

    } // namespace Core
} // namespace djv

//! This macro provides private implementation members.
#define DJV_PRIVATE() \
    struct Private; \
    std::unique_ptr<Private> _p

//! This macro provides a raw pointer to the private implementation.
#define DJV_PRIVATE_PTR() \
    auto & p = *_p

//! This macro makes a class non-copyable.
#define DJV_NON_COPYABLE(name) \
    name(const name &) = delete; \
    name & operator = (const name &) = delete

//! This macro marks strings for extraction.
#define DJV_TEXT(nameSpace, arg) (arg)

//! This macro provides an assert.
#if defined(DJV_ASSERT)
#undef DJV_ASSERT
#define DJV_ASSERT(value) \
    if (!(value)) \
        djv::Core::_assert(__FILE__, __LINE__)
#else
#define DJV_ASSERT(value)
#endif

#if defined(DJV_PLATFORM_WINDOWS)
//! \bug [S 1.0] https://social.msdn.microsoft.com/Forums/vstudio/en-US/8f40dcd8-c67f-4eba-9134-a19b9178e481/vs-2015-rc-linker-stdcodecvt-error?forum=vcgeneral
typedef unsigned int djv_char_t;
#define DJV_SNPRINTF sprintf_s
#define DJV_STRNCPY strncpy_s
#else // DJV_PLATFORM_WINDOWS
typedef char32_t djv_char_t;
#define DJV_SNPRINTF snprintf
#define DJV_STRNCPY strncpy
#endif // DJV_PLATFORM_WINDOWS
