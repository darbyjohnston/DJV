// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/ViewApp.h>

#include <djvUI/Widget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the window background widget.
        class WindowBackgroundWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(WindowBackgroundWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            WindowBackgroundWidget();

        public:
            ~WindowBackgroundWidget() override;

            static std::shared_ptr<WindowBackgroundWidget> create(const std::shared_ptr<System::Context>&);

        protected:
            void _paintEvent(System::Event::Paint &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

