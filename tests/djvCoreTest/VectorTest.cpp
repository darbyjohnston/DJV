//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

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
        
        void VectorTest::run(const std::vector<std::string>& args)
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

