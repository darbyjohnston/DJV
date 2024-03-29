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
#include <djvUI/ShortcutData.h>
#include <djvUI/ToolButton.h>

#include <djvScene3D/IO.h>
#include <djvScene3D/Scene.h>

#include <djvSystem/Context.h>
#include <djvSystem/Timer.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv;

void MainWindow::_init(const std::shared_ptr<System::Context>& context)
{
    Window::_init(context);

    _actions["File/Open"] = UI::Action::create();
    _actions["File/Open"]->setIcon("djvIconFileOpen");
    _actions["File/Open"]->setShortcut(GLFW_KEY_O, UI::getSystemModifier());
    _actions["File/Reload"] = UI::Action::create();
    _actions["File/Reload"]->setShortcut(GLFW_KEY_R, UI::getSystemModifier());
    _actions["File/Close"] = UI::Action::create();
    _actions["File/Close"]->setIcon("djvIconFileClose");
    _actions["File/Close"]->setShortcut(GLFW_KEY_E, UI::getSystemModifier());
    _actions["File/Exit"] = UI::Action::create();
    _actions["File/Exit"]->setShortcut(GLFW_KEY_Q, UI::getSystemModifier());

    _actions["View/Frame"] = UI::Action::create();
    _actions["View/Frame"]->setIcon("djvIconViewFrame");
    _actions["View/Frame"]->setShortcut(GLFW_KEY_F);
    
    _actions["Scene/Rotate+X"] = UI::Action::create();
    _actions["Scene/Rotate+X"]->setShortcut(GLFW_KEY_X);
    _actions["Scene/Rotate-X"] = UI::Action::create();
    _actions["Scene/Rotate-X"]->setShortcut(GLFW_KEY_X, GLFW_MOD_SHIFT);
    _actions["Scene/Rotate+Y"] = UI::Action::create();
    _actions["Scene/Rotate+Y"]->setShortcut(GLFW_KEY_Y);
    _actions["Scene/Rotate-Y"] = UI::Action::create();
    _actions["Scene/Rotate-Y"]->setShortcut(GLFW_KEY_Y, GLFW_MOD_SHIFT);
    _actions["Scene/Rotate+Z"] = UI::Action::create();
    _actions["Scene/Rotate+Z"]->setShortcut(GLFW_KEY_Z);
    _actions["Scene/Rotate-Z"] = UI::Action::create();
    _actions["Scene/Rotate-Z"]->setShortcut(GLFW_KEY_Z, GLFW_MOD_SHIFT);

    _actions["Tools/Settings"] = UI::Action::create();
    _actions["Tools/Settings"]->setButtonType(UI::ButtonType::Toggle);
    _actions["Tools/Settings"]->setIcon("djvIconSettings");
    _actions["Tools/Settings"]->setShortcut(GLFW_KEY_T, UI::getSystemModifier());

    for (const auto& i : _actions)
    {
        addAction(i.second);
    }

    _sceneRotateActionGroup = UI::ActionGroup::create(UI::ButtonType::Exclusive);
    _sceneRotateActionGroup->setActions({
        _actions["Scene/Rotate+X"],
        _actions["Scene/Rotate-X"],
        _actions["Scene/Rotate+Y"],
        _actions["Scene/Rotate-Y"],
        _actions["Scene/Rotate+Z"],
        _actions["Scene/Rotate-Z"] });

    auto fileMenu = UI::Menu::create(context);
    fileMenu->setText("File");
    fileMenu->addAction(_actions["File/Open"]);
    fileMenu->addAction(_actions["File/Reload"]);
    fileMenu->addAction(_actions["File/Close"]);
    fileMenu->addSeparator();
    fileMenu->addAction(_actions["File/Exit"]);

    auto viewMenu = UI::Menu::create(context);
    viewMenu->setText("View");
    viewMenu->addAction(_actions["View/Frame"]);

    auto sceneMenu = UI::Menu::create(context);
    sceneMenu->setText("Scene");
    sceneMenu->addAction(_actions["Scene/Rotate+X"]);
    sceneMenu->addAction(_actions["Scene/Rotate-X"]);
    sceneMenu->addAction(_actions["Scene/Rotate+Y"]);
    sceneMenu->addAction(_actions["Scene/Rotate-Y"]);
    sceneMenu->addAction(_actions["Scene/Rotate+Z"]);
    sceneMenu->addAction(_actions["Scene/Rotate-Z"]);

    auto toolsMenu = UI::Menu::create(context);
    toolsMenu->setText("Tools");
    toolsMenu->addAction(_actions["Tools/Settings"]);

    auto openButton = UI::ToolButton::create(context);
    openButton->addAction(_actions["File/Open"]);
    auto closeButton = UI::ToolButton::create(context);
    closeButton->addAction(_actions["File/Close"]);
    auto frameButton = UI::ToolButton::create(context);
    frameButton->addAction(_actions["View/Frame"]);
    auto settingsButton = UI::ToolButton::create(context);
    settingsButton->addAction(_actions["Tools/Settings"]);

    auto menuBar = UI::MenuBar::create(context);
    menuBar->addChild(fileMenu);
    menuBar->addChild(viewMenu);
    menuBar->addChild(sceneMenu);
    menuBar->addChild(toolsMenu);
    menuBar->addSeparator(UI::Side::Right);
    _fileInfoLabel = UI::Text::Label::create(context);
    _fileInfoLabel->setTextHAlign(UI::TextHAlign::Left);
    _fileInfoLabel->setTextElide(40);
    _fileInfoLabel->setMargin(UI::Layout::Margin(UI::MetricsRole::Margin, UI::MetricsRole::Margin, UI::MetricsRole::MarginSmall, UI::MetricsRole::MarginSmall));
    menuBar->addChild(_fileInfoLabel);
    menuBar->setStretch(_fileInfoLabel, UI::Side::Right);
    menuBar->addSeparator(UI::Side::Right);
    menuBar->addChild(openButton);
    menuBar->addChild(closeButton);
    menuBar->addSeparator(UI::Side::Right);
    menuBar->addChild(frameButton);
    menuBar->addSeparator(UI::Side::Right);
    menuBar->addChild(settingsButton);

    _sceneWidget = UIComponents::SceneWidget::create(context);

    auto vLayout = UI::VerticalLayout::create(context);
    vLayout->setSpacing(UI::MetricsRole::None);
    vLayout->addChild(menuBar);
    vLayout->addSeparator();
    _splitter = UI::Layout::Splitter::create(UI::Orientation::Horizontal, context);
    _splitter->addChild(_sceneWidget);
    vLayout->addChild(_splitter);
    vLayout->setStretch(_splitter);
    addChild(vLayout);

    auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
    _actions["File/Open"]->setClickedCallback(
        [weak]
        {
            if (auto widget = weak.lock())
            {
                widget->_open();
            }
        });

    _actions["File/Reload"]->setClickedCallback(
        [weak]
        {
            if (auto widget = weak.lock())
            {
                if (widget->_reloadCallback)
                {
                    widget->_reloadCallback();
                }
            }
        });

    _actions["File/Close"]->setClickedCallback(
        [weak]
        {
            if (auto widget = weak.lock())
            {
                if (widget->_openCallback)
                {
                    widget->_openCallback(System::File::Info());
                }
            }
        });

    _actions["File/Exit"]->setClickedCallback(
        [weak]
        {
            if (auto widget = weak.lock())
            {
                if (widget->_exitCallback)
                {
                    widget->_exitCallback();
                }
            }
        });

    _actions["View/Frame"]->setClickedCallback(
        [weak]
        {
            if (auto widget = weak.lock())
            {
                widget->_sceneWidget->frameView();
            }
        });

    auto contextWeak = std::weak_ptr<System::Context>(context);
    _actions["Tools/Settings"]->setCheckedCallback(
        [weak, contextWeak](bool value)
        {
            if (auto context = contextWeak.lock())
            {
                if (auto widget = weak.lock())
                {
                    if (value)
                    {
                        auto cameraWidget = CameraWidget::create(context);
                        auto renderWidget = RenderWidget::create(context);
                        auto infoWidget = InfoWidget::create(context);
                        widget->_settingsWidget = SettingsWidget::create(context);
                        widget->_settingsWidget->addChild(cameraWidget);
                        widget->_settingsWidget->addChild(renderWidget);
                        widget->_settingsWidget->addChild(infoWidget);

                        cameraWidget->setCameraDataCallback(
                            [weak](const Scene3D::PolarCameraData& value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    widget->_sceneWidget->setCameraData(value);
                                }
                            });

                        renderWidget->setRenderOptionsCallback(
                            [weak](const UIComponents::SceneRenderOptions& value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    widget->_sceneWidget->setRenderOptions(value);
                                }
                            });

                        widget->_cameraDataObserver = Core::Observer::Value<Scene3D::PolarCameraData>::create(
                            widget->_sceneWidget->observeCameraData(),
                            [cameraWidget, weak](const Scene3D::PolarCameraData& value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    cameraWidget->setCameraData(value);
                                }
                            });

                        widget->_renderOptionsObserver = Core::Observer::Value<UIComponents::SceneRenderOptions>::create(
                            widget->_sceneWidget->observeRenderOptions(),
                            [renderWidget, weak](const UIComponents::SceneRenderOptions& value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    renderWidget->setRenderOptions(value);
                                }
                            });

                        widget->_bboxObserver = Core::Observer::Value<Math::BBox3f>::create(
                            widget->_sceneWidget->observeBBox(),
                            [infoWidget, weak](const Math::BBox3f& value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    infoWidget->setBBox(value);
                                }
                            });

                        widget->_primitivesCountObserver = Core::Observer::Value<size_t>::create(
                            widget->_sceneWidget->observePrimitivesCount(),
                            [infoWidget, weak](size_t value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    infoWidget->setPrimitivesCount(value);
                                }
                            });

                        widget->_pointCountObserver = Core::Observer::Value<size_t>::create(
                            widget->_sceneWidget->observePointCount(),
                            [infoWidget, weak](size_t value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    infoWidget->setPointCount(value);
                                }
                            });

                        widget->_statsTimer = System::Timer::create(context);
                        widget->_statsTimer->setRepeating(true);
                        widget->_statsTimer->start(
                            System::getTimerDuration(System::TimerValue::Slow),
                            [infoWidget, weak, contextWeak](const std::chrono::steady_clock::time_point&, const Core::Time::Duration&)
                            {
                                if (auto context = contextWeak.lock())
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        infoWidget->setFPS(context->getFPSAverage());
                                    }
                                }
                            });

                        widget->_splitter->addChild(widget->_settingsWidget);
                        widget->_splitter->setSplit(widget->_splitterSplit);
                    }
                    else
                    {
                        widget->_splitterSplit = widget->_splitter->getSplit();
                        widget->_splitter->removeChild(widget->_settingsWidget);
                        widget->_settingsWidget.reset();
                    }
                }
            }
        });

    _sceneRotateActionGroup->setExclusiveCallback(
        [weak](int value)
        {
            if (auto widget = weak.lock())
            {
                widget->_sceneWidget->setSceneRotate(static_cast<UIComponents::SceneRotate>(value + 1));
                widget->_sceneWidget->frameView();
            }
        });
}

