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
            //! Push button.
            class Push : public IButton
            {
                DJV_NON_COPYABLE(Push);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                Push();

            public:
                ~Push() override;

                static std::shared_ptr<Push> create(const std::shared_ptr<System::Context>&);

                //! \name Icon
                ///@{

                std::string getIcon() const;

                void setIcon(const std::string&);

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

                TextHAlign getTextHAlign() const;
                const Layout::Margin& getInsideMargin() const;

                void setTextHAlign(TextHAlign);
                void setInsideMargin(const Layout::Margin&);

                ///@}

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
                void _keyPressEvent(System::Event::KeyPress&) override;
                void _textFocusEvent(System::Event::TextFocus&) override;
                void _textFocusLostEvent(System::Event::TextFocusLost&) override;

            private:
                void _actionUpdate();

                DJV_PRIVATE();
            };

        } // namespace Button

        typedef Button::Push PushButton;

    } // namespace UI
} // namespace djv
