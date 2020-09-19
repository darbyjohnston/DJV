// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvMathTest/FrameNumberTest.h>

#include <djvMath/FrameNumber.h>

#include <iostream>
#include <sstream>

using namespace djv::Core;
using namespace djv::Math;

namespace djv
{
    namespace MathTest
    {
        FrameNumberTest::FrameNumberTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::MathTest::FrameNumberTest", tempPath, context)
        {}
        
        void FrameNumberTest::run()
        {
            _sequence();
            _operators();
        }

        void FrameNumberTest::_sequence()
        {
            {
                const Frame::Sequence sequence;
                DJV_ASSERT(std::vector<Frame::Range>() == sequence.getRanges());
                DJV_ASSERT(0 == sequence.getPad());
                DJV_ASSERT(!sequence.isValid());
                DJV_ASSERT(!sequence.contains(0));
                DJV_ASSERT(0 == sequence.getFrameCount());
                DJV_ASSERT(Frame::invalid == sequence.getFrame(0));
                DJV_ASSERT(Frame::invalid == sequence.getIndex(0));
                DJV_ASSERT(Frame::invalid == sequence.getLastIndex());
            }
            
            {
                const Frame::Sequence sequence(Frame::Range(0, 99), 4);
                DJV_ASSERT(1 == sequence.getRanges().size());
                DJV_ASSERT(4 == sequence.getPad());
                DJV_ASSERT(sequence.isValid());
                DJV_ASSERT(sequence.contains(0));
                DJV_ASSERT(100 == sequence.getFrameCount());
                DJV_ASSERT(0 == sequence.getFrame(0));
                DJV_ASSERT(0 == sequence.getIndex(0));
                DJV_ASSERT(99 == sequence.getLastIndex());
            }
            
            {
                const Frame::Sequence sequence({ Frame::Range(0, 9), Frame::Range(10, 99) }, 4);
                DJV_ASSERT(1 == sequence.getRanges().size());
                DJV_ASSERT(4 == sequence.getPad());
                DJV_ASSERT(sequence.isValid());
                DJV_ASSERT(sequence.contains(0));
                DJV_ASSERT(100 == sequence.getFrameCount());
                DJV_ASSERT(0 == sequence.getFrame(0));
                DJV_ASSERT(0 == sequence.getIndex(0));
                DJV_ASSERT(99 == sequence.getLastIndex());
            }
            
            {
                Frame::Sequence sequence({ Frame::Range(10, 9), Frame::Range(3, 1) });
                const auto& ranges = sequence.getRanges();
                DJV_ASSERT(ranges[0] == Frame::Range(1, 3));
                DJV_ASSERT(ranges[1] == Frame::Range(9, 10));
            }

            {
                Frame::Sequence sequence;
                for (Frame::Number i = -10; i <= 10; ++i)
                {
                    sequence.add(Frame::Range(i));
                }
                DJV_ASSERT(sequence.getRanges()[0] == Frame::Range(-10, 10));
            }

            {
                Frame::Sequence sequence(Frame::Range(1, 3));
                sequence.add(Frame::Range(3, 10));
                DJV_ASSERT(sequence.getRanges()[0] == Frame::Range(1, 10));
                sequence.add(Frame::Range(12, 100));
                DJV_ASSERT(sequence.getRanges()[0] == Frame::Range(1, 10));
            }
        }
                
        void FrameNumberTest::_operators()
        {
            {
                const Frame::Sequence sequence(Frame::Range(1, 3));
                DJV_ASSERT(sequence == sequence);
                DJV_ASSERT(sequence != Frame::Sequence());
            }
        }
                
    } // namespace MathTest
} // namespace djv

