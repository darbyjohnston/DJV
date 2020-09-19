// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/File.h>

namespace djv
{
    namespace System
    {
        namespace File
        {
            Error::Error(const std::string& what) :
                std::runtime_error(what)
            {}

        } // namespace File
    } // namespace System
} // namespace djv

