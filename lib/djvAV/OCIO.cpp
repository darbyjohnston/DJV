// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/OCIO.h>

#include <tuple>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace OCIO
        {
            Convert::Convert()
            {}

            Convert::Convert(const std::string& input, const std::string& output) :
                input(input),
                output(output)
            {}

            bool Convert::isValid() const
            {
                return !(input.empty() || output.empty());
            }

            bool Convert::operator == (const Convert& other) const
            {
                return
                    input == other.input &&
                    output == other.output;
            }

            bool Convert::operator < (const Convert& other) const
            {
                return std::tie(input, output) < std::tie(other.input, other.output);
            }

            View::View()
            {}
            
            bool View::operator == (const View& other) const
            {
                return
                    name == other.name &&
                    colorSpace == other.colorSpace &&
                    looks == other.looks;
            }
            
            Display::Display()
            {}

            bool Display::operator == (const Display& other) const
            {
                return
                    name == other.name &&
                    defaultView == other.defaultView &&
                    views == other.views;
            }

        } // namespace OCIO
    } // namespace AV
} // namespace djv

