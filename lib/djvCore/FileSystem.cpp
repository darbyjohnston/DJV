// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/FileSystem.h>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            Error::Error(const std::string& what) :
                std::runtime_error(what)
            {}

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

