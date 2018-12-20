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
        //! This class provides a button for tab widgets.
        class TabButton : public IButton
        {
            DJV_NON_COPYABLE(TabButton);

        protected:
            void _init(const std::string& text, Core::Context *);
            TabButton();

        public:
            virtual ~TabButton();

            static std::shared_ptr<TabButton> create(const std::string&, Core::Context *);

            const std::string& getText() const;
            void setText(const std::string&);

            TextHAlign getTextHAlign() const;
            TextVAlign getTextVAlign() const;
            void setTextHAlign(TextHAlign);
            void setTextVAlign(TextVAlign);

            const std::string & getFontFace() const;
            MetricsRole getFontSizeRole() const;
            void setFontFace(const std::string &);
            void setFontSizeRole(MetricsRole);

            float getHeightForWidth(float) const override;
            void preLayoutEvent(Core::PreLayoutEvent&) override;
            void layoutEvent(Core::LayoutEvent&) override;
            void paintEvent(Core::PaintEvent&) override;

            void updateEvent(Core::UpdateEvent&) override;

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace UI
} // namespace djv
