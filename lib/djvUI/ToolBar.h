// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/RowLayout.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a tool bar widget.
        class ToolBar : public Widget
        {
            DJV_NON_COPYABLE(ToolBar);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ToolBar();

        public:
            ~ToolBar() override;

            static std::shared_ptr<ToolBar> create(const std::shared_ptr<System::Context>&);

            void setStretch(const std::shared_ptr<Widget>&, RowStretch = RowStretch::Expand);

            void addSeparator();
            void addSpacer(MetricsRole = MetricsRole::Spacing);
            void addExpander();

            float getHeightForWidth(float) const override;

            void addAction(const std::shared_ptr<Action>&) override;
            void removeAction(const std::shared_ptr<Action>&) override;
            void clearActions() override;

            void addChild(const std::shared_ptr<IObject>&) override;
            void removeChild(const std::shared_ptr<IObject>&) override;
            void clearChildren() override;

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

