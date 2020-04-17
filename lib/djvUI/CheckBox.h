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
                void _init(const std::shared_ptr<Core::Context>&);
                CheckBox();

            public:
                virtual ~CheckBox();

                static std::shared_ptr<CheckBox> create(const std::shared_ptr<Core::Context>&);

                std::string getText() const;
                void setText(const std::string &);

                const std::string & getFont() const;
                const std::string & getFontFace() const;
                MetricsRole getFontSizeRole() const;
                void setFont(const std::string &);
                void setFontFace(const std::string &);
                void setFontSizeRole(MetricsRole);

                bool acceptFocus(TextFocusDirection) override;

            protected:
                void _preLayoutEvent(Core::Event::PreLayout &) override;
                void _layoutEvent(Core::Event::Layout &) override;
                void _paintEvent(Core::Event::Paint&) override;
                void _buttonPressEvent(Core::Event::ButtonPress&) override;
                void _keyPressEvent(Core::Event::KeyPress&) override;
                void _textFocusEvent(Core::Event::TextFocus&) override;
                void _textFocusLostEvent(Core::Event::TextFocusLost&) override;

            private:
                Core::BBox2f _getCheckGeometry() const;
                Core::BBox2f _getLabelGeometry() const;

                DJV_PRIVATE();
            };

        } // namespace Button

        typedef Button::CheckBox CheckBox;

    } // namespace UI
} // namespace djv
