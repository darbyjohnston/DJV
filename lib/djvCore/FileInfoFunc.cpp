// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/FileInfoFunc.h>

#include <djvCore/FileInfoPrivate.h>
#include <djvCore/FrameNumberFunc.h>
#include <djvCore/PathFunc.h>

#include <algorithm>
#include <array>

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
            
            void fileSequence(FileInfo& fileInfo, const DirectoryListOptions& options, std::vector<FileInfo>& out)
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

            void sort(const DirectoryListOptions& options, std::vector<FileInfo>& out)
            {
                for (auto& i : out)
                {
                    if (FileType::Sequence == i.getType())
                    {
                        const auto sequence = i.getSequence();
                        std::stringstream s;
                        s << sequence;
                        Path path = i.getPath();
                        path.setNumber(s.str());
                        i.setPath(path, FileType::Sequence, sequence, false);
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

            bool isSequenceWildcard(const std::string& value) noexcept
            {
                auto i = value.begin();
                auto end = value.end();
                for (; i != end && '#' == *i; ++i)
                    ;
                return !value.empty() && i == end;
            }

            FileInfo getFileSequence(const Path& path, const std::set<std::string>& extensions)
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

            DJV_ENUM_HELPERS_IMPLEMENTATION(FileType);
            DJV_ENUM_HELPERS_IMPLEMENTATION(DirectoryListSort);

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

