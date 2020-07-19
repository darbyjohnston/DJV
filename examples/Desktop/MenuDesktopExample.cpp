// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvDesktopApp/Application.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuBar.h>
#include <djvUI/RowLayout.h>
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

        // Create a menu bar.
        auto menuBar = UI::MenuBar::create(app);

        // Create a menu with push button items.
        auto menu = UI::Menu::create(app);
        menu->setText("Menu");
        for (size_t i = 0; i < 3; ++i)
        {
            auto action = UI::Action::create();
            std::stringstream ss;
            ss << "Item " << i;
            action->setText(ss.str());
            menu->addAction(action);
        }
        menuBar->addChild(menu);

        // Create a menu with toggle button items.
        auto toggleMenu = UI::Menu::create(app);
        toggleMenu->setText("Toggle");
        std::vector<std::shared_ptr<UI::Action> > actions;
        for (size_t i = 0; i < 3; ++i)
        {
            auto action = UI::Action::create();
            std::stringstream ss;
            ss << "Item " << i;
            action->setText(ss.str());
            actions.push_back(action);
            toggleMenu->addAction(action);
        }
        auto toggleActionGroup = UI::ActionGroup::create(UI::ButtonType::Toggle);
        toggleActionGroup->setActions(actions);
        menuBar->addChild(toggleMenu);

        // Create a menu with radio button items.
        auto radioMenu = UI::Menu::create(app);
        radioMenu->setText("Radio");
        actions.clear();
        for (size_t i = 0; i < 3; ++i)
        {
            auto action = UI::Action::create();
            std::stringstream ss;
            ss << "Item " << i;
            action->setText(ss.str());
            actions.push_back(action);
            radioMenu->addAction(action);
        }
        auto radioActionGroup = UI::ActionGroup::create(UI::ButtonType::Radio);
        radioActionGroup->setActions(actions);
        radioActionGroup->setChecked(0);
        menuBar->addChild(radioMenu);

        // Create a menu with exclusive button items.
        auto exclusiveMenu = UI::Menu::create(app);
        exclusiveMenu->setText("Exclusive");
        actions.clear();
        for (size_t i = 0; i < 3; ++i)
        {
            auto action = UI::Action::create();
            std::stringstream ss;
            ss << "Item " << i;
            action->setText(ss.str());
            actions.push_back(action);
            exclusiveMenu->addAction(action);
        }
        auto exclusiveActionGroup = UI::ActionGroup::create(UI::ButtonType::Exclusive);
        exclusiveActionGroup->setActions(actions);
        menuBar->addChild(exclusiveMenu);

        // Create a layout.
        auto layout = UI::VerticalLayout::create(app);
        layout->addChild(menuBar);
        layout->addExpander();

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
