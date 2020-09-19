// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/OSFuncTest.h>

#include <djvCore/OSFunc.h>
#include <djvCore/StringFunc.h>

#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        OSFuncTest::OSFuncTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::CoreTest::OSFuncTest", tempPath, context)
        {}
        
        void OSFuncTest::run()
        {
            {
                std::stringstream ss;
                ss << "Information: " << OS::getInformation();
                _print(ss.str());
            }
            
            {
                std::stringstream ss;
                ss << "RAM: " << OS::getRAMSize();
                _print(ss.str());
            }

            {
                std::stringstream ss;
                ss << "User name: " << OS::getUserName();
                _print(ss.str());
            }
            
            {
                std::stringstream ss;
                ss << "Terminal width: " << OS::getTerminalWidth();
                _print(ss.str());
            }
            
            for (const auto i : { OS::ListSeparator::Unix, OS::ListSeparator::Windows })
            {
                std::stringstream ss;
                ss << "List separator: " << OS::getListSeparator(i);
                _print(ss.str());
            }
            
            {
                std::stringstream ss;
                ss << "Current list separator: " << OS::getCurrentListSeparator();
                _print(ss.str());
            }
            
            {
                const std::string name = "OS_TEST";
                OS::setEnv(name, "1");
                std::string env;
                DJV_ASSERT(OS::getEnv(name, env));
                DJV_ASSERT("1" == env);
                int intEnv = 0;
                DJV_ASSERT(OS::getIntEnv(name, intEnv));
                DJV_ASSERT(1 == intEnv);
                OS::clearEnv(name);
                DJV_ASSERT(!OS::getEnv(name, env));
            }
            
            {
                const std::string name = "OS_TEST";
                OS::setEnv(
                    name,
                    String::join({ "one", "two", "three" },
                    OS::getCurrentListSeparator()));
                std::vector<std::string> value;
                DJV_ASSERT(OS::getStringListEnv(name, value));
                DJV_ASSERT(3 == value.size());
                DJV_ASSERT("one" == value[0]);
                DJV_ASSERT("two" == value[1]);
                DJV_ASSERT("three" == value[2]);
            }
        }
        
    } // namespace CoreTest
} // namespace djv

