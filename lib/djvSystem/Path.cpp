// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/Path.h>

#include <djvSystem/PathFunc.h>

namespace djv
{
    namespace System
    {
        namespace File
        {
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

        } // namespace File
    } // namespace System
} // namespace djv

