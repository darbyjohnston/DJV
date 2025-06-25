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

            std::shared_ptr<feather_tk::ValueObserver<bool> > frameViewObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::DisplayOptions> > displayOptionsObserver;
            std::shared_ptr<feather_tk::ValueObserver<bool> > hudObserver;
        };

        void ViewActions::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow)
        {
            IActions::_init(context, app, "View");
            FEATHER_TK_P();

            auto mainWindowWeak = std::weak_ptr<MainWindow>(mainWindow);
            _actions["Frame"] = feather_tk::Action::create(
                "Frame",
                "ViewFrame",
                [mainWindowWeak](bool value)
                {
                    if (auto mainWindow = mainWindowWeak.lock())
                    {
                        mainWindow->getViewport()->setFrameView(value);
                    }
                });

            _actions["ZoomReset"] = feather_tk::Action::create(
                "Zoom Reset",
                "ViewZoomReset",
                [mainWindowWeak]
                {
                    if (auto mainWindow = mainWindowWeak.lock())
                    {
                        mainWindow->getViewport()->viewZoomReset();
                    }
                });

            _actions["ZoomIn"] = feather_tk::Action::create(
                "Zoom In",
                "ViewZoomIn",
                [mainWindowWeak]
                {
                    if (auto mainWindow = mainWindowWeak.lock())
                    {
                        mainWindow->getViewport()->viewZoomIn();
                    }
                });

            _actions["ZoomOut"] = feather_tk::Action::create(
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
            _actions["Red"] = feather_tk::Action::create(
                "Red Channel",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto displayOptions = app->getViewportModel()->getDisplayOptions();
                        displayOptions.channels = value ?
                            feather_tk::ChannelDisplay::Red :
                            feather_tk::ChannelDisplay::Color;
                        app->getViewportModel()->setDisplayOptions(displayOptions);
                    }
                });

            _actions["Green"] = feather_tk::Action::create(
                "Green Channel",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto displayOptions = app->getViewportModel()->getDisplayOptions();
                        displayOptions.channels = value ?
                            feather_tk::ChannelDisplay::Green :
                            feather_tk::ChannelDisplay::Color;
                        app->getViewportModel()->setDisplayOptions(displayOptions);
                    }
                });

            _actions["Blue"] = feather_tk::Action::create(
                "Blue Channel",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto displayOptions = app->getViewportModel()->getDisplayOptions();
                        displayOptions.channels = value ?
                            feather_tk::ChannelDisplay::Blue :
                            feather_tk::ChannelDisplay::Color;
                        app->getViewportModel()->setDisplayOptions(displayOptions);
                    }
                });

            _actions["Alpha"] = feather_tk::Action::create(
                "Alpha Channel",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto displayOptions = app->getViewportModel()->getDisplayOptions();
                        displayOptions.channels = value ?
                            feather_tk::ChannelDisplay::Alpha :
                            feather_tk::ChannelDisplay::Color;
                        app->getViewportModel()->setDisplayOptions(displayOptions);
                    }
                });

            _actions["MirrorHorizontal"] = feather_tk::Action::create(
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

            _actions["MirrorVertical"] = feather_tk::Action::create(
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

            _actions["HUD"] = feather_tk::Action::create(
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

            p.frameViewObserver = feather_tk::ValueObserver<bool>::create(
                mainWindow->getViewport()->observeFrameView(),
                [this](bool value)
                {
                    _actions["Frame"]->setChecked(value);
                });

            p.displayOptionsObserver = feather_tk::ValueObserver<tl::timeline::DisplayOptions>::create(
                app->getViewportModel()->observeDisplayOptions(),
                [this](const tl::timeline::DisplayOptions& value)
                {
                    _actions["Red"]->setChecked(feather_tk::ChannelDisplay::Red == value.channels);
                    _actions["Green"]->setChecked(feather_tk::ChannelDisplay::Green == value.channels);
                    _actions["Blue"]->setChecked(feather_tk::ChannelDisplay::Blue == value.channels);
                    _actions["Alpha"]->setChecked(feather_tk::ChannelDisplay::Alpha == value.channels);

                    _actions["MirrorHorizontal"]->setChecked(value.mirror.x);
                    _actions["MirrorVertical"]->setChecked(value.mirror.y);
                });

            p.hudObserver = feather_tk::ValueObserver<bool>::create(
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
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow)
        {
            auto out = std::shared_ptr<ViewActions>(new ViewActions);
            out->_init(context, app, mainWindow);
            return out;
        }
    }
}
