//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
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

#include "SceneViewExample.h"

#include <djvUIComponents/UIComponentsSystem.h>

#include <djvUI/Menu.h>
#include <djvUI/MenuBar.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Shortcut.h>
#include <djvUI/ToolBar.h>

#include <djvScene/SceneSystem.h>

#include <djvCore/Error.h>
#include <djvCore/LogSystem.h>
#include <djvCore/Timer.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv;

void MainWindow::_init(const std::shared_ptr<Core::Context>& context)
{
    Window::_init(context);

    _actions["Open"] = UI::Action::create();
    _actions["Open"]->setIcon("djvIconFileOpen");
    _actions["Open"]->setShortcut(GLFW_KEY_O, UI::Shortcut::getSystemModifier());
    _actions["Close"] = UI::Action::create();
    _actions["Close"]->setIcon("djvIconFileClose");
    _actions["Close"]->setShortcut(GLFW_KEY_E, UI::Shortcut::getSystemModifier());
    _actions["Exit"] = UI::Action::create();
    _actions["Exit"]->setShortcut(GLFW_KEY_Q, UI::Shortcut::getSystemModifier());
    for (const auto& i : _actions)
    {
        addAction(i.second);
    }

    auto menu = UI::Menu::create(context);
    menu->setText("File");
    menu->addAction(_actions["Open"]);
    menu->addAction(_actions["Close"]);
    menu->addSeparator();
    menu->addAction(_actions["Exit"]);
    auto menuBar = UI::MenuBar::create(context);
    menuBar->setBackgroundRole(UI::ColorRole::OverlayLight);
    menuBar->addChild(menu);

    _sceneWidget = UI::SceneWidget::create(context);

    _trianglesLabel = UI::Label::create(context);
    _trianglesLabel2 = UI::Label::create(context);

    auto toolBar = UI::ToolBar::create(context);
    toolBar->setBackgroundRole(UI::ColorRole::OverlayLight);
    toolBar->addAction(_actions["Open"]);
    toolBar->addAction(_actions["Close"]);

    addChild(_sceneWidget);
    auto vLayout = UI::VerticalLayout::create(context);
    vLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
    auto hLayout = UI::HorizontalLayout::create(context);
    hLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
    hLayout->addChild(menuBar);
    hLayout->setStretch(menuBar, UI::RowStretch::Expand);
    hLayout->addSeparator();
    hLayout->addChild(toolBar);
    vLayout->addChild(hLayout);
    vLayout->addExpander();
    hLayout = UI::HorizontalLayout::create(context);
    hLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
    hLayout->setBackgroundRole(UI::ColorRole::OverlayLight);
    hLayout->setHAlign(UI::HAlign::Left);
    hLayout->setVAlign(UI::VAlign::Bottom);
    hLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
    hLayout->addChild(_trianglesLabel);
    hLayout->addChild(_trianglesLabel2);
    addChild(hLayout);
    addChild(vLayout);

    auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
    _actionObservers["Open"] = Core::ValueObserver<bool>::create(
        _actions["Open"]->observeClicked(),
        [weak](bool value)
        {
            if (value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_open();
                }
            }
        });

    _actionObservers["Close"] = Core::ValueObserver<bool>::create(
        _actions["Close"]->observeClicked(),
        [weak](bool value)
        {
            if (value)
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_openCallback)
                    {
                        widget->_openCallback(Core::FileSystem::FileInfo());
                    }
                }
            }
        });

    _actionObservers["Exit"] = Core::ValueObserver<bool>::create(
        _actions["Exit"]->observeClicked(),
        [weak](bool value)
        {
            if (value)
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_exitCallback)
                    {
                        widget->_exitCallback();
                    }
                }
            }
        });

    _statsTimer = Core::Time::Timer::create(context);
    _statsTimer->setRepeating(true);
    _statsTimer->start(
        Core::Time::getMilliseconds(Core::Time::TimerValue::Slow),
        [weak](float)
        {
            if (auto widget = weak.lock())
            {
                std::stringstream ss;
                ss << widget->_sceneWidget->getTriangleCount();
                widget->_trianglesLabel2->setText(ss.str());
            }
        });
}

MainWindow::MainWindow()
{}

MainWindow::~MainWindow()
{}

void MainWindow::setScene(const std::shared_ptr<Scene::Scene>& value)
{
    _sceneWidget->setScene(value);
}

void MainWindow::setOpenCallback(const std::function<void(const Core::FileSystem::FileInfo)>& value)
{
    _openCallback = value;
}

void MainWindow::setExitCallback(const std::function<void(void)>& value)
{
    _exitCallback = value;
}

std::shared_ptr<MainWindow> MainWindow::create(const std::shared_ptr<Core::Context>& context)
{
    auto out = std::shared_ptr<MainWindow>(new MainWindow);
    out->_init(context);
    return out;
}

