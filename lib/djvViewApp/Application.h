// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvDesktopApp/Application.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the application.
        class Application : public Desktop::Application
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
            void _parseCmdLine(std::list<std::string>&) override;
            void _printUsage() override;
            void _readIcon(const std::string&);

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

