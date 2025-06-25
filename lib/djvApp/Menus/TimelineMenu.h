// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <feather-tk/ui/Menu.h>

namespace djv
{
    namespace app
    {
        class TimelineActions;

        //! Timeline menu.
        class TimelineMenu : public feather_tk::Menu
        {
            FEATHER_TK_NON_COPYABLE(TimelineMenu);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<TimelineActions>&,
                const std::shared_ptr<IWidget>& parent);

            TimelineMenu();

        public:
            ~TimelineMenu();

            static std::shared_ptr<TimelineMenu> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<TimelineActions>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

        private:
            FEATHER_TK_PRIVATE();
        };
    }
}
