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

            std::shared_ptr<ApplicationSettings> settings;

            std::vector<std::string> cmdlinePaths;

            std::vector<std::shared_ptr<AV::IO::IRead> > read;
            std::vector<std::shared_ptr<AV::Image::Image> > icons;
            std::shared_ptr<Time::Timer> timer;

            std::shared_ptr<MainWindow> mainWindow;
            std::shared_ptr<NUXWidget> nuxWidget;
        };
        
        void Application::_init(const std::string& argv0)
        {
            Desktop::Application::_init(argv0);
            DJV_PRIVATE_PTR();

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

        std::shared_ptr<Application> Application::create(const std::string& argv0)
        {
            auto out = std::shared_ptr<Application>(new Application);
            out->_init(argv0);
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

            // Show the main window.
            p.mainWindow->show();

            Desktop::Application::run();
        }

        void Application::_parseArgs(std::list<std::string>& args)
        {
            Desktop::Application::_parseArgs(args);
            DJV_PRIVATE_PTR();
            if (0 == getExitCode())
            {
                auto arg = args.begin();
                while (arg != args.end())
                {
                    if ("-full_screen" == *arg)
                    {
                    }
                    else if ("-screen" == *arg)
                    {
                    }
                    else if ("-cs_config" == *arg)
                    {
                    }
                    else if ("-cs_input" == *arg)
                    {
                    }
                    else if ("-cs_display" == *arg)
                    {
                    }
                    else if ("-cs_view" == *arg)
                    {
                    }
                    else if ("-pixel_aspect" == *arg)
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
                    else if ("-version" == *arg)
                    {
                    }
                    else if ("-log_console" == *arg)
                    {
                    }
                    else if ("-init_settings" == *arg)
                    {
                    }
                    else
                    {
                        p.cmdlinePaths.push_back(*arg);
                        arg = args.erase(arg);
                    }
                }
            }
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

