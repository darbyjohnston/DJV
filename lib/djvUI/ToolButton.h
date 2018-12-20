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

    namespace UI
    {
        //! This class provides a tool button widget.
        class ToolButton : public IButton
        {
            DJV_NON_COPYABLE(ToolButton);

        protected:
            void _init(Core::Context *);
            ToolButton();

        public:
            virtual ~ToolButton();

            static std::shared_ptr<ToolButton> create(Core::Context *);
            static std::shared_ptr<ToolButton> create(const Core::Path&, Core::Context *);

            const Core::Path& getIcon() const;
            void setIcon(const Core::Path&);

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
