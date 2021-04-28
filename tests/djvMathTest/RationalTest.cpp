// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvMathTest/RationalTest.h>

#include <djvMath/Rational.h>

#include <sstream>

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
            _serialize();
        }

        void RationalTest::_ctor()
        {
            {
                const Math::IntRational r;
                DJV_ASSERT(0 == r.getNum());
                DJV_ASSERT(0 == r.getDen());
                DJV_ASSERT(!r.isValid());
            }
            
            {
                const Math::IntRational r(24);
                DJV_ASSERT(24 == r.getNum());
                DJV_ASSERT(1 == r.getDen());
                DJV_ASSERT(r.isValid());
            }
            
            {
                const Math::IntRational r(24, 1001);
                DJV_ASSERT(24 == r.getNum());
                DJV_ASSERT(1001 == r.getDen());
            }
        }
        
        void RationalTest::_conversion()
        {
            {
                const Math::IntRational r(24);
                DJV_ASSERT(24.F == r.toFloat());
                DJV_ASSERT(Math::IntRational::fromFloat(24.F) == r);
            }
        }
        
        void RationalTest::_util()
        {
            {
                const Math::IntRational r(24);
                DJV_ASSERT(r.swap() == Math::IntRational(1, 24));
            }
        }
        
        void RationalTest::_operators()
        {
            {
                const Math::IntRational r(24);
                DJV_ASSERT(r == r);
                DJV_ASSERT(Math::IntRational() != r);
            }
        }

        void RationalTest::_serialize()
        {
            {
                const Math::IntRational r(24);
                std::stringstream ss;
                ss << r;
                Math::IntRational r2;
                ss >> r2;
                DJV_ASSERT(r == r2);
            }

            try
            {
                Math::IntRational r;
                std::stringstream ss;
                ss >> r;
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}

            {
                const Math::IntRational r(24);
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(r, allocator);
                Math::IntRational r2;
                fromJSON(json, r2);
                DJV_ASSERT(r == r2);
            }

            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                Math::IntRational value;
                fromJSON(json, value);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }

    } // namespace MathTest
} // namespace djv

