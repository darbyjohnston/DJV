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

#include "MainWindow.h"

#include "CameraTool.h"
#include "InfoTool.h"
#include "RenderTool.h"

#include <djvUI/Menu.h>
#include <djvUI/MenuBar.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Shortcut.h>
#include <djvUI/ToolBar.h>

#include <djvScene/IO.h>
#include <djvScene/Scene.h>

#include <djvCore/Context.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv;

void MainWindow::_init(const std::shared_ptr<Core::Context>& context)
{
    Window::_init(context);

    _actions["File"]["Open"] = UI::Action::create();
    _actions["File"]["Open"]->setIcon("djvIconFileOpen");
    _actions["File"]["Open"]->setShortcut(GLFW_KEY_O, UI::Shortcut::getSystemModifier());
    _actions["File"]["Reload"] = UI::Action::create();
    _actions["File"]["Reload"]->setShortcut(GLFW_KEY_R, UI::Shortcut::getSystemModifier());
    _actions["File"]["Close"] = UI::Action::create();
    _actions["File"]["Close"]->setIcon("djvIconFileClose");
    _actions["File"]["Close"]->setShortcut(GLFW_KEY_E, UI::Shortcut::getSystemModifier());
    _actions["File"]["Exit"] = UI::Action::create();
    _actions["File"]["Exit"]->setShortcut(GLFW_KEY_Q, UI::Shortcut::getSystemModifier());

    _actions["View"]["Frame"] = UI::Action::create();
    _actions["View"]["Frame"]->setIcon("djvIconViewFrame");
    _actions["View"]["Frame"]->setShortcut(GLFW_KEY_F);
    
    _actions["Scene"]["Rotate+X"] = UI::Action::create();
    _actions["Scene"]["Rotate+X"]->setShortcut(GLFW_KEY_X);
    _actions["Scene"]["Rotate-X"] = UI::Action::create();
    _actions["Scene"]["Rotate-X"]->setShortcut(GLFW_KEY_X, GLFW_MOD_SHIFT);
    _actions["Scene"]["Rotate+Y"] = UI::Action::create();
    _actions["Scene"]["Rotate+Y"]->setShortcut(GLFW_KEY_Y);
    _actions["Scene"]["Rotate-Y"] = UI::Action::create();
    _actions["Scene"]["Rotate-Y"]->setShortcut(GLFW_KEY_Y, GLFW_MOD_SHIFT);
    _actions["Scene"]["Rotate+Z"] = UI::Action::create();
    _actions["Scene"]["Rotate+Z"]->setShortcut(GLFW_KEY_Z);
    _actions["Scene"]["Rotate-Z"] = UI::Action::create();
    _actions["Scene"]["Rotate-Z"]->setShortcut(GLFW_KEY_Z, GLFW_MOD_SHIFT);

    _actions["Tools"]["Camera"] = UI::Action::create();
    _actions["Tools"]["Camera"]->setButtonType(UI::ButtonType::Toggle);
    _actions["Tools"]["Camera"]->setShortcut(GLFW_KEY_1);
    _actions["Tools"]["Render"] = UI::Action::create();
    _actions["Tools"]["Render"]->setButtonType(UI::ButtonType::Toggle);
    _actions["Tools"]["Render"]->setShortcut(GLFW_KEY_2);
    _actions["Tools"]["Info"] = UI::Action::create();
    _actions["Tools"]["Info"]->setButtonType(UI::ButtonType::Toggle);
    _actions["Tools"]["Info"]->setShortcut(GLFW_KEY_3);

    for (const auto& i : _actions)
    {
        for (const auto& j : i.second)
        {
            addAction(j.second);
        }
    }

    _sceneRotateActionGroup = UI::ActionGroup::create(UI::ButtonType::Exclusive);
    _sceneRotateActionGroup->addAction(_actions["Scene"]["Rotate+X"]);
    _sceneRotateActionGroup->addAction(_actions["Scene"]["Rotate-X"]);
    _sceneRotateActionGroup->addAction(_actions["Scene"]["Rotate+Y"]);
    _sceneRotateActionGroup->addAction(_actions["Scene"]["Rotate-Y"]);
    _sceneRotateActionGroup->addAction(_actions["Scene"]["Rotate+Z"]);
    _sceneRotateActionGroup->addAction(_actions["Scene"]["Rotate-Z"]);

    auto fileMenu = UI::Menu::create(context);
    fileMenu->setText("File");
    fileMenu->addAction(_actions["File"]["Open"]);
    fileMenu->addAction(_actions["File"]["Reload"]);
    fileMenu->addAction(_actions["File"]["Close"]);
    fileMenu->addSeparator();
    fileMenu->addAction(_actions["File"]["Exit"]);

    auto viewMenu = UI::Menu::create(context);
    viewMenu->setText("View");
    viewMenu->addAction(_actions["View"]["Frame"]);

    auto sceneMenu = UI::Menu::create(context);
    sceneMenu->setText("Scene");
    sceneMenu->addAction(_actions["Scene"]["Rotate+X"]);
    sceneMenu->addAction(_actions["Scene"]["Rotate-X"]);
    sceneMenu->addAction(_actions["Scene"]["Rotate+Y"]);
    sceneMenu->addAction(_actions["Scene"]["Rotate-Y"]);
    sceneMenu->addAction(_actions["Scene"]["Rotate+Z"]);
    sceneMenu->addAction(_actions["Scene"]["Rotate-Z"]);

    auto toolsMenu = UI::Menu::create(context);
    toolsMenu->setText("Tools");
    toolsMenu->addAction(_actions["Tools"]["Camera"]);
    toolsMenu->addAction(_actions["Tools"]["Render"]);
    toolsMenu->addAction(_actions["Tools"]["Info"]);

    auto menuBar = UI::MenuBar::create(context);
    menuBar->setBackgroundRole(UI::ColorRole::OverlayLight);
    menuBar->addChild(fileMenu);
    menuBar->addChild(viewMenu);
    menuBar->addChild(sceneMenu);
    menuBar->addChild(toolsMenu);

    _sceneWidget = UI::SceneWidget::create(context);

    _cameraTool = CameraTool::create(context);
    _cameraTool->hide();

    _renderTool = RenderTool::create(context);
    _renderTool->hide();

    _infoTool = InfoTool::create(context);
    _infoTool->hide();

    _mdiCanvas = UI::MDI::Canvas::create(context);
    _mdiCanvas->addChild(_cameraTool);
    _mdiCanvas->addChild(_renderTool);
    _mdiCanvas->addChild(_infoTool);

    auto toolBar = UI::ToolBar::create(context);
    toolBar->setBackgroundRole(UI::ColorRole::OverlayLight);
    toolBar->addAction(_actions["File"]["Open"]);
    toolBar->addAction(_actions["File"]["Close"]);
    toolBar->addSeparator();
    toolBar->addAction(_actions["View"]["Frame"]);

    addChild(_sceneWidget);
    addChild(_mdiCanvas);
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
    addChild(vLayout);

    auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
    _cameraTool->setCameraInfoCallback(
        [weak](const UI::CameraInfo& value)
        {
            if (auto widget = weak.lock())
            {
                widget->_sceneWidget->setCameraInfo(value);
            }
        });
    _cameraTool->setCloseCallback(
        [weak]
        {
            if (auto widget = weak.lock())
            {
                widget->_actions["Tools"]["Camera"]->setChecked(false);
            }
        });

    _renderTool->setCloseCallback(
        [weak]
        {
            if (auto widget = weak.lock())
            {
                widget->_actions["Tools"]["Render"]->setChecked(false);
            }
        });

    _infoTool->setCloseCallback(
        [weak]
        {
            if (auto widget = weak.lock())
            {
                widget->_actions["Tools"]["Info"]->setChecked(false);
            }
        });

    _sceneRotateActionGroup->setExclusiveCallback(
        [weak](int value)
        {
            if (auto widget = weak.lock())
            {
                widget->_sceneWidget->setSceneRotate(static_cast<UI::SceneRotate>(value + 1));
                widget->_sceneWidget->frameView();
            }
        });
    
    _actionObservers["File"]["Open"] = Core::ValueObserver<bool>::create(
        _actions["File"]["Open"]->observeClicked(),
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

    _actionObservers["File"]["Reload"] = Core::ValueObserver<bool>::create(
        _actions["File"]["Reload"]->observeClicked(),
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

    _actionObservers["File"]["Close"] = Core::ValueObserver<bool>::create(
        _actions["File"]["Close"]->observeClicked(),
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

    _actionObservers["File"]["Exit"] = Core::ValueObserver<bool>::create(
        _actions["File"]["Exit"]->observeClicked(),
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

    _actionObservers["View"]["Frame"] = Core::ValueObserver<bool>::create(
        _actions["View"]["Frame"]->observeClicked(),
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

    _actionObservers["Tools"]["Camera"] = Core::ValueObserver<bool>::create(
        _actions["Tools"]["Camera"]->observeChecked(),
        [weak](bool value)
        {
            if (auto widget = weak.lock())
            {
                widget->_cameraTool->setVisible(value);
            }
        });

    _actionObservers["Tools"]["Render"] = Core::ValueObserver<bool>::create(
        _actions["Tools"]["Render"]->observeChecked(),
        [weak](bool value)
        {
            if (auto widget = weak.lock())
            {
                widget->_renderTool->setVisible(value);
            }
        });

    _actionObservers["Tools"]["Info"] = Core::ValueObserver<bool>::create(
        _actions["Tools"]["Info"]->observeChecked(),
        [weak](bool value)
        {
            if (auto widget = weak.lock())
            {
                widget->_infoTool->setVisible(value);
            }
        });

    _cameraInfoObserver = Core::ValueObserver<UI::CameraInfo>::create(
        _sceneWidget->observeCameraInfo(),
        [weak](const UI::CameraInfo& value)
        {
            if (auto widget = weak.lock())
            {
                widget->_cameraTool->setCameraInfo(value);
            }
        });

    _bboxObserver = Core::ValueObserver<Core::BBox3f>::create(
        _sceneWidget->observeBBox(),
        [weak](const Core::BBox3f& value)
        {
            if (auto widget = weak.lock())
            {
                widget->_infoTool->setBBox(value);
            }
        });

    _primitivesCountObserver = Core::ValueObserver<size_t>::create(
        _sceneWidget->observePrimitivesCount(),
        [weak](size_t value)
        {
            if (auto widget = weak.lock())
            {
                widget->_infoTool->setPrimitivesCount(value);
            }
        });

    _triangleCountObserver = Core::ValueObserver<size_t>::create(
        _sceneWidget->observeTriangleCount(),
        [weak](size_t value)
        {
            if (auto widget = weak.lock())
            {
                widget->_infoTool->setTriangleCount(value);
            }
        });

    _statsTimer = Core::Time::Timer::create(context);
    _statsTimer->setRepeating(true);
    auto contextWeak = std::weak_ptr<Core::Context>(context);
    _statsTimer->start(
        Core::Time::getMilliseconds(Core::Time::TimerValue::Slow),
        [weak, contextWeak](float)
        {
            if (auto context = contextWeak.lock())
            {
                if (auto widget = weak.lock())
                {
                    widget->_infoTool->setFPS(context->getFPSAverage());
                }
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
    _actions["File"]["Open"]->setText(_getText(DJV_TEXT("Open")));
    _actions["File"]["Open"]->setTooltip(_getText(DJV_TEXT("Open a file")));
    _actions["File"]["Reload"]->setText(_getText(DJV_TEXT("Reload")));
    _actions["File"]["Reload"]->setTooltip(_getText(DJV_TEXT("Reload the current file")));
    _actions["File"]["Close"]->setText(_getText(DJV_TEXT("Close")));
    _actions["File"]["Close"]->setTooltip(_getText(DJV_TEXT("Close the current file")));
    _actions["File"]["Exit"]->setText(_getText(DJV_TEXT("Exit")));
    _actions["File"]["Exit"]->setTooltip(_getText(DJV_TEXT("Exit the application")));

    _actions["View"]["Frame"]->setText(_getText(DJV_TEXT("Frame")));
    _actions["View"]["Frame"]->setTooltip(_getText(DJV_TEXT("Frame the view")));

    _actions["Scene"]["Rotate+X"]->setText(_getText(DJV_TEXT("Rotate X 90")));
    _actions["Scene"]["Rotate+X"]->setTooltip(_getText(DJV_TEXT("Rotate the scene +90 degrees in X")));
    _actions["Scene"]["Rotate-X"]->setText(_getText(DJV_TEXT("Rotate X -90")));
    _actions["Scene"]["Rotate-X"]->setTooltip(_getText(DJV_TEXT("Rotate the scene -90 degrees in X")));
    _actions["Scene"]["Rotate+Y"]->setText(_getText(DJV_TEXT("Rotate Y 90")));
    _actions["Scene"]["Rotate+Y"]->setTooltip(_getText(DJV_TEXT("Rotate the scene +90 degrees in Y")));
    _actions["Scene"]["Rotate-Y"]->setText(_getText(DJV_TEXT("Rotate Y -90")));
    _actions["Scene"]["Rotate-Y"]->setTooltip(_getText(DJV_TEXT("Rotate the scene -90 degrees in Y")));
    _actions["Scene"]["Rotate+Z"]->setText(_getText(DJV_TEXT("Rotate Z 90")));
    _actions["Scene"]["Rotate+Z"]->setTooltip(_getText(DJV_TEXT("Rotate the scene +90 degrees in Z")));
    _actions["Scene"]["Rotate-Z"]->setText(_getText(DJV_TEXT("Rotate Z -90")));
    _actions["Scene"]["Rotate+Z"]->setTooltip(_getText(DJV_TEXT("Rotate the scene -90 degrees in Z")));

    _actions["Tools"]["Camera"]->setText(_getText(DJV_TEXT("Camera")));
    _actions["Tools"]["Camera"]->setTooltip(_getText(DJV_TEXT("Show the camera tool")));
    _actions["Tools"]["Render"]->setText(_getText(DJV_TEXT("Render")));
    _actions["Tools"]["Render"]->setTooltip(_getText(DJV_TEXT("Show the render tool")));
    _actions["Tools"]["Info"]->setText(_getText(DJV_TEXT("Information")));
    _actions["Tools"]["Info"]->setTooltip(_getText(DJV_TEXT("Show the information tool")));
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
