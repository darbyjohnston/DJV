// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvSystem/Path.h>

namespace djv
{
    namespace System
    {
        namespace File
        {
            char Path::getCurrentSeparator() noexcept
            {
                return getSeparator(PathSeparator::Unix);
            }
            
        } // namespace File
    } // namespace System
} // namespace djv

