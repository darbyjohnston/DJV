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
                [weak, fileInfo](const std::chrono::steady_clock::time_point&, const Core::Time::Unit&)
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
