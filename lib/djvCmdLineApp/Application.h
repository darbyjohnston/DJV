// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/Time.h>

#include <djvSystem/Context.h>

struct GLFWwindow;

#if defined(DJV_PLATFORM_WINDOWS)
#define DJV_MAIN() int wmain(int argc, wchar_t* argv[])
#else
#define DJV_MAIN() int main(int argc, char* argv[])
#endif

namespace djv
{
    //! This namespaces provides command line application functionality.
    namespace CmdLine
    {
        struct Arguments
        {
            std::unique_ptr<AV::Time::Units> timeUnits;
            std::unique_ptr<bool> logConsole;
        };

        //! This class provides a command-line application.
        class Application : public System::Context
        {
            DJV_NON_COPYABLE(Application);

        protected:
            void _init(std::list<std::string>&);
            Application();

        public:
            ~Application() override;

            static std::shared_ptr<Application> create(std::list<std::string>&);

            virtual void run();

            int getExitCode() const;
            void exit(int);

            static std::list<std::string> args(int, char**);
            static std::list<std::string> args(int, wchar_t**);

        protected:
            //! Throws:
            //! - std::runtime_error
            virtual void _parseCmdLine(std::list<std::string>&);
            virtual void _printUsage();

            bool _isRunning() const;
            void _setRunning(bool);

        private:
            void _printVersion();

            DJV_PRIVATE();
        };

    } // namespace CmdLine
} // namespace djv
