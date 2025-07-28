// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Actions/TimelineActions.h>

#include <djvApp/App.h>
#include <djvApp/MainWindow.h>

#include <tlTimelineUI/TimelineWidget.h>

namespace djv
{
    namespace app
    {
        struct TimelineActions::Private
        {
            std::weak_ptr<MainWindow> mainWindow;

            std::shared_ptr<feather_tk::ValueObserver<bool> > frameViewObserver;
            std::shared_ptr<feather_tk::ValueObserver<bool> > scrollBarsObserver;
            std::shared_ptr<feather_tk::ValueObserver<bool> > autoScrollObserver;
            std::shared_ptr<feather_tk::ValueObserver<bool> > stopOnScrubObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timelineui::DisplayOptions> > displayOptionsObserver;
            std::shared_ptr<feather_tk::ValueObserver<TimelineSettings> > settingsObserver;
        };

        void TimelineActions::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow)
        {
            IActions::_init(context, app, "Timeline");
            FEATHER_TK_P();

            p.mainWindow = mainWindow;

            auto appWeak = std::weak_ptr<App>(app);
            _actions["Minimize"] = feather_tk::Action::create(
                "Minimize Timeline",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto settings = app->getSettingsModel()->getTimeline();
                        settings.minimize = value;
                        app->getSettingsModel()->setTimeline(settings);
                    }
                });

            _actions["FrameView"] = feather_tk::Action::create(
                "Frame Timeline View",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto settings = app->getSettingsModel()->getTimeline();
                        settings.frameView = value;
                        app->getSettingsModel()->setTimeline(settings);
                    }
                });

            _actions["ScrollBars"] = feather_tk::Action::create(
                "Scroll Bars",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto settings = app->getSettingsModel()->getTimeline();
                        settings.scrollBars = value;
                        app->getSettingsModel()->setTimeline(settings);
                    }
                });

            _actions["AutoScroll"] = feather_tk::Action::create(
                "Auto Scroll",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto settings = app->getSettingsModel()->getTimeline();
                        settings.autoScroll = value;
                        app->getSettingsModel()->setTimeline(settings);
                    }
                });

            _actions["StopOnScrub"] = feather_tk::Action::create(
                "Stop Playback When Scrubbing",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto settings = app->getSettingsModel()->getTimeline();
                        settings.stopOnScrub = value;
                        app->getSettingsModel()->setTimeline(settings);
                    }
                });

            _actions["ThumbnailsNone"] = feather_tk::Action::create(
                "No Thumbnails",
                [appWeak]
                {
                    if (auto app = appWeak.lock())
                    {
                        auto settings = app->getSettingsModel()->getTimeline();
                        settings.thumbnails = TimelineThumbnails::None;
                        app->getSettingsModel()->setTimeline(settings);
                    }
                });

            _actions["ThumbnailsSmall"] = feather_tk::Action::create(
                "Small Thumbnails",
                [appWeak]
                {
                    if (auto app = appWeak.lock())
                    {
                        auto settings = app->getSettingsModel()->getTimeline();
                        settings.thumbnails = TimelineThumbnails::Small;
                        app->getSettingsModel()->setTimeline(settings);
                    }
                });

            _actions["ThumbnailsMedium"] = feather_tk::Action::create(
                "Medium Thumbnails",
                [appWeak]
                {
                    if (auto app = appWeak.lock())
                    {
                        auto settings = app->getSettingsModel()->getTimeline();
                        settings.thumbnails = TimelineThumbnails::Medium;
                        app->getSettingsModel()->setTimeline(settings);
                    }
                });

            _actions["ThumbnailsLarge"] = feather_tk::Action::create(
                "Large Thumbnails",
                [appWeak]
                {
                    if (auto app = appWeak.lock())
                    {
                        auto settings = app->getSettingsModel()->getTimeline();
                        settings.thumbnails = TimelineThumbnails::Large;
                        app->getSettingsModel()->setTimeline(settings);
                    }
                });

            _tooltips =
            {
                { "Minimize", "Minimize the timeline." },
                { "FrameView", "Frame the timeline view." },
                { "ScrollBars", "Toggle the scroll bars." },
                { "AutoScroll", "Automatically scroll the timeline to the current frame." },
                { "StopOnScrub", "Stop playback when scrubbing the timeline." },
                { "Thumbnails", "Toggle timeline thumbnails." },
                { "ThumbnailsSmall", "Small timeline thumbnails." },
                { "ThumbnailsMedium", "Medium timeline thumbnails." },
                { "ThumbnailsLarge", "Large timeline thumbnails." }
            };

            _shortcutsUpdate(app->getSettingsModel()->getShortcuts());

            p.frameViewObserver = feather_tk::ValueObserver<bool>::create(
                mainWindow->getTimelineWidget()->observeFrameView(),
                [this](bool value)
                {
                    _actions["FrameView"]->setChecked(value);
                });

            p.autoScrollObserver = feather_tk::ValueObserver<bool>::create(
                mainWindow->getTimelineWidget()->observeAutoScroll(),
                [this](bool value)
                {
                    _actions["AutoScroll"]->setChecked(value);
                });

            p.scrollBarsObserver = feather_tk::ValueObserver<bool>::create(
                mainWindow->getTimelineWidget()->observeScrollBarsVisible(),
                [this](bool value)
                {
                    _actions["ScrollBars"]->setChecked(value);
                });

            p.stopOnScrubObserver = feather_tk::ValueObserver<bool>::create(
                mainWindow->getTimelineWidget()->observeStopOnScrub(),
                [this](bool value)
                {
                    _actions["StopOnScrub"]->setChecked(value);
                });

            p.displayOptionsObserver = feather_tk::ValueObserver<tl::timelineui::DisplayOptions>::create(
                mainWindow->getTimelineWidget()->observeDisplayOptions(),
                [this](const tl::timelineui::DisplayOptions& value)
                {
                    std::map<int, TimelineThumbnails> sizeToThumbnails;
                    for (auto i : getTimelineThumbnailsEnums())
                    {
                        sizeToThumbnails[getTimelineThumbnailsSize(i)] = i;
                    }
                    auto j = sizeToThumbnails.find(value.thumbnailHeight);
                    _actions["ThumbnailsNone"]->setChecked(j != sizeToThumbnails.end() && TimelineThumbnails::None == j->second);
                    _actions["ThumbnailsSmall"]->setChecked(j != sizeToThumbnails.end() && TimelineThumbnails::Small == j->second);
                    _actions["ThumbnailsMedium"]->setChecked(j != sizeToThumbnails.end() && TimelineThumbnails::Medium == j->second);
                    _actions["ThumbnailsLarge"]->setChecked(j != sizeToThumbnails.end() && TimelineThumbnails::Large == j->second);
                });

            p.settingsObserver = feather_tk::ValueObserver<TimelineSettings>::create(
                app->getSettingsModel()->observeTimeline(),
                [this](const TimelineSettings& value)
                {
                    _actions["Minimize"]->setChecked(value.minimize);
                });
        }

        TimelineActions::TimelineActions() :
            _p(new Private)
        {}

        TimelineActions::~TimelineActions()
        {}

        std::shared_ptr<TimelineActions> TimelineActions::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow)
        {
            auto out = std::shared_ptr<TimelineActions>(new TimelineActions);
            out->_init(context, app, mainWindow);
            return out;
        }
    }
}
