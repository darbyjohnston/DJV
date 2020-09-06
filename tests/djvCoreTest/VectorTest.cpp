// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/VectorTest.h>

#include <djvCore/Memory.h>
#include <djvCore/Vector.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        VectorTest::VectorTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::VectorTest", tempPath, context)
        {}
        
        void VectorTest::run()
        {
            _compare();
            _serialize();
            _hash();
        }

        void VectorTest::_compare()
        {
            {
                const glm::vec2 a(0.F, 1.F);
                DJV_ASSERT(fuzzyCompare(a, a));
                DJV_ASSERT(!fuzzyCompare(a, glm::vec2(0.F, 0.F)));
            }
            
            {
                const glm::vec3 a(0.F, 1.F, 2.F);
                DJV_ASSERT(fuzzyCompare(a, a));
                DJV_ASSERT(!fuzzyCompare(a, glm::vec3(0.F, 0.F, 0.F)));
            }
            
            {
                const glm::vec4 a(0.F, 1.F, 2.F, 3.F);
                DJV_ASSERT(fuzzyCompare(a, a));
                DJV_ASSERT(!fuzzyCompare(a, glm::vec4(0.F, 0.F, 0.F, 0.F)));
            }
        }
        
        void VectorTest::_serialize()
        {
            {
                const glm::ivec2 v(0, 1);
                std::stringstream ss;
                ss << v;
                glm::ivec2 v2(0, 0);
                ss >> v2;
                DJV_ASSERT(v == v2);
            }

            try
            {
                glm::ivec2 v;
                std::stringstream ss;
                ss >> v;
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
            
            {
                const glm::vec2 v(0.F, 1.F);
                std::stringstream ss;
                ss << v;
                glm::vec2 v2(0.F, 0.F);
                ss >> v2;
                DJV_ASSERT(v == v2);
            }

            try
            {
                glm::vec2 v;
                std::stringstream ss;
                ss >> v;
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
            
            {
                const glm::vec3 v(0.F, 1.F, 2.F);
                std::stringstream ss;
                ss << v;
                glm::vec3 v2(0.F, 0.F, 0.F);
                ss >> v2;
                DJV_ASSERT(v == v2);
            }

            try
            {
                glm::vec3 v;
                std::stringstream ss;
                ss >> v;
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
            
            {
                const glm::vec4 v(0.F, 1.F, 2.F, 3.F);
                std::stringstream ss;
                ss << v;
                glm::vec4 v2(0.F, 0.F, 0.F, 0.F);
                ss >> v2;
                DJV_ASSERT(v == v2);
            }

            try
            {
                glm::vec4 v;
                std::stringstream ss;
                ss >> v;
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
            
            {
                const glm::ivec2 v(0, 1);
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(v, allocator);
                glm::ivec2 v2(0, 0);
                fromJSON(json, v2);
                DJV_ASSERT(v == v2);
            }

            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                glm::ivec2 v;
                fromJSON(json, v);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
            
            {
                const glm::vec2 v(0.F, 1.F);
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(v, allocator);
                glm::vec2 v2(0.F, 0.F);
                fromJSON(json, v2);
                DJV_ASSERT(v == v2);
            }

            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                glm::vec2 v;
                fromJSON(json, v);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
            
            {
                const glm::vec3 v(0.F, 1.F, 2.F);
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(v, allocator);
                glm::vec3 v2(0.F, 0.F, 0.F);
                fromJSON(json, v2);
                DJV_ASSERT(v == v2);
            }

            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                glm::vec3 v;
                fromJSON(json, v);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
            
            {
                const glm::vec4 v(0.F, 1.F, 2.F, 3.F);
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(v, allocator);
                glm::vec4 v2(0.F, 0.F, 0.F, 0.F);
                fromJSON(json, v2);
                DJV_ASSERT(v == v2);
            }

            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                glm::vec4 v;
                fromJSON(json, v);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }
        
        void VectorTest::_hash()
        {
            {
                size_t i = 0;
                const glm::ivec2 v(0, 1);
                Memory::hashCombine(i, v);
                std::stringstream ss;
                ss << "hash: " << v << " = " << i;
                _print(ss.str());
            }
            
            {
                size_t i = 0;
                const glm::vec2 v(0.F, 1.F);
                Memory::hashCombine(i, v);
                std::stringstream ss;
                ss << "hash: " << v << " = " << i;
                _print(ss.str());
            }
        }

    } // namespace CoreTest
} // namespace djv

