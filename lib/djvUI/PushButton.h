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
            //! This class provides a button widget with a raised appearance. It is
            //! generally used for confirmation buttons.
            class Push : public IButton
            {
                DJV_NON_COPYABLE(Push);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                Push();

            public:
                ~Push() override;

                static std::shared_ptr<Push> create(const std::shared_ptr<Core::Context>&);

                std::string getIcon() const;
                void setIcon(const std::string&);

                std::string getText() const;
                void setText(const std::string&);

                TextHAlign getTextHAlign() const;
                void setTextHAlign(TextHAlign);

                const std::string& getFont() const;
                const std::string& getFontFace() const;
                MetricsRole getFontSizeRole() const;
                void setFont(const std::string&);
                void setFontFace(const std::string&);
                void setFontSizeRole(MetricsRole);

                const Layout::Margin& getInsideMargin() const;
                void setInsideMargin(const Layout::Margin&);

                void setForegroundColorRole(ColorRole) override;

                bool acceptFocus(TextFocusDirection) override;

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;
                void _paintEvent(Core::Event::Paint&) override;
                void _buttonPressEvent(Core::Event::ButtonPress&) override;
                void _keyPressEvent(Core::Event::KeyPress&) override;
                void _textFocusEvent(Core::Event::TextFocus&) override;
                void _textFocusLostEvent(Core::Event::TextFocusLost&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Button

        typedef Button::Push PushButton;

    } // namespace UI
} // namespace djv
