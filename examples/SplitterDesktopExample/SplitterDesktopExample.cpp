//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvDesktop/Application.h>

#include <djvUI/ScrollWidget.h>
#include <djvUI/Splitter.h>
#include <djvUI/TextBlock.h>
#include <djvUI/Window.h>

#include <djvCore/Error.h>
#include <djvCore/String.h>

using namespace djv;

int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        auto app = Desktop::Application::create(argc, argv);

        auto textBlock1 = UI::TextBlock::create(app.get());
        textBlock1->setText(Core::String::getRandomText(1000));
        textBlock1->setFontSizeRole(UI::MetricsRole::FontLarge);
        textBlock1->setMargin(UI::MetricsRole::Margin);

        auto textBlock2 = UI::TextBlock::create(app.get());
        textBlock2->setText(Core::String::getRandomText(1000));
        textBlock2->setFontSizeRole(UI::MetricsRole::FontLarge);
        textBlock2->setMargin(UI::MetricsRole::Margin);

        auto scrollWidget1 = UI::ScrollWidget::create(UI::ScrollType::Vertical, app.get());
        scrollWidget1->addWidget(textBlock1);

        auto scrollWidget2 = UI::ScrollWidget::create(UI::ScrollType::Vertical, app.get());
        scrollWidget2->addWidget(textBlock2);

        auto splitter = UI::Splitter::create(UI::Orientation::Horizontal, app.get());
        splitter->addWidget(scrollWidget1);
        splitter->addWidget(scrollWidget2);

        auto window = UI::Window::create(app.get());
        window->addWidget(splitter);
        window->show();

        return app->run();
    }
    catch (const std::exception & e)
    {
        std::cout << Core::format(e) << std::endl;
    }
    return r;
}
