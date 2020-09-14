// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCore/PathFunc.h>

#include <djvCore/MemoryFunc.h>
#include <djvCore/StringFunc.h>

#include <array>

extern "C"
{
#include <fseq.h>

} // extern "C"

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            void removeTrailingSeparator(std::string& value)
            {
                if (value.size() > 2)
                {
                    const auto i = value.end() - 1;
                    if (Path::getSeparator(PathSeparator::Unix) == *i ||
                        Path::getSeparator(PathSeparator::Windows) == *i)
                    {
                        value.erase(i);
                    }
                }
            }
            
            void split(
                const std::string& in,
                std::string& path,
                std::string& base,
                std::string& number,
                std::string& extension)
            {
                FSeqFileNameSizes sizes;
                fseqFileNameSizesInit(&sizes);
                fseqFileNameParseSizes(in.c_str(), &sizes, FSEQ_STRING_LEN);
                path = in.substr(0, sizes.path);
                base = in.substr(sizes.path, sizes.base);
                number = in.substr(sizes.path + static_cast<size_t>(sizes.base), sizes.number);
                extension = in.substr(sizes.path + static_cast<size_t>(sizes.base) + static_cast<size_t>(sizes.number), sizes.extension);
            }

            std::vector<std::string> splitDir(const std::string& value)
            {
                std::vector<std::string> out;

                // Handle the root path.
                std::string tmp = value;
                if (tmp.size() > 0 &&
                    '/' == tmp[0])
                {
                    out.push_back("/");
                    tmp.erase(0, 1);
                }
                else if (
                    tmp.size() > 1 &&
                    '\\' == tmp[0] &&
                    '\\' == tmp[1])
                {
                    size_t i = 2;
                    for (; i < tmp.size() && tmp[i] != '\\'; ++i)
                        ;
                    out.push_back(tmp.substr(0, i));
                    tmp.erase(0, i);
                }
                else if (
                    tmp.size() > 0 &&
                    '\\' == tmp[0])
                {
                    out.push_back("\\");
                    tmp.erase(0, 1);
                }

                for (const auto& i : String::split(tmp, { '/', '\\' }))
                {
                    out.push_back(i);
                }

                return out;
            }

            std::string joinDirs(const std::vector<std::string>& value, char separator)
            {
                std::string out;

                // Handle the root path.
                std::vector<std::string> tmp = value;
                if (tmp.size() && tmp[0].size() == 1 && '/' == tmp[0][0])
                {
                    out += tmp[0];
                    tmp.erase(tmp.begin());
                }
                else if (tmp.size() && tmp[0].size() == 1 && '\\' == tmp[0][0])
                {
                    out += tmp[0];
                    tmp.erase(tmp.begin());
                }

                if (tmp.size())
                {
                    out += String::join(tmp, separator);
                }

                return out;
            }

            DJV_ENUM_HELPERS_IMPLEMENTATION(ResourcePath);

        } // namespace FileSystem
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::FileSystem,
        ResourcePath,
        DJV_TEXT("resource_path_application"),
        DJV_TEXT("resource_path_documents"),
        DJV_TEXT("resource_path_log_file"),
        DJV_TEXT("resource_path_settings_file"),
        DJV_TEXT("resource_path_audio"),
        DJV_TEXT("resource_path_fonts"),
        DJV_TEXT("resource_path_icons"),
        DJV_TEXT("resource_path_images"),
        DJV_TEXT("resource_path_models"),
        DJV_TEXT("resource_path_shaders"),
        DJV_TEXT("resource_path_text"),
        DJV_TEXT("resource_path_color"),
        DJV_TEXT("resource_path_documentation"));

    rapidjson::Value toJSON(const Core::FileSystem::Path& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        out.AddMember("DirectoryName", toJSON(value.getDirectoryName(), allocator), allocator);
        out.AddMember("BaseName", toJSON(value.getBaseName(), allocator), allocator);
        out.AddMember("Number", toJSON(value.getNumber(), allocator), allocator);
        out.AddMember("Extension", toJSON(value.getExtension(), allocator), allocator);
        return out;
    }

    void fromJSON(const rapidjson::Value& value, Core::FileSystem::Path& out)
    {
        if (value.IsObject())
        {
            std::string directoryName;
            std::string baseName;
            std::string number;
            std::string extension;
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("DirectoryName", i.name.GetString()))
                {
                    fromJSON(i.value, directoryName);
                }
                else if (0 == strcmp("BaseName", i.name.GetString()))
                {
                    fromJSON(i.value, baseName);
                }
                else if (0 == strcmp("Number", i.name.GetString()))
                {
                    fromJSON(i.value, number);
                }
                else if (0 == strcmp("Extension", i.name.GetString()))
                {
                    fromJSON(i.value, extension);
                }
            }
            out = Core::FileSystem::Path(directoryName, baseName, number, extension);
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    std::ostream& operator << (std::ostream& os, const Core::FileSystem::Path& value)
    {
        os << value.get();
        return os;
    }

} // namespace djv

namespace std
{
    std::size_t hash<djv::Core::FileSystem::Path>::operator() (const djv::Core::FileSystem::Path& value) const noexcept
    {
        size_t hash = 0;
        djv::Core::Memory::hashCombine<std::string>(hash, value.get());
        return hash;
    }

} // namespace std

