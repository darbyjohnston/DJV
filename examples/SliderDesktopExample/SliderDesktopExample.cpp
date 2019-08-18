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
    int r = 0;
    try
    {
        // Create an application.
        auto app = std::unique_ptr<Desktop::Application>(Desktop::Application::create(argc, argv));

        // Create a top-level layout.
        auto layout = UI::VerticalLayout::create(app.get());
        layout->setMargin(UI::MetricsRole::MarginLarge);
        layout->setSpacing(UI::MetricsRole::SpacingLarge);

        // Create some integer sliders.
        auto vLayout = UI::VerticalLayout::create(app.get());
        vLayout->setMargin(UI::MetricsRole::Margin);
        const std::vector<Core::IntRange> intRange =
        {
            Core::IntRange(    0,   10),
            Core::IntRange(    1,  100),
            Core::IntRange(-1000, 1000)
        };
        for (size_t i = 0; i < 3; ++i)
        {
            auto slider = UI::IntSlider::create(app.get());
            slider->setRange(intRange[i]);
            slider->setValueCallback(
                [](int value)
            {
                std::cout << value << std::endl;
            });
            vLayout->addChild(slider);
        }
        auto groupBox = UI::GroupBox::create("Integer Sliders", app.get());
        groupBox->addChild(vLayout);
        layout->addChild(groupBox);
        
        // Create some floating-point sliders.
        vLayout = UI::VerticalLayout::create(app.get());
        vLayout->setMargin(UI::MetricsRole::Margin);
        const std::vector<Core::FloatRange> floatRange =
        {
            Core::FloatRange(   0.f,   1.f),
            Core::FloatRange(   1.f,  10.f),
            Core::FloatRange(-100.f, 100.f)
        };
        for (size_t i = 0; i < 3; ++i)
        {
            auto slider = UI::FloatSlider::create(app.get());
            slider->setRange(floatRange[i]);
            slider->setValueCallback(
                [](float value)
            {
                std::cout << value << std::endl;
            });
            vLayout->addChild(slider);
        }
        groupBox = UI::GroupBox::create("Floating-Point Sliders", app.get());
        groupBox->addChild(vLayout);
        layout->addChild(groupBox);

        // Create a window and show it.
        auto window = UI::Window::create(app.get());
        window->addChild(layout);
        window->show();

        return app->run();
    }
    catch (const std::exception & e)
    {
        std::cout << Core::Error::format(e) << std::endl;
    }
    return r;
}
