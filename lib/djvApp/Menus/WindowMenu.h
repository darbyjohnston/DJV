// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <feather-tk/ui/Menu.h>

namespace djv
{
    namespace app
    {
        class MainWindow;
        class WindowActions;

        //! Window menu.
        class WindowMenu : public feather_tk::Menu
        {
            FEATHER_TK_NON_COPYABLE(WindowMenu);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<MainWindow>&,
                const std::shared_ptr<WindowActions>&,
                const std::shared_ptr<IWidget>& parent);

            WindowMenu();

        public:
            ~WindowMenu();

            static std::shared_ptr<WindowMenu> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<MainWindow>&,
                const std::shared_ptr<WindowActions>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

        private:
            FEATHER_TK_PRIVATE();
        };
    }
}
