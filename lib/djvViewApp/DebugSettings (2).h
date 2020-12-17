// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/ISettings.h>

#include <djvCore/ValueObserver.h>

#include <map>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the miscellaneous tools settings.
        class MiscToolsSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(MiscToolsSettings);

        protected:
            void _init(const std::shared_ptr<System::Context>&);

            MiscToolsSettings();

        public:
            ~MiscToolsSettings() override;

            static std::shared_ptr<MiscToolsSettings> create(const std::shared_ptr<System::Context>&);

            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeMessagesPopup() const;

            void setMessagesPopup(bool);

            std::map<std::string, bool> getDebugBellowsState() const;

            void setDebugBellowsState(const std::map<std::string, bool>&);

            void load(const rapidjson::Value &) override;
            rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

