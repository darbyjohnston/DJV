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
        //! This class provides the input settings.
        class InputSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(InputSettings);

        protected:
            void _init(const std::shared_ptr<Core::Context>& context);

            InputSettings();

        public:
            static std::shared_ptr<InputSettings> create(const std::shared_ptr<Core::Context>&);

            std::shared_ptr<Core::MapSubject<std::string, std::vector<UI::ShortcutData> > > observeShortcuts() const;
            void setShortcuts(const UI::ShortcutDataMap&);
            void addShortcut(const std::string&, const std::vector<UI::ShortcutData>&, bool overwrite = false);
            void addShortcut(const std::string&, int key, bool overwrite = false);
            void addShortcut(const std::string&, int key, int keyModifiers, bool overwrite = false);

            std::shared_ptr<Core::IValueSubject<ScrollWheelSpeed> > observeScrollWheelSpeed() const;
            void setScrollWheelSpeed(ScrollWheelSpeed);

            void load(const rapidjson::Value &) override;
            rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

