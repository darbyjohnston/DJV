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
        //! This class provides the messages settings.
        class MessagesSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(MessagesSettings);

        protected:
            void _init(const std::shared_ptr<System::Context>&);

            MessagesSettings();

        public:
            ~MessagesSettings() override;

            static std::shared_ptr<MessagesSettings> create(const std::shared_ptr<System::Context>&);

            std::shared_ptr<Core::Observer::IValueSubject<bool> > observePopup() const;

            void setPopup(bool);

            void load(const rapidjson::Value &) override;
            rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

