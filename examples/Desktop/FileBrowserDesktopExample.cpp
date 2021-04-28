// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvDesktopApp/Application.h>

#include <djvUIComponents/FileBrowser.h>
#include <djvUIComponents/UIComponentsSystem.h>

#include <djvUI/Window.h>

#include <djvSystem/FileInfo.h>

#include <djvCore/Error.h>

#include <iostream>

using namespace djv;

int main(int argc, char ** argv)
{
    int r = 1;
    try
    {
        // Create an application.
        auto args = Desktop::Application::args(argc, argv);
        auto app = Desktop::Application::create(args);

        // Create the systems.
        UIComponents::UIComponentsSystem::create(app);

        // Create a file browser.
        auto fileBrowser = UIComponents::FileBrowser::FileBrowser::create(UI::SelectionType::Multiple, app);
        fileBrowser->setPath(System::File::Path("."));
        fileBrowser->setCallback(
            [](const std::vector<System::File::Info>& value)
            {
                for (const auto& i : value)
                {
                    std::cout << i << std::endl;
                }
                std::cout << std::endl;
            });

        // Create a window and show it.
        auto window = UI::Window::create(app);
        window->addChild(fileBrowser);
        window->show();

        // Run the application.
        app->run();
        r = app->getExitCode();
    }
    catch (const std::exception & e)
    {
        std::cout << Core::Error::format(e) << std::endl;
    }
    return r;
}
