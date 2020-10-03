// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Window.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the new user experience widget.
        class NUXWidget : public UI::Window
        {
            DJV_NON_COPYABLE(NUXWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            NUXWidget();

        public:
            static std::shared_ptr<NUXWidget> create(const std::shared_ptr<System::Context>&);

            void setFinishCallback(const std::function<void(void)>&);

        protected:
            void _initEvent(System::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

