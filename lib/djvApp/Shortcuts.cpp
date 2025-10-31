// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the DJV project.

#include <djvApp/Shortcuts.h>

namespace djv
{
    namespace app
    {
        Shortcut::Shortcut(
            const std::string& name,
            const std::string& text,
            ftk::Key key,
            int modifiers) :
            name(name),
            text(text),
            key(key),
            modifiers(modifiers)
        {}

        bool Shortcut::operator == (const Shortcut& other) const
        {
            return
                name == other.name &&
                text == other.text &&
                key == other.key &&
                modifiers == other.modifiers;
        }

        bool Shortcut::operator != (const Shortcut& other) const
        {
            return !(*this == other);
        }

        void to_json(nlohmann::json& json, const Shortcut& in)
        {
            json["Name"] = in.name;
            json["Text"] = in.text;
            json["Key"] = to_string(in.key);
            json["Modifiers"] = in.modifiers;
        }

        void from_json(const nlohmann::json& json, Shortcut& out)
        {
            json.at("Name").get_to(out.name);
            json.at("Text").get_to(out.text);
            from_string(json.at("Key").get<std::string>(), out.key);
            json.at("Modifiers").get_to(out.modifiers);
        }
    }
}
