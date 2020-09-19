// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2018-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/DrivesModel.h>

#include <djvSystem/FileInfo.h>

//#pragma optimize("", off)

namespace djv
{
    namespace System
    {
        namespace File
        {
            std::vector<Path> DrivesModel::_getDrives()
            {
                std::vector<Path> out;
#if defined(DJV_PLATFORM_MACOS)
                for (const auto& fileInfo : FileInfo::directoryList(Path("/Volumes")))
                {
                    out.push_back(fileInfo.getPath());
                }
#else // DJV_PLATFORM_MACOS
                out.push_back(Path("/"));
#endif // DJV_PLATFORM_MACOS
                return out;
            }

        } // namespace File
    } // namespace System
} // namespace djv

