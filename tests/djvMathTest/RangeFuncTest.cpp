// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvMathTest/RangeFuncTest.h>

#include <djvMath/RangeFunc.h>

using namespace djv::Core;
using namespace djv::Math;

namespace djv
{
    namespace MathTest
    {
        RangeFuncTest::RangeFuncTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::MathTest::RangeFuncTest", tempPath, context)
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
        
    } // namespace MathTest
} // namespace djv

