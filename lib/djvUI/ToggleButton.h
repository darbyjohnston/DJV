// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2017-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/IButton.h>

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            //! Toggle button widget.
            class Toggle : public IButton
            {
                DJV_NON_COPYABLE(Toggle);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                Toggle();

            public:
                ~Toggle() override;

                static std::shared_ptr<Toggle> create(const std::shared_ptr<System::Context>&);

                void setChecked(bool) override;

                bool acceptFocus(TextFocusDirection) override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _paintEvent(System::Event::Paint&) override;
                void _buttonPressEvent(System::Event::ButtonPress&) override;
                void _keyPressEvent(System::Event::KeyPress&) override;
                void _textFocusEvent(System::Event::TextFocus&) override;
                void _textFocusLostEvent(System::Event::TextFocusLost&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Button

        typedef Button::Toggle ToggleButton;

    } // namespace UI
} // namespace djv
