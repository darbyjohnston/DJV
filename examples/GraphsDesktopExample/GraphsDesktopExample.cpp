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

#include <djvUIComponents/LineGraphWidget.h>

#include <djvUI/GroupBox.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Window.h>

#include <djvCore/Error.h>
#include <djvCore/Timer.h>

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
        float v = 0.f;
        auto timer = Core::Time::Timer::create(app);
        timer->setRepeating(true);
        timer->start(
            std::chrono::milliseconds(30),
            [lineGraphWidgets, &v](float diff)
        {
            float f = 5.f;
            for (size_t i = 0; i < lineGraphWidgets.size(); ++i)
            {
                const float vv = sinf(v * f);
                lineGraphWidgets[i]->addSample(vv);

                f *= .5f;
            }
            v += diff;
        });

        // Run the application.
        return app->run();
    }
    catch (const std::exception & e)
    {
        std::cout << Core::Error::format(e) << std::endl;
    }
    return r;
}
