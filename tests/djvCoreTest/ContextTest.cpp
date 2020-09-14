// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/ContextTest.h>

#include <djvCore/Context.h>
#include <djvCore/ISystem.h>
#include <djvCore/String.h>

#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        ContextTest::ContextTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::ContextTest", tempPath, context)
        {}
        
        namespace
        {
            class System : public ISystem
            {
                DJV_NON_COPYABLE(System);
                System()
                {}
                
            public:
                ~System() override
                {}

                static std::shared_ptr<System> create(const std::shared_ptr<Context>& context)
                {
                    auto out = std::shared_ptr<System>(new System);
                    out->_init("System", context);
                    return out;
                }
            };
        
        } // namespace
        
        void ContextTest::run()
        {
            if (auto context = getContext().lock())
            {
                {
                    std::stringstream ss;
                    ss << "name: " << context->getName();
                    _print(ss.str());
                }
                
                for (const auto& i : context->getSystems())
                {
                    std::stringstream ss;
                    ss << "system: " << i->getSystemName();
                    _print(ss.str());
                }
                
                DJV_ASSERT(!context->getSystemT<System>());
                auto system = System::create(context);
                DJV_ASSERT(context->getSystemT<System>());
                DJV_ASSERT(!context->getSystemsT<System>().empty());
                
                auto time = std::chrono::steady_clock::now();
                std::chrono::milliseconds delta;
                for (size_t i = 0; i < 100; ++i)
                {
                    context->tick();
                    auto now = std::chrono::steady_clock::now();
                    delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - time);
                    time = now;
                }
                
                for (const auto& i : context->getSystemTickTimes())
                {
                    std::stringstream ss;
                    ss << i.first << ": " << i.second.count();
                    _print(ss.str());
                }
                
                {
                    std::stringstream ss;
                    ss << "fps averge: " << context->getFPSAverage();
                    _print(ss.str());
                }
            }
        }
        
    } // namespace CoreTest
} // namespace djv

