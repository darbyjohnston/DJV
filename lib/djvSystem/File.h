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
        //! This namespace provides file system functionality.
        namespace File
        {
            //! This class provides a file error.
            class Error : public std::runtime_error
            {
            public:
                explicit Error(const std::string&);
            };
            
            //! \name Utility
            ///@{

            //! This function provides a wrapper for fopen().
            //! Throws:
            //! - std::exception
            FILE* fopen(const std::string& fileName, const std::string& mode);

            ///@}

        } // namespace File
    } // namespace System
} // namespace djv

