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

#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/MDIWindow.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Splitter.h>
#include <djvUI/StackLayout.h>
#include <djvUI/TextBlock.h>
#include <djvUI/ToolButton.h>
#include <djvUI/Window.h>

#include <djvCore/Error.h>
#include <djvCore/String.h>

using namespace djv;

class MDIWindow : public UI::MDI::IWindow
{
    DJV_NON_COPYABLE(MDIWindow);

protected:
    void _init(const std::string & title, Core::Context *);
    MDIWindow();

public:
    static std::shared_ptr<MDIWindow> create(const std::string & title, Core::Context *);

    void setClosedCallback(const std::function<void(void)> &);

    std::shared_ptr<Widget> getMoveHandle() override;
    std::shared_ptr<Widget> getResizeHandle() override;

    void preLayoutEvent(Core::PreLayoutEvent&) override;
    void layoutEvent(Core::LayoutEvent&) override;

private:
    std::shared_ptr<UI::ToolButton> _closeButton;
    std::shared_ptr<UI::Icon> _resizeHandle;
    std::shared_ptr<UI::VerticalLayout> _layout;
    std::function<void(void)> _closedCallback;
};

void MDIWindow::_init(const std::string & title, Core::Context * context)
{
    IWindow::_init(context);

    setBackgroundRole(UI::ColorRole::Background);

    auto titleLabel = UI::Label::create(context);
    titleLabel->setText(title);
    titleLabel->setMargin(UI::MetricsRole::Margin);

    _closeButton = UI::ToolButton::create(context);
    _closeButton->setIcon(context->getPath(Core::ResourcePath::IconsDirectory, "djvIconClose90DPI.png"));

    auto titleBar = UI::HorizontalLayout::create(context);
    titleBar->setClassName("djv::UI::MDI::TitleBar");
    titleBar->addWidget(titleLabel);
    titleBar->addExpander();
    titleBar->addWidget(_closeButton);

    auto textBlock = UI::TextBlock::create(context);
    textBlock->setText(Core::String::getRandomText(100));
    textBlock->setMargin(UI::MetricsRole::Margin);

    auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
    scrollWidget->addWidget(textBlock);

    _resizeHandle = UI::Icon::create(context);
    _resizeHandle->setPointerEnabled(true);
    _resizeHandle->setIcon(context->getPath(Core::ResourcePath::IconsDirectory, "djvIconWindowResizeHandle90DPI.png"));
    _resizeHandle->setHAlign(UI::HAlign::Right);
    _resizeHandle->setVAlign(UI::VAlign::Bottom);

    auto bottomBar = UI::HorizontalLayout::create(context);
    bottomBar->addExpander();
    bottomBar->addWidget(_resizeHandle);

    _layout = UI::VerticalLayout::create(context);
    _layout->setSpacing(UI::MetricsRole::None);
    _layout->addWidget(titleBar);
    _layout->addWidget(scrollWidget, UI::RowLayoutStretch::Expand);
    _layout->addWidget(bottomBar);
    IContainerWidget::addWidget(_layout);
}

MDIWindow::MDIWindow()
{}

std::shared_ptr<MDIWindow> MDIWindow::create(const std::string & title, Core::Context * context)
{
    auto out = std::shared_ptr<MDIWindow>(new MDIWindow);
    out->_init(title, context);
    return out;
}

void MDIWindow::setClosedCallback(const std::function<void(void)> & callback)
{
    _closeButton->setClickedCallback(callback);
}

std::shared_ptr<UI::Widget> MDIWindow::getMoveHandle()
{
    return std::dynamic_pointer_cast<Widget>(shared_from_this());
}

std::shared_ptr<UI::Widget> MDIWindow::getResizeHandle()
{
    return _resizeHandle;
}

void MDIWindow::preLayoutEvent(Core::PreLayoutEvent&)
{
    _setMinimumSize(_layout->getMinimumSize());
}

void MDIWindow::layoutEvent(Core::LayoutEvent&)
{
    _layout->setGeometry(getGeometry());
}

int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        auto app = Desktop::Application::create(argc, argv);

        auto canvas = UI::MDI::Canvas::create(app.get());
        Core::Math::setRandomSeed();
        for (size_t i = 0; i < 3; ++i)
        {
            std::stringstream ss;
            ss << "Window " << i;
            auto window = MDIWindow::create(ss.str(), app.get());
            const glm::vec2 size(Core::Math::getRandom(4, 5) * 100.f, Core::Math::getRandom(2, 3) * 100.f);
            window->resize(size);

            const glm::vec2 pos(Core::Math::getRandom(0, 50) * 10.f, Core::Math::getRandom(0, 50) * 10.f);
            canvas->addWindow(window, pos);

            window->setClosedCallback(
                [canvas, window]
            {
                canvas->removeWindow(window);
            });
        }

        auto window = UI::Window::create(app.get());
        window->addWidget(canvas);
        window->show();

        return app->run();
    }
    catch (const std::exception & e)
    {
        std::cout << Core::format(e) << std::endl;
    }
    return r;
}
