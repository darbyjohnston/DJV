// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUI/ShortcutDataFunc.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    rapidjson::Value toJSON(const UI::ShortcutData& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        out.AddMember("Key", toJSON(UI::ShortcutData::keyToString(value.key), allocator), allocator);
        std::vector<std::string> modifiers;
        if (value.modifiers & GLFW_MOD_SHIFT)
        {
            modifiers.push_back(UI::ShortcutData::modifierToString(GLFW_MOD_SHIFT));
        }
        if (value.modifiers & GLFW_MOD_CONTROL)
        {
            modifiers.push_back(UI::ShortcutData::modifierToString(GLFW_MOD_CONTROL));
        }
        if (value.modifiers & GLFW_MOD_ALT)
        {
            modifiers.push_back(UI::ShortcutData::modifierToString(GLFW_MOD_ALT));
        }
        if (value.modifiers & GLFW_MOD_SUPER)
        {
            modifiers.push_back(UI::ShortcutData::modifierToString(GLFW_MOD_SUPER));
        }
        out.AddMember("Modifiers", toJSON(modifiers, allocator), allocator);
        return out;
    }

    rapidjson::Value toJSON(const UI::ShortcutDataPair& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        out.AddMember("Primary", toJSON(value.primary, allocator), allocator);
        out.AddMember("Secondary", toJSON(value.secondary, allocator), allocator);
        return out;
    }

    void fromJSON(const rapidjson::Value& value, UI::ShortcutData& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("Key", i.name.GetString()))
                {
                    std::string s;
                    fromJSON(i.value, s);
                    out.key = UI::ShortcutData::keyFromString(s);
                }
                else if (0 == strcmp("Modifiers", i.name.GetString()))
                {
                    std::vector<std::string> modifiers;
                    fromJSON(i.value, modifiers);
                    for (const auto& j : modifiers)
                    {
                        out.modifiers |= UI::ShortcutData::modifierFromString(j);
                    }
                }
            }
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, UI::ShortcutDataPair& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("Primary", i.name.GetString()))
                {
                    fromJSON(i.value, out.primary);
                }
                else if (0 == strcmp("Secondary", i.name.GetString()))
                {
                    fromJSON(i.value, out.secondary);
                }
            }
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv
