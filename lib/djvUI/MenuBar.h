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

        //! This class provides a menu bar widget.
        class MenuBar : public Widget
        {
            DJV_NON_COPYABLE(MenuBar);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            MenuBar();

        public:
            virtual ~MenuBar();

            static std::shared_ptr<MenuBar> create(const std::shared_ptr<Core::Context>&);

            void setSide(const std::shared_ptr<Widget>&, Side);

            void setStretch(const std::shared_ptr<Widget>&, Layout::RowStretch, Side);

            void addSeparator(Side);
            void addSpacer(Side, MetricsRole = MetricsRole::Spacing);
            void addExpander(Side);

            float getHeightForWidth(float) const override;

            void addChild(const std::shared_ptr<IObject>&) override;
            void removeChild(const std::shared_ptr<IObject>&) override;

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

            bool _eventFilter(const std::shared_ptr<IObject>&, Core::Event::Event&) override;

        private:
            void _openCallback(bool, const std::weak_ptr<Menu>&);
            void _closeCallback(const std::weak_ptr<Menu>&);
            
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

