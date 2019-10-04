//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvCoreTest/BBoxTest.h>

#include <djvCore/BBox.h>

namespace djv
{
    using namespace Core;

    namespace CoreTest
    {
        BBoxTest::BBoxTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::BBoxTest", context)
        {}
        
        void BBoxTest::run(const std::vector<std::string>& args)
        {
            _ctors();
            _components();
            _utils();
            _comparison();
            _operators();
            _serialize();
        }

        void BBoxTest::_ctors()
        {
            {
                BBox2i b;
                b = BBox2i(glm::ivec2(0, 1));
                DJV_ASSERT(0 == b.min.x);
                DJV_ASSERT(1 == b.min.y);
                DJV_ASSERT(0 == b.max.x);
                DJV_ASSERT(1 == b.max.y);
                b = BBox2i(glm::ivec2(0, 1), glm::ivec2(2, 3));
                DJV_ASSERT(0 == b.min.x);
                DJV_ASSERT(1 == b.min.y);
                DJV_ASSERT(2 == b.max.x);
                DJV_ASSERT(3 == b.max.y);
                b = BBox2i(0, 1, 2, 3);
                DJV_ASSERT(0 == b.min.x);
                DJV_ASSERT(1 == b.min.y);
                DJV_ASSERT(1 == b.max.x);
                DJV_ASSERT(3 == b.max.y);
            }
            {
                BBox2f b;
                b = BBox2f(glm::vec2(0.F, 1.F));
                DJV_ASSERT(0.F == b.min.x);
                DJV_ASSERT(1.F == b.min.y);
                DJV_ASSERT(0.F == b.max.x);
                DJV_ASSERT(1.F == b.max.y);
                b = BBox2f(glm::vec2(0.F, 1.F), glm::vec2(2.F, 3.F));
                DJV_ASSERT(0.F == b.min.x);
                DJV_ASSERT(1.F == b.min.y);
                DJV_ASSERT(2.F == b.max.x);
                DJV_ASSERT(3.F == b.max.y);
                b = BBox2f(0.F, 1.F, 2.F, 3.F);
                DJV_ASSERT(0.F == b.min.x);
                DJV_ASSERT(1.F == b.min.y);
                DJV_ASSERT(2.F == b.max.x);
                DJV_ASSERT(4.F == b.max.y);
            }
            {
                BBox3f b;
                b = BBox3f(glm::vec3(0.F, 1.F, 2.F));
                DJV_ASSERT(0.F == b.min.x);
                DJV_ASSERT(1.F == b.min.y);
                DJV_ASSERT(2.F == b.min.z);
                DJV_ASSERT(0.F == b.max.x);
                DJV_ASSERT(1.F == b.max.y);
                DJV_ASSERT(2.F == b.max.z);
                b = BBox3f(glm::vec3(0.F, 1.F, 2.F), glm::vec3(3.F, 4.F, 5.F));
                DJV_ASSERT(0.F == b.min.x);
                DJV_ASSERT(1.F == b.min.y);
                DJV_ASSERT(2.F == b.min.z);
                DJV_ASSERT(3.F == b.max.x);
                DJV_ASSERT(4.F == b.max.y);
                DJV_ASSERT(5.F == b.max.z);
                b = BBox3f(0.F, 1.F, 2.F, 3.F, 4.F, 5.F);
                DJV_ASSERT(0.F == b.min.x);
                DJV_ASSERT(1.F == b.min.y);
                DJV_ASSERT(2.F == b.min.z);
                DJV_ASSERT(3.F == b.max.x);
                DJV_ASSERT(5.F == b.max.y);
                DJV_ASSERT(7.F == b.max.z);
            }
        }

