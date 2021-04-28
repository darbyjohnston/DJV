// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCmdLineApp/Application.h>

struct GLFWwindow;

namespace djv
{
    //! Desktop applications.
    namespace Desktop
    {
        //! Desktop application.
        class Application : public CmdLine::Application
        {
            DJV_NON_COPYABLE(Application);
            
        protected:
            void _init(std::list<std::string>&);
            Application();

        public:
            ~Application() override;

            static std::shared_ptr<Application> create(std::list<std::string>&);

            void run() override;

        protected:
            void _printUsage() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace Desktop
} // namespace djv
