// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2017-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        //! This namespace provides button widgets.
        namespace Button
        {
            //! This class provides the interface for button widgets.
            class IButton : public Widget
            {
            protected:
                IButton();

            public:
                virtual ~IButton() = 0;

                //! \name Button Type
                ///@{

                ButtonType getButtonType() const;

                virtual void setButtonType(ButtonType);

                ///@}

                //! \name Checked State
                ///@{

                bool isChecked() const;

                virtual void setChecked(bool);

                ///@}

                //! \name Options
                ///@{

                ColorRole getForegroundColorRole() const;

                virtual void setForegroundColorRole(ColorRole);

                ///@}

                //! \name Callbacks
                ///@{

                void setClickedCallback(const std::function<void(void)>&);
                void setCheckedCallback(const std::function<void(bool)>&);
                void doClick();

                ///@}

                void addAction(const std::shared_ptr<Action>&) override;
                void removeAction(const std::shared_ptr<Action>&) override;
                void clearActions() override;

            protected:
                bool _isToggled() const;
                bool _isHovered() const;
                bool _isPressed() const;
                const glm::vec2& _getPressedPos() const;
                void _setCanRejectPressed(bool);

                virtual void _doClick();
                virtual void _doCheck(bool);

                void _pointerEnterEvent(System::Event::PointerEnter&) override;
                void _pointerLeaveEvent(System::Event::PointerLeave&) override;
                void _pointerMoveEvent(System::Event::PointerMove&) override;
                void _buttonPressEvent(System::Event::ButtonPress&) override;
                void _buttonReleaseEvent(System::Event::ButtonRelease&) override;

            private:
                void _actionUpdate();

                DJV_PRIVATE();
            };

        } // namespace Button
    } // namespace UI
} // namespace djv
