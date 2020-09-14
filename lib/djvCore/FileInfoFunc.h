// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Enum.h>
#include <djvCore/FileInfo.h>
#include <djvCore/RapidJSONFunc.h>

#include <sstream>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            //! Get file permissions labels.
            std::string getFilePermissionsLabel(int);

            //! Test whether the string contains all '#' characters.
            bool isSequenceWildcard(const std::string&) noexcept;

            //! Get the contents of the given directory.
            std::vector<FileInfo> directoryList(const Path& path, const DirectoryListOptions& options = DirectoryListOptions());

            //! Get the file sequence for the given file.
            FileInfo getFileSequence(const Path&, const std::set<std::string>& extensions);

            DJV_ENUM_HELPERS(FileType);
            DJV_ENUM_HELPERS(DirectoryListSort);

        } // namespace Core
    } // namespace FileSystem

    DJV_ENUM_SERIALIZE_HELPERS(Core::FileSystem::FileType);
    DJV_ENUM_SERIALIZE_HELPERS(Core::FileSystem::DirectoryListSort);

    rapidjson::Value toJSON(Core::FileSystem::FileType, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(Core::FileSystem::DirectoryListSort, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const Core::FileSystem::FileInfo&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Core::FileSystem::FileType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Core::FileSystem::DirectoryListSort&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Core::FileSystem::FileInfo&);

    std::ostream& operator << (std::ostream&, const Core::FileSystem::FileInfo&);
    
} // namespace djv

