// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <feather-tk/ui/Menu.h>

namespace djv
{
    namespace app
    {
        class ViewActions;

        //! View menu.
        class ViewMenu : public feather_tk::Menu
        {
            FEATHER_TK_NON_COPYABLE(ViewMenu);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<ViewActions>&,
                const std::shared_ptr<IWidget>& parent);

            ViewMenu() = default;

        public:
            ~ViewMenu();

            static std::shared_ptr<ViewMenu> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<ViewActions>&,
                const std::shared_ptr<IWidget>& parent = nullptr);
        };
    }
}
