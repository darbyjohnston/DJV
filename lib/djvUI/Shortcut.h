// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Enum.h>

#include <djvCore/ValueObserver.h>

#include <djvCore/RapidJSON.h>

#include <map>
#include <memory>

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

            bool operator == (const ShortcutData&) const;
        };

        //! This class provides a keyboard shortcut.
        class Shortcut : public std::enable_shared_from_this<Shortcut>
        {
            DJV_NON_COPYABLE(Shortcut);

        protected:
            void _init();
            Shortcut();

        public:
            virtual ~Shortcut();

            static std::shared_ptr<Shortcut> create();
            static std::shared_ptr<Shortcut> create(const ShortcutData&);
            static std::shared_ptr<Shortcut> create(int key);
            static std::shared_ptr<Shortcut> create(int key, int modifiers);

            std::shared_ptr<Core::IValueSubject<ShortcutData> > observeShortcut() const;
            void setShortcut(const ShortcutData&);

            void setCallback(const std::function<void(void)>&);
            void doCallback();

            static int getSystemModifier();

            static std::map<int, std::string> getKeyStrings();
            static std::map<int, std::string> getModifierStrings();

            static std::string keyToString(int);
            static std::string modifierToString(int);
            static int keyFromString(const std::string&);
            static int modifierFromString(const std::string&);

            static std::string getText(
                const std::shared_ptr<Shortcut>&,
                const std::shared_ptr<Core::TextSystem>&);
            static std::string getText(
                int key,
                int keyModifiers,
                const std::shared_ptr<Core::TextSystem>&);

        private:
            DJV_PRIVATE();
        };

    } // namespace UI

    rapidjson::Value toJSON(const UI::ShortcutData&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, UI::ShortcutData&);

} // namespace djv
