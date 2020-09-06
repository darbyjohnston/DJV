// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/RationalTest.h>

#include <djvCore/Rational.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        RationalTest::RationalTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::RationalTest", tempPath, context)
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
        
        void RationalTest::_serialize()
        {
            {
                const Math::Rational r(24);
                std::stringstream ss;
                ss << r;
                Math::Rational r2;
                ss >> r2;
                DJV_ASSERT(r == r2);
            }

            try
            {
                Math::Rational r;
                std::stringstream ss;
                ss >> r;
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}

            {
                const Math::Rational r(24);
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(r, allocator);
                Math::Rational r2;
                fromJSON(json, r2);
                DJV_ASSERT(r == r2);
            }

            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                Math::Rational value;
                fromJSON(json, value);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }
        
    } // namespace CoreTest
} // namespace djv

