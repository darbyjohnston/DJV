// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/RapidJSON.h>

#include <map>
#include <vector>

namespace djv
{
    namespace Core
    {
        class TextSystem;

    } // namespace Core

    namespace UI
    {
        //! This struct provides keyboard shortcut data.
        struct ShortcutData
        {
            explicit ShortcutData(int key = 0, int modifiers = 0);

            int key       = 0;
            int modifiers = 0;

            bool isValid() const;

            static int getSystemModifier();

            static std::map<int, std::string> getKeyStrings();
            static std::map<int, std::string> getModifierStrings();

            static std::string keyToString(int);
            static std::string modifierToString(int);
            static int keyFromString(const std::string&);
            static int modifierFromString(const std::string&);

            static std::string getText(
                const ShortcutData&,
                const std::shared_ptr<Core::TextSystem>&);
            static std::string getText(
                int key,
                int keyModifiers,
                const std::shared_ptr<Core::TextSystem>&);

            bool operator == (const ShortcutData&) const;
            bool operator < (const ShortcutData&) const;
        };

        //! This struct provides primary and secondary shortcut data.
        struct ShortcutDataPair
        {
            ShortcutDataPair();
            ShortcutDataPair(const ShortcutData& primary);
            ShortcutDataPair(const ShortcutData& primary, const ShortcutData& secondary);

            ShortcutData primary;
            ShortcutData secondary;

            bool operator == (const ShortcutDataPair&) const;
            bool operator < (const ShortcutDataPair&) const;
        };

        //! This typedef provides a map of shortcut data.
        typedef std::map<std::string, ShortcutDataPair> ShortcutDataMap;

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

#include <djvUI/ShortcutDataInline.h>
