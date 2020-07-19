// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a tab widget.
        class TabWidget : public Widget
        {
            DJV_NON_COPYABLE(TabWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            TabWidget();

        public:
            ~TabWidget() override;

            static std::shared_ptr<TabWidget> create(const std::shared_ptr<Core::Context>&);
            
            void setText(const std::shared_ptr<Widget>&, const std::string&);

            int getCurrentTab() const;
            void setCurrentTab(int);
            void setCurrentTabCallback(const std::function<void(int)>&);
            void closeCurrentTab();

            float getHeightForWidth(float) const override;

            void addChild(const std::shared_ptr<IObject>&) override;
            void removeChild(const std::shared_ptr<IObject>&) override;
            void clearChildren() override;

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

