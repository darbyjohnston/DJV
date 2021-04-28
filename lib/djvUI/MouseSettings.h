// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Enum.h>

#include <djvUI/ISettings.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            //! Mouse settings.
            class Mouse : public ISettings
            {
                DJV_NON_COPYABLE(Mouse);

            protected:
                void _init(const std::shared_ptr<System::Context>& context);

                Mouse();

            public:
                static std::shared_ptr<Mouse> create(const std::shared_ptr<System::Context>&);

                //! \name Double-Click
                ///@{

                std::shared_ptr<Core::Observer::IValueSubject<float> > observeDoubleClickTime() const;

                void setDoubleClickTime(float);

                ///@}

                //! \name Scrolling
                ///@{

                std::shared_ptr<Core::Observer::IValueSubject<ScrollWheelSpeed> > observeScrollWheelSpeed() const;
                std::shared_ptr<Core::Observer::IValueSubject<bool> > observeReverseScrolling() const;

                void setScrollWheelSpeed(ScrollWheelSpeed);
                void setReverseScrolling(bool);

                ///@}

                void load(const rapidjson::Value&) override;
                rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UI
} // namespace djv

