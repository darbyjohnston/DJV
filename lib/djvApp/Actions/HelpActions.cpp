// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Actions/HelpActions.h>

#include <djvApp/App.h>
#include <djvApp/MainWindow.h>

#include <dtk/core/OS.h>

namespace djv
{
    namespace app
    {
        struct HelpActions::Private
        {
        };

        void HelpActions::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow)
        {
            IActions::_init(context, app, "Help");
            DTK_P();

            _actions["Documentation"] = dtk::Action::create(
                "Documentation",
                []
                {
                    dtk::openURL("https://darbyjohnston.github.io/DJV/index.html");
                });

            std::weak_ptr<MainWindow> mainWindowWeak(mainWindow);
            _actions["About"] = dtk::Action::create(
                "About",
                [mainWindowWeak]
                {
                    if (auto mainWindow = mainWindowWeak.lock())
                    {
                        mainWindow->showAboutDialog();
                    }
                });
        }

        HelpActions::HelpActions() :
            _p(new Private)
        {}

        HelpActions::~HelpActions()
        {}

        std::shared_ptr<HelpActions> HelpActions::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow)
        {
            auto out = std::shared_ptr<HelpActions>(new HelpActions);
            out->_init(context, app, mainWindow);
            return out;
        }
    }
}
