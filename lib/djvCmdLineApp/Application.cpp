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

#include <djvCmdLineApp/Application.h>

#include <djvAV/AVSystem.h>
#include <djvAV/IO.h>
#include <djvAV/OpenGL.h>
#include <djvAV/Render2D.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>
#include <djvCore/LogSystem.h>
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

            // Parse the command-line arguments.
            auto i = args.begin();
            while (i != args.end())
            {
                if ("-h" == *i || "-help" == *i || "--help" == *i)
                {
                    i = args.erase(i);
                    printUsage();
                    exit(1);
                    break;
                }
                else if ("-version" == *i)
                {
                    i = args.erase(i);
                    _printVersion();
                    exit(1);
                    break;
                }
                else if ("-log_console" == *i)
                {
                    i = args.erase(i);
                    auto logSystem = getSystemT<LogSystem>();
                    logSystem->setConsoleOutput(true);
                }
                else
                {
                    ++i;
                }
            }

            // Create the systems.
            auto avSystem = AV::AVSystem::create(shared_from_this());
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
            std::cout << " " << textSystem->getText(DJV_TEXT("cli_general_options")) << std::endl;
            std::cout << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("cli_option_version")) << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("cli_option_version_description")) << std::endl;
            std::cout << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("cli_option_log_console")) << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("cli_option_log_console_description")) << std::endl;
            std::cout << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("cli_option_help")) << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("cli_option_help_description")) << std::endl;
            std::cout << std::endl;
        }

        void Application::run()
        {
            DJV_PRIVATE_PTR();
            auto time = std::chrono::steady_clock::now();
            Time::Unit delta = Time::Unit::zero();
            p.running = true;
            while (p.running)
            {
                tick(time, delta);

                auto end = std::chrono::steady_clock::now();
                delta = std::chrono::duration_cast<Time::Unit>(end - time);
                while (delta < std::chrono::microseconds(1000000 / frameRate))
                {
                    end = std::chrono::steady_clock::now();
                    delta = std::chrono::duration_cast<Time::Unit>(end - time);
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

