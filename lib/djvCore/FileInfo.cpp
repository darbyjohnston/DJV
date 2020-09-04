// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/FileInfo.h>

//#pragma optimize("", off)

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            std::string getFilePermissionsLabel(int in)
            {
                const std::vector<std::string> data =
                {
                    "",
                    "r",
                    "w",
                    "rw",
                    "x",
                    "rx",
                    "wx",
                    "rwx"
                };
                return data[in];
            }
            
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

            void FileInfo::setSequence(const Frame::Sequence& in)
            {
                _sequence = in;
                std::stringstream s;
                s << _sequence;
                _path.setNumber(s.str());
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
            
            bool FileInfo::isSequenceWildcard(const std::string& value) noexcept
            {
                auto i = value.begin();
                auto end = value.end();
                for (; i != end && '#' == *i; ++i)
                    ;
                return !value.empty() && i == end;
            }

            FileInfo FileInfo::getFileSequence(const Path& path, const std::set<std::string>& extensions)
            {
                FileInfo out(path);
                DirectoryListOptions options;
                options.sequences = true;
                options.sequenceExtensions = extensions;
                std::string dir = path.getDirectoryName();
                if (dir.empty())
                {
                    dir = ".";
                }
                for (const auto& fileInfo : directoryList(Path(dir), options))
                {
                    if (fileInfo.isCompatible(out))
                    {
                        out = fileInfo;
                        break;
                    }
                }
                return out;
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

            void FileInfo::_fileSequence(FileInfo& fileInfo, const DirectoryListOptions& options, std::vector<FileInfo>& out)
            {
                std::string extension = fileInfo.getPath().getExtension();
                std::transform(extension.begin(), extension.end(), extension.begin(), tolower);
                const auto i = std::find(
                    options.sequenceExtensions.begin(),
                    options.sequenceExtensions.end(),
                    extension);
                if (options.sequences && i != options.sequenceExtensions.end())
                {
                    const size_t size = out.size();
                    size_t j = 0;
                    for (; j < size; ++j)
                    {
                        if (out[j].addToSequence(fileInfo))
                        {
                            break;
                        }
                    }
                    if (size == j)
                    {
                        out.push_back(fileInfo);
                    }
                }
                else
                {
                    out.push_back(fileInfo);
                }
            }

            void FileInfo::_sort(const DirectoryListOptions& options, std::vector<FileInfo>& out)
            {
                for (auto& i : out)
                {
                    if (FileType::Sequence == i._type)
                    {
                        std::stringstream s;
                        s << i._sequence;
                        i._path.setNumber(s.str());
                    }
                }

                switch (options.sort)
                {
                case DirectoryListSort::Name:
                    std::sort(
                        out.begin(), out.end(),
                        [&options](const FileInfo& a, const FileInfo& b)
                    {
                        return options.reverseSort ?
                            (a.getFileName(Frame::invalid, false) > b.getFileName(Frame::invalid, false)) :
                            (a.getFileName(Frame::invalid, false) < b.getFileName(Frame::invalid, false));
                    });
                    break;
                case DirectoryListSort::Size:
                    std::sort(
                        out.begin(), out.end(),
                        [&options](const FileInfo& a, const FileInfo& b)
                    {
                        return options.reverseSort ? (a.getSize() > b.getSize()) : (a.getSize() < b.getSize());
                    });
                    break;
                case DirectoryListSort::Time:
                    std::sort(
                        out.begin(), out.end(),
                        [&options](const FileInfo& a, const FileInfo& b)
                    {
                        return options.reverseSort ? (a.getTime() > b.getTime()) : (a.getTime() < b.getTime());
                    });
                    break;
                default: break;
                }
                if (options.sortDirectoriesFirst)
                {
                    std::stable_sort(
                        out.begin(), out.end(),
                        [](const FileInfo& a, const FileInfo& b)
                    {
                        return FileType::Directory == a.getType() && b.getType() != FileType::Directory;
                    });
                }
            }

        } // namespace FileSystem
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::FileSystem,
        FileType,
        DJV_TEXT("file_type_file"),
        DJV_TEXT("file_type_sequence"),
        DJV_TEXT("file_type_directory"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::FileSystem,
        DirectoryListSort,
        DJV_TEXT("directory_list_sort_name"),
        DJV_TEXT("directory_list_sort_size"),
        DJV_TEXT("directory_list_sort_time"));

    rapidjson::Value toJSON(Core::FileSystem::FileType value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    rapidjson::Value toJSON(Core::FileSystem::DirectoryListSort value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    rapidjson::Value toJSON(const Core::FileSystem::FileInfo& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        out.AddMember("Path", toJSON(value.getPath(), allocator), allocator);
        out.AddMember("Type", toJSON(value.getType(), allocator), allocator);
        return out;
    }

    void fromJSON(const rapidjson::Value& value, Core::FileSystem::FileType& out)
    {
        if (value.IsString())
        {
            std::stringstream ss(value.GetString());
            ss >> out;
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, Core::FileSystem::DirectoryListSort& out)
    {
        if (value.IsString())
        {
            std::stringstream ss(value.GetString());
            ss >> out;
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, Core::FileSystem::FileInfo& out)
    {
        if (value.IsObject())
        {
            Core::FileSystem::Path path;
            Core::FileSystem::FileType type = Core::FileSystem::FileType::First;
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("Path", i.name.GetString()))
                {
                    fromJSON(i.value, path);
                }
                else if (0 == strcmp("Type", i.name.GetString()))
                {
                    fromJSON(i.value, type);
                }
            }
            Core::Frame::Sequence sequence;
            if (Core::FileSystem::FileType::Sequence == type)
            {
                std::stringstream ss(path.getNumber());
                ss >> sequence;
            }
            out = Core::FileSystem::FileInfo(path, type, sequence);
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    std::ostream& operator << (std::ostream& s, const Core::FileSystem::FileInfo& value)
    {
        s << value.getPath();
        return s;
    }

} // namespace djv
