// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvDesktopApp/Application.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/GroupBox.h>
#include <djvUI/PushButton.h>
#include <djvUI/RowLayout.h>
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

        // Create a top-level layout.
        auto layout = UI::VerticalLayout::create(app);
        layout->setMargin(UI::MetricsRole::MarginLarge);
        layout->setSpacing(UI::MetricsRole::SpacingLarge);

        // Create a group of push buttons.
        auto pushButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);
        auto hLayout = UI::HorizontalLayout::create(app);
        hLayout->setMargin(UI::MetricsRole::Margin);
        for (size_t i = 0; i < 3; ++i)
        {
            std::stringstream ss;
            ss << "Push " << i;
            auto button = UI::PushButton::create(app);
            button->setText(ss.str());
            pushButtonGroup->addButton(button);
            hLayout->addChild(button);
        }
        pushButtonGroup->setPushCallback(
            [](int index)
        {
            std::cout << "Push = " << index << std::endl;
        });
        auto groupBox = UI::GroupBox::create(app);
        groupBox->setText("Push Buttons");
        groupBox->addChild(hLayout);
        layout->addChild(groupBox);

        // Create a group of toggle buttons.
        auto toggleButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Toggle);
        hLayout = UI::HorizontalLayout::create(app);
        hLayout->setMargin(UI::MetricsRole::Margin);
        for (size_t i = 0; i < 3; ++i)
        {
            std::stringstream ss;
            ss << "Toggle " << i;
            auto button = UI::PushButton::create(app);
            button->setText(ss.str());
            toggleButtonGroup->addButton(button);
            hLayout->addChild(button);
        }
        toggleButtonGroup->setToggleCallback(
            [](int index, bool value)
        {
            std::cout << "Toggle " << index << " = " << value << std::endl;
        });
        groupBox = UI::GroupBox::create(app);
        groupBox->setText("Toggle Buttons");
        groupBox->addChild(hLayout);
        layout->addChild(groupBox);

        // Create a group of radio buttons.
        auto radioButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Radio);
        hLayout = UI::HorizontalLayout::create(app);
        hLayout->setMargin(UI::MetricsRole::Margin);
        for (size_t i = 0; i < 3; ++i)
        {
            std::stringstream ss;
            ss << "Radio " << i;
            auto button = UI::PushButton::create(app);
            button->setText(ss.str());
            radioButtonGroup->addButton(button);
            hLayout->addChild(button);
        }
        radioButtonGroup->setRadioCallback(
            [](int index)
        {
            std::cout << "Radio " << index << std::endl;
        });
        groupBox = UI::GroupBox::create(app);
        groupBox->setText("Radio Buttons");
        groupBox->addChild(hLayout);
        layout->addChild(groupBox);

        // Create a group of exclusive buttons.
        auto exclusiveButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Exclusive);
        hLayout = UI::HorizontalLayout::create(app);
        hLayout->setMargin(UI::MetricsRole::Margin);
        for (size_t i = 0; i < 3; ++i)
        {
            std::stringstream ss;
            ss << "Exclusive " << i;
            auto button = UI::PushButton::create(app);
            button->setText(ss.str());
            exclusiveButtonGroup->addButton(button);
            hLayout->addChild(button);
        }
        exclusiveButtonGroup->setExclusiveCallback(
            [](int index)
        {
            std::cout << "Exclusive " << index << std::endl;
        });
        groupBox = UI::GroupBox::create(app);
        groupBox->setText("Exclusive Buttons");
        groupBox->addChild(hLayout);
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
