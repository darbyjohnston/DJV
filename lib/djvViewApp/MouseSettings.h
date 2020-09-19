// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>

#include <djvUI/ISettings.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the mouse settings.
        class MouseSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(MouseSettings);

        protected:
            void _init(const std::shared_ptr<System::Context>& context);

            MouseSettings();

        public:
            static std::shared_ptr<MouseSettings> create(const std::shared_ptr<System::Context>&);

            std::shared_ptr<Core::IValueSubject<ScrollWheelSpeed> > observeScrollWheelSpeed() const;
            void setScrollWheelSpeed(ScrollWheelSpeed);

            void load(const rapidjson::Value &) override;
            rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

