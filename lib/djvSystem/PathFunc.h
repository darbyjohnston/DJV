// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/Path.h>

#include <djvCore/Enum.h>
#include <djvCore/RapidJSONFunc.h>

#include <sstream>
#include <vector>

namespace djv
{
    namespace System
    {
        namespace File
        {
            //! \name Utility
            ///@{

            //! Remove a trailing path separator.
            void removeTrailingSeparator(std::string&);

            //! Create a directory.
            //! Throws:
            //! - std::exception
            void mkdir(const Path&);

            //! Remove a directory. The directory must be empty.
            //! Throws:
            //! - std::exception
            void rmdir(const Path&);

            //! Get the absolute path.
            //! Throws:
            //! - std::exception
            Path getAbsolute(const Path&);

            //! Get the current working directory path.
            //! Throws:
            //! - std::exception
            Path getCWD();

            //! Get the temp file path.
            //! Throws:
            //! - std::exception
            Path getTemp();
            
            //! Get the directory shortcut.
            Path getPath(DirectoryShortcut);

            ///@}

            //! \name Splitting and Joining
            ///@{

            //! Split a path into components.
            void split(
                const std::string& in,
                std::string& directoryName,
                std::string& baseName,
                std::string& number,
                std::string& extension);

            //! Split a directory path into sub-directories.
            //! For example: "var/tmp" -> { "var", "tmp" }
            std::vector<std::string> splitDir(const std::string&);

            //! Join a list of sub-directories into a directory path.
            //! For example: var, tmp -> var/tmp
            std::string joinDirs(
                const std::vector<std::string>&,
                char separator = Path::getCurrentSeparator());

            ///@}

            DJV_ENUM_HELPERS(ResourcePath);
            DJV_ENUM_HELPERS(DirectoryShortcut);

        } // namespace File
    } // namespace System

    DJV_ENUM_SERIALIZE_HELPERS(System::File::ResourcePath);
    DJV_ENUM_SERIALIZE_HELPERS(System::File::DirectoryShortcut);

    rapidjson::Value toJSON(const System::File::Path&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, System::File::Path&);

    std::ostream& operator << (std::ostream&, const System::File::Path&);

} // namespace djv

namespace std
{
    template<>
    struct hash<djv::System::File::Path>
    {
        std::size_t operator() (const djv::System::File::Path&) const noexcept;
    };

} // namespace std

