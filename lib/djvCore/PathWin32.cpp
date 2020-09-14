// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCore/Path.h>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            char Path::getCurrentSeparator() noexcept
            {
                return getSeparator(PathSeparator::Windows);
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

