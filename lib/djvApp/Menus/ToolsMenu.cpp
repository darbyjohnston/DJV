// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the DJV project.

#include <djvApp/Menus/ToolsMenu.h>

#include <djvApp/Actions/ToolsActions.h>
#include <djvApp/Models/ToolsModel.h>
#include <djvApp/App.h>

namespace djv
{
    namespace app
    {
        void ToolsMenu::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ToolsActions>& toolsActions,
            const std::shared_ptr<IWidget>& parent)
        {
            Menu::_init(context, parent);

            const auto labels = getToolLabels();
            auto actions = toolsActions->getActions();
            for (size_t i = 1; i < labels.size(); ++i)
            {
                addAction(actions[labels[i]]);
            }
        }

        ToolsMenu::~ToolsMenu()
        {}

        std::shared_ptr<ToolsMenu> ToolsMenu::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ToolsActions>& toolsActions,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<ToolsMenu>(new ToolsMenu);
            out->_init(context, toolsActions, parent);
            return out;
        }
    }
}
