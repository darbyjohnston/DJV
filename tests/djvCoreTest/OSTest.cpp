// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/OSTest.h>

#include <djvCore/OS.h>
#include <djvCore/Path.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        OSTest::OSTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::OSTest", context)
        {}
        
        void OSTest::run()
        {
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
                ss << "Terminal width: " << OS::getTerminalWidth();
                _print(ss.str());
            }
            
            {
                OS::setEnv("OSTest", "1");
                DJV_ASSERT("1" == OS::getEnv("OSTest"));
                DJV_ASSERT(1 == OS::getIntEnv("OSTest"));
            }

            {
                std::stringstream ss;
                ss << "User name: " << OS::getUserName();
                _print(ss.str());
            }
        }
        
    } // namespace CoreTest
} // namespace djv

