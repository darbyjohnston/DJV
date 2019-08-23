//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvUI/ButtonGroup.h>
#include <djvUI/GroupBox.h>
#include <djvUI/PushButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Window.h>

#include <djvCore/Error.h>

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
        return app->run();
    }
    catch (const std::exception & e)
    {
        std::cout << Core::Error::format(e) << std::endl;
    }
    return r;
}
