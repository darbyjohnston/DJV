// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <dtk/ui/Menu.h>

namespace djv
{
    namespace app
    {
        class AudioActions;

        //! Audio menu.
        class AudioMenu : public dtk::Menu
        {
            DTK_NON_COPYABLE(AudioMenu);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<AudioActions>&,
                const std::shared_ptr<IWidget>& parent);

            AudioMenu() = default;

        public:
            ~AudioMenu();

            static std::shared_ptr<AudioMenu> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<AudioActions>&,
                const std::shared_ptr<IWidget>& parent = nullptr);
        };
    }
}
