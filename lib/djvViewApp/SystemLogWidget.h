// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/MDIWidget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the system log widget.
        class SystemLogWidget : public MDIWidget
        {
            DJV_NON_COPYABLE(SystemLogWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            SystemLogWidget();

        public:
            ~SystemLogWidget() override;

            static std::shared_ptr<SystemLogWidget> create(const std::shared_ptr<System::Context>&);

            void reloadLog();
            void clearLog();

        protected:
            void _initEvent(System::Event::Init &) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

