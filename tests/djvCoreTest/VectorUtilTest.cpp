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

#include <djvCoreTest/VectorUtilTest.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/StringUtil.h>
#include <djvCore/Vector.h>
#include <djvCore/VectorUtil.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        void VectorUtilTest::run(int &, char **)
        {
            DJV_DEBUG("VectorUtilTest::run");
            swap();
            range();
            isSizeValid();
            aspect();
            convert();
        }

        void VectorUtilTest::swap()
        {
            DJV_DEBUG("VectorUtilTest::swap");
            DJV_ASSERT(VectorUtil::swap(glm::ivec2(1, 2)) == glm::ivec2(2, 1));
        }

        void VectorUtilTest::range()
        {
            DJV_DEBUG("VectorUtilTest::range");
            DJV_ASSERT(VectorUtil::min(glm::ivec2(1, 2), glm::ivec2(3, 4)) == glm::ivec2(1, 2));
            DJV_ASSERT(VectorUtil::max(glm::ivec2(1, 2), glm::ivec2(3, 4)) == glm::ivec2(3, 4));
        }

        void VectorUtilTest::isSizeValid()
        {
            DJV_DEBUG("VectorUtilTest::isSizeValid");
            DJV_ASSERT(
                VectorUtil::isSizeValid(glm::ivec2(1, 2)) &&
                !VectorUtil::isSizeValid(glm::ivec2(-1, -2)));
        }

        void VectorUtilTest::aspect()
        {
            DJV_DEBUG("VectorUtilTest::aspect");
            DJV_ASSERT(VectorUtil::aspect(glm::ivec2(2, 1)) == 2.0);
        }

        void VectorUtilTest::convert()
        {
            DJV_DEBUG("VectorUtilTest::convert");
            glm::ivec2 v2i = VectorUtil::ceil(glm::vec2(.5f, .5f));
            DJV_ASSERT(v2i == glm::ivec2(1, 1));
            v2i = VectorUtil::floor(glm::vec2(.5f, .5f));
            DJV_ASSERT(v2i == glm::ivec2(0, 0));
            DJV_ASSERT((QStringList() << "1" << "2") == StringUtil::label(glm::ivec2(1, 2)));
        }

    } // namespace CoreTest
} // namespace djv
