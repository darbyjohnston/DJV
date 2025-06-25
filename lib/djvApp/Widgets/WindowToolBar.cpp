// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Widgets/WindowToolBar.h>

namespace djv
{
    namespace app
    {
        void WindowToolBar::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::map<std::string, std::shared_ptr<feather_tk::Action> >& actions,
            const std::shared_ptr<IWidget>& parent)
        {
            ToolBar::_init(context, feather_tk::Orientation::Horizontal, parent);

            auto tmp = actions;
            addAction(tmp["FullScreen"]);
            addAction(tmp["Secondary"]);
        }

        WindowToolBar::~WindowToolBar()
        {}

        std::shared_ptr<WindowToolBar> WindowToolBar::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::map<std::string, std::shared_ptr<feather_tk::Action> >& actions,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<WindowToolBar>(new WindowToolBar);
            out->_init(context, actions, parent);
            return out;
        }
    }
}
