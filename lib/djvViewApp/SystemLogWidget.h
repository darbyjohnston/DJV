// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the system log widget.
        class SystemLogWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(SystemLogWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            SystemLogWidget();

        public:
            ~SystemLogWidget() override;

            static std::shared_ptr<SystemLogWidget> create(const std::shared_ptr<System::Context>&);
            
            void copyLog();
            void reloadLog();
            void clearLog();
            void setFilter(const std::string&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        //! This class provides a foorter for the system log widget.
        class SystemLogFooterWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(SystemLogFooterWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            SystemLogFooterWidget();

        public:
            ~SystemLogFooterWidget() override;

            static std::shared_ptr<SystemLogFooterWidget> create(const std::shared_ptr<System::Context>&);

            //! \name Callbacks
            ///@{

            void setCopyCallback(const std::function<void(void)>&);
            void setReloadCallback(const std::function<void(void)>&);
            void setClearCallback(const std::function<void(void)>&);
            void setFilterCallback(const std::function<void(const std::string&)>&);

            ///@}

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

