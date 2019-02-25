//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#pragma once

#include <djvCore/Frame.h>
#include <djvCore/Path.h>

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

                Count = 3
            };
            const std::string& getFilePermissionsLabel(int);

            //! This enumeration provides the directory listing sort options.
            enum class DirectoryListSort
            {
                Name,
                Size,
                Permissions,
                Time,

                Count,
                First = Name
            };
            DJV_ENUM_HELPERS(DirectoryListSort);

            //! This struct provides directory listing options.
            struct DirectoryListOptions
            {
                bool fileSequences = false;
                bool showHidden = false;
                DirectoryListSort sort = DirectoryListSort::Name; //! \todo Implement me!
                bool reverseSort = false; //! \todo Implement me!
                bool sortDirectoriesFirst = true; //! \todo Implement me!
                std::string glob = "*";
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
            //! are always sorted in ascending order and frame numbers are always
            //! positive.
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

                //! \name File Path
                ///@{

                const Path& getPath() const { return _path; }
                bool isEmpty() const { return _path.isEmpty(); }
                void setPath(const Path&, bool stat = true);
                void setPath(const std::string&, bool stat = true);

                ///@}

                //! Get the file name.
                //! \param frame Specify a frame number or -1 for the entire sequence.
                //! \param path Include the path in the file name.
                std::string getFileName(Frame::Number = Frame::Invalid, bool path = true) const;

                //! Get whether this file exists.
                inline bool doesExist() const;

                //! \name File Information
                ///@{

                inline FileType getType() const;
                inline uint64_t getSize() const;
                inline uid_t getUser() const;
                inline int getPermissions() const;
                inline time_t getTime() const;

                //! Get information from the file system.
                //! Throws:
                //! - std::exception
                bool stat();

                ///@}

                //! \name File Sequence
                ///@{

                const Frame::Sequence& getSequence() const { return _sequence; }
                void setSequence(const Frame::Sequence&);
                void evalSequence();
                void sortSequence();
                inline bool isSequenceValid() const;
                inline bool isSequenceWildcard() const;
                inline bool sequenceContains(const FileInfo&) const;
                inline bool addToSequence(const FileInfo&);

                ///@}

                //! \name File Utilities
                ///@{

                //! Get the contents of the given directory.
                //! Throws:
                //! - std::exception
                static std::vector<FileInfo> directoryList(const Path&, const DirectoryListOptions& = DirectoryListOptions());

                //! Get the file sequence for the given file.
                //! Throws:
                //! - std::exception
                static FileInfo getFileSequence(const Path&);

                ///@}

                inline bool operator == (const FileInfo&) const;
                inline bool operator != (const FileInfo&) const;
                inline bool operator < (const FileInfo&) const;

                inline operator std::string() const;

            private:
                void _init();

                Path            _path;
                bool            _exists = false;
                FileType        _type = FileType::File;
                uint64_t        _size = 0;
                uid_t           _user = 0;
                int             _permissions = 0;
                time_t          _time = 0;
                Frame::Sequence _sequence;
            };
        } // namespace Core

    } // namespace FileSystem

    DJV_ENUM_SERIALIZE_HELPERS(Core::FileSystem::FileType);
    DJV_ENUM_SERIALIZE_HELPERS(Core::FileSystem::DirectoryListSort);

    std::ostream& operator << (std::ostream&, const Core::FileSystem::FileInfo&);
    
} // namespace djv

#include <djvCore/FileInfoInline.h>

