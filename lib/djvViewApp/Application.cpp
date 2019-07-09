//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvViewApp/Application.h>

#include <djvViewApp/AnnotateSystem.h>
#include <djvViewApp/AudioSystem.h>
#include <djvViewApp/ColorPickerSystem.h>
#include <djvViewApp/FileSystem.h>
#include <djvViewApp/HelpSystem.h>
#include <djvViewApp/ImageSystem.h>
#include <djvViewApp/ImageViewSystem.h>
#include <djvViewApp/MagnifySystem.h>
#include <djvViewApp/MainWindow.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/NUXSystem.h>
#include <djvViewApp/PlaybackSystem.h>
#include <djvViewApp/SettingsSystem.h>
#include <djvViewApp/ToolSystem.h>
#include <djvViewApp/UISettings.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUIComponents/UIComponentsSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct Application::Private
        {
            std::shared_ptr<UISettings> uiSettings;
            std::vector<std::shared_ptr<ISystem> > systems;
            std::shared_ptr<MainWindow> mainWindow;
            std::shared_ptr<NUXWidget> nuxWidget;
        };
        
        void Application::_init(int & argc, char ** argv)
        {
            Desktop::Application::_init(argc, argv);

            UI::UIComponentsSystem::create(this);

            DJV_PRIVATE_PTR();
            p.uiSettings = UISettings::create(this);

            p.systems.push_back(FileSystem::create(this));
            auto windowSystem = WindowSystem::create(this);
            p.systems.push_back(windowSystem);
            p.systems.push_back(ImageViewSystem::create(this));
            p.systems.push_back(ImageSystem::create(this));
            p.systems.push_back(PlaybackSystem::create(this));
            p.systems.push_back(AudioSystem::create(this));
            p.systems.push_back(AnnotateSystem::create(this));
            p.systems.push_back(ColorPickerSystem::create(this));
            p.systems.push_back(MagnifySystem::create(this));
            p.systems.push_back(ToolSystem::create(this));
            p.systems.push_back(HelpSystem::create(this));
            auto nuxSystem = NUXSystem::create(this);
            p.systems.push_back(nuxSystem);
            p.systems.push_back(SettingsSystem::create(this));

            p.mainWindow = MainWindow::create(this);

            windowSystem->setMediaCanvas(p.mainWindow->getMediaCanvas());

            p.nuxWidget = nuxSystem->createNUXWidget();
            if (p.nuxWidget)
            {
                p.mainWindow->addChild(p.nuxWidget);
                p.nuxWidget->setFinishCallback(
                    [this]
                {
                    _p->mainWindow->removeChild(_p->nuxWidget);
                    _p->nuxWidget.reset();
                });
            }

            p.mainWindow->show();
        }

        Application::Application() :
            _p(new Private)
        {}

        Application::~Application()
        {}

        std::unique_ptr<Application> Application::create(int & argc, char ** argv)
        {
            auto out = std::unique_ptr<Application>(new Application);
            out->_init(argc, argv);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

