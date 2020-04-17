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
        VectorTest::VectorTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::VectorTest", context)
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
                auto json = toJSON(v);
                glm::ivec2 v2(0, 0);
                fromJSON(json, v2);
                DJV_ASSERT(v == v2);
            }

            try
            {
                auto json = picojson::value(picojson::object_type, true);
                glm::ivec2 v;
                fromJSON(json, v);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
            
            {
                const glm::vec2 v(0.F, 1.F);
                auto json = toJSON(v);
                glm::vec2 v2(0.F, 0.F);
                fromJSON(json, v2);
                DJV_ASSERT(v == v2);
            }

            try
            {
                auto json = picojson::value(picojson::object_type, true);
                glm::vec2 v;
                fromJSON(json, v);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
            
            {
                const glm::vec3 v(0.F, 1.F, 2.F);
                auto json = toJSON(v);
                glm::vec3 v2(0.F, 0.F, 0.F);
                fromJSON(json, v2);
                DJV_ASSERT(v == v2);
            }

            try
            {
                auto json = picojson::value(picojson::object_type, true);
                glm::vec3 v;
                fromJSON(json, v);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
            
            {
                const glm::vec4 v(0.F, 1.F, 2.F, 3.F);
                auto json = toJSON(v);
                glm::vec4 v2(0.F, 0.F, 0.F, 0.F);
                fromJSON(json, v2);
                DJV_ASSERT(v == v2);
            }

            try
            {
                auto json = picojson::value(picojson::object_type, true);
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

