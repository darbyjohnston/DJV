// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Actions/WindowActions.h>

#include <djvApp/App.h>
#include <djvApp/MainWindow.h>

namespace djv
{
    namespace app
    {
        struct WindowActions::Private
        {
            std::shared_ptr<feather_tk::ValueObserver<bool> > fullScreenObserver;
            std::shared_ptr<feather_tk::ValueObserver<bool> > floatOnTopObserver;
            std::shared_ptr<feather_tk::ValueObserver<bool> > secondaryObserver;
            std::shared_ptr<feather_tk::ValueObserver<WindowSettings> > settingsObserver;
        };

        void WindowActions::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow)
        {
            IActions::_init(context, app, "Window");
            FEATHER_TK_P();

            auto appWeak = std::weak_ptr<App>(app);
            _actions["FullScreen"] = feather_tk::Action::create(
                "Full Screen",
                "WindowFullScreen",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getMainWindow()->setFullScreen(value);
                    }
                });

            _actions["FloatOnTop"] = feather_tk::Action::create(
                "Float On Top",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getMainWindow()->setFloatOnTop(value);
                    }
                });

            _actions["Secondary"] = feather_tk::Action::create(
                "Secondary",
                "WindowSecondary",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->setSecondaryWindow(value);
                    }
                });

            _actions["FileToolBar"] = feather_tk::Action::create(
                "File Tool Bar",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getSettingsModel()->getWindow();
                        options.fileToolBar = value;
                        app->getSettingsModel()->setWindow(options);
                    }
                });

            _actions["CompareToolBar"] = feather_tk::Action::create(
                "Compare Tool Bar",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getSettingsModel()->getWindow();
                        options.compareToolBar = value;
                        app->getSettingsModel()->setWindow(options);
                    }
                });

            _actions["WindowToolBar"] = feather_tk::Action::create(
                "Window Tool Bar",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getSettingsModel()->getWindow();
                        options.windowToolBar = value;
                        app->getSettingsModel()->setWindow(options);
                    }
                });

            _actions["ViewToolBar"] = feather_tk::Action::create(
                "View Tool Bar",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getSettingsModel()->getWindow();
                        options.viewToolBar = value;
                        app->getSettingsModel()->setWindow(options);
                    }
                });

            _actions["ToolsToolBar"] = feather_tk::Action::create(
                "Tools Tool Bar",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getSettingsModel()->getWindow();
                        options.toolsToolBar = value;
                        app->getSettingsModel()->setWindow(options);
                    }
                });

            _actions["TabBar"] = feather_tk::Action::create(
                "Tab Bar",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getSettingsModel()->getWindow();
                        options.tabBar = value;
                        app->getSettingsModel()->setWindow(options);
                    }
                });

            _actions["Timeline"] = feather_tk::Action::create(
                "Timeline",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getSettingsModel()->getWindow();
                        options.timeline = value;
                        app->getSettingsModel()->setWindow(options);
                    }
                });

            _actions["BottomToolBar"] = feather_tk::Action::create(
                "Bottom Tool Bar",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getSettingsModel()->getWindow();
                        options.bottomToolBar = value;
                        app->getSettingsModel()->setWindow(options);
                    }
                });

            _actions["StatusToolBar"] = feather_tk::Action::create(
                "Status Tool Bar",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getSettingsModel()->getWindow();
                        options.statusToolBar = value;
                        app->getSettingsModel()->setWindow(options);
                    }
                });

            _tooltips =
            {
                { "FullScreen", "Toggle the window full screen." },
                { "Secondary", "Toggle the secondary window." }
            };

            _shortcutsUpdate(app->getSettingsModel()->getShortcuts());

            p.fullScreenObserver = feather_tk::ValueObserver<bool>::create(
                mainWindow->observeFullScreen(),
                [this](bool value)
                {
                    _actions["FullScreen"]->setChecked(value);
                });

            p.floatOnTopObserver = feather_tk::ValueObserver<bool>::create(
                mainWindow->observeFloatOnTop(),
                [this](bool value)
                {
                    _actions["FloatOnTop"]->setChecked(value);
                });

            p.secondaryObserver = feather_tk::ValueObserver<bool>::create(
                app->observeSecondaryWindow(),
                [this](bool value)
                {
                    _actions["Secondary"]->setChecked(value);
                });

            p.settingsObserver = feather_tk::ValueObserver<WindowSettings>::create(
                app->getSettingsModel()->observeWindow(),
                [this](const WindowSettings& value)
                {
                    _actions["FileToolBar"]->setChecked(value.fileToolBar);
                    _actions["CompareToolBar"]->setChecked(value.compareToolBar);
                    _actions["WindowToolBar"]->setChecked(value.windowToolBar);
                    _actions["ViewToolBar"]->setChecked(value.viewToolBar);
                    _actions["ToolsToolBar"]->setChecked(value.toolsToolBar);
                    _actions["TabBar"]->setChecked(value.tabBar);
                    _actions["Timeline"]->setChecked(value.timeline);
                    _actions["BottomToolBar"]->setChecked(value.bottomToolBar);
                    _actions["StatusToolBar"]->setChecked(value.statusToolBar);
                });
        }

        WindowActions::WindowActions() :
            _p(new Private)
        {}

        WindowActions::~WindowActions()
        {}

        std::shared_ptr<WindowActions> WindowActions::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow)
        {
            auto out = std::shared_ptr<WindowActions>(new WindowActions);
            out->_init(context, app, mainWindow);
            return out;
        }
    }
}
