//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvViewApp/ApplicationSettings.h>
#include <djvViewApp/AnnotateSystem.h>
#include <djvViewApp/AudioSystem.h>
#include <djvViewApp/ColorPickerSystem.h>
#include <djvViewApp/FileSettings.h>
#include <djvViewApp/FileSystem.h>
#include <djvViewApp/HelpSystem.h>
#include <djvViewApp/ImageSystem.h>
#include <djvViewApp/MagnifySystem.h>
#include <djvViewApp/MainWindow.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/NUXSystem.h>
#include <djvViewApp/PlaybackSystem.h>
#include <djvViewApp/SettingsSystem.h>
#include <djvViewApp/ToolSystem.h>
#include <djvViewApp/ViewSystem.h>
#include <djvViewApp/WindowSettings.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUIComponents/UIComponentsSystem.h>

#include <djvUI/SettingsSystem.h>

#include <djvAV/AVSystem.h>
#include <djvAV/GLFWSystem.h>
#include <djvAV/IO.h>
#include <djvAV/Render2D.h>

#include <djvCore/LogSystem.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/TextSystem.h>
#include <djvCore/Timer.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct Application::Private
        {
            std::vector<std::shared_ptr<ISystem> > systems;

            std::unique_ptr<bool> fullScreenCmdLine;
            std::unique_ptr<int> fullScreenMonitorCmdLine;
            std::unique_ptr<std::string> ocioConfigCmdLine;
            std::unique_ptr<std::string> ocioImageCmdLine;
            std::unique_ptr<std::string> ocioDisplayCmdLine;
            std::unique_ptr<std::string> ocioViewCmdLine;
            std::unique_ptr<std::string> pixelAspectCmdLine;
            std::unique_ptr<Frame::Index> frameStartCmdLine;
            std::unique_ptr<Frame::Index> frameEndCmdLine;
            std::unique_ptr<Time::Speed> fpsCmdLine;

            std::shared_ptr<ApplicationSettings> settings;

            std::vector<std::string> cmdlinePaths;

            std::vector<std::shared_ptr<AV::IO::IRead> > read;
            std::vector<std::shared_ptr<AV::Image::Image> > icons;
            std::shared_ptr<Time::Timer> timer;

            std::shared_ptr<MainWindow> mainWindow;
            std::shared_ptr<NUXWidget> nuxWidget;
        };
        
        void Application::_init(std::list<std::string>& args)
        {
            Desktop::Application::_init(args);
            DJV_PRIVATE_PTR();

            // Parse the command-line arguments.
            auto arg = args.begin();
            while (arg != args.end())
            {
                if ("-full_screen" == *arg)
                {
                    arg = args.erase(arg);
                    p.fullScreenCmdLine.reset(new bool(true));
                }
                else if ("-full_screen_monitor" == *arg)
                {
                    arg = args.erase(arg);
                    int value = 0;
                    std::stringstream ss(*arg);
                    ss >> value;
                    arg = args.erase(arg);
                    p.fullScreenMonitorCmdLine.reset(new int(std::max(value, 0)));
                }
                else if ("-pixel_aspect" == *arg)
                {
                }
                else if ("-ocio_config" == *arg)
                {
                }
                else if ("-ocio_input" == *arg)
                {
                }
                else if ("-ocio_display" == *arg)
                {
                }
                else if ("-ocio_view" == *arg)
                {
                }
                else if ("-frame_start" == *arg)
                {
                }
                else if ("-frame_end" == *arg)
                {
                }
                else if ("-fps" == *arg)
                {
                }
                else
                {
                    p.cmdlinePaths.push_back(*arg);
                    arg = args.erase(arg);
                }
            }

            // Create the systems.
            UI::UIComponentsSystem::create(shared_from_this());
            p.systems.push_back(FileSystem::create(shared_from_this()));
            p.systems.push_back(WindowSystem::create(shared_from_this()));
            p.systems.push_back(ViewSystem::create(shared_from_this()));
            p.systems.push_back(ImageSystem::create(shared_from_this()));
            p.systems.push_back(PlaybackSystem::create(shared_from_this()));
            p.systems.push_back(AudioSystem::create(shared_from_this()));
            p.systems.push_back(ColorPickerSystem::create(shared_from_this()));
            p.systems.push_back(MagnifySystem::create(shared_from_this()));
            //p.systems.push_back(AnnotateSystem::create(shared_from_this()));
            p.systems.push_back(ToolSystem::create(shared_from_this()));
            p.systems.push_back(HelpSystem::create(shared_from_this()));
            p.systems.push_back(NUXSystem::create(shared_from_this()));
            p.systems.push_back(SettingsSystem::create(shared_from_this()));

            // Create settings.
            p.settings = ApplicationSettings::create(shared_from_this());
        }

        Application::Application() :
            _p(new Private)
        {}

        Application::~Application()
        {}

        std::shared_ptr<Application> Application::create(std::list<std::string>& args)
        {
            auto out = std::shared_ptr<Application>(new Application);
            out->_init(args);
            return out;
        }

        void Application::printUsage()
        {
            auto textSystem = getSystemT<Core::TextSystem>();
            std::cout << std::endl;
            std::cout << " " << textSystem->getText(DJV_TEXT("djv_cli_description")) << std::endl;
            std::cout << std::endl;
            std::cout << " " << textSystem->getText(DJV_TEXT("djv_cli_usage")) << std::endl;
            std::cout << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("djv_cli_usage_format")) << std::endl;
            std::cout << std::endl;
            std::cout << " " << textSystem->getText(DJV_TEXT("djv_cli_options")) << std::endl;
            std::cout << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("djv_cli_option_full_screen")) << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("djv_cli_option_full_screen_description")) << std::endl;
            std::cout << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("djv_cli_option_full_screen_monitor")) << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("djv_cli_option_full_screen_monitor_description")) << std::endl;
            std::cout << std::endl;

            Desktop::Application::printUsage();
        }

        void Application::run()
        {
            DJV_PRIVATE_PTR();

            // Create the main window.
            p.mainWindow = MainWindow::create(shared_from_this());
            auto windowSystem = getSystemT<WindowSystem>();
            windowSystem->setMediaCanvas(p.mainWindow->getMediaCanvas());

            // NUX.
            auto nuxSystem = getSystemT<NUXSystem>();
            p.nuxWidget = nuxSystem->createNUXWidget();
            if (p.nuxWidget)
            {
                p.nuxWidget->show();
                auto weak = std::weak_ptr<Application>(std::dynamic_pointer_cast<Application>(shared_from_this()));
                p.nuxWidget->setFinishCallback(
                    [weak]
                    {
                        if (auto app = weak.lock())
                        {
                            app->_p->nuxWidget->close();
                            app->_p->nuxWidget.reset();
                        }
                    });
            }

            // Read the application icons.
            _readIcon("djv-reel-16.png");
            _readIcon("djv-reel-32.png");
            _readIcon("djv-reel-64.png");
            _readIcon("djv-reel-128.png");
            _readIcon("djv-reel-512.png");
            _readIcon("djv-reel-1024.png");
            p.timer = Time::Timer::create(shared_from_this());
            p.timer->setRepeating(true);
            p.timer->start(
                Time::getTime(Time::TimerValue::Fast),
                [this](const std::chrono::steady_clock::time_point&, const Time::Unit&)
                {
                    DJV_PRIVATE_PTR();
                    auto i = p.read.begin();
                    while (i != p.read.end())
                    {
                        bool erase = false;
                        {
                            std::unique_lock<std::mutex> lock((*i)->getMutex());
                            auto& queue = (*i)->getVideoQueue();
                            if (!queue.isEmpty())
                            {
                                erase = true;
                                p.icons.push_back(queue.popFrame().image);
                            }
                            else if (queue.isFinished())
                            {
                                erase = true;
                            }
                        }
                        if (erase)
                        {
                            i = p.read.erase(i);
                        }
                        else
                        {
                            ++i;
                        }
                    }
                    if (!p.read.size())
                    {
                        p.timer->stop();
                        auto glfwSystem = getSystemT<AV::GLFW::System>();
                        auto glfwWindow = glfwSystem->getGLFWWindow();
                        std::vector<GLFWimage> glfwImages;
                        for (const auto& i : p.icons)
                        {
                            glfwImages.push_back(GLFWimage{ i->getWidth(), i->getHeight(), i->getData() });
                        }
                        glfwSetWindowIcon(glfwWindow, glfwImages.size(), glfwImages.data());
                    }
                });

            // Open command-line files.
            auto fileSystem = getSystemT<FileSystem>();
            fileSystem->open(p.cmdlinePaths);

            // Apply command-line arguments.
            if (p.fullScreenMonitorCmdLine && windowSystem)
            {
                auto settingsSystem = getSystemT<UI::Settings::System>();
                auto windowSettings = settingsSystem->getSettingsT<WindowSettings>();
                windowSettings->setFullscreenMonitor(*(p.fullScreenMonitorCmdLine));
            }
            if (p.fullScreenCmdLine && windowSystem)
            {
                windowSystem->setFullScreen(*(p.fullScreenCmdLine));
            }

            // Show the main window.
            p.mainWindow->show();

            Desktop::Application::run();
        }
        
        void Application::_readIcon(const std::string& fileName)
        {
            DJV_PRIVATE_PTR();
            try
            {
                auto resourceSystem = getSystemT<Core::ResourceSystem>();
                const auto& iconsPath = resourceSystem->getPath(Core::FileSystem::ResourcePath::Icons);
                auto io = getSystemT<AV::IO::System>();
                p.read.push_back(io->read(Core::FileSystem::Path(iconsPath, fileName)));
            }
            catch (const std::exception& e)
            {
                std::stringstream ss;
                auto textSystem = getSystemT<TextSystem>();
                ss << textSystem->getText(DJV_TEXT("error_the_file"));
                ss << " '" << fileName << "' ";
                ss << textSystem->getText(DJV_TEXT("error_cannot_be_read")) << ". ";
                ss << e.what();
                auto logSystem = getSystemT<LogSystem>();
                logSystem->log("djv::ViewApp::Application", ss.str(), LogLevel::Error);
            }
        }

    } // namespace ViewApp
} // namespace djv

