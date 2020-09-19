// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/FileInfo.h>

namespace djv
{
    namespace System
    {
        namespace File
        {
            void sequence(Info&, const DirectoryListOptions&, std::vector<Info>&);
            void sort(const DirectoryListOptions&, std::vector<Info>&);

        } // namespace File
    } // namespace System
} // namespace djv

