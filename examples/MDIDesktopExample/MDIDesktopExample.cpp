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

#include <djvUI/Border.h>
#include <djvUI/Label.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/MDIWidget.h>
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

class MDIWidget : public UI::MDI::IWidget
{
    DJV_NON_COPYABLE(MDIWidget);

protected:
    void _init(const std::string & title, Core::Context *);
    MDIWidget();

public:
    static std::shared_ptr<MDIWidget> create(const std::string & title, Core::Context *);

    void setClosedCallback(const std::function<void(void)> &);

    float getHeightForWidth(float) const override;

protected:
    void _preLayoutEvent(Core::Event::PreLayout&) override;
    void _layoutEvent(Core::Event::Layout&) override;

private:
    std::shared_ptr<UI::ToolButton> _closeButton;
    std::shared_ptr<UI::Layout::Vertical> _layout;
    std::shared_ptr<UI::Border> _border;
    std::function<void(void)> _closedCallback;
};

void MDIWidget::_init(const std::string & title, Core::Context * context)
{
    IWidget::_init(context);

    setBackgroundRole(UI::Style::ColorRole::Background);

    auto titleLabel = UI::Label::create(context);
    titleLabel->setText(title);
    titleLabel->setTextColorRole(UI::Style::ColorRole::HeaderForeground);
    titleLabel->setMargin(UI::Style::MetricsRole::Margin);

    _closeButton = UI::ToolButton::create(context);
    _closeButton->setIcon("djvIconCloseSmall");
    _closeButton->setInsideMargin(UI::Style::MetricsRole::MarginSmall);
    _closeButton->setForegroundColorRole(UI::Style::ColorRole::HeaderForeground);
    _closeButton->setHoveredColorRole(UI::Style::ColorRole::HeaderHovered);
    _closeButton->setPressedColorRole(UI::Style::ColorRole::HeaderPressed);
    _closeButton->setCheckedColorRole(UI::Style::ColorRole::HeaderChecked);
    _closeButton->setDisabledColorRole(UI::Style::ColorRole::HeaderDisabled);

    auto titleBar = UI::Layout::Horizontal::create(context);
    titleBar->setClassName("djv::UI::MDI::TitleBar");
    titleBar->setBackgroundRole(UI::Style::ColorRole::HeaderBackground);
    titleBar->addWidget(titleLabel);
    titleBar->addExpander();
    titleBar->addWidget(_closeButton);

    auto textBlock = UI::TextBlock::create(context);
    textBlock->setText(Core::String::getRandomText(100));
    textBlock->setMargin(UI::Style::MetricsRole::Margin);

    auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
    scrollWidget->addWidget(textBlock);

    _layout = UI::Layout::Vertical::create(context);
    _layout->setSpacing(UI::Style::MetricsRole::None);
    _layout->addWidget(titleBar);
    _layout->addWidget(scrollWidget, UI::Layout::RowStretch::Expand);

    _border = UI::Border::create(context);
    _border->addWidget(_layout);
    IContainer::addWidget(_border);
}

MDIWidget::MDIWidget()
{}

std::shared_ptr<MDIWidget> MDIWidget::create(const std::string & title, Core::Context * context)
{
    auto out = std::shared_ptr<MDIWidget>(new MDIWidget);
    out->_init(title, context);
    return out;
}

void MDIWidget::setClosedCallback(const std::function<void(void)> & callback)
{
    _closeButton->setClickedCallback(callback);
}

float MDIWidget::getHeightForWidth(float value) const
{
    return _border->getHeightForWidth(value);
}

void MDIWidget::_preLayoutEvent(Core::Event::PreLayout&)
{
    _setMinimumSize(_border->getMinimumSize());
}

void MDIWidget::_layoutEvent(Core::Event::Layout&)
{
    _border->setGeometry(getGeometry());
}

int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        auto app = Desktop::Application::create(argc, argv);

        auto canvas = UI::MDI::Canvas::create(app.get());
        glm::vec2 pos(50.f, 50.f);
        Core::Math::setRandomSeed();
        for (size_t i = 0; i < 3; ++i)
        {
            std::stringstream ss;
            ss << "Widget " << i;
            auto widget = MDIWidget::create(ss.str(), app.get());
            widget->resize(glm::vec2(600.f, 400.f));
            widget->setParent(canvas);
            widget->setClosedCallback(
                [widget]
            {
                widget->setParent(nullptr);
            });
            canvas->setWidgetPos(widget, pos);
            pos += glm::vec2(100.f, 100.f);
        }

        auto window = UI::Window::create(app.get());
        window->addWidget(canvas);
        window->show();

        return app->run();
    }
    catch (const std::exception & e)
    {
        std::cout << Core::Error::format(e) << std::endl;
    }
    return r;
}
