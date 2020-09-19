// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace UI
    {
        inline ShortcutData::ShortcutData(int key, int modifiers) :
            key(key),
            modifiers(modifiers)
        {}

        inline bool ShortcutData::isValid() const
        {
            return key != 0;
        }

        inline std::string ShortcutData::keyToString(int key)
        {
            const auto& data = getKeyStrings();
            const auto i = data.find(key);
            return i != data.end() ? i->second : std::string();
        }

        inline std::string ShortcutData::modifierToString(int key)
        {
            const auto& data = getModifierStrings();
            const auto i = data.find(key);
            return i != data.end() ? i->second : std::string();
        }

        inline int ShortcutData::keyFromString(const std::string& value)
        {
            int out = 0;
            for (const auto& i : getKeyStrings())
            {
                if (value == i.second)
                {
                    out = i.first;
                    break;
                }
            }
            return out;
        }

        inline int ShortcutData::modifierFromString(const std::string& value)
        {
            int out = 0;
            for (const auto& i : getModifierStrings())
            {
                if (value == i.second)
                {
                    out = i.first;
                    break;
                }
            }
            return out;
        }

        inline std::string ShortcutData::getText(const ShortcutData& shortcutData, const std::shared_ptr<System::TextSystem>& textSystem)
        {
            return getText(shortcutData.key, shortcutData.modifiers, textSystem);
        }

        inline bool ShortcutData::operator == (const ShortcutData & other) const
        {
            return key == other.key && modifiers == other.modifiers;
        }

        inline bool ShortcutData::operator < (const ShortcutData& other) const
        {
            return std::tie(key, modifiers) < std::tie(other.key, other.modifiers);
        }

        inline ShortcutDataPair::ShortcutDataPair()
        {}

        inline ShortcutDataPair::ShortcutDataPair(const ShortcutData& primary) :
            primary(primary)
        {}

        inline ShortcutDataPair::ShortcutDataPair(const ShortcutData& primary, const ShortcutData& secondary) :
            primary(primary),
            secondary(secondary)
        {}

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
