// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/ContextTest.h>

#include <djvCore/Context.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/String.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        ContextTest::ContextTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::ContextTest", context)
        {}
        
        namespace
        {
            class System : public ISystem {};
        
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
                
                {
                    auto resourceSystem = context->getSystemT<ResourceSystem>();
                    DJV_ASSERT(resourceSystem);
                }
                
                {
                    DJV_ASSERT(!context->getSystemT<System>());
                }
                
                auto time = std::chrono::steady_clock::now();
                std::chrono::milliseconds delta;
                for (size_t i = 0; i < 100; ++i)
                {
                    context->tick();
                    auto now = std::chrono::steady_clock::now();
                    delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - time);
                    time = now;
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

