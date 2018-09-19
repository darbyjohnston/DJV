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

#include <djvCoreTest/MathTest.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/Math.h>

#include <QString>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        void MathTest::run(int &, char **)
        {
            DJV_DEBUG("MathTest::run");
            members();
        }

        void MathTest::members()
        {
            DJV_DEBUG("MathTest::members");
            {
                DJV_ASSERT(10 == Math::abs<int>(10));
                DJV_ASSERT(10 == Math::abs<int>(-10));
            }
            {
                DJV_ASSERT(4 == Math::pow(2, 2));
                DJV_ASSERT(Math::fuzzyCompare(4.f, Math::pow(2.f, 2.f)));
            }
            {
                DJV_ASSERT(Math::fuzzyCompare(2.f, Math::sqrt(4.f)));
            }
            {
                DJV_ASSERT(0 == Math::mod<int>(2, 1));
            }
            {
                DJV_ASSERT(1 == Math::wrap<int>(-1, 0, 1));
                DJV_ASSERT(0 == Math::wrap<int>(0, 0, 1));
                DJV_ASSERT(1 == Math::wrap<int>(1, 0, 1));
                DJV_ASSERT(0 == Math::wrap<int>(2, 0, 1));
            }
            {
                DJV_ASSERT(-1 == Math::step<int>(-1, 1));
                DJV_ASSERT(0 == Math::step<int>(0, 1));
                DJV_ASSERT(1 == Math::step<int>(1, 1));
                DJV_ASSERT(1 == Math::step<int>(2, 1));
            }
            {
                DJV_ASSERT(0 == Math::pulse<int>(-1, 0, 1));
                DJV_ASSERT(0 == Math::pulse<int>(0, 0, 1));
                DJV_ASSERT(1 == Math::pulse<int>(1, 0, 1));
                DJV_ASSERT(1 == Math::pulse<int>(2, 0, 1));
            }
            {
                DJV_ASSERT(0 == Math::min(0, 1));
                DJV_ASSERT(1 == Math::max(0, 1));
            }
            {
                DJV_ASSERT(0 == Math::clamp(-1, 0, 1));
                DJV_ASSERT(0 == Math::clamp(0, 0, 1));
                DJV_ASSERT(1 == Math::clamp(1, 0, 1));
                DJV_ASSERT(1 == Math::clamp(2, 0, 1));
            }
            {
                DJV_ASSERT(-1 == Math::lerp(-1, 0, 1));
                DJV_ASSERT(0 == Math::lerp(0, 0, 1));
                DJV_ASSERT(1 == Math::lerp(1, 0, 1));
                DJV_ASSERT(2 == Math::lerp(2, 0, 1));
            }
            {
                DJV_ASSERT(1 == Math::floor(1.5f));
                DJV_ASSERT(2 == Math::ceil(1.5f));
                DJV_ASSERT(2 == Math::round(1.5f));
            }
            {
                DJV_ASSERT(Math::fuzzyCompare(.5f, Math::fraction(1.5f)));
            }
            {
                DJV_ASSERT(4 == Math::toPow2(3));
            }
            {
                DJV_DEBUG_PRINT(Math::degreesToRadians(180.f));
                DJV_DEBUG_PRINT(Math::radiansToDegrees(Math::pi));
                //DJV_ASSERT(Math::fuzzyCompare(Math::pi, Math::degreesToRadians(180.f)));
                //DJV_ASSERT(Math::fuzzyCompare(180.f, Math::radiansToDegrees(Math::pi)));
            }
        }

    } // namespace CoreTest
} // namespace djv
