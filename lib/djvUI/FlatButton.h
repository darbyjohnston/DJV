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
            //! This class provides a button widget with a flat appearance. It is
            //! generally used for toolbar buttons or in lists.
            class Flat : public IButton
            {
                DJV_NON_COPYABLE(Flat);

            protected:
                void _init(Core::Context *);
                Flat();

            public:
                virtual ~Flat();

                static std::shared_ptr<Flat> create(Core::Context *);
                static std::shared_ptr<Flat> create(const std::string&, Core::Context *);
                static std::shared_ptr<Flat> create(const std::string&, const std::string& icon, Core::Context *);

                const std::string& getIcon() const;
                void setIcon(const std::string&);

                const std::string& getText() const;
                void setText(const std::string&);

                TextHAlign getTextHAlign() const;
                TextVAlign getTextVAlign() const;
                void setTextHAlign(TextHAlign);
                void setTextVAlign(TextVAlign);

				ColorRole getTextColorRole() const;
                void setTextColorRole(ColorRole);

                const std::string & getFont() const;
                const std::string & getFontFace() const;
				MetricsRole getFontSizeRole() const;
                void setFont(const std::string &);
                void setFontFace(const std::string &);
                void setFontSizeRole(MetricsRole);

                const Layout::Margin& getInsideMargin() const;
                void setInsideMargin(const Layout::Margin&);

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;

            private:
                void _widgetUpdate();

				DJV_PRIVATE();
            };

        } // namespace Button

		typedef Button::Flat FlatButton;

    } // namespace UI
} // namespace djv
