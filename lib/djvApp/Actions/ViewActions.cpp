// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Actions/ViewActions.h>

#include <djvApp/Models/ViewportModel.h>
#include <djvApp/Widgets/Viewport.h>
#include <djvApp/App.h>
#include <djvApp/MainWindow.h>

#include <sstream>

namespace djv
{
    namespace app
    {
        struct ViewActions::Private
        {

            std::shared_ptr<dtk::ValueObserver<bool> > frameViewObserver;
            std::shared_ptr<dtk::ValueObserver<tl::timeline::DisplayOptions> > displayOptionsObserver;
            std::shared_ptr<dtk::ValueObserver<bool> > hudObserver;
        };

        void ViewActions::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow)
        {
            IActions::_init(context, app, "View");
            DTK_P();

            auto mainWindowWeak = std::weak_ptr<MainWindow>(mainWindow);
            _actions["Frame"] = dtk::Action::create(
                "Frame",
                "ViewFrame",
                [mainWindowWeak](bool value)
                {
                    if (auto mainWindow = mainWindowWeak.lock())
                    {
                        mainWindow->getViewport()->setFrameView(value);
                    }
                });

            _actions["ZoomReset"] = dtk::Action::create(
                "Zoom Reset",
                "ViewZoomReset",
                [mainWindowWeak]
                {
                    if (auto mainWindow = mainWindowWeak.lock())
                    {
                        mainWindow->getViewport()->viewZoomReset();
                    }
                });

            _actions["ZoomIn"] = dtk::Action::create(
                "Zoom In",
                "ViewZoomIn",
                [mainWindowWeak]
                {
                    if (auto mainWindow = mainWindowWeak.lock())
                    {
                        mainWindow->getViewport()->viewZoomIn();
                    }
                });

            _actions["ZoomOut"] = dtk::Action::create(
                "Zoom Out",
                "ViewZoomOut",
                [mainWindowWeak]
                {
                    if (auto mainWindow = mainWindowWeak.lock())
                    {
                        mainWindow->getViewport()->viewZoomOut();
                    }
                });

            auto appWeak = std::weak_ptr<App>(app);
            _actions["Red"] = dtk::Action::create(
                "Red Channel",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto displayOptions = app->getViewportModel()->getDisplayOptions();
                        displayOptions.channels = value ?
                            dtk::ChannelDisplay::Red :
                            dtk::ChannelDisplay::Color;
                        app->getViewportModel()->setDisplayOptions(displayOptions);
                    }
                });

            _actions["Green"] = dtk::Action::create(
                "Green Channel",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto displayOptions = app->getViewportModel()->getDisplayOptions();
                        displayOptions.channels = value ?
                            dtk::ChannelDisplay::Green :
                            dtk::ChannelDisplay::Color;
                        app->getViewportModel()->setDisplayOptions(displayOptions);
                    }
                });

            _actions["Blue"] = dtk::Action::create(
                "Blue Channel",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto displayOptions = app->getViewportModel()->getDisplayOptions();
                        displayOptions.channels = value ?
                            dtk::ChannelDisplay::Blue :
                            dtk::ChannelDisplay::Color;
                        app->getViewportModel()->setDisplayOptions(displayOptions);
                    }
                });

            _actions["Alpha"] = dtk::Action::create(
                "Alpha Channel",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto displayOptions = app->getViewportModel()->getDisplayOptions();
                        displayOptions.channels = value ?
                            dtk::ChannelDisplay::Alpha :
                            dtk::ChannelDisplay::Color;
                        app->getViewportModel()->setDisplayOptions(displayOptions);
                    }
                });

            _actions["MirrorHorizontal"] = dtk::Action::create(
                "Mirror Horizontal",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto displayOptions = app->getViewportModel()->getDisplayOptions();
                        displayOptions.mirror.x = value;
                        app->getViewportModel()->setDisplayOptions(displayOptions);
                    }
                });

            _actions["MirrorVertical"] = dtk::Action::create(
                "Mirror Vertical",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto displayOptions = app->getViewportModel()->getDisplayOptions();
                        displayOptions.mirror.y = value;
                        app->getViewportModel()->setDisplayOptions(displayOptions);
                    }
                });

            _actions["HUD"] = dtk::Action::create(
                "HUD",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getViewportModel()->setHUD(value);
                    }
                });

            _tooltips =
            {
                { "Frame",  "Frame the view to fit the window." },
                { "ZoomReset", "Reset the view zoom to 1:1." },
                { "ZoomIn", "Zoom the view in." },
                { "ZoomOut", "Zoom the view out." },
                { "HUD", "Toggle the HUD (Heads Up Display)." }
            };

            _shortcutsUpdate(app->getSettingsModel()->getShortcuts());

            p.frameViewObserver = dtk::ValueObserver<bool>::create(
                mainWindow->getViewport()->observeFrameView(),
                [this](bool value)
                {
                    _actions["Frame"]->setChecked(value);
                });

            p.displayOptionsObserver = dtk::ValueObserver<tl::timeline::DisplayOptions>::create(
                app->getViewportModel()->observeDisplayOptions(),
                [this](const tl::timeline::DisplayOptions& value)
                {
                    _actions["Red"]->setChecked(dtk::ChannelDisplay::Red == value.channels);
                    _actions["Green"]->setChecked(dtk::ChannelDisplay::Green == value.channels);
                    _actions["Blue"]->setChecked(dtk::ChannelDisplay::Blue == value.channels);
                    _actions["Alpha"]->setChecked(dtk::ChannelDisplay::Alpha == value.channels);

                    _actions["MirrorHorizontal"]->setChecked(value.mirror.x);
                    _actions["MirrorVertical"]->setChecked(value.mirror.y);
                });

            p.hudObserver = dtk::ValueObserver<bool>::create(
                app->getViewportModel()->observeHUD(),
                [this](bool value)
                {
                    _actions["HUD"]->setChecked(value);
                });
        }

        ViewActions::ViewActions() :
            _p(new Private)
        {}

        ViewActions::~ViewActions()
        {}

        std::shared_ptr<ViewActions> ViewActions::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow)
        {
            auto out = std::shared_ptr<ViewActions>(new ViewActions);
            out->_init(context, app, mainWindow);
            return out;
        }
    }
}
