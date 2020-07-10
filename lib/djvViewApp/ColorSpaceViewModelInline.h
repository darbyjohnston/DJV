// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace ViewApp
    {
        inline bool ColorSpaceConfigData::Item::operator == (const ColorSpaceConfigData::Item& other) const
        {
            return
                fileName == other.fileName &&
                name == other.name;
        }

        inline bool ColorSpaceConfigData::operator == (const ColorSpaceConfigData& other) const
        {
            return
                items == other.items &&
                current == other.current;
        }

        inline bool ColorSpaceDisplayData::operator == (const ColorSpaceDisplayData& other) const
        {
            return
                names == other.names &&
                current == other.current;
        }

        inline bool ColorSpaceViewData::operator == (const ColorSpaceViewData& other) const
        {
            return
                names == other.names &&
                current == other.current;
        }

    } // namespace ViewApp
} // namespace djv

