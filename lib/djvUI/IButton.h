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

            void paintEvent(Core::PaintEvent&) override;

            void pointerEnterEvent(Core::PointerEnterEvent&) override;
            void pointerLeaveEvent(Core::PointerLeaveEvent&) override;
            void pointerMoveEvent(Core::PointerMoveEvent&) override;
            void buttonPressEvent(Core::ButtonPressEvent&) override;
            void buttonReleaseEvent(Core::ButtonReleaseEvent&) override;

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
