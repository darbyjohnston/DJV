// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/RangeFuncTest.h>

#include <djvCore/RangeFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        RangeFuncTest::RangeFuncTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::RangeFuncTest", tempPath, context)
        {}
        
        void RangeFuncTest::run()
        {
            _serialize();
        }

        void RangeFuncTest::_serialize()
        {
            {
                const IntRange range(1, 10);
                std::stringstream ss;
                ss << range;
                IntRange range2;
                ss >> range2;
                DJV_ASSERT(range == range2);
            }

            try
            {
                IntRange range;
                std::stringstream ss;
                ss >> range;
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }
        
    } // namespace CoreTest
} // namespace djv

