// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

namespace djv
{
    //! This namespace provides core functionality.
    namespace Core
    {
        //! This function provides an assert (for convenience use the DJV_ASSERT macro).
        void _assert(const char * file, int line);

    } // namespace Core
} // namespace djv

//! This macro provides private implementation members.
#define DJV_PRIVATE() \
    struct Private; \
    std::unique_ptr<Private> _p

//! This macro provides a raw pointer to the private implementation.
#define DJV_PRIVATE_PTR() \
    auto& p = *_p

//! This macro makes a class non-copyable.
#define DJV_NON_COPYABLE(name) \
    name(const name&) = delete; \
    name& operator = (const name&) = delete

//! This macro marks strings for extraction.
#define DJV_TEXT(arg) (arg)

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
//! \bug https://social.msdn.microsoft.com/Forums/vstudio/en-US/8f40dcd8-c67f-4eba-9134-a19b9178e481/vs-2015-rc-linker-stdcodecvt-error?forum=vcgeneral
typedef unsigned int djv_char_t;
#else // DJV_PLATFORM_WINDOWS
typedef char32_t djv_char_t;
#endif // DJV_PLATFORM_WINDOWS

