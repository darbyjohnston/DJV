// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace System
    {
        namespace File
        {
            inline bool DirectoryListOptions::operator == (const DirectoryListOptions& other) const
            {
                return
                    extensions == other.extensions &&
                    sequences == other.sequences &&
                    sequenceExtensions == other.sequenceExtensions &&
                    showHidden == other.showHidden &&
                    sort == other.sort &&
                    reverseSort == other.reverseSort &&
                    sortDirectoriesFirst == other.sortDirectoriesFirst &&
                    filter == other.filter;
            }

            inline const Path& Info::getPath() const noexcept
            {
                return _path;
            }
            
            inline bool Info::isEmpty() const noexcept
            {
                return _path.isEmpty();
            }
            
            inline bool Info::doesExist() const noexcept
            {
                return _exists;
            }

            inline Type Info::getType() const noexcept
            {
                return _type;
            }

            inline uint64_t Info::getSize() const noexcept
            {
                return _size;
            }

            inline uid_t Info::getUser() const noexcept
            {
                return _user;
            }

            inline int Info::getPermissions() const noexcept
            {
                return _permissions;
            }

            inline time_t Info::getTime() const noexcept
            {
                return _time;
            }

            inline const Math::Frame::Sequence& Info::getSequence() const noexcept
            {
                return _sequence;
            }

            inline bool Info::isCompatible(const Info& value) const
            {
                if (_path.getNumber().empty() || value._path.getNumber().empty())
                    return false;
                if (_path.getExtension() != value._path.getExtension())
                    return false;
                if (_path.getBaseName() != value._path.getBaseName())
                    return false;
                if (_path.getDirectoryName() != value._path.getDirectoryName())
                    return false;
                return true;
            }
                        
            inline bool Info::operator == (const Info& in) const
            {
                std::string path = _path.get();

                std::string inPath = in._path.get();
                return
                    in._path == _path &&
                    in._type == _type &&
                    in._size == _size &&
                    in._user == _user &&
                    in._permissions == _permissions &&
                    in._time == _time;
            }

            inline bool Info::operator != (const Info& in) const
            {
                return !(in == *this);
            }

            inline bool Info::operator < (const Info& in) const
            {
                return in._path.get() < _path.get();
            }

            inline Info::operator std::string() const
            {
                return std::string(_path);
            }
            
        } // namespace File
    } // namespace System
} // namespace djv

