// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvDesktopApp/Application.h>

#include <djvUI/ComboBox.h>
#include <djvUI/GridLayout.h>
#include <djvUI/Window.h>

#include <djvCore/ErrorFunc.h>
#include <djvCore/StringFunc.h>

using namespace djv;

int main(int argc, char ** argv)
{
    int r = 1;
    try
    {
        // Create an application.
        auto args = Desktop::Application::args(argc, argv);
        auto app = Desktop::Application::create(args);

        // Create some combo box widgets.
        auto comboBox0 = UI::ComboBox::create(app);
        comboBox0->setItems(Core::String::getRandomNames(5));
        comboBox0->setCurrentItem(0);
        comboBox0->setHAlign(UI::HAlign::Left);
        comboBox0->setVAlign(UI::VAlign::Top);
        auto comboBox1 = UI::ComboBox::create(app);
        comboBox1->setItems(Core::String::getRandomNames(25));
        comboBox0->setCurrentItem(0);
        comboBox1->setHAlign(UI::HAlign::Right);
        comboBox1->setVAlign(UI::VAlign::Top);
        auto comboBox2 = UI::ComboBox::create(app);
        comboBox2->setItems(Core::String::getRandomNames(5));
        comboBox0->setCurrentItem(0);
        comboBox2->setHAlign(UI::HAlign::Left);
        comboBox2->setVAlign(UI::VAlign::Bottom);
        auto comboBox3 = UI::ComboBox::create(app);
        comboBox3->setItems(Core::String::getRandomNames(25));
        comboBox0->setCurrentItem(0);
        comboBox3->setHAlign(UI::HAlign::Right);
        comboBox3->setVAlign(UI::VAlign::Bottom);

        // Create a layout for the widgets.
        auto layout = UI::GridLayout::create(app);
        layout->setMargin(UI::MetricsRole::MarginLarge);
        layout->addChild(comboBox0);
        layout->setGridPos(comboBox0, glm::ivec2(0, 0));
        layout->setStretch(comboBox0, UI::GridStretch::Both);
        layout->addChild(comboBox1);
        layout->setGridPos(comboBox1, glm::ivec2(1, 0));
        layout->setStretch(comboBox1, UI::GridStretch::Both);
        layout->addChild(comboBox2);
        layout->setGridPos(comboBox2, glm::ivec2(0, 1));
        layout->setStretch(comboBox2, UI::GridStretch::Both);
        layout->addChild(comboBox3);
        layout->setGridPos(comboBox3, glm::ivec2(1, 1));
        layout->setStretch(comboBox3, UI::GridStretch::Both);

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
