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
        namespace Button
        {
            //! This enum provides the menu button styles.
            enum class MenuStyle
            {
                Flat,
                Tool,
                ComboBox
            };

            //! This class provides a button for menu widgets.
            class Menu : public Widget
            {
                DJV_NON_COPYABLE(Menu);

            protected:
                void _init(MenuStyle, const std::shared_ptr<Core::Context>&);
                Menu();

            public:
                virtual ~Menu();

                static std::shared_ptr<Menu> create(MenuStyle, const std::shared_ptr<Core::Context>&);

                const std::string& getIcon() const;
                const std::string& getPopupIcon() const;
                void setIcon(const std::string&);
                void setPopupIcon(const std::string&);

                const std::string & getText() const;
                void setText(const std::string &);

                const std::string& getFont() const;
                const std::string& getFontFace() const;
                MetricsRole getFontSizeRole() const;
                void setFont(const std::string&);
                void setFontFace(const std::string&);
                void setFontSizeRole(MetricsRole);

                bool isOpen() const;
                void setOpen(bool);
                void setOpenCallback(const std::function<void(bool)>&);

                MenuStyle getMenuStyle() const;

                void setTextFocusEnabled(bool);

                bool acceptFocus(TextFocusDirection) override;

            protected:
                void _preLayoutEvent(Core::Event::PreLayout &) override;
                void _layoutEvent(Core::Event::Layout &) override;
                void _paintEvent(Core::Event::Paint &) override;
                void _pointerEnterEvent(Core::Event::PointerEnter &) override;
                void _pointerLeaveEvent(Core::Event::PointerLeave &) override;
                void _pointerMoveEvent(Core::Event::PointerMove &) override;
                void _buttonPressEvent(Core::Event::ButtonPress&) override;
                void _buttonReleaseEvent(Core::Event::ButtonRelease&) override;
                void _keyPressEvent(Core::Event::KeyPress&) override;
                void _textFocusEvent(Core::Event::TextFocus&) override;
                void _textFocusLostEvent(Core::Event::TextFocusLost&) override;

            private:
                bool _isHovered() const;

                void _doOpenCallback();

                DJV_PRIVATE();
            };

        } // namespace Button
    } // namespace UI
} // namespace djv
