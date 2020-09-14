// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/FileInfoPrivate.h>

#include <djvCore/FrameNumberFunc.h>
#include <djvCore/PathFunc.h>

//#pragma optimize("", off)

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            bool DirectoryListOptions::operator == (const DirectoryListOptions& other) const
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

            FileInfo::FileInfo()
            {}

            FileInfo::FileInfo(const Path& path, bool stat)
            {
                setPath(path, stat);
            }

            FileInfo::FileInfo(const std::string& path, bool stat)
            {
                setPath(Path(path), stat);
            }

            FileInfo::FileInfo(const Path& path, FileType fileType, const Frame::Sequence& sequence, bool stat)
            {
                setPath(path, fileType, sequence, stat);
            }

            const Path& FileInfo::getPath() const noexcept
            {
                return _path;
            }
            
            bool FileInfo::isEmpty() const noexcept
            {
                return _path.isEmpty();
            }
            
            void FileInfo::setPath(const Path& value, bool stat)
            {
                _path           = value;
                _exists         = false;
                _type           = FileType::File;
                _size           = 0;
                _user           = 0;
                _permissions    = 0;
                _time           = 0;
                _sequence       = Frame::Sequence();

                // Get information from the file system.
                if (stat)
                {
                    this->stat();
                }
            }

            void FileInfo::setPath(const Path& value, FileType fileType, const Frame::Sequence& sequence, bool stat)
            {
                _path           = value;
                _exists         = false;
                _type           = fileType;
                _size           = 0;
                _user           = 0;
                _permissions    = 0;
                _time           = 0;
                _sequence       = sequence;

                // Get information from the file system.
                if (stat)
                {
                    this->stat();
                }
            }
            
            std::string FileInfo::getFileName(Frame::Number frame, bool path) const
            {
                std::stringstream ss;
                if (_path.isRoot())
                {
                    ss << _path;
                }
                else
                {
                    if (path)
                    {
                        ss << _path.getDirectoryName();
                    }
                    ss << _path.getBaseName();
                    if (FileType::Sequence == _type && _sequence.isValid() && frame != Frame::invalid)
                    {
                        ss << Frame::toString(frame, _sequence.getPad());
                    }
                    else if (FileType::Sequence == _type && _sequence.isValid())
                    {
                        ss << _sequence;
                    }
                    else
                    {
                        ss << _path.getNumber();
                    }
                    ss << _path.getExtension();
                }
                return ss.str();
            }

            bool FileInfo::doesExist() const noexcept
            {
                return _exists;
            }

            FileType FileInfo::getType() const noexcept
            {
                return _type;
            }

            uint64_t FileInfo::getSize() const noexcept
            {
                return _size;
            }

            uid_t FileInfo::getUser() const noexcept
            {
                return _user;
            }

            int FileInfo::getPermissions() const noexcept
            {
                return _permissions;
            }

            time_t FileInfo::getTime() const noexcept
            {
                return _time;
            }

            const Frame::Sequence& FileInfo::getSequence() const noexcept
            {
                return _sequence;
            }

            bool FileInfo::isCompatible(const FileInfo& value) const
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

            void FileInfo::setSequence(const Frame::Sequence& in)
            {
                _sequence = in;
                std::stringstream s;
                s << _sequence;
                _path.setNumber(s.str());
            }

            bool FileInfo::addToSequence(const FileInfo& value)
            {
                if (isCompatible(value))
                {
                    if (_type != FileType::Sequence)
                    {
                        _sequence = _parseSequence(_path.getNumber());
                        if (_sequence.isValid())
                        {
                            _type = FileType::Sequence;
                        }
                    }
                    const Frame::Sequence sequence = _parseSequence(value.getPath().getNumber());
                    for (const auto& range : sequence.getRanges())
                    {
                        _sequence.add(range);
                    }
                    {
                        std::stringstream ss;
                        ss << _sequence;
                        _path.setNumber(ss.str());
                    }
                    if (sequence.getPad() > _sequence.getPad())
                    {
                        _sequence.setPad(sequence.getPad());
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
                        
            bool FileInfo::operator == (const FileInfo& in) const
            {
                return
                    in._path == _path &&
                    in._type == _type &&
                    in._size == _size &&
                    in._user == _user &&
                    in._permissions == _permissions &&
                    in._time == _time;
            }

            bool FileInfo::operator != (const FileInfo& in) const
            {
                return !(in == *this);
            }

            bool FileInfo::operator < (const FileInfo& in) const
            {
                return in._path.get() < _path.get();
            }

            FileInfo::operator std::string() const
            {
                return std::string(_path);
            }
            
            Frame::Sequence FileInfo::_parseSequence(const std::string& number)
            {
                Frame::Sequence out;
                std::stringstream ss(number);
                ss >> out;
                return out;
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

