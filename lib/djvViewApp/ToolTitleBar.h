// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides a tool widget title bar.
        class ToolTitleBar : public UI::Widget
        {
            DJV_NON_COPYABLE(ToolTitleBar);

        protected:
            void _init(
                const std::string&,
                const std::shared_ptr<System::Context>&);
            ToolTitleBar();

        public:
            ~ToolTitleBar() override;

            static std::shared_ptr<ToolTitleBar> create(
                const std::string&,
                const std::shared_ptr<System::Context>&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

