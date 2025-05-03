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
            std::map<int, std::shared_ptr<dtk::Action> > thumbnailsSizeItems;

            std::shared_ptr<dtk::ValueObserver<bool> > frameViewObserver;
            std::shared_ptr<dtk::ValueObserver<bool> > scrollToCurrentFrameObserver;
            std::shared_ptr<dtk::ValueObserver<bool> > stopOnScrubObserver;
            std::shared_ptr<dtk::ValueObserver<tl::timelineui::DisplayOptions> > displayOptionsObserver;
        };

        void TimelineActions::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow)
        {
            IActions::_init(context, app, "Timeline");
            DTK_P();

            p.mainWindow = mainWindow;

            auto appWeak = std::weak_ptr<App>(app);
            _actions["FrameView"] = dtk::Action::create(
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

            _actions["Scroll"] = dtk::Action::create(
                "Scroll To Current Frame",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto settings = app->getSettingsModel()->getTimeline();
                        settings.scroll = value;
                        app->getSettingsModel()->setTimeline(settings);
                    }
                });

            _actions["StopOnScrub"] = dtk::Action::create(
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

            _actions["Thumbnails"] = dtk::Action::create(
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

            _actions["ThumbnailsSmall"] = dtk::Action::create(
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

            _actions["ThumbnailsMedium"] = dtk::Action::create(
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

            _actions["ThumbnailsLarge"] = dtk::Action::create(
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
                { "Scroll", "Scroll the timeline view to the current frame." },
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

            p.frameViewObserver = dtk::ValueObserver<bool>::create(
                mainWindow->getTimelineWidget()->observeFrameView(),
                [this](bool value)
                {
                    _actions["FrameView"]->setChecked(value);
                });

            p.scrollToCurrentFrameObserver = dtk::ValueObserver<bool>::create(
                mainWindow->getTimelineWidget()->observeScrollToCurrentFrame(),
                [this](bool value)
                {
                    _actions["Scroll"]->setChecked(value);
                });

            p.stopOnScrubObserver = dtk::ValueObserver<bool>::create(
                mainWindow->getTimelineWidget()->observeStopOnScrub(),
                [this](bool value)
                {
                    _actions["StopOnScrub"]->setChecked(value);
                });

            p.displayOptionsObserver = dtk::ValueObserver<tl::timelineui::DisplayOptions>::create(
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
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow)
        {
            auto out = std::shared_ptr<TimelineActions>(new TimelineActions);
            out->_init(context, app, mainWindow);
            return out;
        }

        void TimelineActions::_thumbnailsSizeUpdate()
        {
            DTK_P();
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
