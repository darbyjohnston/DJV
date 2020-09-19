// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <stdexcept>
#include <string>

#include <stdio.h>

namespace djv
{
    namespace System
    {
        namespace File
        {
            //! This function provides a wrapper for fopen().
            //! Throws:
            //! - std::exception
            FILE* fopen(const std::string& fileName, const std::string& mode);

        } // namespace File
    } // namespace System
} // namespace djv

