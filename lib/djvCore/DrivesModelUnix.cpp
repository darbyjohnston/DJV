// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2018-2020 Darby Johnston
// All rights reserved.

#include <djvCore/DrivesModel.h>

#include <djvCore/FileInfo.h>

//#pragma optimize("", off)

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            std::vector<Path> DrivesModel::_getDrives()
            {
                std::vector<Path> out;
#if defined(DJV_PLATFORM_OSX)
                for (const auto& fileInfo : FileInfo::directoryList(Path("/Volumes")))
                {
                    out.push_back(fileInfo.getPath());
                }
#elif defined(DJV_PLATFORM_LINUX)
                out.push_back(Path("/"));
#endif // DJV_PLATFORM_OSX
                return out;
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

