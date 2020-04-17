// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/Core.h>

#include <iostream>

#include <stdlib.h>

namespace djv
{
    namespace Core
    {
        void _assert(const char * file, int line)
        {
            std::cout << "ASSERT: " << file << ":" << line << std::endl;
            abort();
        }

    } // namespace Core
} // namespace djv
