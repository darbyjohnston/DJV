// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvMathTest/RationalTest.h>

#include <djvMath/Rational.h>

using namespace djv::Core;
using namespace djv::Math;

namespace djv
{
    namespace MathTest
    {
        RationalTest::RationalTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::MathTest::RationalTest", tempPath, context)
        {}
        
        void RationalTest::run()
        {
            _ctor();
            _conversion();
            _util();
            _operators();
        }

        void RationalTest::_ctor()
        {
            {
                const Math::Rational r;
                DJV_ASSERT(0 == r.getNum());
                DJV_ASSERT(0 == r.getDen());
                DJV_ASSERT(!r.isValid());
            }
            
            {
                const Math::Rational r(24);
                DJV_ASSERT(24 == r.getNum());
                DJV_ASSERT(1 == r.getDen());
                DJV_ASSERT(r.isValid());
            }
            
            {
                const Math::Rational r(24, 1001);
                DJV_ASSERT(24 == r.getNum());
                DJV_ASSERT(1001 == r.getDen());
            }
        }
        
        void RationalTest::_conversion()
        {
            {
                const Math::Rational r(24);
                DJV_ASSERT(24.F == r.toFloat());
                DJV_ASSERT(Math::Rational::fromFloat(24.F) == r);
            }
        }
        
        void RationalTest::_util()
        {
            {
                const Math::Rational r(24);
                DJV_ASSERT(r.swap() == Math::Rational(1, 24));
            }
        }
        
        void RationalTest::_operators()
        {
            {
                const Math::Rational r(24);
                DJV_ASSERT(r == r);
                DJV_ASSERT(Math::Rational() != r);
            }
        }
        
    } // namespace MathTest
} // namespace djv

