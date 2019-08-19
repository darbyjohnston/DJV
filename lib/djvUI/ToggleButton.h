// Copyright (c) 2017-2018 Darby Johnston
// All rights reserved.
//
// See LICENSE.txt for licensing information.

#pragma once

#include <djvUI/IButton.h>

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            //! This class provides a toggle button.
            class Toggle : public IButton
            {
                DJV_NON_COPYABLE(Toggle);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                Toggle();

            public:
                virtual ~Toggle();

                static std::shared_ptr<Toggle> create(const std::shared_ptr<Core::Context>&);

                void setChecked(bool) override;

            protected:
                void _styleEvent(Core::Event::Style&) override;
                void _preLayoutEvent(Core::Event::PreLayout &) override;
                void _paintEvent(Core::Event::Paint&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Button

        typedef Button::Toggle ToggleButton;

    } // namespace UI
} // namespace djv
