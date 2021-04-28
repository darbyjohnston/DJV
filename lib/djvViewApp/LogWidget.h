// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/ToolBar.h>

namespace djv
{
    namespace ViewApp
    {
        //! Log widget.
        class LogWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(LogWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            LogWidget();

        public:
            ~LogWidget() override;

            static std::shared_ptr<LogWidget> create(const std::shared_ptr<System::Context>&);
            
            void copyLog();
            void reloadLog();
            void clearLog();
            void setFilter(const std::string&);

            float getHeightForWidth(float) const override;

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        //! Log tool bar widget.
        class LogToolBar : public UI::ToolBar
        {
            DJV_NON_COPYABLE(LogToolBar);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            LogToolBar();

        public:
            ~LogToolBar() override;

            static std::shared_ptr<LogToolBar> create(const std::shared_ptr<System::Context>&);

            //! \name Callbacks
            ///@{

            void setCopyCallback(const std::function<void(void)>&);
            void setReloadCallback(const std::function<void(void)>&);
            void setClearCallback(const std::function<void(void)>&);
            void setFilterCallback(const std::function<void(const std::string&)>&);

            ///@}

        protected:
            void _initEvent(System::Event::Init&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

