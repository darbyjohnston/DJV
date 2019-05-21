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

#include <djvCoreTest/MathTest.h>

#include <djvCore/Math.h>
#include <djvCore/Rational.h>

namespace djv
{
    namespace CoreTest
    {
        MathTest::MathTest(Core::Context * context) :
            ITest("djv::CoreTest::MathTest", context)
        {}
        
        void MathTest::run(int & argc, char ** argv)
        {
            {
                DJV_ASSERT(Core::Math::clamp(0, 1, 2) == 1);
                DJV_ASSERT(Core::Math::clamp(3, 1, 2) == 2);
            }
            {
                const Core::Math::Rational r;
                DJV_ASSERT(0 == r.getNum());
                DJV_ASSERT(0 == r.getDen());
                DJV_ASSERT(!r.isValid());
            }
            {
                const Core::Math::Rational r(1, 2);
                DJV_ASSERT(1 == r.getNum());
                DJV_ASSERT(2 == r.getDen());
                DJV_ASSERT(r.isValid());
            }
            {
                const Core::Math::Rational r(2, 2);
                const float f = Core::Math::Rational::toFloat(r);
                DJV_ASSERT(1.f == f);
                DJV_ASSERT(Core::Math::Rational(1, 1) == Core::Math::Rational::fromFloat(f));
            }
        }
        
    } // namespace CoreTest
} // namespace djv

