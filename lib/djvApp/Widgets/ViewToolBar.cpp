// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Widgets/ViewToolBar.h>

#include <djvApp/Actions/ViewActions.h>

namespace djv
{
    namespace app
    {
        void ViewToolBar::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<ViewActions>& viewActions,
            const std::shared_ptr<IWidget>& parent)
        {
            ToolBar::_init(context, dtk::Orientation::Horizontal, parent);

            auto actions = viewActions->getActions();
            addAction(actions["Frame"]);
            addAction(actions["ZoomReset"]);
        }

        ViewToolBar::~ViewToolBar()
        {}

        std::shared_ptr<ViewToolBar> ViewToolBar::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<ViewActions>& viewActions,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<ViewToolBar>(new ViewToolBar);
            out->_init(context, viewActions, parent);
            return out;
        }
    }
}
