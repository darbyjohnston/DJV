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
        //! This class provides the input settings.
        class InputSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(InputSettings);

        protected:
            void _init(const std::shared_ptr<Core::Context>& context);

            InputSettings();

        public:
            static std::shared_ptr<InputSettings> create(const std::shared_ptr<Core::Context>&);

            std::shared_ptr<Core::IValueSubject<ScrollWheelSpeed> > observeScrollWheelSpeed() const;
            void setScrollWheelSpeed(ScrollWheelSpeed);

            void load(const picojson::value &) override;
            picojson::value save() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

