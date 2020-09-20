// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/ViewApp.h>

#include <djvUI/ISettings.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the new user experience settings.
        class NUXSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(NUXSettings);

        protected:
            void _init(const std::shared_ptr<System::Context>& context);

            NUXSettings();

        public:
            static std::shared_ptr<NUXSettings> create(const std::shared_ptr<System::Context>&);

            std::shared_ptr<Core::IValueSubject<bool> > observeNUX() const;

            void setNUX(bool);

            void load(const rapidjson::Value &) override;
            rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

