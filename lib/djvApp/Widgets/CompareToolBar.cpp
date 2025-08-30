// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Widgets/CompareToolBar.h>

#include <tlTimeline/CompareOptions.h>

#include <feather-tk/ui/RowLayout.h>
#include <feather-tk/ui/ToolButton.h>

namespace djv
{
    namespace app
    {
        void CompareToolBar::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::map<std::string, std::shared_ptr<ftk::Action> >& actions,
            const std::shared_ptr<IWidget>& parent)
        {
            ToolBar::_init(context, ftk::Orientation::Horizontal, parent);

            auto tmp = actions;
            const auto labels = tl::timeline::getCompareLabels();
            for (size_t i = 0; i < labels.size(); ++i)
            {
                addAction(tmp[labels[i]]);
            }
        }

        CompareToolBar::~CompareToolBar()
        {}

        std::shared_ptr<CompareToolBar> CompareToolBar::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::map<std::string, std::shared_ptr<ftk::Action> >& actions,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<CompareToolBar>(new CompareToolBar);
            out->_init(context, actions, parent);
            return out;
        }
    }
}
