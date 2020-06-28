// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvDesktopApp/Application.h>

#include <djvUIComponents/SearchBox.h>

#include <djvUI/CheckBox.h>
#include <djvUI/ComboBox.h>
#include <djvUI/IntSlider.h>
#include <djvUI/ListWidget.h>
#include <djvUI/PushButton.h>
#include <djvUI/RowLayout.h>
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
    std::shared_ptr<UI::IntSlider> _countSlider;
    std::shared_ptr<UI::ListWidget> _listWidget;
    std::shared_ptr<UI::Window> _window;
};

void Application::_init(std::list<std::string>& args)
{
    Desktop::Application::_init(args);

    // Create widgets for modifying the list.
    _countSlider = UI::IntSlider::create(shared_from_this());
    auto newButton = UI::PushButton::create(shared_from_this());
    newButton->setText("New List");

    // Create widgets for list options.
    auto typeComboBox = UI::ComboBox::create(shared_from_this());
    typeComboBox->setItems({ "Push", "Toggle", "Radio", "Exclusive" });
    const UI::ButtonType buttonType = UI::ButtonType::Radio;
    typeComboBox->setCurrentItem(static_cast<int>(buttonType));
    auto alternateRowsCheckBox = UI::CheckBox::create(shared_from_this());
    alternateRowsCheckBox->setText("Alternate rows");

    // Create a search box.
    auto searchBox = UI::SearchBox::create(shared_from_this());

    // Create a list widget.
    _listWidget = UI::ListWidget::create(buttonType, shared_from_this());
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
    hLayout->addSeparator();
    hLayout->addChild(typeComboBox);
    hLayout->addChild(alternateRowsCheckBox);
    hLayout->addSeparator();
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
    typeComboBox->setCallback(
        [this](int value)
        {
            _listWidget->setButtonType(static_cast<UI::ButtonType>(value));
        });
    alternateRowsCheckBox->setCheckedCallback(
        [this](bool value)
        {
            _listWidget->setAlternateRowsRoles(
                UI::ColorRole::None,
                value ? UI::ColorRole::Trough : UI::ColorRole::None);
        });
    searchBox->setFilterCallback(
        [this](const std::string& value)
        {
            _listWidget->setFilter(value);
        });
    _listWidget->setPushCallback(
        [this](int value)
        {
            std::cout << "Push: " << value << std::endl;
        });
    _listWidget->setToggleCallback(
        [this](int value, bool checked)
        {
            std::cout << "Toggle: " << value << " = " << checked << std::endl;
        });
    _listWidget->setRadioCallback(
        [this](int value)
        {
            std::cout << "Radio: " << value << std::endl;
        });
    _listWidget->setExclusiveCallback(
        [this](int value)
        {
            std::cout << "Exclusive: " << value << std::endl;
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
    _listWidget->setItems(_list);
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
