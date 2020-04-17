// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            inline FileInfo::FileInfo()
            {}

            inline FileInfo::FileInfo(const Path & path, bool stat)
            {
                setPath(path, stat);
            }

            inline FileInfo::FileInfo(const Path& path, FileType fileType, bool stat)
            {
                setPath(path, fileType, stat);
            }

            inline FileInfo::FileInfo(const std::string& path, bool stat)
            {
                setPath(Path(path), stat);
            }

            inline const Path& FileInfo::getPath() const
            {
                return _path;
            }

            inline bool FileInfo::isEmpty() const
            {
                return _path.isEmpty();
            }

            inline const Frame::Sequence& FileInfo::getSequence() const
            {
                return _sequence;
            }

            inline bool FileInfo::doesExist() const
            {
                return _exists;
            }

            inline FileType FileInfo::getType() const
            {
                return _type;
            }

            inline uint64_t FileInfo::getSize() const
            {
                return _size;
            }

            inline uid_t FileInfo::getUser() const
            {
                return _user;
            }

            inline int FileInfo::getPermissions() const
            {
                return _permissions;
            }

            inline time_t FileInfo::getTime() const
            {
                return _time;
            }

            inline bool FileInfo::isSequenceValid() const
            {
                return
                    _type != FileType::Directory &&
                    !_path.getNumber().empty() &&
                    _sequence.ranges.size();
            }

            inline bool FileInfo::isCompatible(const FileInfo& value) const
            {
                if (!isSequenceValid())
                    return false;
                if (_path.getExtension() != value._path.getExtension())
                    return false;
                if (_path.getBaseName() != value._path.getBaseName())
                    return false;
                return true;
            }

            inline bool FileInfo::addToSequence(const FileInfo& value)
            {
                if (isCompatible(value))
                {
                    for (const auto& range : value._sequence.ranges)
                    {
                        if (!_sequence.merge(range))
                        {
                            _sequence.ranges.push_back(range);
                        }
                    }
                    if (value._sequence.pad > _sequence.pad)
                    {
                        _sequence.pad = value._sequence.pad;
                    }
                    _size += value._size;
                    if (value._user > _user)
                    {
                        _user = value._user;
                    }
                    if (value._time > _time)
                    {
                        _time = value._time;
                    }
                    return true;
                }
                return false;
            }
            
            inline bool FileInfo::isSequenceWildcard(const std::string& value)
            {
                auto i = value.begin();
                auto end = value.end();
                for (; i != end && '#' == *i; ++i)
                    ;
                return !value.empty() && i == end;
            }

            inline bool FileInfo::operator == (const FileInfo & in) const
            {
                return
                    in._path == _path &&
                    in._type == _type &&
                    in._size == _size &&
                    in._user == _user &&
                    in._permissions == _permissions &&
                    in._time == _time;
            }

            inline bool FileInfo::operator != (const FileInfo & in) const
            {
                return !(in == *this);
            }

            inline bool FileInfo::operator < (const FileInfo & in) const
            {
                return in._path.get() < _path.get();
            }

            inline FileInfo::operator std::string() const
            {
                return std::string(_path);
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv
