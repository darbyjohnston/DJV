// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/SceneWidget.h>
#include <djvUIComponents/FileBrowserDialog.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Drawer.h>
#include <djvUI/Label.h>
#include <djvUI/Window.h>

#include <djvSystem/FileInfo.h>
#include <djvSystem/Timer.h>

#include <djvCore/ValueObserver.h>

class CameraWidget;
class InfoWidget;
class RenderWidget;

class MainWindow : public djv::UI::Window
{
    DJV_NON_COPYABLE(MainWindow);

protected:
    void _init(const std::shared_ptr<djv::System::Context>&);
    MainWindow();

public:
    ~MainWindow() override;

    static std::shared_ptr<MainWindow> create(const std::shared_ptr<djv::System::Context>&);

    void setScene(
        const djv::System::File::Info&,
        const std::shared_ptr<djv::Scene3D::Scene>&);

    void setOpenCallback(const std::function<void(const djv::System::File::Info)>&);
    void setReloadCallback(const std::function<void(void)>&);
    void setExitCallback(const std::function<void(void)>&);

protected:
    void _initEvent(djv::System::Event::Init&) override;

private:
    void _open();

    std::map<std::string, std::map<std::string, std::shared_ptr<djv::UI::Action> > > _actions;
    std::shared_ptr<djv::UI::ActionGroup> _sceneRotateActionGroup;

    std::shared_ptr<djv::UI::Label> _fileInfoLabel;
    std::shared_ptr<djv::UI::SceneWidget> _sceneWidget;
    std::shared_ptr<djv::UI::Drawer> _settingsDrawer;

    std::shared_ptr<djv::UI::FileBrowser::Dialog> _fileBrowserDialog;
    djv::System::File::Path _fileBrowserPath = djv::System::File::Path(".");
    
    std::function<void(const djv::System::File::Info)> _openCallback;
    std::function<void(void)> _reloadCallback;
    std::function<void(void)> _exitCallback;
    
    std::shared_ptr<djv::Core::Observer::ValueObserver<djv::Scene3D::PolarCameraData> > _cameraDataObserver;
    std::shared_ptr<djv::Core::Observer::ValueObserver<djv::UI::SceneRenderOptions> > _renderOptionsObserver;
    std::shared_ptr<djv::Core::Observer::ValueObserver<djv::Math::BBox3f> > _bboxObserver;
    std::shared_ptr<djv::Core::Observer::ValueObserver<size_t> > _primitivesCountObserver;
    std::shared_ptr<djv::Core::Observer::ValueObserver<size_t> > _pointCountObserver;

    std::shared_ptr<djv::System::Timer> _statsTimer;
};
