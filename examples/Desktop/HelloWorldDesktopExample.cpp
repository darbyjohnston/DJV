// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvDesktopApp/Application.h>

#include <djvUI/Label.h>
#include <djvUI/Window.h>

#include <djvCore/Error.h>

using namespace djv;

int main(int argc, char ** argv)
{
    int r = 1;
    try
    {
        // Create an application.
        auto args = Desktop::Application::args(argc, argv);
        auto app = Desktop::Application::create(args);

        // Create a label.
        auto label = UI::Label::create(app);
        label->setText("Hello world!");
        label->setFontSizeRole(UI::MetricsRole::FontHeader);
        label->setHAlign(UI::HAlign::Center);
        label->setVAlign(UI::VAlign::Center);

        // Create a window and show it.
        auto window = UI::Window::create(app);
        window->addChild(label);
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
