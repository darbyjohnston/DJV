// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/FileInfo.h>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            void fileSequence(FileInfo&, const DirectoryListOptions&, std::vector<FileInfo>&);
            void sort(const DirectoryListOptions&, std::vector<FileInfo>&);

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

