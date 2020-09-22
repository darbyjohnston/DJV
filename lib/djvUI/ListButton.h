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
            //! This class provides a button for use in lists.
            class List : public IButton
            {
                DJV_NON_COPYABLE(List);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                List();

            public:
                ~List() override;

                static std::shared_ptr<List> create(const std::shared_ptr<System::Context>&);

                std::string getIcon() const;

                void setIcon(const std::string&);

                std::string getRightIcon() const;

                void setRightIcon(const std::string&);

                std::string getText() const;

                void setText(const std::string&);

                TextHAlign getTextHAlign() const;

                void setTextHAlign(TextHAlign);

                std::string getRightText() const;

                void setRightText(const std::string&);

                const std::string& getFont() const;
                const std::string& getFontFace() const;
                MetricsRole getFontSizeRole() const;

                void setFont(const std::string&);
                void setFontFace(const std::string&);
                void setFontSizeRole(MetricsRole);

                size_t getElide() const;

                void setElide(size_t);

                const Layout::Margin& getInsideMargin() const;

                void setInsideMargin(const Layout::Margin&);

                void setForegroundColorRole(ColorRole) override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;
                void _paintEvent(System::Event::Paint&) override;

            private:
                void _widgetUpdate();

                DJV_PRIVATE();
            };

        } // namespace Button

        typedef Button::List ListButton;

    } // namespace UI
} // namespace djv
