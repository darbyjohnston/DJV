// Copyright (c) 2017-2018 Darby Johnston
// All rights reserved.
//
// See LICENSE.txt for licensing information.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        //! This namespace provides button widgets.
        namespace Button
        {
            //! This class provides an interface for button widgets.
            //!
            //! \todo [1.0 S] Add an option for auto-repeat.
            class IButton : public Widget
            {
            protected:
                IButton();

            public:
                virtual ~IButton() = 0;

                ButtonType getButtonType() const;
                void setButtonType(ButtonType);

                bool isChecked() const;
                void setChecked(bool);

                Style::ColorRole getForegroundColorRole() const;
                Style::ColorRole getCheckedColorRole() const;
                Style::ColorRole getCheckedForegroundColorRole() const;
                void setForegroundColorRole(Style::ColorRole);
                void setCheckedColorRole(Style::ColorRole);
                void setCheckedForegroundColorRole(Style::ColorRole);

                void setClickedCallback(const std::function<void(void)>&);
                void setCheckedCallback(const std::function<void(bool)>&);

            protected:
                bool _isToggled() const;
                bool _isHovered() const;
                bool _isPressed() const;
                const glm::vec2& _getPressedPos() const;
                void _setCanRejectPressed(bool);
                Style::ColorRole _getForegroundColorRole() const;

                virtual void _doClickedCallback();
                virtual void _doCheckedCallback(bool);

                void _paintEvent(Core::Event::Paint&) override;
                void _pointerEnterEvent(Core::Event::PointerEnter&) override;
                void _pointerLeaveEvent(Core::Event::PointerLeave&) override;
                void _pointerMoveEvent(Core::Event::PointerMove&) override;
                void _buttonPressEvent(Core::Event::ButtonPress&) override;
                void _buttonReleaseEvent(Core::Event::ButtonRelease&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Button
    } // namespace UI
} // namespace djv
