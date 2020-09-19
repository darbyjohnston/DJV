// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/FileInfoFunc.h>

#include <djvSystem/FileInfoPrivate.h>
#include <djvSystem/PathFunc.h>

#include <djvMath/FrameNumberFunc.h>

#include <algorithm>
#include <array>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace System
    {
        namespace File
        {
            std::string getPermissionsLabel(int in)
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
            
            void sequence(Info& info, const DirectoryListOptions& options, std::vector<Info>& out)
            {
                std::string extension = info.getPath().getExtension();
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
                        if (out[j].addToSequence(info))
                        {
                            break;
                        }
                    }
                    if (size == j)
                    {
                        out.push_back(info);
                    }
                }
                else
                {
                    out.push_back(info);
                }
            }

            void sort(const DirectoryListOptions& options, std::vector<Info>& out)
            {
                for (auto& i : out)
                {
                    if (Type::Sequence == i.getType())
                    {
                        const auto sequence = i.getSequence();
                        std::stringstream s;
                        s << sequence;
                        Path path = i.getPath();
                        path.setNumber(s.str());
                        i.setPath(path, Type::Sequence, sequence, false);
                    }
                }

                switch (options.sort)
                {
                case DirectoryListSort::Name:
                    std::sort(
                        out.begin(), out.end(),
                        [&options](const Info& a, const Info& b)
                    {
                        return options.reverseSort ?
                            (a.getFileName(Math::Frame::invalid, false) > b.getFileName(Math::Frame::invalid, false)) :
                            (a.getFileName(Math::Frame::invalid, false) < b.getFileName(Math::Frame::invalid, false));
                    });
                    break;
                case DirectoryListSort::Size:
                    std::sort(
                        out.begin(), out.end(),
                        [&options](const Info& a, const Info& b)
                    {
                        return options.reverseSort ? (a.getSize() > b.getSize()) : (a.getSize() < b.getSize());
                    });
                    break;
                case DirectoryListSort::Time:
                    std::sort(
                        out.begin(), out.end(),
                        [&options](const Info& a, const Info& b)
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
                        [](const Info& a, const Info& b)
                    {
                        return Type::Directory == a.getType() && b.getType() != Type::Directory;
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

            Info getSequence(const Path& path, const std::set<std::string>& extensions)
            {
                Info out(path);
                DirectoryListOptions options;
                options.sequences = true;
                options.sequenceExtensions = extensions;
                std::string dir = path.getDirectoryName();
                if (dir.empty())
                {
                    dir = ".";
                }
                for (const auto& info : directoryList(Path(dir), options))
                {
                    if (info.isCompatible(out))
                    {
                        out = info;
                        break;
                    }
                }
                return out;
            }

            DJV_ENUM_HELPERS_IMPLEMENTATION(Type);
            DJV_ENUM_HELPERS_IMPLEMENTATION(DirectoryListSort);

        } // namespace File
    } // namespace System

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        System::File,
        Type,
        DJV_TEXT("file_type_file"),
        DJV_TEXT("file_type_sequence"),
        DJV_TEXT("file_type_directory"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        System::File,
        DirectoryListSort,
        DJV_TEXT("directory_list_sort_name"),
        DJV_TEXT("directory_list_sort_size"),
        DJV_TEXT("directory_list_sort_time"));

    rapidjson::Value toJSON(System::File::Type value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    rapidjson::Value toJSON(System::File::DirectoryListSort value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    rapidjson::Value toJSON(const System::File::Info& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        out.AddMember("Path", toJSON(value.getPath(), allocator), allocator);
        out.AddMember("Type", toJSON(value.getType(), allocator), allocator);
        return out;
    }

    void fromJSON(const rapidjson::Value& value, System::File::Type& out)
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

    void fromJSON(const rapidjson::Value& value, System::File::DirectoryListSort& out)
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

    void fromJSON(const rapidjson::Value& value, System::File::Info& out)
    {
        if (value.IsObject())
        {
            System::File::Path path;
            System::File::Type type = System::File::Type::First;
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
            Math::Frame::Sequence sequence;
            if (System::File::Type::Sequence == type)
            {
                std::stringstream ss(path.getNumber());
                ss >> sequence;
            }
            out = System::File::Info(path, type, sequence);
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    std::ostream& operator << (std::ostream& s, const System::File::Info& value)
    {
        s << value.getPath();
        return s;
    }

} // namespace djv

