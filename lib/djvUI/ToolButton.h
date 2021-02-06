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
            //! This class provides a button for use with toolbars.
            class Tool : public IButton
            {
                DJV_NON_COPYABLE(Tool);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                Tool();

            public:
                ~Tool() override;

                static std::shared_ptr<Tool> create(const std::shared_ptr<System::Context>&);

                //! \name Icon
                ///@{

                const std::string& getIcon() const;
                const std::string& getCheckedIcon() const;

                void setIcon(const std::string&);
                void setCheckedIcon(const std::string&);

                ///@}

                //! \name Text
                ///@{

                std::string getText() const;

                void setText(const std::string&);

                ///@}

                //! \name Font
                ///@{

                const std::string& getFont() const;
                const std::string& getFontFace() const;
                MetricsRole getFontSizeRole() const;

                void setFont(const std::string&);
                void setFontFace(const std::string&);
                void setFontSizeRole(MetricsRole);

                ///@}

                //! \name Options
                ///@{

                MetricsRole getIconSizeRole() const;
                TextHAlign getTextHAlign() const;
                size_t getTextElide() const;
                const Layout::Margin& getInsideMargin() const;
                bool isTextFocusEnabled() const;
                bool hasAutoRepeat() const;

                void setIconSizeRole(MetricsRole);
                void setTextHAlign(TextHAlign);
                void setTextElide(size_t);
                void setInsideMargin(const Layout::Margin&);
                void setTextFocusEnabled(bool);
                void setAutoRepeat(bool);

                ///@}

                void setChecked(bool) override;
                void setForegroundColorRole(ColorRole) override;

                void addAction(const std::shared_ptr<Action>&) override;
                void removeAction(const std::shared_ptr<Action>&) override;
                void clearActions() override;
                bool acceptFocus(TextFocusDirection) override;

            protected:
                void _doClick() override;
                void _doCheck(bool) override;

                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;
                void _paintEvent(System::Event::Paint&) override;
                void _buttonPressEvent(System::Event::ButtonPress&) override;
                void _buttonReleaseEvent(System::Event::ButtonRelease&) override;
                void _keyPressEvent(System::Event::KeyPress&) override;
                void _textFocusEvent(System::Event::TextFocus&) override;
                void _textFocusLostEvent(System::Event::TextFocusLost&) override;

            private:
                void _actionUpdate();
                void _widgetUpdate();
                void _iconUpdate();

                DJV_PRIVATE();
            };

        } // namespace Button

        typedef Button::Tool ToolButton;

    } // namespace UI
} // namespace djv
