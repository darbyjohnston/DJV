// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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
        DJV_TEXT("directory_shortcut_home"),
        DJV_TEXT("directory_shortcut_desktop"),
        DJV_TEXT("directory_shortcut_documents"),
        DJV_TEXT("directory_shortcut_downloads"));

} // namespace djv
