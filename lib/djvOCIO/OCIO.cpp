// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvOCIO/OCIO.h>

using namespace djv::Core;

namespace djv
{
    namespace OCIO
    {
        Convert::Convert()
        {}

        Convert::Convert(const std::string& input, const std::string& output) :
            input(input),
            output(output)
        {}

        View::View()
        {}

        Display::Display()
        {}

    } // namespace OCIO
} // namespace djv

