// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/ISystemTest.h>

#include <djvCore/Context.h>
#include <djvCore/ISystem.h>
#include <djvCore/ResourceSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
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
                    ss << _getResourceSystem()->getPath(FileSystem::ResourcePath::Application);
                    _log(ss.str());
                }
            }
            
            TestSystem()
            {}

        public:
            static std::shared_ptr<TestSystem> create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<TestSystem>(new TestSystem);
                out->_init(context);
                return out;
            }
        };

        ISystemTest::ISystemTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::ISystemTest", context)
        {}
                
        void ISystemTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto system = TestSystem::create(context);
            }
        }
                
    } // namespace CoreTest
} // namespace djv

