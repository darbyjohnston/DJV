// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystemTest/ContextTest.h>

#include <djvSystem/Context.h>
#include <djvSystem/ISystem.h>

#include <djvCore/String.h>

#include <sstream>

using namespace djv::Core;
using namespace djv::System;

namespace djv
{
    namespace SystemTest
    {
        ContextTest::ContextTest(
            const File::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::SystemTest::ContextTest", tempPath, context)
        {}
        
        namespace
        {
            class TestSystem : public ISystem
            {
                DJV_NON_COPYABLE(TestSystem);
                TestSystem()
                {}
                
            public:
                ~TestSystem() override
                {}

                static std::shared_ptr<TestSystem> create(const std::shared_ptr<Context>& context)
                {
                    auto out = std::shared_ptr<TestSystem>(new TestSystem);
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
                
                DJV_ASSERT(!context->getSystemT<TestSystem>());
                auto system = TestSystem::create(context);
                DJV_ASSERT(context->getSystemT<TestSystem>());
                DJV_ASSERT(!context->getSystemsT<TestSystem>().empty());
                
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
        
    } // namespace SystemTest
} // namespace djv

