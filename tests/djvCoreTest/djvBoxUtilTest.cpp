//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

#include <djvBoxUtilTest.h>

#include <djvCore/Assert.h>
#include <djvCore/BoxUtil.h>
#include <djvCore/Debug.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        void BoxUtilTest::run(int &, char **)
        {
            DJV_DEBUG("BoxUtilTest::run");
            corners();
            swap();
            intersect();
            bound();
            expand();
            border();
            isSizeValid();
        }

        void BoxUtilTest::corners()
        {
            DJV_DEBUG("BoxUtilTest::corners");
            DJV_ASSERT(
                BoxUtil::corners(glm::ivec2(0, 10), glm::ivec2(9, 29)) == Box2i(0, 10, 10, 20));
            DJV_ASSERT(
                BoxUtil::corners(glm::vec2(0.f, 10.f), glm::vec2(10.f, 30.f)) ==
                Box2f(0.f, 10.f, 10.f, 20.f));
        }

        void BoxUtilTest::swap()
        {
            DJV_DEBUG("BoxUtilTest::swap");
            DJV_ASSERT(BoxUtil::swap(Box2i(0, 10, 10, 20)) == Box2i(10, 0, 20, 10));
        }

        void BoxUtilTest::intersect()
        {
            DJV_DEBUG("BoxUtilTest::intersect");
            DJV_ASSERT(
                BoxUtil::intersect(Box2i(0, 5, 10, 15), Box2i(5, 10, 15, 20)) ==
                Box2i(5, 10, 5, 10));
            DJV_ASSERT(BoxUtil::intersect(Box2i(0, 5, 10, 15), glm::ivec2(5, 10)));
            DJV_ASSERT(!BoxUtil::intersect(Box2i(0, 5, 10, 15), glm::ivec2(-1, -1)));
        }

        void BoxUtilTest::bound()
        {
            DJV_DEBUG("BoxUtilTest::bound");
            DJV_ASSERT(
                BoxUtil::bound(Box2i(0, 5, 5, 10), Box2i(5, 10, 10, 15)) ==
                Box2i(0, 5, 15, 20));
        }

        void BoxUtilTest::expand()
        {
            DJV_DEBUG("BoxUtilTest::expand");
            DJV_ASSERT(
                BoxUtil::expand(Box2i(0, 5, 5, 10), glm::ivec2(9, 19)) ==
                Box2i(0, 5, 10, 15));
        }

        void BoxUtilTest::border()
        {
            DJV_DEBUG("BoxUtilTest::border");
            DJV_ASSERT(
                BoxUtil::border(Box2i(0, 5, 5, 10), glm::ivec2(5, 5)) ==
                Box2i(-5, 0, 15, 20));
        }

        void BoxUtilTest::isSizeValid()
        {
            DJV_DEBUG("BoxUtilTest::isSizeValid");
            DJV_ASSERT(BoxUtil::isSizeValid(Box2i(0, 5, 10, 15)));
        }

    } // namespace CoreTest
} // namespace djv
