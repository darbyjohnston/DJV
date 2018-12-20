// Copyright (c) 2017-2018 Darby Johnston
// All rights reserved.
//
// See LICENSE.txt for licensing information.

#pragma once

#include <djvUI/IButton.h>

namespace djv
{
    namespace Core
    {
        class Path;

    } // namespace Core

    namespace Image
    {
        class Data;
    
    } // namespace Image

    namespace UI
    {
        //! This class provides a push button widget.
        class PushButton : public IButton
        {
            DJV_NON_COPYABLE(PushButton);

        protected:
            void _init(Core::Context *);
            PushButton();

        public:
            virtual ~PushButton();

            static std::shared_ptr<PushButton> create(Core::Context *);
            static std::shared_ptr<PushButton> create(const std::string&, Core::Context *);
            static std::shared_ptr<PushButton> create(const std::string&, const Core::Path& icon, Core::Context *);

            const Core::Path& getIcon() const;
            void setIcon(const Core::Path&);

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

            const Margin& getInsideMargin() const;
            void setInsideMargin(const Margin&);

            float getHeightForWidth(float) const override;

            void updateEvent(Core::UpdateEvent&) override;
            void preLayoutEvent(Core::PreLayoutEvent&) override;
            void layoutEvent(Core::LayoutEvent&) override;
            void paintEvent(Core::PaintEvent&) override;

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace UI
} // namespace djv
