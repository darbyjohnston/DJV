// Copyright (c) 2017-2018 Darby Johnston
// All rights reserved.
//
// See LICENSE.txt for licensing information.

#pragma once

#include <djvUI/IButton.h>

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            //! This class provides a button widget for use in lists.
            class List : public IButton
            {
                DJV_NON_COPYABLE(List);

            protected:
                void _init(Core::Context *);
                List();

            public:
                virtual ~List();

                static std::shared_ptr<List> create(Core::Context *);
                static std::shared_ptr<List> create(const std::string&, Core::Context *);
                static std::shared_ptr<List> create(const std::string&, const std::string& icon, Core::Context *);

                const std::string& getIcon() const;
                void setIcon(const std::string&);

                const std::string& getText() const;
                void setText(const std::string&);

                TextHAlign getTextHAlign() const;
                TextVAlign getTextVAlign() const;
                void setTextHAlign(TextHAlign);
                void setTextVAlign(TextVAlign);

                Style::ColorRole getTextColorRole() const;
                void setTextColorRole(Style::ColorRole);

                const std::string & getFont() const;
                const std::string & getFontFace() const;
                Style::MetricsRole getFontSizeRole() const;
                void setFont(const std::string &);
                void setFontFace(const std::string &);
                void setFontSizeRole(Style::MetricsRole);

                const Layout::Margin& getInsideMargin() const;
                void setInsideMargin(const Layout::Margin&);

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;

            private:
                void _widgetUpdate();

                struct Private;
                std::unique_ptr<Private> _p;
            };

        } // namespace Button
    } // namespace UI
} // namespace djv
