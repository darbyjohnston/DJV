// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <stdexcept>
#include <string>

namespace djv
{
    namespace Core
    {
        //! This namespace provides file system functionality.
        namespace FileSystem
        {
            //! This class provides a file system error.
            class Error : public std::runtime_error
            {
            public:
                explicit Error(const std::string&);
            };

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

