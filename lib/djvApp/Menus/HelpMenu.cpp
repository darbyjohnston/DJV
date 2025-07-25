// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Menus/HelpMenu.h>

#include <djvApp/Actions/HelpActions.h>

namespace djv
{
    namespace app
    {
        void HelpMenu::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<HelpActions>& helpActions,
            const std::shared_ptr<IWidget>& parent)
        {
            Menu::_init(context, parent);

            auto actions = helpActions->getActions();
            addAction(actions["Documentation"]);
            addAction(actions["About"]);
        }

        HelpMenu::~HelpMenu()
        {}

        std::shared_ptr<HelpMenu> HelpMenu::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<HelpActions>& actions,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<HelpMenu>(new HelpMenu);
            out->_init(context, actions, parent);
            return out;
        }
    }
}
