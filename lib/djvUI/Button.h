// Copyright (c) 2017-2018 Darby Johnston
// All rights reserved.
//
// See LICENSE.txt for licensing information.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace Core
    {
        class Path;

    } // namespace Core

    namespace Image
    {
        class Data;
    
    } // namespace Image

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

        protected:
            bool _isToggled() const;
            bool _isHovered() const;
            bool _isPressed() const;
            const glm::vec2& _getPressedPos() const;
            void _setCanRejectPressed(bool);

            virtual void _doClickedCallback();
            virtual void _doCheckedCallback(bool);

            void _paintEvent(Core::PaintEvent&) override;
            void _pointerEnterEvent(Core::PointerEnterEvent&) override;
            void _pointerLeaveEvent(Core::PointerLeaveEvent&) override;
            void _pointerMoveEvent(Core::PointerMoveEvent&) override;
            void _buttonPressEvent(Core::ButtonPressEvent&) override;
            void _buttonReleaseEvent(Core::ButtonReleaseEvent&) override;

            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! This class manages a group of buttons.
        class ButtonGroup : public std::enable_shared_from_this<ButtonGroup>
        {
            DJV_NON_COPYABLE(ButtonGroup);

        protected:
            ButtonGroup();

        public:
            ~ButtonGroup();

            static std::shared_ptr<ButtonGroup> create(ButtonType);

            const std::vector<std::shared_ptr<IButton> >& getButtons() const;
            size_t getButtonCount() const;
            int getButtonIndex(const std::shared_ptr<IButton>&) const;
            void addButton(const std::shared_ptr<IButton>&);
            void removeButton(const std::shared_ptr<IButton>&);
            void clearButtons();

            ButtonType getButtonType() const;
            void setButtonType(ButtonType);

            int getChecked() const;
            void setChecked(int, bool = true);

            void setClickedCallback(const std::function<void(int)>&);
            void setCheckedCallback(const std::function<void(int, bool)>&);
            void setRadioCallback(const std::function<void(int)>&);

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! This class provides a button widget.
        class Button : public IButton
        {
            DJV_NON_COPYABLE(Button);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            Button();

        public:
            virtual ~Button();

            static std::shared_ptr<Button> create(const std::shared_ptr<Core::Context>&);
            static std::shared_ptr<Button> create(const Core::Path&, const std::shared_ptr<Core::Context>&);
            static std::shared_ptr<Button> create(const Core::Path&, const std::string&, const std::shared_ptr<Core::Context>&);

            const Core::Path& getIcon() const;
            void setIcon(const Core::Path&);

            const std::string& getText() const;
            void setText(const std::string&);

            TextHAlign getTextHAlign() const;
            TextVAlign getTextVAlign() const;
            void setTextHAlign(TextHAlign);
            void setTextVAlign(TextVAlign);

            FontFace getFontFace() const;
            MetricsRole getFontSizeRole() const;
            void setFontFace(FontFace);
            void setFontSizeRole(MetricsRole);

            const Margin& getInsideMargin() const;
            void setInsideMargin(const Margin&);

            float getHeightForWidth(float) const override;

        protected:
            void _updateEvent(Core::UpdateEvent&) override;
            void _preLayoutEvent(Core::PreLayoutEvent&) override;
            void _layoutEvent(Core::LayoutEvent&) override;
            void _paintEvent(Core::PaintEvent&) override;

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! This class provides a button widget for use in lists.
        class ListButton : public IButton
        {
            DJV_NON_COPYABLE(ListButton);

        protected:
            void _init(const std::string& text, const std::shared_ptr<Core::Context>&);
            ListButton();

        public:
            virtual ~ListButton();

            static std::shared_ptr<ListButton> create(const std::shared_ptr<Core::Context>&);
            static std::shared_ptr<ListButton> create(const std::string&, const std::shared_ptr<Core::Context>&);

            const std::string& getText() const;
            void setText(const std::string&);

            TextHAlign getTextHAlign() const;
            TextVAlign getTextVAlign() const;
            void setTextHAlign(TextHAlign);
            void setTextVAlign(TextVAlign);

            FontFace getFontFace() const;
            MetricsRole getFontSizeRole() const;
            void setFontFace(FontFace);
            void setFontSizeRole(MetricsRole);

            float getHeightForWidth(float) const override;

        protected:
            void _updateEvent(Core::UpdateEvent&) override;
            void _preLayoutEvent(Core::PreLayoutEvent&) override;
            void _layoutEvent(Core::LayoutEvent&) override;

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! This class provides a toggle button.
        class ToggleButton : public IButton
        {
            DJV_NON_COPYABLE(ToggleButton);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ToggleButton();

        public:
            virtual ~ToggleButton();

            static std::shared_ptr<ToggleButton> create(const std::shared_ptr<Core::Context>&);

        protected:
            void _preLayoutEvent(Core::PreLayoutEvent&) override;
            void _paintEvent(Core::PaintEvent&) override;

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace UI
} // namespace djv
