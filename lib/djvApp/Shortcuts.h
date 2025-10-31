// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the DJV project.

#pragma once

#include <ftk/UI/Event.h>

namespace djv
{
    namespace app
    {
        //! Keyboard shortcut.
        struct Shortcut
        {
            Shortcut() = default;
            Shortcut(
                const std::string& name,
                const std::string& text,
                ftk::Key = ftk::Key::Unknown,
                int modifiers = 0);

            std::string     name;
            std::string     text;
            ftk::Key key       = ftk::Key::Unknown;
            int             modifiers = 0;

            bool operator == (const Shortcut&) const;
            bool operator != (const Shortcut&) const;
        };

        //! \name Serialize
        ///@{

        void to_json(nlohmann::json&, const Shortcut&);

        void from_json(const nlohmann::json&, Shortcut&);

        ///@}
    }
}
