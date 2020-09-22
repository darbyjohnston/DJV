// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <tuple>

namespace djv
{
    namespace OCIO
    {
        inline bool Convert::isValid() const
        {
            return !(input.empty() || output.empty());
        }

        inline bool Convert::operator == (const Convert& other) const
        {
            return
                input == other.input &&
                output == other.output;
        }

        inline bool Convert::operator < (const Convert& other) const
        {
            return std::tie(input, output) < std::tie(other.input, other.output);
        }

        inline bool View::operator == (const View& other) const
        {
            return
                name == other.name &&
                colorSpace == other.colorSpace &&
                looks == other.looks;
        }

        inline bool Display::operator == (const Display& other) const
        {
            return
                name == other.name &&
                defaultView == other.defaultView &&
                views == other.views;
        }

    } // namespace OCIO
} // namespace djv

