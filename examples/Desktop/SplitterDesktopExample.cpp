// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvDesktopApp/Application.h>

#include <djvUI/ScrollWidget.h>
#include <djvUI/Splitter.h>
#include <djvUI/TextBlock.h>
#include <djvUI/Window.h>

#include <djvCore/Error.h>
#include <djvCore/String.h>

using namespace djv;

int main(int argc, char ** argv)
{
    int r = 1;
    try
    {
        // Create an application.
        auto args = Desktop::Application::args(argc, argv);
        auto app = Desktop::Application::create(args);

        // Create a splitter.
        auto splitter = UI::Layout::Splitter::create(UI::Orientation::Horizontal, app);

        // Create some widgets and add them to the splitter.
        for (size_t i = 0; i < 3; ++i)
        {
            auto textBlock = UI::TextBlock::create(app);
            textBlock->setText(Core::String::getRandomText(20));
            textBlock->setFontSizeRole(UI::MetricsRole::FontLarge);
            textBlock->setMargin(UI::Layout::Margin(UI::MetricsRole::Margin));

            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, app);
            scrollWidget->addChild(textBlock);

            splitter->addChild(scrollWidget);
        }

        // Create a window and show it.
        auto window = UI::Window::create(app);
        window->addChild(splitter);
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
