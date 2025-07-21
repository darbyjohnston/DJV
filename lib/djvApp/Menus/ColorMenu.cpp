// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Menus/ColorMenu.h>

#include <djvApp/Actions/ColorActions.h>

namespace djv
{
    namespace app
    {
        void ColorMenu::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<ColorActions>& colorActions,
            const std::shared_ptr<IWidget>& parent)
        {
            Menu::_init(context, parent);

            auto actions = colorActions->getActions();
            addAction(actions["OCIO"]);
        }

        ColorMenu::~ColorMenu()
        {}

        std::shared_ptr<ColorMenu> ColorMenu::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<ColorActions>& actions,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<ColorMenu>(new ColorMenu);
            out->_init(context, actions, parent);
            return out;
        }
    }
}
