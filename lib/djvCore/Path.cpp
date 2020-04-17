// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/Path.h>

#include <djvCore/FileInfo.h>
#include <djvCore/Math.h>
#include <djvCore/Memory.h>

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

            void Path::append(const std::string & value, char separator)
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
                const std::string path = get();
                const bool unixStyle = path.size() == 1 ? '/' == path[0] : false;
                //! \todo This is buggy.
                const bool windowsStyle1 = path.size() == 3 ? (':' == path[1] && '\\' == path[2]) : false;
                const bool windowsStyle2 = path.size() == 3 ? (':' == path[1] && '/' == path[2]) : false;
                return unixStyle || windowsStyle1 || windowsStyle2;
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

            void Path::setDirectoryName(const std::string & value)
            {
                _directoryName = value;
            }

            void Path::setFileName(const std::string & value)
            {
                set(_directoryName + value);
            }

            void Path::setBaseName(const std::string & value)
            {
                _baseName = value;
            }

            void Path::setNumber(const std::string & value)
            {
                _number = value;
            }

            void Path::setExtension(const std::string & value)
            {
                _extension = value;
            }

            void Path::removeTrailingSeparator(std::string & value)
            {
                if (value.size() > 2)
                {
                    const auto i = value.end() - 1;
                    if (getSeparator(PathSeparator::Unix) == *i || getSeparator(PathSeparator::Windows) == *i)
                    {
                        value.erase(i);
                    }
                }
            }

            void Path::split(
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

            std::vector<std::string> Path::splitDir(const std::string & value)
            {
                std::vector<std::string> out;

                // Save the root path.
                std::string tmp = value;
                if (tmp.size() > 1 && isSeparator(tmp[0]) && isSeparator(tmp[1]))
                {
                    out.push_back(std::string(2, tmp[0]));
                    tmp.erase(tmp.begin(), tmp.begin() + 1);
                }
                else if (tmp.size() > 0 && isSeparator(tmp[0]))
                {
                    out.push_back(std::string(1, tmp[0]));
                    tmp.erase(tmp.begin());
                }

                for (const auto & i : String::split(tmp, { '/', '\\' }))
                {
                    out.push_back(i);
                }

                return out;
            }

            std::string Path::joinDirs(const std::vector<std::string> & value, char separator)
            {
                std::string out;

                // Restore the root path.
                std::vector<std::string> tmp = value;
                if (tmp.size() && tmp[0].size() == 2 && isSeparator(tmp[0][0]) && isSeparator(tmp[0][1]))
                {
                    out += std::string(2, separator);
                    tmp.erase(tmp.begin());
                }
                else if (tmp.size() && tmp[0].size() == 1 && isSeparator(tmp[0][0]))
                {
                    out += std::string(1, separator);
                    tmp.erase(tmp.begin());
                }

                out += String::join(tmp, separator);

                return out;
            }

        } // namespace FileSystem
    } // namespace Core

    picojson::value toJSON(const Core::FileSystem::Path & value)
    {
        return toJSON(value.get());
    }

    void fromJSON(const picojson::value & value, Core::FileSystem::Path & out)
    {
        if (value.is<std::string>())
        {
            std::string s;
            fromJSON(value, s);
            out.set(s);
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    std::ostream & operator << (std::ostream & os, const Core::FileSystem::Path & value)
    {
        os << value.get();
        return os;
    }

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

} // namespace djv

