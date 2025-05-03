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
            std::shared_ptr<dtk::ValueObserver<bool> > fullScreenObserver;
            std::shared_ptr<dtk::ValueObserver<bool> > floatOnTopObserver;
            std::shared_ptr<dtk::ValueObserver<bool> > secondaryObserver;
            std::shared_ptr<dtk::ValueObserver<WindowSettings> > settingsObserver;
        };

        void WindowActions::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow)
        {
            IActions::_init(context, app, "Window");
            DTK_P();

            auto appWeak = std::weak_ptr<App>(app);
            _actions["FullScreen"] = dtk::Action::create(
                "Full Screen",
                "WindowFullScreen",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getMainWindow()->setFullScreen(value);
                    }
                });

            _actions["FloatOnTop"] = dtk::Action::create(
                "Float On Top",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getMainWindow()->setFloatOnTop(value);
                    }
                });

            _actions["Secondary"] = dtk::Action::create(
                "Secondary",
                "WindowSecondary",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->setSecondaryWindow(value);
                    }
                });

            _actions["FileToolBar"] = dtk::Action::create(
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

            _actions["CompareToolBar"] = dtk::Action::create(
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

            _actions["WindowToolBar"] = dtk::Action::create(
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

            _actions["ViewToolBar"] = dtk::Action::create(
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

            _actions["ToolsToolBar"] = dtk::Action::create(
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

            _actions["TabBar"] = dtk::Action::create(
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

            _actions["Timeline"] = dtk::Action::create(
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

            _actions["BottomToolBar"] = dtk::Action::create(
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

            _actions["StatusToolBar"] = dtk::Action::create(
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

            p.fullScreenObserver = dtk::ValueObserver<bool>::create(
                mainWindow->observeFullScreen(),
                [this](bool value)
                {
                    _actions["FullScreen"]->setChecked(value);
                });

            p.floatOnTopObserver = dtk::ValueObserver<bool>::create(
                mainWindow->observeFloatOnTop(),
                [this](bool value)
                {
                    _actions["FloatOnTop"]->setChecked(value);
                });

            p.secondaryObserver = dtk::ValueObserver<bool>::create(
                app->observeSecondaryWindow(),
                [this](bool value)
                {
                    _actions["Secondary"]->setChecked(value);
                });

            p.settingsObserver = dtk::ValueObserver<WindowSettings>::create(
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
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow)
        {
            auto out = std::shared_ptr<WindowActions>(new WindowActions);
            out->_init(context, app, mainWindow);
            return out;
        }
    }
}
