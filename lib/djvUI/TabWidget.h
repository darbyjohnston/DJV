// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        class TabBar;

        //! Tab widget.
        class TabWidget : public Widget
        {
            DJV_NON_COPYABLE(TabWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            TabWidget();

        public:
            ~TabWidget() override;

            static std::shared_ptr<TabWidget> create(const std::shared_ptr<System::Context>&);

            //! \name Tabs
            ///@{

            const std::shared_ptr<TabBar>& getTabBar() const;

            void setText(const std::shared_ptr<Widget>&, const std::string&);
            void setTooltip(const std::shared_ptr<Widget>&, const std::string&);

            ///@}

            //! \name Current Tab
            ///@{

            int getCurrentTab() const;

            void setCurrentTab(int);

            void setCurrentTabCallback(const std::function<void(int)>&);

            ///@}

            //! \name Options
            ///@{

            bool areTabsClosable() const;

            void setTabsClosable(bool);

            void setTabCloseCallback(const std::function<void(int)>&);

            size_t getTextElide() const;

            void setTextElide(size_t);

            ///@]

            float getHeightForWidth(float) const override;

            void addChild(const std::shared_ptr<IObject>&) override;
            void removeChild(const std::shared_ptr<IObject>&) override;
            void clearChildren() override;

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

