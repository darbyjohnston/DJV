// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvDesktopApp/Application.h>

#include <djvUI/FloatSlider.h>
#include <djvUI/GridLayout.h>
#include <djvUI/GroupBox.h>
#include <djvUI/IntSlider.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Window.h>

#include <djvCore/Error.h>
#include <djvCore/NumericValueModels.h>
#include <djvCore/Range.h>

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

        // Create some integer sliders.
        auto vLayout = UI::VerticalLayout::create(app);
        vLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::Margin));
        const std::vector<Core::IntRange> intRange =
        {
            Core::IntRange(    0,   10),
            Core::IntRange(    1,  100),
            Core::IntRange(-1000, 1000)
        };
        for (size_t i = 0; i < 3; ++i)
        {
            auto slider = UI::IntSlider::create(app);
            slider->setRange(intRange[i]);
            slider->setValueCallback(
                [](int value)
            {
                std::cout << value << std::endl;
            });
            vLayout->addChild(slider);
        }
        auto groupBox = UI::GroupBox::create(app);
        groupBox->setText("Integer Sliders");
        groupBox->addChild(vLayout);
        layout->addChild(groupBox);
        
        // Create some floating-point sliders.
        vLayout = UI::VerticalLayout::create(app);
        vLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::Margin));
        const std::vector<Core::FloatRange> floatRange =
        {
            Core::FloatRange(   0.f,   1.f),
            Core::FloatRange(   1.f,  10.f),
            Core::FloatRange(-100.f, 100.f)
        };
        for (size_t i = 0; i < 3; ++i)
        {
            auto slider = UI::FloatSlider::create(app);
            slider->setRange(floatRange[i]);
            slider->setValueCallback(
                [](float value)
            {
                std::cout << value << std::endl;
            });
            vLayout->addChild(slider);
        }
        groupBox = UI::GroupBox::create(app);
        groupBox->setText("Floating-Point Sliders");
        groupBox->addChild(vLayout);
        layout->addChild(groupBox);

        // Create a window and show it.
        auto window = UI::Window::create(app);
        window->addChild(layout);
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
