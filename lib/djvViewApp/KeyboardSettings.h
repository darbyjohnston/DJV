// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>

#include <djvUI/ISettings.h>
#include <djvUI/ShortcutData.h>

#include <djvCore/MapObserver.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace ViewApp
    {
        //! Keyboard settings.
        class KeyboardSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(KeyboardSettings);

        protected:
            void _init(const std::shared_ptr<System::Context>& context);

            KeyboardSettings();

        public:
            static std::shared_ptr<KeyboardSettings> create(const std::shared_ptr<System::Context>&);

            std::shared_ptr<Core::Observer::MapSubject<std::string, UI::ShortcutDataPair> > observeShortcuts() const;

            void setShortcuts(const UI::ShortcutDataMap&);
            void addShortcut(const std::string&, const UI::ShortcutDataPair&, bool overwrite = false);
            void addShortcut(const std::string&, int key, bool overwrite = false);
            void addShortcut(const std::string&, int key, int keyModifiers, bool overwrite = false);

            void load(const rapidjson::Value &) override;
            rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

