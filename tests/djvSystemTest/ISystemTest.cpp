// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystemTest/ISystemTest.h>

#include <djvSystem/Context.h>
#include <djvSystem/ISystem.h>
#include <djvSystem/ResourceSystem.h>

using namespace djv::Core;
using namespace djv::System;

namespace djv
{
    namespace SystemTest
    {
        class TestSystem : public ISystem
        {
            DJV_NON_COPYABLE(TestSystem);
            void _init(const std::shared_ptr<Context>& context)
            {
                ISystem::_init("TestSystem", context);
                _log(_getText("TestSystem"));
                {
                    std::stringstream ss;
                    ss << "Application path: ";
                    ss << _getResourceSystem()->getPath(File::ResourcePath::Application);
                    _log(ss.str());
                }
            }
            
            TestSystem()
            {}

        public:
            static std::shared_ptr<TestSystem> create(const std::shared_ptr<Context>& context)
            {
                auto out = context->getSystemT<TestSystem>();
                if (!out)
                {
                    out = std::shared_ptr<TestSystem>(new TestSystem);
                    out->_init(context);
                }
                return out;
            }
        };

        ISystemTest::ISystemTest(
            const File::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::SystemTest::ISystemTest", tempPath, context)
        {}
                
        void ISystemTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto system = TestSystem::create(context);
                DJV_ASSERT(system->getDependencies().empty());
            }
        }
                
    } // namespace SystemTest
} // namespace djv

