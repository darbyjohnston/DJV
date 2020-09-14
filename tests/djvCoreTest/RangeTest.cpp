// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/RangeTest.h>

#include <djvCore/Range.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        RangeTest::RangeTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::RangeTest", tempPath, context)
        {}
        
        void RangeTest::run()
        {
            _ctor();
            _util();
            _operators();
        }

        void RangeTest::_ctor()
        {
            {
                IntRange range;
                DJV_ASSERT(0 == range.getMin());
                DJV_ASSERT(0 == range.getMax());
            }
            
            {
                IntRange range(1);
                DJV_ASSERT(1 == range.getMin());
                DJV_ASSERT(1 == range.getMax());
            }
            
            {
                IntRange range(10, 1);
                DJV_ASSERT(1 == range.getMin());
                DJV_ASSERT(10 == range.getMax());
            }
        }
        
        void RangeTest::_util()
        {
            {
                IntRange range(1, 10);
                range.zero();
                DJV_ASSERT(0 == range.getMin());
                DJV_ASSERT(0 == range.getMax());
            }
            
            {
                FloatRange range(1.F, 10.F);
                range.zero();
                DJV_ASSERT(0.F == range.getMin());
                DJV_ASSERT(0.F == range.getMax());
            }
            
            {
                IntRange range(1, 10);
                DJV_ASSERT(range.intersects(IntRange(5, 20)));
                DJV_ASSERT(!range.intersects(IntRange(11, 20)));
            }
            
            {
                IntRange range(1, 10);
                range.expand(20);
                DJV_ASSERT(IntRange(1, 20) == range);
            }
            
            {
                IntRange range(1, 10);
                range.expand(IntRange(5, 20));
                DJV_ASSERT(IntRange(1, 20) == range);
            }            
        }
        
        void RangeTest::_operators()
        {
            {
                IntRange range(1, 10);
                DJV_ASSERT(range == range);
                DJV_ASSERT(IntRange() != range);
                DJV_ASSERT(IntRange() < range);
            }
        }
        
    } // namespace CoreTest
} // namespace djv

