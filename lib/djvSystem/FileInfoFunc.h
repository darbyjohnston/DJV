// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/FileInfo.h>

#include <djvCore/Enum.h>
#include <djvCore/RapidJSONFunc.h>

#include <sstream>

namespace djv
{
    namespace System
    {
        namespace File
        {
            //! \name Utility
            ///@{

            //! Get the contents of the given directory.
            std::vector<Info> directoryList(const Path& path, const DirectoryListOptions& options = DirectoryListOptions());

            ///@}

            //! \name Sequences
            ///@{

            //! Test whether the string contains all '#' characters.
            bool isSequenceWildcard(const std::string&) noexcept;

            //! Get the file sequence for the given file.
            Info getSequence(const Path&, const std::set<std::string>& extensions);

            ///@}

            //! \name Conversion
            ///@{

            //! Get file permissions labels.
            std::string getPermissionsLabel(int);

            ///@}

            DJV_ENUM_HELPERS(Type);
            DJV_ENUM_HELPERS(DirectoryListSort);

        } // namespace File
    } // namespace System

    DJV_ENUM_SERIALIZE_HELPERS(System::File::Type);
    DJV_ENUM_SERIALIZE_HELPERS(System::File::DirectoryListSort);

    rapidjson::Value toJSON(System::File::Type, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(System::File::DirectoryListSort, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const System::File::Info&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, System::File::Type&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, System::File::DirectoryListSort&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, System::File::Info&);

    std::ostream& operator << (std::ostream&, const System::File::Info&);
    
} // namespace djv

