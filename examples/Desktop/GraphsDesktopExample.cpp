// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvDesktopApp/Application.h>

#include <djvUIComponents/LineGraphWidget.h>

#include <djvUI/GroupBox.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Window.h>

#include <djvCore/Error.h>
#include <djvCore/Timer.h>

using namespace djv;

int main(int argc, char ** argv)
{
    int r = 1;
    try
    {
        // Create an application.
        auto args = Desktop::Application::args(argc, argv);
        auto app = Desktop::Application::create(args);

        // Create a top-level layout.
        auto layout = UI::VerticalLayout::create(app);
        layout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginLarge));
        layout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingLarge));

        // Create some line graph widgets.
        std::vector<std::shared_ptr<UI::LineGraphWidget> > lineGraphWidgets;
        auto vLayout = UI::VerticalLayout::create(app);
        for (size_t i = 0; i < 3; ++i)
        {
            auto lineGraphWidget = UI::LineGraphWidget::create(app);
            lineGraphWidgets.push_back(lineGraphWidget);
            vLayout->addChild(lineGraphWidget);
        }
        auto groupBox = UI::GroupBox::create(app);
        groupBox->setText("Line Graphs");
        groupBox->addChild(vLayout);
        layout->addChild(groupBox);

        // Create a window and show it.
        auto window = UI::Window::create(app);
        window->addChild(layout);
        window->show();

        // Start a timer to drive the line graphs.
        Core::Time::Duration t = Core::Time::Duration::zero();
        auto timer = Core::Time::Timer::create(app);
        timer->setRepeating(true);
        timer->start(
            std::chrono::milliseconds(30),
            [lineGraphWidgets, &t](const std::chrono::steady_clock::time_point&, const Core::Time::Duration& value)
        {
            float f = 5.f;
            for (size_t i = 0; i < lineGraphWidgets.size(); ++i)
            {
                const float vv = sinf(t.count() / 1000.F * f);
                lineGraphWidgets[i]->addSample(vv);

                f *= .5f;
            }
            t += value;
        });

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
