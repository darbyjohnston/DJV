// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <tlTimeline/Player.h>

#include <feather-tk/ui/Menu.h>

namespace djv
{
    namespace app
    {
        class PlaybackActions;

        //! Playback menu.
        class PlaybackMenu : public feather_tk::Menu
        {
            FEATHER_TK_NON_COPYABLE(PlaybackMenu);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<PlaybackActions>&,
                const std::shared_ptr<IWidget>& parent);

            PlaybackMenu() = default;

        public:
            ~PlaybackMenu();

            static std::shared_ptr<PlaybackMenu> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<PlaybackActions>&,
                const std::shared_ptr<IWidget>& parent = nullptr);
        };
    }
}
