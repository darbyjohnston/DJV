// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/ErrorFunc.h>

#include <sstream>

namespace djv
{
    namespace Core
    {
        namespace Error
        {
            std::string format(const std::string& value)
            {
                std::stringstream ss;
                ss << "ERROR: " << value;
                return ss.str();
            }

            std::string format(const std::exception& e)
            {
                return format(e.what());
            }

        } // namespace Error
    } // namespace Core
} // namespace djv
