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

            ColorRole getCheckedColorRole() const;
            void setCheckedColorRole(ColorRole);

            void setClickedCallback(const std::function<void(void)>&);
            void setCheckedCallback(const std::function<void(bool)>&);

            void paintEvent(Core::Event::Paint&) override;

            void pointerEnterEvent(Core::Event::PointerEnter&) override;
            void pointerLeaveEvent(Core::Event::PointerLeave&) override;
            void pointerMoveEvent(Core::Event::PointerMove&) override;
            void buttonPressEvent(Core::Event::ButtonPress&) override;
            void buttonReleaseEvent(Core::Event::ButtonRelease&) override;

        protected:
            bool _isToggled() const;
            bool _isHovered() const;
            bool _isPressed() const;
            const glm::vec2& _getPressedPos() const;
            void _setCanRejectPressed(bool);

            virtual void _doClickedCallback();
            virtual void _doCheckedCallback(bool);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace UI
} // namespace djv
