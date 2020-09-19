// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/ViewApp.h>

#include <djvUI/ISettings.h>

#include <djvCore/MapObserver.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the application settings.
        class ApplicationSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(ApplicationSettings);

        protected:
            void _init(const std::shared_ptr<System::Context>& context);

            ApplicationSettings();

        public:
            static std::shared_ptr<ApplicationSettings> create(const std::shared_ptr<System::Context>&);

            std::shared_ptr<Core::IMapSubject<std::string, bool> > observeSettingsBellows() const;
            void setSettingsBellows(const std::map<std::string, bool>&);

            void load(const rapidjson::Value &) override;
            rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

