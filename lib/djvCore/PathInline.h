// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/Memory.h>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            inline Path::Path()
            {}

            inline Path::Path(const std::string& value)
            {
                set(value);
            }

            inline Path::Path(const Path& value, const std::string& append)
            {
                *this = value;
                this->append(append);
            }

            inline Path::Path(const std::string& value, const std::string& append)
            {
                set(value);
                this->append(append);
            }

            inline std::string Path::get() const
            {
                return _directoryName + _baseName + _number + _extension;
            }

            inline bool Path::isEmpty() const
            {
                return get().empty();
            }

            inline bool Path::isServer() const
            {
                return "\\\\" == _directoryName && !_baseName.empty();
            }

            inline const std::string& Path::getDirectoryName() const
            {
                return _directoryName;
            }

            inline std::string Path::getFileName() const
            {
                return _baseName + _number + _extension;
            }

            inline const std::string& Path::getBaseName() const
            {
                return _baseName;
            }

            inline const std::string& Path::getNumber() const
            {
                return _number;
            }

            inline const std::string& Path::getExtension() const
            {
                return _extension;
            }

            inline bool Path::isSeparator(char c)
            {
                return '/' == c || '\\' == c;
            }

            inline char Path::getSeparator(PathSeparator value)
            {
                return PathSeparator::Unix == value ? '/' : '\\';
            }

            inline char Path::getCurrentSeparator()
            {
#if defined(DJV_PLATFORM_WINDOWS)
                return getSeparator(PathSeparator::Windows);
#else
                return getSeparator(PathSeparator::Unix);
#endif // DJV_PLATFORM_WINDOWS
            }

            inline bool Path::operator == (const Path & other) const
            {
                return get() == other.get();
            }

            inline bool Path::operator != (const Path & other) const
            {
                return !(*this == other);
            }

            inline bool Path::operator < (const Path & other) const
            {
                return get().compare(other.get()) < 0;
            }

            inline Path::operator std::string() const
            {
                return get();
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

namespace std
{
    inline std::size_t hash<djv::Core::FileSystem::Path>::operator() (const djv::Core::FileSystem::Path & value) const noexcept
    {
        size_t hash = 0;
        djv::Core::Memory::hashCombine<std::string>(hash, value.get());
        return hash;
    }

} // namespace std
