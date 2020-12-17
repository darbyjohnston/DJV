// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/ISettings.h>

#include <djvCore/ValueObserver.h>

#include <map>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides debugging settings.
        class DebugSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(DebugSettings);

        protected:
            void _init(const std::shared_ptr<System::Context>&);

            DebugSettings();

        public:
            ~DebugSettings() override;

            static std::shared_ptr<DebugSettings> create(const std::shared_ptr<System::Context>&);

            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeMessagesPopup() const;

            std::map<std::string, bool> getBellowsState() const;

            void setBellowsState(const std::map<std::string, bool>&);

            void load(const rapidjson::Value &) override;
            rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

