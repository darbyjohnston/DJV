// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <feather-tk/ui/Menu.h>

namespace djv
{
    namespace app
    {
        class ColorActions;

        //! Color menu.
        class ColorMenu : public feather_tk::Menu
        {
            FEATHER_TK_NON_COPYABLE(ColorMenu);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<ColorActions>&,
                const std::shared_ptr<IWidget>& parent);

            ColorMenu() = default;

        public:
            ~ColorMenu();

            static std::shared_ptr<ColorMenu> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<ColorActions>&,
                const std::shared_ptr<IWidget>& parent = nullptr);
        };
    }
}
