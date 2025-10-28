// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <ftk/UI/Menu.h>

namespace djv
{
    namespace app
    {
        class ToolsActions;

        //! Tools menu.
        class ToolsMenu : public ftk::Menu
        {
            FTK_NON_COPYABLE(ToolsMenu);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ToolsActions>&,
                const std::shared_ptr<IWidget>& parent);

            ToolsMenu() = default;

        public:
            ~ToolsMenu();

            static std::shared_ptr<ToolsMenu> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ToolsActions>&,
                const std::shared_ptr<IWidget>& parent = nullptr);
        };
    }
}
