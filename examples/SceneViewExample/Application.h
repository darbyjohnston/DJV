// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvDesktopApp/Application.h>

#include <djvScene/IO.h>
#include <djvScene/Scene.h>

#include <djvSystem/Timer.h>

class MainWindow;

class Application : public djv::Desktop::Application
{
    DJV_NON_COPYABLE(Application);

protected:
    void _init(std::list<std::string>&);
    Application();

public:
    ~Application() override;

    static std::shared_ptr<Application> create(std::list<std::string>&);

    void run() override;

private:
    void _open(const djv::System::File::Info&);
    void _close();

    std::vector<djv::System::File::Info> _inputs;
    djv::System::File::Info _fileInfo;
    std::shared_ptr<djv::Scene::Scene> _scene;
    std::shared_ptr<djv::Scene::IO::IRead> _sceneRead;
    std::future<std::shared_ptr<djv::Scene::Scene> > _sceneReadFuture;

    std::shared_ptr<djv::System::Timer> _futureTimer;

    std::shared_ptr<MainWindow> _mainWindow;
};
