// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/MathFuncTest.h>

#include <djvCore/Math.h>
#include <djvCore/MathFunc.h>
#include <djvCore/RationalFunc.h>

#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        MathFuncTest::MathFuncTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::MathFuncTest", tempPath, context)
        {}
        
        void MathFuncTest::run()
        {
            _misc();
            _rational();
            _random();
            _conversion();
            _comparison();
        }

        void MathFuncTest::_misc()
        {
            {
                DJV_ASSERT(Math::clamp(0, 1, 2) == 1);
                DJV_ASSERT(Math::clamp(3, 1, 2) == 2);
            }
            {
                DJV_ASSERT(Math::getNumDigits(1) == 1);
                DJV_ASSERT(Math::getNumDigits(10) == 2);
                DJV_ASSERT(Math::getNumDigits(100) == 3);
                DJV_ASSERT(Math::getNumDigits(1000) == 4);
                DJV_ASSERT(Math::getNumDigits(10000) == 5);
                DJV_ASSERT(Math::getNumDigits(100000) == 6);
                DJV_ASSERT(Math::getNumDigits(1000000) == 7);
                DJV_ASSERT(Math::getNumDigits(10000000) == 8);
                DJV_ASSERT(Math::getNumDigits(100000000) == 9);
            }
            {
                DJV_ASSERT(0 == Math::closest(0, {}));
                DJV_ASSERT(0 == Math::closest(0, { 0 }));
                DJV_ASSERT(0 == Math::closest(0, { 0, 1, 2 }));
                DJV_ASSERT(1 == Math::closest(1, { 0, 1, 2 }));
                DJV_ASSERT(2 == Math::closest(2, { 0, 1, 2 }));
                DJV_ASSERT(1 == Math::closest(2, { 0, 1, 4 }));
                DJV_ASSERT(2 == Math::closest(3, { 0, 1, 4 }));
            }
        }
        
        void MathFuncTest::_rational()
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

        void MathFuncTest::_random()
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

        void MathFuncTest::_conversion()
        {
            {
                DJV_ASSERT(.5F == Math::getFraction<float>(.5F));
                DJV_ASSERT(.5F == Math::getFraction<float>(1.5F));
            }
            
            {
                DJV_ASSERT(1 == Math::toPow2(0));
                DJV_ASSERT(1 == Math::toPow2(1));
                DJV_ASSERT(2 == Math::toPow2(2));
                DJV_ASSERT(4 == Math::toPow2(3));
            }
            
            {
                DJV_ASSERT(fuzzyCompare(Math::rad2deg(0.F), 0.F));
                DJV_ASSERT(fuzzyCompare(Math::rad2deg(Math::pi), 180.F));
                DJV_ASSERT(fuzzyCompare(Math::deg2rad(0.F), 0.F));
                DJV_ASSERT(fuzzyCompare(Math::deg2rad(180.F), Math::pi));
            }
        }

        void MathFuncTest::_comparison()
        {
            {
                DJV_ASSERT(Math::haveSameSign(1, 1));
                DJV_ASSERT(Math::haveSameSign(-1, -1));
                DJV_ASSERT(!Math::haveSameSign(1, -1));
            }
        }
        
    } // namespace CoreTest
} // namespace djv

