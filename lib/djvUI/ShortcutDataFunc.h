// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/ShortcutData.h>

#include <djvCore/RapidJSONFunc.h>

#include <map>
#include <memory>
#include <vector>

namespace djv
{
    namespace UI
    {
        //! \name Shortcuts
        ///@{

        int getSystemModifier();

        std::map<int, std::string> getKeyStrings();
        std::map<int, std::string> getModifierStrings();

        std::string keyToString(int);
        std::string modifierToString(int);
        int keyFromString(const std::string&);
        int modifierFromString(const std::string&);

        std::string getText(
            const ShortcutData&,
            const std::shared_ptr<System::TextSystem>&);
        std::string getText(
            int key,
            int keyModifiers,
            const std::shared_ptr<System::TextSystem>&);

        ///@}

    } // namespace UI

    rapidjson::Value toJSON(const UI::ShortcutData&, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const UI::ShortcutDataPair&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, UI::ShortcutData&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, UI::ShortcutDataPair&);

} // namespace djv
