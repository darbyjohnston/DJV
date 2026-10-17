// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the DJV project.

#include <djv/App/CompareToolBar.h>

#include <tlRender/Timeline/CompareOptions.h>

#include <ftk/UI/ChoiceToolButton.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/ScreenshotTag.h>
#include <ftk/UI/ToolButton.h>

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

            setMarginRole(ftk::SizeRole::MarginInside);

            // The compare modes are one choice, so they take one place: a
            // button for the mode chosen last, on or off, and an arrow for
            // the list. Eight buttons were most of a row that is short of
            // room on a tablet.
            auto tmp = actions;
            const auto enums = tl::getCompareEnums();
            const auto labels = tl::getCompareLabels();
            std::vector<std::shared_ptr<ftk::Action> > modes;
            for (size_t i = 0; i < labels.size(); ++i)
            {
                // Not comparing is the state with none of them on, so it has
                // no entry. Asking the map for one would add an empty one.
                if (tl::Compare::None == enums[i])
                    continue;
                modes.push_back(tmp[labels[i]]);
            }
            auto button = ftk::ChoiceToolButton::create(context, modes);
            button->getMenuButton()->setTooltip("Choose the compare mode");
            ftk::setScreenshotTag(button->getButton(), "Compare.Mode");
            ftk::setScreenshotTag(button->getMenuButton(), "Compare.ModeMenu");
            addWidget(button);
            for (const auto& action : modes)
            {
                addOverflowAction(action);
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
