// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCore/OSFunc.h>

#include <djvCore/StringFunc.h>

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

            bool getStringListEnv(const std::string& name, std::vector<std::string>& out)
            {
                std::string value;
                if (OS::getEnv(name, value))
                {
                    out = String::split(value, OS::getCurrentListSeparator());
                    return true;
                }
                return false;
            }

            bool getIntEnv(const std::string& name, int& out)
            {
                std::string value;
                if (getEnv(name, value))
                {
                    out = !value.empty() ? std::stoi(value) : 0;
                    return true;
                }
                return false;
            }

        } // namespace OS
    } // namespace Core
} // namespace djv
