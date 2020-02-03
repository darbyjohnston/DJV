//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvCore/OS.h>

#include <djvCore/String.h>

#include <algorithm>

//#pragma optimize("", off)

namespace djv
{
    namespace Core
    {
        namespace OS
        {
            char getListSeparator(ListSeparator value)
            {
                return ListSeparator::Unix == value ? ':' : ';';
            }

            char getCurrentListSeparator()
            {
#if defined(DJV_PLATFORM_WINDOWS)
                return getListSeparator(ListSeparator::Windows);
#else
                return getListSeparator(ListSeparator::Unix);
#endif // DJV_PLATFORM_WINDOWS
            }

            std::vector<std::string> getStringListEnv(const std::string& name)
            {
                return String::split(OS::getEnv(name), OS::getCurrentListSeparator());
            }

            int getIntEnv(const std::string & name)
            {
                const std::string env = getEnv(name);
                return !env.empty() ? std::stoi(env) : 0;
            }

        } // namespace OS
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::OS,
        DirectoryShortcut,
        DJV_TEXT("Home"),
        DJV_TEXT("Desktop"),
        DJV_TEXT("Documents"),
        DJV_TEXT("Downloads"));

} // namespace djv
