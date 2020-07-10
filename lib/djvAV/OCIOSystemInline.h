// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

namespace djv
{
    namespace AV
    {
        namespace OCIO
        {
            inline Config::Config()
            {}

            inline bool Config::isValid() const
            {
                return !fileName.empty();
            }

            inline bool Config::operator == (const Config& other) const
            {
                return
                    fileName == other.fileName &&
                    name == other.name &&
                    display == other.display &&
                    view == other.view &&
                    imageColorSpaces == other.imageColorSpaces;
            }

            inline bool Config::operator != (const Config& other) const
            {
                return !(*this == other);
            }

        } // namespace OCIO
    } // namespace AV
} // namespace djv

