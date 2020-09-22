// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace System
    {
        namespace File
        {
            inline std::string Path::get() const
            {
                return _directoryName + _baseName + _number + _extension;
            }
            
            inline bool Path::isSeparator(char c) noexcept
            {
                return '/' == c || '\\' == c;
            }

            inline char Path::getSeparator(PathSeparator value) noexcept
            {
                return PathSeparator::Unix == value ? '/' : '\\';
            }
            
            inline bool Path::isEmpty() const noexcept
            {
                return get().empty();
            }

            inline const std::string& Path::getDirectoryName() const noexcept
            {
                return _directoryName;
            }

            inline std::string Path::getFileName() const
            {
                return _baseName + _number + _extension;
            }

            inline const std::string& Path::getBaseName() const noexcept
            {
                return _baseName;
            }

            inline const std::string& Path::getNumber() const noexcept
            {
                return _number;
            }

            inline const std::string& Path::getExtension() const noexcept
            {
                return _extension;
            }

            inline void Path::setDirectoryName(const std::string& value)
            {
                _directoryName = value;
            }

            inline void Path::setBaseName(const std::string& value)
            {
                _baseName = value;
            }

            inline void Path::setNumber(const std::string& value)
            {
                _number = value;
            }

            inline void Path::setExtension(const std::string& value)
            {
                _extension = value;
            }

            inline bool Path::operator == (const Path& other) const
            {
                return get() == other.get();
            }

            inline bool Path::operator != (const Path& other) const
            {
                return !(*this == other);
            }

            inline bool Path::operator < (const Path& other) const
            {
                return get().compare(other.get()) < 0;
            }

            inline Path::operator std::string() const
            {
                return get();
            }

        } // namespace File
    } // namespace System
} // namespace djv

