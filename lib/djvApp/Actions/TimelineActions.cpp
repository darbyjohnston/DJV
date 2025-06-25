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
            std::map<int, std::shared_ptr<feather_tk::Action> > thumbnailsSizeItems;

            std::shared_ptr<feather_tk::ValueObserver<bool> > frameViewObserver;
            std::shared_ptr<feather_tk::ValueObserver<bool> > scrollBarsObserver;
            std::shared_ptr<feather_tk::ValueObserver<bool> > autoScrollObserver;
            std::shared_ptr<feather_tk::ValueObserver<bool> > stopOnScrubObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timelineui::DisplayOptions> > displayOptionsObserver;
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

            _actions["Thumbnails"] = feather_tk::Action::create(
                "Thumbnails",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto settings = app->getSettingsModel()->getTimeline();
                        settings.display.thumbnails = value;
                        app->getSettingsModel()->setTimeline(settings);
                    }
                });

            _actions["ThumbnailsSmall"] = feather_tk::Action::create(
                "Small",
                [appWeak]
                {
                    if (auto app = appWeak.lock())
                    {
                        auto settings = app->getSettingsModel()->getTimeline();
                        settings.display.thumbnailHeight = 100;
                        settings.display.waveformHeight = settings.display.thumbnailHeight / 2;
                        app->getSettingsModel()->setTimeline(settings);
                    }
                });

            _actions["ThumbnailsMedium"] = feather_tk::Action::create(
                "Medium",
                [appWeak]
                {
                    if (auto app = appWeak.lock())
                    {
                        auto settings = app->getSettingsModel()->getTimeline();
                        settings.display.thumbnailHeight = 200;
                        settings.display.waveformHeight = settings.display.thumbnailHeight / 2;
                        app->getSettingsModel()->setTimeline(settings);
                    }
                });

            _actions["ThumbnailsLarge"] = feather_tk::Action::create(
                "Large",
                [appWeak]
                {
                    if (auto app = appWeak.lock())
                    {
                        auto settings = app->getSettingsModel()->getTimeline();
                        settings.display.thumbnailHeight = 300;
                        settings.display.waveformHeight = settings.display.thumbnailHeight / 2;
                        app->getSettingsModel()->setTimeline(settings);
                    }
                });

            _tooltips =
            {
                { "FrameView", "Frame the timeline view." },
                { "ScrollBars", "Toggle the scroll bars." },
                { "AutoScroll", "Automatically scroll the timeline to the current frame." },
                { "StopOnScrub", "Stop playback when scrubbing the timeline." },
                { "Thumbnails", "Toggle timeline thumbnails." },
                { "ThumbnailsSmall", "Small timeline thumbnails." },
                { "ThumbnailsMedium", "Medium timeline thumbnails." },
                { "ThumbnailsLarge", "Large timeline thumbnails." }
            };

            p.thumbnailsSizeItems[100] = _actions["ThumbnailsSmall"];
            p.thumbnailsSizeItems[200] = _actions["ThumbnailsMedium"];
            p.thumbnailsSizeItems[300] = _actions["ThumbnailsLarge"];

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
                    _actions["Thumbnails"]->setChecked(value.thumbnails);
                    _thumbnailsSizeUpdate();
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

        void TimelineActions::_thumbnailsSizeUpdate()
        {
            FEATHER_TK_P();
            if (auto mainWindow = p.mainWindow.lock())
            {
                const auto options = mainWindow->getTimelineWidget()->getDisplayOptions();
                auto i = p.thumbnailsSizeItems.find(options.thumbnailHeight);
                if (i == p.thumbnailsSizeItems.end())
                {
                    i = p.thumbnailsSizeItems.begin();
                }
                for (auto item : p.thumbnailsSizeItems)
                {
                    const bool checked = item == *i;
                    item.second->setChecked(checked);
                }
            }
        }
    }
}
