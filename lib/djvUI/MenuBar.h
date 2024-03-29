// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/RowLayout.h>

namespace djv
{
    namespace UI
    {
        class Menu;

        //! Menu bar widget.
        class MenuBar : public Widget
        {
            DJV_NON_COPYABLE(MenuBar);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            MenuBar();

        public:
            ~MenuBar() override;

            static std::shared_ptr<MenuBar> create(const std::shared_ptr<System::Context>&);

            void setSide(const std::shared_ptr<Widget>&, Side);

            void setStretch(const std::shared_ptr<Widget>&, Side, RowStretch = RowStretch::Expand);

            void addSeparator(Side);
            void addSpacer(Side, MetricsRole = MetricsRole::Spacing);
            void addExpander(Side);

            float getHeightForWidth(float) const override;

            void addChild(const std::shared_ptr<IObject>&) override;
            void removeChild(const std::shared_ptr<IObject>&) override;

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            bool _eventFilter(const std::shared_ptr<IObject>&, System::Event::Event&) override;

        private:
            void _openCallback(bool, const std::weak_ptr<Menu>&);
            void _closeCallback(const std::weak_ptr<Menu>&);
            
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

