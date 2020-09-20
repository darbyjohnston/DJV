// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace UI
    {
        inline bool ShortcutData::isValid() const
        {
            return key != 0;
        }

        inline bool ShortcutData::operator == (const ShortcutData & other) const
        {
            return key == other.key && modifiers == other.modifiers;
        }

        inline bool ShortcutData::operator < (const ShortcutData& other) const
        {
            return std::tie(key, modifiers) < std::tie(other.key, other.modifiers);
        }

        inline bool ShortcutDataPair::operator == (const ShortcutDataPair& other) const
        {
            return primary == other.primary && secondary == other.secondary;
        }

        inline bool ShortcutDataPair::operator < (const ShortcutDataPair& other) const
        {
            return std::tie(primary, secondary) < std::tie(other.primary, other.secondary);
        }

    } // namespace UI
} // namespace djv