        void BBoxTest::_components()
        {
            {
                BBox2i b(0, 1, 2, 3);
                DJV_ASSERT(0 == b.x());
                DJV_ASSERT(1 == b.y());
                DJV_ASSERT(2 == b.w());
                DJV_ASSERT(3 == b.h());
                DJV_ASSERT(b.isValid());
                b.max.x = 0;
                b.max.y = 0;
                DJV_ASSERT(!b.isValid());
                b = BBox2i(0, 1, 2, 4);
                DJV_ASSERT(glm::ivec2(2, 4) == b.getSize());
                DJV_ASSERT(glm::ivec2(1, 3) == b.getCenter());
                DJV_ASSERT(8 == b.getArea());
                DJV_ASSERT(.5F == b.getAspect());
                b.zero();
                DJV_ASSERT(0 == b.min.x);
                DJV_ASSERT(0 == b.min.y);
                DJV_ASSERT(0 == b.max.x);
                DJV_ASSERT(0 == b.max.y);
            }
            {
                BBox2f b(0.F, 1.F, 2.F, 3.F);
                DJV_ASSERT(0.F == b.x());
                DJV_ASSERT(1.F == b.y());
                DJV_ASSERT(2.F == b.w());
                DJV_ASSERT(3.F == b.h());
                DJV_ASSERT(b.isValid());
                b.max.x = 0.F;
                b.max.y = 0.F;
                DJV_ASSERT(!b.isValid());
                b = BBox2f(0.F, 1.F, 2.F, 4.F);
                DJV_ASSERT(glm::vec2(2.F, 4.F) == b.getSize());
                DJV_ASSERT(glm::vec2(1.F, 3.F) == b.getCenter());
                DJV_ASSERT(8.F == b.getArea());
                DJV_ASSERT(.5F == b.getAspect());
                b.zero();
                DJV_ASSERT(0.F == b.min.x);
                DJV_ASSERT(0.F == b.min.y);
                DJV_ASSERT(0.F == b.max.x);
                DJV_ASSERT(0.F == b.max.y);
            }
            {
                BBox3f b(0.F, 1.F, 2.F, 3.F, 4.F, 5.F);
                DJV_ASSERT(0.F == b.x());
                DJV_ASSERT(1.F == b.y());
                DJV_ASSERT(2.F == b.z());
                DJV_ASSERT(3.F == b.w());
                DJV_ASSERT(4.F == b.h());
                DJV_ASSERT(5.F == b.d());
                DJV_ASSERT(b.isValid());
                b.max.x = 0.F;
                b.max.y = 0.F;
                b.max.z = 0.F;
                DJV_ASSERT(!b.isValid());
                b = BBox3f(0.F, 1.F, 2.F, 3.f, 4.f, 5.f);
                DJV_ASSERT(glm::vec3(3.F, 4.F, 5.F) == b.getSize());
                DJV_ASSERT(glm::vec3(1.5F, 3.F, 4.5F) == b.getCenter());
                b.zero();
                DJV_ASSERT(0.F == b.min.x);
                DJV_ASSERT(0.F == b.min.y);
                DJV_ASSERT(0.F == b.min.z);
                DJV_ASSERT(0.F == b.max.x);
                DJV_ASSERT(0.F == b.max.y);
                DJV_ASSERT(0.F == b.max.z);
            }
        }

