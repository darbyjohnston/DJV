// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/BBoxFuncTest.h>

#include <djvCore/BBoxFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        BBoxFuncTest::BBoxFuncTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::BBoxFuncTest", tempPath, context)
        {}
        
        void BBoxFuncTest::run()
        {
            _comparison();
            _operators();
            _serialize();
        }

        void BBoxFuncTest::_comparison()
        {
            {
                const BBox2f a(0.F, 1.F, 2.F, 3.F);
                const BBox2f b(0.F, 0.F, 0.F, 0.F);
                DJV_ASSERT(a == a);
                DJV_ASSERT(a != b);
                DJV_ASSERT(fuzzyCompare(a, a));
                DJV_ASSERT(!fuzzyCompare(a, b));
            }
            
            {
                const BBox3f a(0.F, 1.F, 2.F, 3.F, 4.F, 5.F);
                const BBox3f b(0.F, 0.F, 0.F, 0.F, 0.F, 0.F);
                DJV_ASSERT(a == a);
                DJV_ASSERT(a != b);
                DJV_ASSERT(fuzzyCompare(a, a));
                DJV_ASSERT(!fuzzyCompare(a, b));
            }
        }

        void BBoxFuncTest::_operators()
        {
            {
                BBox3f a(0.F, 1.F, 2.F, 3.F, 4.F, 5.F);
                BBox3f b = a * glm::mat4x4(1.f);
                DJV_ASSERT(a == b);
            }
        }

        void BBoxFuncTest::_serialize()
        {
            {
                const BBox2i a(0, 1, 2, 3);
                BBox2i b;
                std::stringstream ss;
                ss << a;
                ss >> b;
                DJV_ASSERT(a == b);
            }

            try
            {
                std::stringstream ss;
                BBox2i b;
                ss >> b;
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
            
            {
                const BBox2i a(0, 1, 2, 3);
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(a, allocator);
                BBox2i b;
                fromJSON(json, b);
                DJV_ASSERT(a == b);
            }
            
            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                BBox2i b;
                fromJSON(json, b);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
            
            {
                const BBox2f a(0.F, 1.F, 2.F, 3.F);
                BBox2f b;
                std::stringstream ss;
                ss << a;
                ss >> b;
                DJV_ASSERT(a == b);
            }

            try
            {
                std::stringstream ss;
                BBox2f b;
                ss >> b;
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}

            {
                const BBox2f a(0.F, 1.F, 2.F, 3.F);
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(a, allocator);
                BBox2f b;
                fromJSON(json, b);
                DJV_ASSERT(a == b);
            }

            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                BBox2f b;
                fromJSON(json, b);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}

            {
                const BBox3f a(0.F, 1.F, 2.F, 3.F, 4.F, 5.F);
                BBox3f b;
                std::stringstream ss;
                ss << a;
                ss >> b;
                DJV_ASSERT(a == b);
            }

            try
            {
                std::stringstream ss;
                BBox3f b;
                ss >> b;
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}

            {
                const BBox3f a(0.F, 1.F, 2.F, 3.F, 4.F, 5.F);
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(a, allocator);
                BBox3f b;
                fromJSON(json, b);
                DJV_ASSERT(a == b);
            }

            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                BBox3f b;
                fromJSON(json, b);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }

    } // namespace CoreTest
} // namespace djv
