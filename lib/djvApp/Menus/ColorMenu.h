// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <ftk/UI/Menu.h>

namespace djv
{
    namespace app
    {
        class ColorActions;

        //! Color menu.
        class ColorMenu : public ftk::Menu
        {
            FTK_NON_COPYABLE(ColorMenu);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ColorActions>&,
                const std::shared_ptr<IWidget>& parent);

            ColorMenu() = default;

        public:
            ~ColorMenu();

            static std::shared_ptr<ColorMenu> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ColorActions>&,
                const std::shared_ptr<IWidget>& parent = nullptr);
        };
    }
}