MainWindow::MainWindow()
{}

MainWindow::~MainWindow()
{}

void MainWindow::setScene(
    const djv::System::File::Info& fileInfo,
    const std::shared_ptr<Scene3D::Scene>& value)
{
    _fileInfoLabel->setText(fileInfo.getFileName());
    _sceneWidget->setScene(value);
    _sceneWidget->frameView();
}

void MainWindow::setOpenCallback(const std::function<void(const System::File::Info)>& value)
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

std::shared_ptr<MainWindow> MainWindow::create(const std::shared_ptr<System::Context>& context)
{
    auto out = std::shared_ptr<MainWindow>(new MainWindow);
    out->_init(context);
    return out;
}

void MainWindow::_initEvent(System::Event::Init& event)
{
    if (event.getData().text)
    {
        _actions["File/Open"]->setText(_getText(DJV_TEXT("action_file_open")));
        _actions["File/Open"]->setTooltip(_getText(DJV_TEXT("action_file_open_tooltip")));
        _actions["File/Reload"]->setText(_getText(DJV_TEXT("action_file_reload")));
        _actions["File/Reload"]->setTooltip(_getText(DJV_TEXT("action_file_reload_tooltip")));
        _actions["File/Close"]->setText(_getText(DJV_TEXT("action_file_close")));
        _actions["File/Close"]->setTooltip(_getText(DJV_TEXT("action_file_close_tooltip")));
        _actions["File/Exit"]->setText(_getText(DJV_TEXT("action_file_exit")));
        _actions["File/Exit"]->setTooltip(_getText(DJV_TEXT("action_file_exit_tooltip")));

        _actions["View/Frame"]->setText(_getText(DJV_TEXT("action_view_frame")));
        _actions["View/Frame"]->setTooltip(_getText(DJV_TEXT("action_view_frame_tooltip")));

        _actions["Scene/Rotate+X"]->setText(_getText(DJV_TEXT("action_scene_rotate_x_90")));
        _actions["Scene/Rotate+X"]->setTooltip(_getText(DJV_TEXT("action_scene_rotate_x_90_tooltip")));
        _actions["Scene/Rotate-X"]->setText(_getText(DJV_TEXT("action_scene_rotate_x_-90")));
        _actions["Scene/Rotate-X"]->setTooltip(_getText(DJV_TEXT("action_scene_rotate_x_-90_tooltip")));
        _actions["Scene/Rotate+Y"]->setText(_getText(DJV_TEXT("action_scene_rotate_y_90")));
        _actions["Scene/Rotate+Y"]->setTooltip(_getText(DJV_TEXT("action_scene_rotate_y_90_tooltip")));
        _actions["Scene/Rotate-Y"]->setText(_getText(DJV_TEXT("action_scene_rotate_y_-90")));
        _actions["Scene/Rotate-Y"]->setTooltip(_getText(DJV_TEXT("action_scene_rotate_y_-90_tooltip")));
        _actions["Scene/Rotate+Z"]->setText(_getText(DJV_TEXT("action_scene_rotate_z_90")));
        _actions["Scene/Rotate+Z"]->setTooltip(_getText(DJV_TEXT("action_scene_rotate_z_90_tooltip")));
        _actions["Scene/Rotate-Z"]->setText(_getText(DJV_TEXT("action_scene_rotate_z_-90")));
        _actions["Scene/Rotate+Z"]->setTooltip(_getText(DJV_TEXT("action_scene_rotate_z_-90_tooltip")));

        _actions["Tools/Settings"]->setText(_getText(DJV_TEXT("action_tool_settings")));
        _actions["Tools/Settings"]->setTooltip(_getText(DJV_TEXT("action_tool_settings_tooltip")));
    }
}

void MainWindow::_open()
{
    if (auto context = getContext().lock())
    {
        if (_fileBrowserDialog)
        {
            _fileBrowserDialog->close();
        }
        _fileBrowserDialog = UIComponents::FileBrowser::Dialog::create(UI::SelectionType::Single, context);
        auto io = context->getSystemT<Scene3D::IO::IOSystem>();
        _fileBrowserDialog->setFileExtensions(io->getFileExtensions());
        _fileBrowserDialog->setPath(_fileBrowserPath);
        auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
        _fileBrowserDialog->setCallback(
            [weak](const std::vector<System::File::Info>& value)
            {
                if (auto widget = weak.lock())
                {
                    const auto temp = value;
                    if (widget->_fileBrowserDialog)
                    {
                        widget->_fileBrowserPath = widget->_fileBrowserDialog->getPath();
                        widget->_fileBrowserDialog->close();
                        widget->_fileBrowserDialog.reset();
                    }
                    if (!temp.empty() && widget->_openCallback)
                    {
                        widget->_openCallback(temp[0]);
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
