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

#include <djvScene/Light.h>
#include <djvScene/SceneSystem.h>

#include <djvCore/Error.h>
#include <djvCore/LogSystem.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/Timer.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace djv;

void MainWindow::_init(const std::shared_ptr<Core::Context>& context)
{
    Window::_init(context);

    _actions["Open"] = UI::Action::create();
    _actions["Open"]->setIcon("djvIconFileOpen");
    _actions["Open"]->setShortcut(GLFW_KEY_O, UI::Shortcut::getSystemModifier());
    _actions["Reload"] = UI::Action::create();
    _actions["Reload"]->setShortcut(GLFW_KEY_R, UI::Shortcut::getSystemModifier());
    _actions["Close"] = UI::Action::create();
    _actions["Close"]->setIcon("djvIconFileClose");
    _actions["Close"]->setShortcut(GLFW_KEY_E, UI::Shortcut::getSystemModifier());
    _actions["Exit"] = UI::Action::create();
    _actions["Exit"]->setShortcut(GLFW_KEY_Q, UI::Shortcut::getSystemModifier());
    _actions["Frame"] = UI::Action::create();
    _actions["Frame"]->setIcon("djvIconViewFrame");
    _actions["Frame"]->setShortcut(GLFW_KEY_F);
    _actions["SceneRotateX"] = UI::Action::create();
    _actions["SceneRotateX"]->setShortcut(GLFW_KEY_X);
    _actions["SceneRotate-X"] = UI::Action::create();
    _actions["SceneRotate-X"]->setShortcut(GLFW_KEY_X, GLFW_MOD_SHIFT);
    _actions["SceneRotateY"] = UI::Action::create();
    _actions["SceneRotateY"]->setShortcut(GLFW_KEY_Y);
    _actions["SceneRotate-Y"] = UI::Action::create();
    _actions["SceneRotate-Y"]->setShortcut(GLFW_KEY_Y, GLFW_MOD_SHIFT);
    _actions["SceneRotateZ"] = UI::Action::create();
    _actions["SceneRotateZ"]->setShortcut(GLFW_KEY_Z);
    _actions["SceneRotate-Z"] = UI::Action::create();
    _actions["SceneRotate-Z"]->setShortcut(GLFW_KEY_Z, GLFW_MOD_SHIFT);
    for (const auto& i : _actions)
    {
        addAction(i.second);
    }

    _sceneRotateActionGroup = UI::ActionGroup::create(UI::ButtonType::Exclusive);
    _sceneRotateActionGroup->addAction(_actions["SceneRotateX"]);
    _sceneRotateActionGroup->addAction(_actions["SceneRotate-X"]);
    _sceneRotateActionGroup->addAction(_actions["SceneRotateY"]);
    _sceneRotateActionGroup->addAction(_actions["SceneRotate-Y"]);
    _sceneRotateActionGroup->addAction(_actions["SceneRotateZ"]);
    _sceneRotateActionGroup->addAction(_actions["SceneRotate-Z"]);

    auto fileMenu = UI::Menu::create(context);
    fileMenu->setText("File");
    fileMenu->addAction(_actions["Open"]);
    fileMenu->addAction(_actions["Reload"]);
    fileMenu->addAction(_actions["Close"]);
    fileMenu->addSeparator();
    fileMenu->addAction(_actions["Exit"]);

    auto viewMenu = UI::Menu::create(context);
    viewMenu->setText("View");
    viewMenu->addAction(_actions["Frame"]);

    auto sceneMenu = UI::Menu::create(context);
    sceneMenu->setText("Scene");
    sceneMenu->addAction(_actions["SceneRotateX"]);
    sceneMenu->addAction(_actions["SceneRotate-X"]);
    sceneMenu->addAction(_actions["SceneRotateY"]);
    sceneMenu->addAction(_actions["SceneRotate-Y"]);
    sceneMenu->addAction(_actions["SceneRotateZ"]);
    sceneMenu->addAction(_actions["SceneRotate-Z"]);

    auto menuBar = UI::MenuBar::create(context);
    menuBar->setBackgroundRole(UI::ColorRole::OverlayLight);
    menuBar->addChild(fileMenu);
    menuBar->addChild(viewMenu);
    menuBar->addChild(sceneMenu);

    _sceneWidget = UI::SceneWidget::create(context);

    _infoLabel = UI::Label::create(context);

    auto toolBar = UI::ToolBar::create(context);
    toolBar->setBackgroundRole(UI::ColorRole::OverlayLight);
    toolBar->addAction(_actions["Open"]);
    toolBar->addAction(_actions["Close"]);
    toolBar->addSeparator();
    toolBar->addAction(_actions["Frame"]);

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
    hLayout->addChild(_infoLabel);
    addChild(hLayout);
    addChild(vLayout);

    auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
    _sceneRotateActionGroup->setExclusiveCallback(
        [weak](int value)
        {
            if (auto widget = weak.lock())
            {
                widget->_sceneWidget->setSceneRotate(static_cast<UI::SceneRotate>(value + 1));
                widget->_sceneWidget->frameView();
            }
        });
    
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

    _actionObservers["Reload"] = Core::ValueObserver<bool>::create(
        _actions["Reload"]->observeClicked(),
        [weak](bool value)
        {
            if (value)
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_reloadCallback)
                    {
                        widget->_reloadCallback();
                    }
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

    _actionObservers["Frame"] = Core::ValueObserver<bool>::create(
        _actions["Frame"]->observeClicked(),
        [weak](bool value)
        {
            if (value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_sceneWidget->frameView();
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
                widget->_textUpdate();
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
    _sceneWidget->frameView();
}

void MainWindow::setOpenCallback(const std::function<void(const Core::FileSystem::FileInfo)>& value)
{
    _openCallback = value;
}

void MainWindow::setReloadCallback(const std::function<void(void)>& value)
{
    _reloadCallback = value;
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
    _textUpdate();
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

void MainWindow::_textUpdate()
{
    _actions["Open"]->setText(_getText(DJV_TEXT("Open")));
    _actions["Open"]->setTooltip(_getText(DJV_TEXT("Open a file")));
    _actions["Reload"]->setText(_getText(DJV_TEXT("Reload")));
    _actions["Reload"]->setTooltip(_getText(DJV_TEXT("Reload the current file")));
    _actions["Close"]->setText(_getText(DJV_TEXT("Close")));
    _actions["Close"]->setTooltip(_getText(DJV_TEXT("Close the current file")));
    _actions["Exit"]->setText(_getText(DJV_TEXT("Exit")));
    _actions["Exit"]->setTooltip(_getText(DJV_TEXT("Exit the application")));
    _actions["Frame"]->setText(_getText(DJV_TEXT("Frame")));
    _actions["Frame"]->setTooltip(_getText(DJV_TEXT("Frame the view")));
    _actions["SceneRotateX"]->setText(_getText(DJV_TEXT("Rotate X 90")));
    _actions["SceneRotateX"]->setTooltip(_getText(DJV_TEXT("Rotate the scene +90 degrees in X")));
    _actions["SceneRotate-X"]->setText(_getText(DJV_TEXT("Rotate X -90")));
    _actions["SceneRotate-X"]->setTooltip(_getText(DJV_TEXT("Rotate the scene -90 degrees in X")));
    _actions["SceneRotateY"]->setText(_getText(DJV_TEXT("Rotate Y 90")));
    _actions["SceneRotateY"]->setTooltip(_getText(DJV_TEXT("Rotate the scene +90 degrees in Y")));
    _actions["SceneRotate-Y"]->setText(_getText(DJV_TEXT("Rotate Y -90")));
    _actions["SceneRotate-Y"]->setTooltip(_getText(DJV_TEXT("Rotate the scene -90 degrees in Y")));
    _actions["SceneRotateZ"]->setText(_getText(DJV_TEXT("Rotate Z 90")));
    _actions["SceneRotateZ"]->setTooltip(_getText(DJV_TEXT("Rotate the scene +90 degrees in Z")));
    _actions["SceneRotate-Z"]->setText(_getText(DJV_TEXT("Rotate Z -90")));
    _actions["SceneRotateZ"]->setTooltip(_getText(DJV_TEXT("Rotate the scene -90 degrees in Z")));

    {
        std::stringstream ss;
        ss << "Primitives: " << _sceneWidget->getPrimitivesCount() << ", triangles: " << _sceneWidget->getTriangleCount();
        _infoLabel->setText(ss.str());
    }
}

void Application::_init(const std::vector<std::string>& args)
{
    Desktop::Application::_init(args);

    std::vector<Core::FileSystem::Path> cmdlinePaths;
    for (auto arg = args.begin() + 1; arg != args.end(); ++arg)
    {
        cmdlinePaths.push_back(Core::FileSystem::Path(*arg));
    }

    Scene::SceneSystem::create(shared_from_this());
    UI::UIComponentsSystem::create(shared_from_this());

    try
    {
        auto resourceSystem = getSystemT<Core::ResourceSystem>();
        auto io = getSystemT<Scene::IO::System>();
        _pointLightSceneRead = io->read(Core::FileSystem::Path(resourceSystem->getPath(Core::FileSystem::ResourcePath::Models), "djvPointLight.3dm"));
        _pointLightSceneFuture = _pointLightSceneRead->getScene();
    }
    catch (const std::exception& e)
    {
        auto logSystem = getSystemT<Core::LogSystem>();
        logSystem->log("SceneViewExample", e.what(), Core::LogLevel::Error);
    }

    _futureTimer = Core::Time::Timer::create(shared_from_this());
    _futureTimer->setRepeating(true);

    _mainWindow = MainWindow::create(shared_from_this());

    auto weak = std::weak_ptr<Application>(std::dynamic_pointer_cast<Application>(shared_from_this()));
    _mainWindow->setOpenCallback(
        [weak](const Core::FileSystem::FileInfo& value)
        {
            if (auto app = weak.lock())
            {
                app->_open(value);
            }
        });
    _mainWindow->setReloadCallback(
        [weak]
        {
            if (auto app = weak.lock())
            {
                app->_open(app->_fileInfo);
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

    for (const auto& i : cmdlinePaths)
    {
        _open(i);
    }

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
    _fileInfo = fileInfo;
    if (!_fileInfo.isEmpty())
    {
        try
        {
            auto io = getSystemT<Scene::IO::System>();
            _sceneRead = io->read(_fileInfo);
            _sceneFuture = _sceneRead->getScene();
            auto weak = std::weak_ptr<Application>(std::dynamic_pointer_cast<Application>(shared_from_this()));
            _futureTimer->start(
                Core::Time::getMilliseconds(Core::Time::TimerValue::Medium),
                [weak](float)
                {
                    if (auto app = weak.lock())
                    {
                        try
                        {
                            if (app->_pointLightSceneFuture.valid())
                            {
                                app->_pointLightScene = app->_pointLightSceneFuture.get();
                                for (const auto& i : app->_pointLightScene->getPrimitives())
                                {
                                    app->_pointLightPrimitives.push_back(i);
                                }
                            }
                            if (app->_sceneFuture.valid() &&
                                app->_sceneFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                            {
                                app->_scene = app->_sceneFuture.get();
                                //for (const auto& i : app->_scene->getPrimitives())
                                //{
                                //    app->_createPointLights(i);
                                //}
                                app->_mainWindow->setScene(app->_scene);
                            }
                        }
                        catch (const std::exception& e)
                        {
                            auto logSystem = app->getSystemT<Core::LogSystem>();
                            logSystem->log("SceneViewExample", e.what(), Core::LogLevel::Error);
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

void Application::_createPointLights(const std::shared_ptr<djv::Scene::IPrimitive>& primitive)
{
    if (auto pointLight = std::dynamic_pointer_cast<Scene::PointLight>(primitive))
    {
        auto instance = Scene::InstancePrimitive::create();
        instance->setInstances(_pointLightPrimitives);
        pointLight->addChild(instance);
    }
    for (const auto& i : primitive->getPrimitives())
    {
        _createPointLights(i);
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
