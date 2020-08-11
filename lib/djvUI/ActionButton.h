// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/IButton.h>

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            //! This class provides a button for an action.
            class ActionButton : public Button::IButton
            {
                DJV_NON_COPYABLE(ActionButton);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                ActionButton();

            public:
                ~ActionButton() override;

                static std::shared_ptr<ActionButton> create(const std::shared_ptr<Core::Context>&);

                bool hasShowShortcuts() const;
                void setShowShortcuts(bool);

                void setChecked(bool) override;
                void setButtonType(ButtonType) override;

                void addAction(const std::shared_ptr<Action>&) override;
                void removeAction(const std::shared_ptr<Action>&) override;
                void clearActions() override;

                bool acceptFocus(TextFocusDirection) override;

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;
                void _paintEvent(Core::Event::Paint&) override;
                void _keyPressEvent(Core::Event::KeyPress&) override;
                void _textFocusEvent(Core::Event::TextFocus&) override;
                void _textFocusLostEvent(Core::Event::TextFocusLost&) override;

            private:
                void _actionUpdate();
                void _widgetUpdate();

                DJV_PRIVATE();
            };

        } // namespace Button

        using Button::ActionButton;

    } // namespace UI
} // namespace djv

