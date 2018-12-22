//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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
            inline bool Path::isPathSeparator(char c)
            {
                return '/' == c || '\\' == c;
            }

            inline bool Path::isDigit(char c)
            {
                return
                    '0' == c ||
                    '1' == c ||
                    '2' == c ||
                    '3' == c ||
                    '4' == c ||
                    '5' == c ||
                    '6' == c ||
                    '7' == c ||
                    '8' == c ||
                    '9' == c;
            }

            inline bool Path::isSequence(char c)
            {
                return isDigit(c) || isSequenceSeparator(c) || '#' == c;
            }

            inline bool Path::isSequenceSeparator(char c)
            {
                return '-' == c || ',' == c;
            }

            inline char Path::getPathSeparator(PathSeparator value)
            {
                return PathSeparator::Unix == value ? '/' : '\\';
            }

            char Path::getCurrentPathSeparator()
            {
#if defined(DJV_PLATFORM_WINDOWS)
                return getPathSeparator(PathSeparator::Windows);
#else
                return getPathSeparator(PathSeparator::Unix);
#endif // DJV_PLATFORM_WINDOWS
            }

            inline bool Path::operator == (const Path& other) const
            {
                return other._value == _value;
            }

            inline bool Path::operator != (const Path& other) const
            {
                return !(*this == other);
            }

            inline bool Path::operator < (const Path& other) const
            {
                return _value.compare(other._value) < 0;
            }

            inline Path::operator std::string() const
            {
                return _value;
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

namespace std
{
    inline std::size_t hash<djv::Core::FileSystem::Path>::operator() (const djv::Core::FileSystem::Path& value) const noexcept
    {
        size_t hash = 0;
        djv::Core::Memory::hashCombine<std::string>(hash, value.get());
        return hash;
    }

} // namespace std
