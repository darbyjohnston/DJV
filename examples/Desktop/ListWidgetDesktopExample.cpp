// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvDesktopApp/Application.h>

#include <djvUIComponents/SearchBox.h>

#include <djvUI/IntSlider.h>
#include <djvUI/ListWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ToolButton.h>
#include <djvUI/Window.h>

#include <djvCore/Error.h>
#include <djvCore/TextSystem.h>

using namespace djv;

class Application : public Desktop::Application
{
    DJV_NON_COPYABLE(Application);

protected:
    void _init(std::list<std::string>&);
    Application();

public:
    ~Application() override;

    static std::shared_ptr<Application> create(std::list<std::string>&);

private:
    std::vector<std::string> _createList() const;
    void _widgetUpdate();

    int _listSize = 20;
    std::vector<std::string> _list;
    int _currentItem = 0;
    std::string _filter;
    std::shared_ptr<UI::IntSlider> _countSlider;
    std::shared_ptr<UI::ListWidget> _listWidget;
    std::shared_ptr<UI::Window> _window;
};

void Application::_init(std::list<std::string>& args)
{
    Desktop::Application::_init(args);

    // Create widgets for modifying the list.
    _countSlider = UI::IntSlider::create(shared_from_this());
    auto newButton = UI::ToolButton::create(shared_from_this());
    newButton->setText("New");

    // Create a search box.
    auto searchBox = UI::SearchBox::create(shared_from_this());

    // Create a list widget.
    _listWidget = UI::ListWidget::create(shared_from_this());
    _listWidget->setBorder(false);

    // Layout the widgets.
    auto layout = UI::VerticalLayout::create(shared_from_this());
    layout->setSpacing(UI::MetricsRole::None);
    auto hLayout = UI::HorizontalLayout::create(shared_from_this());
    hLayout->setMargin(UI::MetricsRole::MarginSmall);
    hLayout->setSpacing(UI::MetricsRole::SpacingSmall);
    hLayout->addChild(_countSlider);
    hLayout->setStretch(_countSlider, UI::Layout::RowStretch::Expand);
    hLayout->addChild(newButton);
    hLayout->addChild(searchBox);
    layout->addChild(hLayout);
    layout->addSeparator();
    layout->addChild(_listWidget);
    layout->setStretch(_listWidget, UI::Layout::RowStretch::Expand);

    // Create a window.
    _window = UI::Window::create(shared_from_this());
    _window->addChild(layout);

    // Initialize the list.
    _list = _createList();
    _widgetUpdate();

    // Setup callbacks.
    _countSlider->setValueCallback(
        [this](int value)
    {
        _listSize = value;
        _list = _createList();
        _widgetUpdate();
    });
    newButton->setClickedCallback(
        [this]
    {
        _list = _createList();
        _widgetUpdate();
    });
    searchBox->setFilterCallback(
        [this](const std::string& value)
    {
        _filter = value;
        _widgetUpdate();
    });
    _listWidget->setCurrentItemCallback(
        [this](int value)
    {
        _currentItem = value;
    });

    // Show the window.
    _window->show();
}

Application::Application()
{}

Application::~Application()
{}

std::shared_ptr<Application> Application::create(std::list<std::string>& args)
{
    auto out = std::shared_ptr<Application>(new Application);
    out->_init(args);
    return out;
}

std::vector<std::string> Application::_createList() const
{
    return Core::String::getRandomNames(_listSize);
}

void Application::_widgetUpdate()
{
    _countSlider->setValue(_listSize);

    // Update the list items.
    std::vector<std::string> items;
    std::vector<size_t> indices;
    auto textSystem = getSystemT<Core::TextSystem>();
    for (size_t i = 0; i < _list.size(); ++i)
    {
        const auto& item = _list[i];
        if (Core::String::match(item, _filter))
        {
            items.push_back(textSystem->getText(item));
            indices.push_back(i);
        }
    }
    _listWidget->setItems(items);

    // Update the list current item.
    size_t item = 0;
    if (indices.size())
    {
        item = Core::Math::closest(static_cast<size_t>(_currentItem), indices);
        _currentItem = indices[item];
    }
    _listWidget->setCurrentItem(item);
}

int main(int argc, char ** argv)
{
    int r = 1;
    try
    {
        auto args = Desktop::Application::args(argc, argv);
        auto app = Application::create(args);
        app->run();
        r = app->getExitCode();
    }
    catch (const std::exception & e)
    {
        std::cout << Core::Error::format(e) << std::endl;
    }
    return r;
}