void MainWindow::_initEvent(Core::Event::Init&)
{
    _actions["Open"]->setText(_getText(DJV_TEXT("Open")));
    _actions["Open"]->setTooltip(_getText(DJV_TEXT("Open a file")));
    _actions["Close"]->setText(_getText(DJV_TEXT("Close")));
    _actions["Close"]->setTooltip(_getText(DJV_TEXT("Close the current file")));
    _actions["Exit"]->setText(_getText(DJV_TEXT("Exit")));
    _actions["Exit"]->setTooltip(_getText(DJV_TEXT("Exit the application")));

    _trianglesLabel->setText(_getText(DJV_TEXT("Triangles")) + ":");
}

void MainWindow::_open()
{
    if (auto context = getContext().lock())
    {
        if (_fileBrowserDialog)
        {
            _fileBrowserDialog->close();
        }
        _fileBrowserDialog = UI::FileBrowser::Dialog::create(context);
        auto io = context->getSystemT<Scene::IO::System>();
        _fileBrowserDialog->setFileExtensions(io->getFileExtensions());
        _fileBrowserDialog->setPath(_fileBrowserPath);
        auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
        _fileBrowserDialog->setCallback(
            [weak](const Core::FileSystem::FileInfo& value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_fileBrowserPath = widget->_fileBrowserDialog->getPath();
                    widget->_fileBrowserDialog->close();
                    widget->_fileBrowserDialog.reset();
                    if (widget->_openCallback)
                    {
                        widget->_openCallback(value);
                    }
                }
            });
        _fileBrowserDialog->setCloseCallback(
            [weak]
            {
                if (auto widget = weak.lock())
                {
                    widget->_fileBrowserPath = widget->_fileBrowserDialog->getPath();
                    widget->_fileBrowserDialog->close();
                    widget->_fileBrowserDialog.reset();
                }
            });
        _fileBrowserDialog->show();
    }
}

void Application::_init(const std::vector<std::string>& args)
{
    Desktop::Application::_init(args);

    Scene::SceneSystem::create(shared_from_this());
    UI::UIComponentsSystem::create(shared_from_this());

    _sceneFutureTimer = Core::Time::Timer::create(shared_from_this());
    _sceneFutureTimer->setRepeating(true);

    _mainWindow = MainWindow::create(shared_from_this());
    _mainWindow->setScene(_scene);

    auto weak = std::weak_ptr<Application>(std::dynamic_pointer_cast<Application>(shared_from_this()));
    _mainWindow->setOpenCallback(
        [weak](const Core::FileSystem::FileInfo& value)
        {
            if (auto app = weak.lock())
            {
                app->_open(value);
            }
        });
    _mainWindow->setExitCallback(
        [weak]
        {
            if (auto app = weak.lock())
            {
                app->exit();
            }
        });

    _mainWindow->show();
}

Application::Application()
{}

Application::~Application()
{}

std::shared_ptr<Application> Application::create(const std::vector<std::string>& args)
{
    auto out = std::shared_ptr<Application>(new Application);
    out->_init(args);
    return out;
}

void Application::_open(const Core::FileSystem::FileInfo& fileInfo)
{
    if (_scene)
    {
        _mainWindow->setScene(nullptr);
        _scene.reset();
    }
    if (!fileInfo.isEmpty())
    {
        try
        {
            auto io = getSystemT<Scene::IO::System>();
            _read = io->read(fileInfo);
            _sceneFuture = _read->getScene();
            auto weak = std::weak_ptr<Application>(std::dynamic_pointer_cast<Application>(shared_from_this()));
            _sceneFutureTimer->start(
                Core::Time::getMilliseconds(Core::Time::TimerValue::Medium),
                [weak](float)
                {
                    if (auto app = weak.lock())
                    {
                        if (app->_sceneFuture.valid())
                        {
                            try
                            {
                                app->_scene = app->_sceneFuture.get();
                                app->_sceneFutureTimer->stop();
                                app->_mainWindow->setScene(app->_scene);
                            }
                            catch (const std::exception& e)
                            {
                                auto logSystem = app->getSystemT<Core::LogSystem>();
                                logSystem->log("SceneViewExample", e.what(), Core::LogLevel::Error);
                            }
                        }
                    }
                });
        }
        catch (const std::exception& e)
        {
            auto logSystem = getSystemT<Core::LogSystem>();
            logSystem->log("SceneViewExample", e.what(), Core::LogLevel::Error);
        }
    }
}

int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        std::vector<std::string> args;
        for (int i = 0; i < argc; ++i)
        {
            args.push_back(argv[i]);
        }
        r = Application::create(args)->run();
    }
    catch (const std::exception & e)
    {
        std::cout << Core::Error::format(e) << std::endl;
    }
    return r;
}
