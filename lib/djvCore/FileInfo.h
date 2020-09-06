// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/FrameNumber.h>
#include <djvCore/Path.h>
#include <djvCore/RapidJSON.h>

#include <set>

#include <sys/types.h>

#if defined(DJV_PLATFORM_WINDOWS)
typedef int uid_t;
#endif // DJV_PLATFORM_WINDOWS

namespace djv
{
    namespace Core
    {
        //! This namespace provides file system functionality.
        namespace FileSystem
        {
            //! This enumeration provides file types.
            enum class FileType
            {
                File,      //!< Regular file
                Sequence,  //!< File sequence
                Directory, //!< Directory

                Count,
                First = File
            };
            DJV_ENUM_HELPERS(FileType);

            //! This enumeration provides the file permissions.
            enum class FilePermissions
            {
                Read  = 1, //!< Readable
                Write = 2, //!< Writable
                Exec  = 4, //!< Executable
            };
            std::string getFilePermissionsLabel(int);

            //! This enumeration provides the directory listing sort options.
            enum class DirectoryListSort
            {
                Name,
                Size,
                Time,

                Count,
                First = Name
            };
            DJV_ENUM_HELPERS(DirectoryListSort);

            //! This struct provides directory listing options.
            struct DirectoryListOptions
            {
                std::set<std::string>       extensions;
                bool                        sequences               = false;
                std::set<std::string>       sequenceExtensions;
                bool                        showHidden              = false;
                DirectoryListSort           sort                    = DirectoryListSort::Name;
                bool                        reverseSort             = false;
                bool                        sortDirectoriesFirst    = true;
                std::string                 filter;

                bool operator == (const DirectoryListOptions&) const;
            };

            //! This class provides information about files and file sequences.
            //!
            //! A file sequence is a list of file names that share a common name and
            //! have frame numbers. File sequences are used to store animation or movie
            //! footage where each frame is an individual file.
            //!
            //! A file sequence is expressed as a file name with a start and end frame
            //! separated with a dash ('-'), for example "render.1-100.exr". Multiple
            //! start and end frames and individual frames are separated with a comma
            //! (','), for example "render.1-10,20-30,33,35,37.exr". File sequences
            //! are always sorted in ascending order.
            //!
            //! File sequences may also have frames with padded zeroes, for example
            //! "render.0001-0100.exr".
            //!
            //! Note the file sequences return information for the entire sequence.
            class FileInfo
            {
            public:
                FileInfo();
                FileInfo(const Path&, bool stat = true);
                FileInfo(const std::string&, bool stat = true);
                FileInfo(const Path&, FileType, const Frame::Sequence&, bool stat = true);

                //! \name Path
                ///@{

                const Path& getPath() const noexcept;
                bool isEmpty() const noexcept;
                void setPath(const Path& path, bool stat = true);
                void setPath(const Path& path, FileType fileType, const Frame::Sequence&, bool stat = true);

                //! Get the file name.
                //! \param frame Specify a frame number or Frame::invalid for the entire sequence.
                //! \param path Include the path in the file name.
                std::string getFileName(Frame::Number frame = Frame::invalid, bool path = true) const;

                //! Get whether this file exists.
                bool doesExist() const noexcept;

                ///@}

                //! \name File Information
                ///@{

                FileType getType() const noexcept;
                uint64_t getSize() const noexcept;
                uid_t getUser() const noexcept;
                int getPermissions() const noexcept;
                time_t getTime() const noexcept;

                //! Get information from the file system.
                bool stat(std::string* error = nullptr);

                ///@}

                //! \name File Sequences
                ///@{

                const Frame::Sequence& getSequence() const noexcept;
                void setSequence(const Frame::Sequence&);
                bool isCompatible(const FileInfo&) const;
                bool addToSequence(const FileInfo&);
                
                ///@}

                //! \name Utilities
                ///@{

                //! Test whether the string contains all '#' characters.
                static bool isSequenceWildcard(const std::string&) noexcept;

                //! Get the contents of the given directory.
                static std::vector<FileInfo> directoryList(const Path& path, const DirectoryListOptions& options = DirectoryListOptions());

                //! Get the file sequence for the given file.
                static FileInfo getFileSequence(const Path&, const std::set<std::string>& extensions);

                ///@}

                bool operator == (const FileInfo&) const;
                bool operator != (const FileInfo&) const;
                bool operator < (const FileInfo&) const;

                explicit operator std::string() const;

            private:
                static Frame::Sequence _parseSequence(const std::string&);
                static void _fileSequence(FileInfo&, const DirectoryListOptions&, std::vector<FileInfo>&);
                static void _sort(const DirectoryListOptions&, std::vector<FileInfo>&);
                
                Path            _path;
                bool            _exists      = false;
                FileType        _type        = FileType::File;
                uint64_t        _size        = 0;
                uid_t           _user        = 0;
                int             _permissions = 0;
                time_t          _time        = 0;
                Frame::Sequence _sequence;
            };
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

