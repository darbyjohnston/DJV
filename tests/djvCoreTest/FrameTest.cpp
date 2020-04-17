// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/FrameTest.h>

#include <djvCore/Frame.h>

#include <iostream>
#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        FrameTest::FrameTest(const std::shared_ptr<Context>& context) :
            ITest("djv::CoreTest::FrameTest", context)
        {}
        
        void FrameTest::run()
        {
            _sequence();
            _util();
            _conversion();
            _serialize();
        }

        void FrameTest::_sequence()
        {
            {
                const Frame::Sequence sequence;
                DJV_ASSERT(0 == sequence.ranges.size());
                DJV_ASSERT(0 == sequence.pad);
                DJV_ASSERT(!sequence.isValid());
                DJV_ASSERT(!sequence.contains(0));
                DJV_ASSERT(0 == sequence.getSize());
                DJV_ASSERT(Frame::invalid == sequence.getFrame(0));
                DJV_ASSERT(Frame::invalid == sequence.getIndex(0));
            }
            
            {
                const Frame::Sequence sequence(Frame::Range(0, 99), 4);
                DJV_ASSERT(1 == sequence.ranges.size());
                DJV_ASSERT(4 == sequence.pad);
                DJV_ASSERT(sequence.isValid());
                DJV_ASSERT(sequence.contains(0));
                DJV_ASSERT(100 == sequence.getSize());
                DJV_ASSERT(0 == sequence.getFrame(0));
                DJV_ASSERT(0 == sequence.getIndex(0));                
            }
            
            {
                const Frame::Sequence sequence({ Frame::Range(0, 9), Frame::Range(10, 99) }, 4);
                DJV_ASSERT(2 == sequence.ranges.size());
                DJV_ASSERT(4 == sequence.pad);
                DJV_ASSERT(sequence.isValid());
                DJV_ASSERT(sequence.contains(0));
                DJV_ASSERT(100 == sequence.getSize());
                DJV_ASSERT(0 == sequence.getFrame(0));
                DJV_ASSERT(0 == sequence.getIndex(0));                
            }
            
            {
                Frame::Sequence sequence({ Frame::Range(10, 9), Frame::Range(3, 1) });
                sequence.sort();
                DJV_ASSERT(sequence.ranges[0] == Frame::Range(1, 3));
                DJV_ASSERT(sequence.ranges[1] == Frame::Range(9, 10));
            }
            
            {
                Frame::Sequence sequence(Frame::Range(1, 3));
                sequence.merge(Frame::Range(3, 10));
                DJV_ASSERT(sequence.ranges[0] == Frame::Range(1, 10));
                sequence.merge(Frame::Range(12, 100));
                DJV_ASSERT(sequence.ranges[0] == Frame::Range(1, 10));
            }
        }
        
        void FrameTest::_util()
        {
            {
                Frame::Range range = Frame::invalidRange;
                DJV_ASSERT(!Frame::isValid(range));
                range = Frame::Range(1, 100);
                DJV_ASSERT(Frame::isValid(range));
            }
            
            {
                Frame::Range range(100, 1);
                Frame::sort(range);
                DJV_ASSERT(Frame::Range(1, 100) == range);
            }
        }
        
        void FrameTest::_conversion()
        {
            {
                std::vector<Frame::Number> frames = {};
                const auto sequence = Frame::fromFrames(frames);
                {
                    std::stringstream ss;
                    ss << sequence;
                    _print(ss.str());
                }
                DJV_ASSERT(0 == sequence.ranges.size());
            }
            
            {
                std::vector<Frame::Number> frames = { 1 };
                const auto sequence = Frame::fromFrames(frames);
                {
                    std::stringstream ss;
                    ss << sequence;
                    _print(ss.str());
                }
                DJV_ASSERT(1 == sequence.ranges.size());
                DJV_ASSERT(1 == sequence.ranges[0].min);
                DJV_ASSERT(1 == sequence.ranges[0].max);
            }
            
            {
                std::vector<Frame::Number> frames = { 1, 2, 3 };
                const auto sequence = Frame::fromFrames(frames);
                {
                    std::stringstream ss;
                    ss << sequence;
                    _print(ss.str());
                }
                DJV_ASSERT(1 == sequence.ranges.size());
                DJV_ASSERT(1 == sequence.ranges[0].min);
                DJV_ASSERT(3 == sequence.ranges[0].max);
            }
            
            {
                std::vector<Frame::Number> frames = { 1, 3 };
                const auto sequence = Frame::fromFrames(frames);
                {
                    std::stringstream ss;
                    ss << sequence;
                    _print(ss.str());
                }
                DJV_ASSERT(2 == sequence.ranges.size());
                DJV_ASSERT(1 == sequence.ranges[0].min);
                DJV_ASSERT(1 == sequence.ranges[0].max);
                DJV_ASSERT(3 == sequence.ranges[1].min);
                DJV_ASSERT(3 == sequence.ranges[1].max);
            }
            
            {
                std::vector<Frame::Number> frames = { 1, 2, 3, 5, 6, 8 };
                const auto sequence = Frame::fromFrames(frames);
                {
                    std::stringstream ss;
                    ss << sequence;
                    _print(ss.str());
                }
                DJV_ASSERT(3 == sequence.ranges.size());
                DJV_ASSERT(1 == sequence.ranges[0].min);
                DJV_ASSERT(3 == sequence.ranges[0].max);
                DJV_ASSERT(5 == sequence.ranges[1].min);
                DJV_ASSERT(6 == sequence.ranges[1].max);
                DJV_ASSERT(8 == sequence.ranges[2].min);
                DJV_ASSERT(8 == sequence.ranges[2].max);
            }
            
            {
                const Frame::Sequence sequence(
                {
                    Frame::Range(1, 3),
                    Frame::Range(5, 6),
                    Frame::Range(8)
                });
                const size_t size = sequence.getSize();
                DJV_ASSERT(6 == size);
                const std::vector<Frame::Number> frames = Frame::toFrames(sequence);
                for (size_t i = 0; i < size; ++i)
                {
                    const Frame::Number frame = sequence.getFrame(i);
                    DJV_ASSERT(frame == frames[i]);
                }
            }
            
            {
                const Frame::Sequence sequence(
                {
                    Frame::Range(1, 3),
                    Frame::Range(5, 6),
                    Frame::Range(8)
                });
                DJV_ASSERT(Frame::invalidIndex == sequence.getIndex(0));
                DJV_ASSERT(0 == sequence.getIndex(1));
                DJV_ASSERT(2 == sequence.getIndex(3));
                DJV_ASSERT(5 == sequence.getIndex(8));
            }
            
            {
                DJV_ASSERT("10" == Frame::toString(10));
                DJV_ASSERT("-10" == Frame::toString(-10));
                DJV_ASSERT("0010" == Frame::toString(10, 4));
                DJV_ASSERT("1-10" == Frame::toString(Frame::Range(1, 10)));
                DJV_ASSERT("0001-0010" == Frame::toString(Frame::Range(1, 10), 4));
                DJV_ASSERT("1-10" == Frame::toString(Frame::Sequence(Frame::Range(1, 10))));
                DJV_ASSERT("0001-0010" == Frame::toString(Frame::Sequence(Frame::Range(1, 10), 4)));
            }
            
            {
                Frame::Range range = Frame::invalidRange;
                size_t pad = 0;
                Frame::fromString("01-0010", range, pad);
                DJV_ASSERT(range == Frame::Range(1, 10));
                DJV_ASSERT(4 == pad);
            }
        }
        
        void FrameTest::_serialize()
        {
            {
                const Frame::Range range(1, 100);
                std::stringstream ss;
                ss << range;
                Frame::Range range2;
                ss >> range2;
                DJV_ASSERT(range == range2);
            }
        }
                
    } // namespace CoreTest
} // namespace djv

