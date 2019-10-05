//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
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

#include <djvCoreTest/RangeTest.h>

#include <djvCore/Range.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        RangeTest::RangeTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::RangeTest", context)
        {}
        
        void RangeTest::run(const std::vector<std::string>& args)
        {
            _ctor();
            _util();
            _operators();
            _serialize();
        }

        void RangeTest::_ctor()
        {
            {
                IntRange range;
                DJV_ASSERT(0 == range.min);
                DJV_ASSERT(0 == range.max);
            }
            
            {
                IntRange range(1);
                DJV_ASSERT(1 == range.min);
                DJV_ASSERT(1 == range.max);
            }
            
            {
                IntRange range(1, 10);
                DJV_ASSERT(1 == range.min);
                DJV_ASSERT(10 == range.max);
            }
        }
        
        void RangeTest::_util()
        {
            {
                IntRange range(1, 10);
                range.zero();
                DJV_ASSERT(0 == range.min);
                DJV_ASSERT(0 == range.max);
            }
            
            {
                IntRange range(1, 10);
                DJV_ASSERT(range.intersects(IntRange(5, 20)));
                DJV_ASSERT(!range.intersects(IntRange(11, 20)));
            }
            
            {
                IntRange range(1, 10);
                range.expand(20);
                DJV_ASSERT(IntRange(1, 20) == range);
            }
            
            {
                IntRange range(1, 10);
                range.expand(IntRange(5, 20));
                DJV_ASSERT(IntRange(1, 20) == range);
            }            
            
            {
                IntRange range(10, 1);
                range.sort();
                DJV_ASSERT(IntRange(1, 10) == range);
            }
        }
        
        void RangeTest::_operators()
        {
            {
                IntRange range(1, 10);
                DJV_ASSERT(range == range);
                DJV_ASSERT(IntRange() != range);
                DJV_ASSERT(IntRange() < range);
            }
        }
        
        void RangeTest::_serialize()
        {
            {
                const IntRange range(1, 10);
                std::stringstream ss;
                ss << range;
                IntRange range2;
                ss >> range2;
                DJV_ASSERT(range == range2);
            }
        }
        
    } // namespace CoreTest
} // namespace djv

