// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystemTest/LogSystemTest.h>

#include <djvSystem/Context.h>
#include <djvSystem/LogSystem.h>

#include <iostream>

using namespace djv::Core;
using namespace djv::System;

namespace djv
{
    namespace SystemTest
    {
        LogSystemTest::LogSystemTest(
            const File::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITickTest("djv::SystemTest::LogSystemTest", tempPath, context)
        {}
                
        void LogSystemTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto system = context->getSystemT<LogSystem>();
                
                auto warningsObserver = ListObserver<std::string>::create(
                    system->observeWarnings(),
                    [](const std::vector<std::string>& value)
                    {
                        for (const auto& i : value)
                        {
                            std::cout << i << std::endl;
                        }
                    });
                auto errorsObserver = ListObserver<std::string>::create(
                    system->observeErrors(),
                    [](const std::vector<std::string>& value)
                    {
                        for (const auto& i : value)
                        {
                            std::cout << i << std::endl;
                        }
                    });
                
                system->setConsoleOutput(true);
                DJV_ASSERT(system->hasConsoleOutput());
                
                _tickFor(std::chrono::milliseconds(500));
                
                system->log("LogSystemTest", "Message");
                system->log("LogSystemTest", "Warning", LogLevel::Warning);
                system->log("LogSystemTest", "Error", LogLevel::Error);

                _tickFor(std::chrono::milliseconds(500));

                system->setConsoleOutput(false);
            }
        }
                
    } // namespace SystemTest
} // namespace djv