        void BBoxTest::_utils()
        {
            {
                BBox2i a(0, 1, 2, 3);
                DJV_ASSERT(a.contains(BBox2i(0, 1, 1, 1)));
                DJV_ASSERT(!a.contains(BBox2i(3, 5, 1, 1)));
                DJV_ASSERT(a.contains(glm::ivec2(0, 1)));
                DJV_ASSERT(!a.contains(glm::ivec2(0, 0)));
                DJV_ASSERT(a.intersects(BBox2i(0, 1, 3, 4)));
                const BBox2i b = a.intersect(BBox2i(1, 2, 3, 4));
                DJV_ASSERT(b == BBox2i(1, 2, 1, 2));
                a.expand(BBox2i(1, 2, 3, 4));
                DJV_ASSERT(a == BBox2i(0, 1, 4, 5));
                a.expand(glm::ivec2(4, 6));
                DJV_ASSERT(a == BBox2i(0, 1, 5, 6));
                a = a.margin(glm::ivec2(1, 2));
                DJV_ASSERT(a == BBox2i(-1, -1, 7, 10));
                a = a.margin(-1);
                DJV_ASSERT(a == BBox2i(0, 0, 5, 8));
                a = a.margin(1, 2, 3, 4);
                DJV_ASSERT(a == BBox2i(-1, -2, 9, 14));
            }
            {
                BBox2f a(0.F, 1.F, 2.F, 3.F);
                DJV_ASSERT(a.contains(BBox2f(0.F, 1.F, 1.F, 1.F)));
                DJV_ASSERT(!a.contains(BBox2f(3.F, 5.F, 1.F, 1.F)));
                DJV_ASSERT(a.contains(glm::vec2(0.F, 1.F)));
                DJV_ASSERT(!a.contains(glm::vec2(0.F, 0.F)));
                DJV_ASSERT(a.intersects(BBox2f(0.F, 1.F, 3.F, 4.F)));
                const BBox2f b = a.intersect(BBox2f(1.F, 2.F, 3.F, 4.F));
                DJV_ASSERT(b == BBox2f(1.F, 2.F, 1.F, 2.F));
                a.expand(BBox2f(1.F, 2.F, 3.F, 4.F));
                DJV_ASSERT(a == BBox2f(0.F, 1.F, 4.F, 5.F));
                a.expand(glm::vec2(4.F, 6.F));
                DJV_ASSERT(a == BBox2f(0.F, 1.F, 4.F, 5.F));
                a = a.margin(glm::vec2(1.F, 2.F));
                DJV_ASSERT(a == BBox2f(-1.F, -1.F, 6.F, 9.F));
                a = a.margin(-1.F);
                DJV_ASSERT(a == BBox2f(0.F, 0.F, 4.F, 7.F));
                a = a.margin(1.F, 2.F, 3.F, 4.F);
                DJV_ASSERT(a == BBox2f(-1.F, -2.F, 8.F, 13.F));
            }
            {
                BBox3f a(0.F, 1.F, 2.F, 3.F, 4.f, 5.f);
                DJV_ASSERT(a.contains(BBox3f(0.F, 1.F, 2.f, 1.F, 1.F, 1.F)));
                DJV_ASSERT(!a.contains(BBox3f(3.F, 5.F, 7.F, 1.F, 1.F, 1.F)));
                DJV_ASSERT(a.contains(glm::vec3(0.F, 1.F, 2.F)));
                DJV_ASSERT(!a.contains(glm::vec3(0.F, 0.F, 0.F)));
                DJV_ASSERT(a.intersects(BBox3f(0.F, 1.F, 2.f, 3.F, 4.F, 5.f)));
                const BBox3f b = a.intersect(BBox3f(1.F, 2.F, 3.F, 3.F, 4.F, 5.F));
                DJV_ASSERT(b == BBox3f(1.F, 2.F, 3.F, 2.F, 3.F, 4.F));
                a.expand(BBox3f(1.F, 2.F, 3.F, 4.F, 5.F, 6.F));
                DJV_ASSERT(a == BBox3f(0.F, 1.F, 2.F, 5.F, 6.F, 7.F));
                a.expand(glm::vec3(4.F, 6.F, 8.F));
                DJV_ASSERT(a == BBox3f(0.F, 1.F, 2.F, 5.F, 6.F, 7.F));
            }
        }

        void BBoxTest::_comparison()
        {
            {
                const BBox2i a(0, 1, 2, 3);
                const BBox2i b(0, 0, 0, 0);
                DJV_ASSERT(a == a);
                DJV_ASSERT(a != b);
            }
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

        void BBoxTest::_operators()
        {
            {
                BBox3f a(0.F, 1.F, 2.F, 3.F, 4.F, 5.F);
                BBox3f b = a * glm::mat4x4(1.f);
                DJV_ASSERT(a == b);
            }
        }

        void BBoxTest::_serialize()
        {
            {
                const BBox2i a(0, 1, 2, 3);
                auto json = toJSON(a);
                BBox2i b;
                fromJSON(json, b);
                DJV_ASSERT(a == b);
                std::stringstream ss;
                ss << a;
                ss >> b;
                DJV_ASSERT(a == b);
            }
            {
                const BBox2f a(0.F, 1.F, 2.F, 3.F);
                auto json = toJSON(a);
                BBox2f b;
                fromJSON(json, b);
                DJV_ASSERT(a == b);
                std::stringstream ss;
                ss << a;
                ss >> b;
                DJV_ASSERT(a == b);
            }
            {
                const BBox3f a(0.F, 1.F, 2.F, 3.F, 4.F, 5.F);
                auto json = toJSON(a);
                BBox3f b;
                fromJSON(json, b);
                DJV_ASSERT(a == b);
                std::stringstream ss;
                ss << a;
                ss >> b;
                DJV_ASSERT(a == b);
            }
        }

    } // namespace CoreTest
} // namespace djv
