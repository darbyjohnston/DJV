// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2017-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            //! This class provides a button for menu widgets.
            class Menu : public Widget
            {
                DJV_NON_COPYABLE(Menu);

            protected:
                void _init(MenuButtonStyle, const std::shared_ptr<System::Context>&);
                Menu();

            public:
                ~Menu() override;

                static std::shared_ptr<Menu> create(MenuButtonStyle, const std::shared_ptr<System::Context>&);

                bool isOpen() const;
                void setOpen(bool);
                void setOpenCallback(const std::function<void(bool)>&);

                const std::string& getIcon() const;
                const std::string& getPopupIcon() const;
                void setIcon(const std::string&);
                void setPopupIcon(const std::string&);

                const std::string& getText() const;
                void setText(const std::string&);

                const std::string& getFontFamily() const;
                const std::string& getFontFace() const;
                MetricsRole getFontSizeRole() const;
                void setFontFamily(const std::string&);
                void setFontFace(const std::string&);
                void setFontSizeRole(MetricsRole);

                int getElide() const;
                void setElide(int);

                MetricsRole getInsideMargin() const;
                void setInsideMargin(MetricsRole);

                MenuButtonStyle getMenuButtonStyle() const;

                void setTextFocusEnabled(bool);

                bool acceptFocus(TextFocusDirection) override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;
                void _paintEvent(System::Event::Paint&) override;
                void _pointerEnterEvent(System::Event::PointerEnter&) override;
                void _pointerLeaveEvent(System::Event::PointerLeave&) override;
                void _pointerMoveEvent(System::Event::PointerMove&) override;
                void _buttonPressEvent(System::Event::ButtonPress&) override;
                void _buttonReleaseEvent(System::Event::ButtonRelease&) override;
                void _keyPressEvent(System::Event::KeyPress&) override;
                void _textFocusEvent(System::Event::TextFocus&) override;
                void _textFocusLostEvent(System::Event::TextFocusLost&) override;

            private:
                bool _isHovered() const;
                                
                void _doOpenCallback();

                DJV_PRIVATE();
            };

        } // namespace Button
    } // namespace UI
} // namespace djv
