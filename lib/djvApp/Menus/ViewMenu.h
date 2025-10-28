// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <ftk/UI/Menu.h>

namespace djv
{
    namespace app
    {
        class ViewActions;

        //! View menu.
        class ViewMenu : public ftk::Menu
        {
            FTK_NON_COPYABLE(ViewMenu);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ViewActions>&,
                const std::shared_ptr<IWidget>& parent);

            ViewMenu() = default;

        public:
            ~ViewMenu();

            static std::shared_ptr<ViewMenu> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ViewActions>&,
                const std::shared_ptr<IWidget>& parent = nullptr);
        };
    }
}
