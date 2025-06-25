// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Menus/TimelineMenu.h>

#include <djvApp/Actions/TimelineActions.h>
#include <djvApp/App.h>
#include <djvApp/MainWindow.h>

#include <tlTimelineUI/TimelineWidget.h>

namespace djv
{
    namespace app
    {
        struct TimelineMenu::Private
        {
            std::map<std::string, std::shared_ptr<feather_tk::Menu> > menus;
        };

        void TimelineMenu::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<TimelineActions>& timelineActions,
            const std::shared_ptr<IWidget>& parent)
        {
            Menu::_init(context, parent);
            FEATHER_TK_P();

            auto actions = timelineActions->getActions();
            addAction(actions["FrameView"]);
            addAction(actions["ScrollBars"]);
            addAction(actions["AutoScroll"]);
            addAction(actions["StopOnScrub"]);
            addAction(actions["Thumbnails"]);
            p.menus["ThumbnailSize"] = addSubMenu("Thumbnails Size");
            p.menus["ThumbnailSize"]->addAction(actions["ThumbnailsSmall"]);
            p.menus["ThumbnailSize"]->addAction(actions["ThumbnailsMedium"]);
            p.menus["ThumbnailSize"]->addAction(actions["ThumbnailsLarge"]);
        }

        TimelineMenu::TimelineMenu() :
            _p(new Private)
        {}

        TimelineMenu::~TimelineMenu()
        {}

        std::shared_ptr<TimelineMenu> TimelineMenu::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<TimelineActions>& timelineActions,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<TimelineMenu>(new TimelineMenu);
            out->_init(context, timelineActions, parent);
            return out;
        }
    }
}
