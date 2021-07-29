// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/Path.h>

#include <djvCore/Memory.h>
#include <djvCore/String.h>

#include <algorithm>
#include <array>

#include <fseq.h>

namespace djv
{
    namespace System
    {
        namespace File
        {
            DJV_ENUM_HELPERS_IMPLEMENTATION(ResourcePath);
            DJV_ENUM_HELPERS_IMPLEMENTATION(DirectoryShortcut);

            Path::Path()
            {}

            Path::Path(const std::string& value)
            {
                set(value);
            }

            Path::Path(const Path& value, const std::string& append)
            {
                *this = value;
                this->append(append);
            }

            Path::Path(const std::string& value, const std::string& append)
            {
                set(value);
                this->append(append);
            }

            Path::Path(
                const std::string& directoryName,
                const std::string& baseName,
                const std::string& number,
                const std::string& extension) :
                _directoryName(directoryName),
                _baseName(baseName),
                _number(number),
                _extension(extension)
            {}
            
            void Path::set(std::string value)
            {
                if (value == get())
                    return;
#if defined(DJV_PLATFORM_WINDOWS)
                if (!value.empty() && value[value.size() - 1] == ':')
                {
                    value += getSeparator(PathSeparator::Windows);
                }
#endif // DJV_PLATFORM_WINDOWS
                split(value, _directoryName, _baseName, _number, _extension);
            }
            
            void Path::append(const std::string& value, char separator)
            {
                const std::string path = get();
                const size_t pathSize = path.size();
                if (value.size() && pathSize && !isSeparator(path[pathSize - 1]))
                {
                    set(path + separator + value);
                }
                else
                {
                    set(path + value);
                }
            }

            bool Path::isRoot() const
            {
                bool out = false;
                if (1 == _directoryName.size() &&
                    '/' == _directoryName[0] &&
                    _baseName.empty() &&
                    _number.empty() &&
                    _extension.empty())
                {
                    // UNIX style: "/"
                    out = true;
                }
                else if (
                    3 == _directoryName.size() &&
                    (_directoryName[0] >= 'A' || _directoryName[0] <= 'Z') &&
                    (_directoryName[0] >= 'a' || _directoryName[0] <= 'z') &&
                    ':' == _directoryName[1] &&
                    ('\\' == _directoryName[2] || '/' == _directoryName[2]) &&
                    _baseName.empty() &&
                    _number.empty() &&
                    _extension.empty())
                {
                    // Windows style: "C:\"
                    out = true;
                }
                return out;
            }

            bool Path::isServer() const
            {
                bool out = false;
                if ("\\\\" == _directoryName && !_baseName.empty())
                {
                    out = true;
                }
                else if (_directoryName.size() > 2 &&
                    '\\' == _directoryName[0] &&
                    '\\' == _directoryName[1] &&
                    _baseName.empty() &&
                    _number.empty() &&
                    _extension.empty())
                {
                    const size_t directoryNameSize = _directoryName.size();
                    size_t i = 2;
                    for (; i != '\\' && i < directoryNameSize; ++i)
                        ;
                    if (directoryNameSize == i)
                    {
                        out = true;
                    }
                }
                return out;
            }

            bool Path::cdUp(char separator)
            {
                auto subDirectories = splitDir(get());
                if (subDirectories.size() > 1)
                {
                    subDirectories.pop_back();
                    const auto join = joinDirs(subDirectories, separator);
                    set(join);
                    return true;
                }
                return false;
            }
            
            void Path::setFileName(const std::string& value)
            {
                set(_directoryName + value);
            }

            bool Path::operator == (const Path& other) const
            {
                std::string s = get();
                removeTrailingSeparator(s);
                std::string s2 = other.get();
                removeTrailingSeparator(s2);
                return s == s2;
            }

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

                for (const auto& i : Core::String::split(tmp, { '/', '\\' }))
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
                    out += Core::String::join(tmp, separator);
                }

                return out;
            }

        } // namespace File
    } // namespace System

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        System::File,
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

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        System::File,
        DirectoryShortcut,
        DJV_TEXT("directory_shortcut_home"),
        DJV_TEXT("directory_shortcut_desktop"),
        DJV_TEXT("directory_shortcut_documents"),
        DJV_TEXT("directory_shortcut_downloads"));

    rapidjson::Value toJSON(const System::File::Path& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        out.AddMember("DirectoryName", toJSON(value.getDirectoryName(), allocator), allocator);
        out.AddMember("BaseName", toJSON(value.getBaseName(), allocator), allocator);
        out.AddMember("Number", toJSON(value.getNumber(), allocator), allocator);
        out.AddMember("Extension", toJSON(value.getExtension(), allocator), allocator);
        return out;
    }

    void fromJSON(const rapidjson::Value& value, System::File::Path& out)
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
            out = System::File::Path(directoryName, baseName, number, extension);
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    std::ostream& operator << (std::ostream& os, const System::File::Path& value)
    {
        os << value.get();
        return os;
    }

} // namespace djv

namespace std
{
    std::size_t hash<djv::System::File::Path>::operator() (const djv::System::File::Path& value) const noexcept
    {
        size_t hash = 0;
        djv::Core::Memory::hashCombine<std::string>(hash, value.get());
        return hash;
    }

} // namespace std

