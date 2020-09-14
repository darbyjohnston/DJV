// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCmdLineApp/Application.h>

#include <djvScene/IO.h>
#include <djvScene/SceneSystem.h>

#include <djvAV/AVSystem.h>
#include <djvAV/AudioSystem.h>
#include <djvAV/FontSystem.h>
#include <djvAV/GLFWSystem.h>
#include <djvAV/IO.h>
#include <djvAV/OpenGL.h>
#include <djvAV/Render2D.h>
#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>
#include <djvCore/LogSystem.h>
#include <djvCore/OS.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/StringFormat.h>
#include <djvCore/StringFunc.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace CmdLine
    {
        namespace
        {
            //! \todo Should this be configurable?
            const size_t frameRate = 60;

        } // namespace

        struct Application::Private
        {
            bool running = false;
            int exit = 0;
        };

        void Application::_init(std::list<std::string>& args)
        {
            std::string argv0;
            if (args.size())
            {
                argv0 = args.front();
                args.pop_front();
            }
            Context::_init(argv0);

            // Parse the command line.
            auto logSystem = getSystemT<LogSystem>();
            {
                std::stringstream ss;
                ss << "Command line arguments: " << String::joinList(args, ' ');
                logSystem->log("djv::CmdLine::Application", ss.str());
            }
            auto arg = args.begin();
            while (arg != args.end())
            {
                if ("-log_console" == *arg)
                {
                    arg = args.erase(arg);
                    logSystem->setConsoleOutput(true);
                }
                else if ("-version" == *arg)
                {
                    arg = args.erase(arg);
                    _printVersion();
                    exit(1);
                    break;
                }
                else
                {
                    ++arg;
                }
            }

            // Create the systems.
            auto avSystem = AV::AVSystem::create(shared_from_this());
            auto sceneSystem = Scene::SceneSystem::create(shared_from_this());
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

        void Application::run()
        {
            DJV_PRIVATE_PTR();
            auto time = std::chrono::steady_clock::now();
            Time::Duration delta = Time::Duration::zero();
            p.running = true;
            while (p.running)
            {
                tick();

                auto end = std::chrono::steady_clock::now();
                delta = std::chrono::duration_cast<Time::Duration>(end - time);
                while (delta < std::chrono::microseconds(1000000 / frameRate))
                {
                    end = std::chrono::steady_clock::now();
                    delta = std::chrono::duration_cast<Time::Duration>(end - time);
                }
                time = end;
            }
        }

        int Application::getExitCode() const
        {
            return _p->exit;
        }

        void Application::exit(int value)
        {
            DJV_PRIVATE_PTR();
            p.running = false;
            p.exit = value;
        }

        std::list<std::string> Application::args(int argc, char** argv)
        {
            std::list<std::string> out;
            for (int i = 0; i < argc; ++i)
            {
                out.push_back(argv[i]);
            }
            return out;
        }

        std::list<std::string> Application::args(int argc, wchar_t** argv)
        {
            std::list<std::string> out;
            for (int i = 0; i < argc; ++i)
            {
                out.push_back(String::fromWide(argv[i]));
            }
            return out;
        }

        void Application::_parseCmdLine(std::list<std::string>& args)
        {
            auto textSystem = getSystemT<Core::TextSystem>();
            auto arg = args.begin();
            while (arg != args.end())
            {
                if ("-time_units" == *arg)
                {
                    arg = args.erase(arg);
                    if (args.end() == arg)
                    {
                        throw std::runtime_error(String::Format("{0}: {1}").
                            arg("-time_units").
                            arg(textSystem->getText(DJV_TEXT("error_cannot_parse_argument"))));
                    }
                    Time::Units value = Time::Units::First;
                    std::string s = textSystem->getID(*arg);
                    std::stringstream ss(s);
                    ss >> value;
                    arg = args.erase(arg);
                    auto avSystem = getSystemT<AV::AVSystem>();
                    avSystem->setTimeUnits(value);
                }
                else if ("-h" == *arg || "-help" == *arg || "--help" == *arg)
                {
                    arg = args.erase(arg);
                    _printUsage();
                    exit(1);
                    break;
                }
                else
                {
                    ++arg;
                }
            }
        }

        void Application::_printUsage()
        {
            auto textSystem = getSystemT<Core::TextSystem>();
            std::cout << " " << textSystem->getText(DJV_TEXT("cli_general_options")) << std::endl;
            std::cout << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("cli_option_time_units")) << std::endl;
            {
                std::vector<std::string> options;
                std::string value;
                for (auto i : Time::getUnitsEnums())
                {
                    std::stringstream ss;
                    ss << i;
                    options.push_back("\"" + textSystem->getText(ss.str()) + "\"");
                }
                {
                    std::stringstream ss;
                    auto avSystem = getSystemT<AV::AVSystem>();
                    ss << avSystem->observeTimeUnits()->get();
                    value = "\"" + textSystem->getText(ss.str()) + "\"";
                }
                const std::string s = String::Format(textSystem->getText(DJV_TEXT("cli_option_time_units_description"))).
                    arg(String::join(options, ", ")).
                    arg(value);
                std::cout << "   " << s << std::endl;
            }
            std::cout << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("cli_option_log_console")) << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("cli_option_log_console_description")) << std::endl;
            std::cout << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("cli_option_version")) << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("cli_option_version_description")) << std::endl;
            std::cout << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("cli_option_help")) << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("cli_option_help_description")) << std::endl;
            std::cout << std::endl;
        }

        bool Application::_isRunning() const
        {
            return _p->running;
        }

        void Application::_setRunning(bool value)
        {
            _p->running = value;
        }

        void Application::_printVersion()
        {
            std::cout << DJV_VERSION << std::endl;
        }

    } // namespace CmdLine
} // namespace djv

