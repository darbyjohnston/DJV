// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/OSTest.h>

#include <djvCore/OS.h>
#include <djvCore/Path.h>
#include <djvCore/String.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        OSTest::OSTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::OSTest", tempPath, context)
        {}
        
        void OSTest::run()
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
            
            for (auto i : OS::getDirectoryShortcutEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "directory shortcut string: " << _getText(ss.str());
                _print(ss2.str());
            }
            
            for (auto i : OS::getDirectoryShortcutEnums())
            {
                std::stringstream ss;
                ss << "directory shortcut: " << OS::getPath(i);
                _print(ss.str());
            }
        }
        
    } // namespace CoreTest
} // namespace djv

