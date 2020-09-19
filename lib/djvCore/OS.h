// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

namespace djv
{
    namespace Core
    {
        //! This namespace provides operating system functionality.
        namespace OS
        {
            //! This enumeration provides OS specific environment variable
            //! list separators.
            enum class ListSeparator
            {
                Unix,
                Windows
            };
            
        } // namespace OS
    } // namespace Core
} // namespace djv
