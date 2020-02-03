//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvDesktopApp/Application.h>

#include <djvUI/ComboBox.h>
#include <djvUI/GridLayout.h>
#include <djvUI/Window.h>

#include <djvCore/Error.h>
#include <djvCore/String.h>

using namespace djv;

int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        // Create an application.
        std::vector<std::string> args;
        for (int i = 0; i < argc; ++i)
        {
            args.push_back(argv[i]);
        }
        auto app = Desktop::Application::create(args);

        // Create some combo box widgets.
        auto comboBox0 = UI::ComboBox::create(app);
        comboBox0->setItems(Core::String::getRandomNames(5));
        comboBox0->setCurrentItem(0);
        comboBox0->setHAlign(UI::HAlign::Left);
        comboBox0->setVAlign(UI::VAlign::Top);
        auto comboBox1 = UI::ComboBox::create(app);
        comboBox1->setItems(Core::String::getRandomNames(25));
        comboBox1->setCurrentItem(0);
        comboBox1->setHAlign(UI::HAlign::Right);
        comboBox1->setVAlign(UI::VAlign::Top);
        auto comboBox2 = UI::ComboBox::create(app);
        comboBox2->setItems(Core::String::getRandomNames(5));
        comboBox2->setCurrentItem(0);
        comboBox2->setHAlign(UI::HAlign::Left);
        comboBox2->setVAlign(UI::VAlign::Bottom);
        auto comboBox3 = UI::ComboBox::create(app);
        comboBox3->setItems(Core::String::getRandomNames(25));
        comboBox3->setCurrentItem(0);
        comboBox3->setHAlign(UI::HAlign::Right);
        comboBox3->setVAlign(UI::VAlign::Bottom);

        // Create a layout for the widgets.
        auto layout = UI::GridLayout::create(app);
        layout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginLarge));
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
        return app->run();
    }
    catch (const std::exception & e)
    {
        std::cout << Core::Error::format(e) << std::endl;
    }
    return r;
}
