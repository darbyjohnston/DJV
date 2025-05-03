// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <tlTimeline/Player.h>

#include <dtk/ui/Menu.h>

namespace djv
{
    namespace app
    {
        class FrameActions;

        //! Frame menu.
        class FrameMenu : public dtk::Menu
        {
            DTK_NON_COPYABLE(FrameMenu);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<FrameActions>&,
                const std::shared_ptr<IWidget>& parent);

            FrameMenu() = default;

        public:
            ~FrameMenu();

            static std::shared_ptr<FrameMenu> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<FrameActions>&,
                const std::shared_ptr<IWidget>& parent = nullptr);
        };
    }
}

