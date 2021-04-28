// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <exception>
#include <string>

namespace djv
{
    namespace Core
    {
        //! Error handling.
        namespace Error
        {
            //! Format an error for display to the user.
            std::string format(const std::string&);

            //! Format a std::exception for display to the user.
            std::string format(const std::exception&);

#if defined(DJV_PLATFORM_WINDOWS)
            //! Get an error string from a Windows system call.
            std::string getLastError();
#endif // DJV_PLATFORM_WINDOWS

        } // namespace Error
    } // namespace Core
} // namespace djv
