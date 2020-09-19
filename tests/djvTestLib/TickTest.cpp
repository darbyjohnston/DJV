// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/TickTest.h>

#include <djvSystem/Context.h>

#include <thread>

using namespace djv::Core;

namespace djv
{
    namespace Test
    {
        namespace
        {
            //! \todo Should this be configurable?
            const size_t frameRate = 60;
        
        } // namespace
        
        ITickTest::ITickTest(
            const std::string& name,
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest(name, tempPath, context)
        {}
        
        ITickTest::~ITickTest()
        {}

        void ITickTest::_tickFor(const Time::Duration& value)
        {
            if (auto context = getContext().lock())
            {
                auto time = std::chrono::steady_clock::now();
                auto timeout = time + value;
                Time::Duration delta;
                while (time < timeout)
                {
                    context->tick();

                    auto t = std::chrono::steady_clock::now();
                    delta = std::chrono::duration_cast<std::chrono::milliseconds>(t - time);
                    const float sleep = 1 / static_cast<float>(frameRate) - delta.count();
                    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(sleep * 1000)));
                    
                    t = std::chrono::steady_clock::now();
                    delta = std::chrono::duration_cast<std::chrono::milliseconds>(t - time);
                    time = t;
                }
            }
        }
                
    } // namespace Test
} // namespace djv

