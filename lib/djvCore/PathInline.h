//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

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

            inline bool Path::isPathSeparator(char c)
            {
                return '/' == c || '\\' == c;
            }

            inline char Path::getPathSeparator(PathSeparator value)
            {
                return PathSeparator::Unix == value ? '/' : '\\';
            }

            inline char Path::getCurrentPathSeparator()
            {
#if defined(DJV_PLATFORM_WINDOWS)
                return getPathSeparator(PathSeparator::Windows);
#else
                return getPathSeparator(PathSeparator::Unix);
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
