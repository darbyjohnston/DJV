// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include "Application.h"

#include "MainWindow.h"

#include <djvUIComponents/UIComponentsSystem.h>

#include <djvScene/Light.h>
#include <djvScene/SceneSystem.h>

#include <djvCore/LogSystem.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/Timer.h>

using namespace djv;

void Application::_init(std::list<std::string>& args)
{
    Desktop::Application::_init(args);
    
    while (args.size())
    {
        _inputs.push_back(args.front());
        args.pop_front();
    }

    Scene::SceneSystem::create(shared_from_this());
    UI::UIComponentsSystem::create(shared_from_this());
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

void Application::run()
{
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
                app->exit(0);
            }
        });

    for (const auto& i : _inputs)
    {
        _open(i);
    }

    _mainWindow->show();

    Desktop::Application::run();
}

void Application::_open(const Core::FileSystem::FileInfo& fileInfo)
{
    if (_scene)
    {
        _mainWindow->setScene(Core::FileSystem::FileInfo(), nullptr);
        _scene.reset();
    }
    _fileInfo = fileInfo;
    if (!_fileInfo.isEmpty())
    {
        try
        {
            auto io = getSystemT<Scene::IO::System>();
            _sceneRead = io->read(_fileInfo);
            _sceneReadFuture = _sceneRead->getScene();
            auto weak = std::weak_ptr<Application>(std::dynamic_pointer_cast<Application>(shared_from_this()));
            _futureTimer->start(
                Core::Time::getTime(Core::Time::TimerValue::Medium),
                [weak, fileInfo](const std::chrono::steady_clock::time_point&, const Core::Time::Duration&)
                {
                    if (auto app = weak.lock())
                    {
                        try
                        {
                            if (app->_sceneReadFuture.valid() &&
                                app->_sceneReadFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                            {
                                app->_scene = app->_sceneReadFuture.get();
                                //app->_scene->printPrimitives();
                                //app->_scene->printLayers();
                                app->_mainWindow->setScene(fileInfo, app->_scene);
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
