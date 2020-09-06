// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/EnumTest.h>

#include <djvCore/Memory.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        EnumTest::EnumTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::EnumTest", tempPath, context)
        {}
        
        void EnumTest::run()
        {
            for (auto i : Memory::getEndianEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "endian string: " << _getText(ss.str());
                _print(ss2.str());
            }
            
            for (auto i : Memory::getEndianEnums())
            {
                std::stringstream ss;
                ss << i;
                Memory::Endian j = Memory::Endian::First;
                ss >> j;
                DJV_ASSERT(i == j);
            }
            
            try
            {
                Memory::Endian endian = Memory::Endian::First;
                std::stringstream ss;
                ss >> endian;
                DJV_ASSERT(true);
            }
            catch (const std::exception& e)
            {
                _print(e.what());
            }
        }
        
    } // namespace CoreTest
} // namespace djv

