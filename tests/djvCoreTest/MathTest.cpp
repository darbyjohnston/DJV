// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/MathTest.h>

#include <djvCore/Math.h>
#include <djvCore/Rational.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        MathTest::MathTest(const std::shared_ptr<Context>& context) :
            ITest("djv::CoreTest::MathTest", context)
        {}
        
        void MathTest::run()
        {
            _rational();
            _random();
        }

        void MathTest::_misc()
        {
            {
                DJV_ASSERT(Math::clamp(0, 1, 2) == 1);
                DJV_ASSERT(Math::clamp(3, 1, 2) == 2);
            }
        }
        
        void MathTest::_rational()
        {
            {
                const Math::Rational r;
                DJV_ASSERT(0 == r.getNum());
                DJV_ASSERT(0 == r.getDen());
                DJV_ASSERT(!r.isValid());
            }
            
            {
                const Math::Rational r(1, 2);
                DJV_ASSERT(1 == r.getNum());
                DJV_ASSERT(2 == r.getDen());
                DJV_ASSERT(r.isValid());
            }
            
            {
                const Math::Rational r(2, 2);
                const float f = r.toFloat();
                DJV_ASSERT(1.f == f);
                DJV_ASSERT(Math::Rational(1, 1) == Math::Rational::fromFloat(f));
            }
            
            {
                const Math::Rational r(1, 2);
                DJV_ASSERT(r == r);
                DJV_ASSERT(r != Math::Rational());
            }
            
            {
                const Math::Rational r(1, 2);
                std::stringstream ss;
                ss << r;
                Math::Rational r2;
                ss >> r2;
                DJV_ASSERT(r == r2);
            }
            
            try
            {
                std::stringstream ss;
                Math::Rational r;
                ss >> r;
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
            
            try
            {
                std::stringstream ss("1/2/3");
                Math::Rational r;
                ss >> r;
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }

        void MathTest::_random()
        {
            for (size_t i = 0; i < 10; ++i)
            {
                std::stringstream ss;
                ss << "Random 0.0-1.0: " << Math::getRandom(1.f);
                _print(ss.str());
            }

            Math::setRandomSeed(1);
            for (size_t i = 0; i < 10; ++i)
            {
                std::stringstream ss;
                ss << "Random 1.0-2.0: " << Math::getRandom(1.f, 2.f);
                _print(ss.str());
            }

            Math::setRandomSeed();
            for (size_t i = 0; i < 10; ++i)
            {
                std::stringstream ss;
                ss << "Random -1.0-1.0: " << Math::getRandom(-1.f, 1.f);
                _print(ss.str());
            }
        }
        
    } // namespace CoreTest
} // namespace djv

