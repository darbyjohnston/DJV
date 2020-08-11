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
                void _init(const std::shared_ptr<Core::Context>&);
                Tool();

            public:
                ~Tool() override;

                static std::shared_ptr<Tool> create(const std::shared_ptr<Core::Context>&);

                void setIcon(const std::string&);
                void setIconSizeRole(MetricsRole);
                void setCheckedIcon(const std::string&);

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

                MetricsRole getInsideMargin() const;
                void setInsideMargin(MetricsRole);

                void setTextFocusEnabled(bool);

                void setAutoRepeat(bool);

                void setChecked(bool) override;
                void setForegroundColorRole(ColorRole) override;

                void addAction(const std::shared_ptr<Action>&) override;
                void removeAction(const std::shared_ptr<Action>&) override;
                void clearActions() override;
                bool acceptFocus(TextFocusDirection) override;

            protected:
                void _doClick() override;
                void _doCheck(bool) override;

                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;
                void _paintEvent(Core::Event::Paint&) override;
                void _buttonPressEvent(Core::Event::ButtonPress&) override;
                void _buttonReleaseEvent(Core::Event::ButtonRelease&) override;
                void _keyPressEvent(Core::Event::KeyPress&) override;
                void _textFocusEvent(Core::Event::TextFocus&) override;
                void _textFocusLostEvent(Core::Event::TextFocusLost&) override;

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
