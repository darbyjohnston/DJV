// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvMathTest/RationalFuncTest.h>

#include <djvMath/RationalFunc.h>

#include <sstream>

using namespace djv::Core;
using namespace djv::Math;

namespace djv
{
    namespace MathTest
    {
        RationalFuncTest::RationalFuncTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::MathTest::RationalFuncTest", tempPath, context)
        {}
        
        void RationalFuncTest::run()
        {
            _serialize();
        }

        void RationalFuncTest::_serialize()
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
        
    } // namespace MathTest
} // namespace djv

