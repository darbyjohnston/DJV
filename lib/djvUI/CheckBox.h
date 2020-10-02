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
            //! This class provides a check box button.
            class CheckBox : public IButton
            {
                DJV_NON_COPYABLE(CheckBox);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                CheckBox();

            public:
                ~CheckBox() override;

                static std::shared_ptr<CheckBox> create(const std::shared_ptr<System::Context>&);

                //! \name Text
                ///@{

                std::string getText() const;

                void setText(const std::string&);

                ///@}

                //! \name Font
                ///@{

                const std::string& getFontFamily() const;
                const std::string& getFontFace() const;
                MetricsRole getFontSizeRole() const;

                void setFontFamily(const std::string&);
                void setFontFace(const std::string&);
                void setFontSizeRole(MetricsRole);

                ///@}

                bool acceptFocus(TextFocusDirection) override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;
                void _paintEvent(System::Event::Paint&) override;
                void _buttonPressEvent(System::Event::ButtonPress&) override;
                void _keyPressEvent(System::Event::KeyPress&) override;
                void _textFocusEvent(System::Event::TextFocus&) override;
                void _textFocusLostEvent(System::Event::TextFocusLost&) override;

            private:
                Math::BBox2f _getCheckGeometry() const;
                Math::BBox2f _getLabelGeometry() const;

                void _widgetUpdate();

                DJV_PRIVATE();
            };

        } // namespace Button

        typedef Button::CheckBox CheckBox;

    } // namespace UI
} // namespace djv
