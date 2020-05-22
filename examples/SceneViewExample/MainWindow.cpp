// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include "MainWindow.h"

#include "CameraWidget.h"
#include "InfoWidget.h"
#include "RenderWidget.h"
#include "SettingsWidget.h"

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

    _actions["Tools"]["Settings"] = UI::Action::create();
    _actions["Tools"]["Settings"]->setButtonType(UI::ButtonType::Toggle);
    _actions["Tools"]["Settings"]->setIcon("djvIconSettings");
    _actions["Tools"]["Settings"]->setShortcut(GLFW_KEY_T, UI::Shortcut::getSystemModifier());

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
    toolsMenu->addAction(_actions["Tools"]["Settings"]);

    auto menuBar = UI::MenuBar::create(context);
    menuBar->setBackgroundRole(UI::ColorRole::OverlayLight);
    menuBar->addChild(fileMenu);
    menuBar->addChild(viewMenu);
    menuBar->addChild(sceneMenu);
    menuBar->addChild(toolsMenu);

    _fileInfoLabel = UI::Label::create(context);
    _fileInfoLabel->setTextHAlign(UI::TextHAlign::Left);
    _fileInfoLabel->setMargin(UI::Layout::Margin(UI::MetricsRole::Margin, UI::MetricsRole::Margin, UI::MetricsRole::MarginSmall, UI::MetricsRole::MarginSmall));

    _sceneWidget = UI::SceneWidget::create(context);

    _cameraWidget = CameraWidget::create(context);
    _renderWidget = RenderWidget::create(context);
    _infoWidget = InfoWidget::create(context);
    auto settingsWidget = SettingsWidget::create(context);
    settingsWidget->addChild(_cameraWidget);
    settingsWidget->addChild(_renderWidget);
    settingsWidget->addChild(_infoWidget);
    _settingsDrawer = UI::Drawer::create(UI::Side::Right, context);
    _settingsDrawer->addChild(settingsWidget);

    auto toolBar = UI::ToolBar::create(context);
    toolBar->setBackgroundRole(UI::ColorRole::OverlayLight);
    toolBar->addChild(_fileInfoLabel);
    toolBar->setStretch(_fileInfoLabel, UI::RowStretch::Expand);
    toolBar->addSeparator();
    toolBar->addAction(_actions["File"]["Open"]);
    toolBar->addAction(_actions["File"]["Close"]);
    toolBar->addSeparator();
    toolBar->addAction(_actions["View"]["Frame"]);
    toolBar->addSeparator();
    toolBar->addAction(_actions["Tools"]["Settings"]);

    addChild(_sceneWidget);
    auto vLayout = UI::VerticalLayout::create(context);
    vLayout->setSpacing(UI::MetricsRole::None);
    auto hLayout = UI::HorizontalLayout::create(context);
    hLayout->setSpacing(UI::MetricsRole::None);
    hLayout->addChild(menuBar);
    hLayout->addSeparator();
    hLayout->addChild(toolBar);
    hLayout->setStretch(toolBar, UI::RowStretch::Expand);
    vLayout->addChild(hLayout);
    vLayout->addChild(_settingsDrawer);
    vLayout->setStretch(_settingsDrawer, UI::RowStretch::Expand);
    addChild(vLayout);

    auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
    _cameraWidget->setCameraDataCallback(
        [weak](const Scene::PolarCameraData& value)
        {
            if (auto widget = weak.lock())
            {
                widget->_sceneWidget->setCameraData(value);
            }
        });

    _renderWidget->setRenderOptionsCallback(
        [weak](const UI::SceneRenderOptions& value)
        {
            if (auto widget = weak.lock())
            {
                widget->_sceneWidget->setRenderOptions(value);
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

    _actionObservers["Tools"]["Settings"] = Core::ValueObserver<bool>::create(
        _actions["Tools"]["Settings"]->observeChecked(),
        [weak](bool value)
        {
            if (auto widget = weak.lock())
            {
                widget->_settingsDrawer->setOpen(value);
            }
        });

    _cameraDataObserver = Core::ValueObserver<Scene::PolarCameraData>::create(
        _sceneWidget->observeCameraData(),
        [weak](const Scene::PolarCameraData& value)
        {
            if (auto widget = weak.lock())
            {
                widget->_cameraWidget->setCameraData(value);
            }
        });

    _renderOptionsObserver = Core::ValueObserver<UI::SceneRenderOptions>::create(
        _sceneWidget->observeRenderOptions(),
        [weak](const UI::SceneRenderOptions& value)
        {
            if (auto widget = weak.lock())
            {
                widget->_renderWidget->setRenderOptions(value);
            }
        });

    _bboxObserver = Core::ValueObserver<Core::BBox3f>::create(
        _sceneWidget->observeBBox(),
        [weak](const Core::BBox3f& value)
        {
            if (auto widget = weak.lock())
            {
                widget->_infoWidget->setBBox(value);
            }
        });

    _primitivesCountObserver = Core::ValueObserver<size_t>::create(
        _sceneWidget->observePrimitivesCount(),
        [weak](size_t value)
        {
            if (auto widget = weak.lock())
            {
                widget->_infoWidget->setPrimitivesCount(value);
            }
        });

    _pointCountObserver = Core::ValueObserver<size_t>::create(
        _sceneWidget->observePointCount(),
        [weak](size_t value)
        {
            if (auto widget = weak.lock())
            {
                widget->_infoWidget->setPointCount(value);
            }
        });

    _statsTimer = Core::Time::Timer::create(context);
    _statsTimer->setRepeating(true);
    auto contextWeak = std::weak_ptr<Core::Context>(context);
    _statsTimer->start(
        Core::Time::getTime(Core::Time::TimerValue::Slow),
        [weak, contextWeak](const std::chrono::steady_clock::time_point&, const Core::Time::Duration&)
        {
            if (auto context = contextWeak.lock())
            {
                if (auto widget = weak.lock())
                {
                    widget->_infoWidget->setFPS(context->getFPSAverage());
                }
            }
        });
}

MainWindow::MainWindow()
{}

MainWindow::~MainWindow()
{}

void MainWindow::setScene(
    const djv::Core::FileSystem::FileInfo& fileInfo,
    const std::shared_ptr<Scene::Scene>& value)
{
    _fileInfoLabel->setText(fileInfo.getFileName());
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
    _actions["File"]["Open"]->setText(_getText(DJV_TEXT("action_file_open")));
    _actions["File"]["Open"]->setTooltip(_getText(DJV_TEXT("action_file_open_tooltip")));
    _actions["File"]["Reload"]->setText(_getText(DJV_TEXT("action_file_reload")));
    _actions["File"]["Reload"]->setTooltip(_getText(DJV_TEXT("action_file_reload_tooltip")));
    _actions["File"]["Close"]->setText(_getText(DJV_TEXT("action_file_close")));
    _actions["File"]["Close"]->setTooltip(_getText(DJV_TEXT("action_file_close_tooltip")));
    _actions["File"]["Exit"]->setText(_getText(DJV_TEXT("action_file_exit")));
    _actions["File"]["Exit"]->setTooltip(_getText(DJV_TEXT("action_file_exit_tooltip")));

    _actions["View"]["Frame"]->setText(_getText(DJV_TEXT("action_view_frame")));
    _actions["View"]["Frame"]->setTooltip(_getText(DJV_TEXT("action_view_frame_tooltip")));

    _actions["Scene"]["Rotate+X"]->setText(_getText(DJV_TEXT("action_scene_rotate_x_90")));
    _actions["Scene"]["Rotate+X"]->setTooltip(_getText(DJV_TEXT("action_scene_rotate_x_90_tooltip")));
    _actions["Scene"]["Rotate-X"]->setText(_getText(DJV_TEXT("action_scene_rotate_x_-90")));
    _actions["Scene"]["Rotate-X"]->setTooltip(_getText(DJV_TEXT("action_scene_rotate_x_-90_tooltip")));
    _actions["Scene"]["Rotate+Y"]->setText(_getText(DJV_TEXT("action_scene_rotate_y_90")));
    _actions["Scene"]["Rotate+Y"]->setTooltip(_getText(DJV_TEXT("action_scene_rotate_y_90_tooltip")));
    _actions["Scene"]["Rotate-Y"]->setText(_getText(DJV_TEXT("action_scene_rotate_y_-90")));
    _actions["Scene"]["Rotate-Y"]->setTooltip(_getText(DJV_TEXT("action_scene_rotate_y_-90_tooltip")));
    _actions["Scene"]["Rotate+Z"]->setText(_getText(DJV_TEXT("action_scene_rotate_z_90")));
    _actions["Scene"]["Rotate+Z"]->setTooltip(_getText(DJV_TEXT("action_scene_rotate_z_90_tooltip")));
    _actions["Scene"]["Rotate-Z"]->setText(_getText(DJV_TEXT("action_scene_rotate_z_-90")));
    _actions["Scene"]["Rotate+Z"]->setTooltip(_getText(DJV_TEXT("action_scene_rotate_z_-90_tooltip")));

    _actions["Tools"]["Settings"]->setText(_getText(DJV_TEXT("action_tool_settings")));
    _actions["Tools"]["Settings"]->setTooltip(_getText(DJV_TEXT("action_tool_settings_tooltip")));
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
                    if (widget->_fileBrowserDialog)
                    {
                        widget->_fileBrowserPath = widget->_fileBrowserDialog->getPath();
                        widget->_fileBrowserDialog->close();
                        widget->_fileBrowserDialog.reset();
                    }
                }
            });
        _fileBrowserDialog->show();
    }
}
