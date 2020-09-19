// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvDesktopApp/Application.h>

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

#include <djvCore/ErrorFunc.h>
#include <djvCore/RandomFunc.h>
#include <djvCore/StringFunc.h>

using namespace djv;

class MDIWidget : public UI::MDI::IWidget
{
    DJV_NON_COPYABLE(MDIWidget);

protected:
    void _init(const std::string & title, const std::shared_ptr<System::Context>&);
    MDIWidget();

public:
    ~MDIWidget() override;

    static std::shared_ptr<MDIWidget> create(const std::string & title, const std::shared_ptr<System::Context>&);

    void setClosedCallback(const std::function<void(void)> &);

    float getHeightForWidth(float) const override;

protected:
    void _preLayoutEvent(System::Event::PreLayout&) override;
    void _layoutEvent(System::Event::Layout&) override;

private:
    std::shared_ptr<UI::ToolButton> _closeButton;
    std::shared_ptr<UI::Border> _border;
    std::function<void(void)> _closedCallback;
};

void MDIWidget::_init(const std::string & title, const std::shared_ptr<System::Context>& context)
{
    IWidget::_init(context);

    auto titleLabel = UI::Label::create(context);
    titleLabel->setText(title);
    titleLabel->setMargin(UI::MetricsRole::Margin);

    _closeButton = UI::ToolButton::create(context);
    _closeButton->setIcon("djvIconClose");

    auto titleBar = UI::HorizontalLayout::create(context);
    titleBar->setClassName("djv::UI::MDI::TitleBar");
    titleBar->setBackgroundRole(UI::ColorRole::BackgroundHeader);
    titleBar->addChild(titleLabel);
    titleBar->addExpander();
    titleBar->addChild(_closeButton);

    auto textBlock = UI::TextBlock::create(context);
    textBlock->setText(Core::String::getRandomText(20));
    textBlock->setMargin(UI::MetricsRole::Margin);

    auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
    scrollWidget->setBorder(false);
    scrollWidget->addChild(textBlock);

    auto layout = UI::VerticalLayout::create(context);
    layout->setSpacing(UI::MetricsRole::None);
    layout->setBackgroundRole(UI::ColorRole::Background);
    layout->addChild(titleBar);
    layout->addChild(scrollWidget);
    layout->setStretch(scrollWidget, UI::RowStretch::Expand);

    _border = UI::Border::create(context);
    _border->setMargin(UI::MetricsRole::Shadow);
    _border->addChild(layout);
    addChild(_border);
}

MDIWidget::MDIWidget()
{}

MDIWidget::~MDIWidget()
{}

std::shared_ptr<MDIWidget> MDIWidget::create(const std::string & title, const std::shared_ptr<System::Context>& context)
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

void MDIWidget::_preLayoutEvent(System::Event::PreLayout&)
{
    _setMinimumSize(_border->getMinimumSize());
}

void MDIWidget::_layoutEvent(System::Event::Layout&)
{
    _border->setGeometry(getGeometry());
}

int main(int argc, char ** argv)
{
    int r = 1;
    try
    {
        auto args = Desktop::Application::args(argc, argv);
        auto app = Desktop::Application::create(args);

        auto canvas = UI::MDI::Canvas::create(app);
        glm::vec2 pos(50.f, 50.f);
        Core::Random::setRandomSeed();
        for (size_t i = 0; i < 3; ++i)
        {
            std::stringstream ss;
            ss << "Widget " << i;
            auto widget = MDIWidget::create(ss.str(), app);
            widget->resize(glm::vec2(600.f, 400.f));
            canvas->addChild(widget);
            auto weak = std::weak_ptr<MDIWidget>(std::dynamic_pointer_cast<MDIWidget>(widget));
            widget->setClosedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto parent = widget->getParent().lock())
                        {
                            parent->removeChild(widget);
                        }
                    }
                });
            canvas->setWidgetPos(widget, pos);
            pos += glm::vec2(100.f, 100.f);
        }

        auto window = UI::Window::create(app);
        window->addChild(canvas);
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
