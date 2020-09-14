// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/FrameNumberFuncTest.h>

#include <djvCore/FrameNumberFunc.h>

#include <iostream>
#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        FrameNumberFuncTest::FrameNumberFuncTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::FrameNumberFuncTest", tempPath, context)
        {}
        
        void FrameNumberFuncTest::run()
        {
            _conversion();
            _serialize();
        }
        
        void FrameNumberFuncTest::_conversion()
        {
            {
                std::vector<Frame::Number> frames = {};
                const auto sequence = Frame::fromFrames(frames);
                {
                    std::stringstream ss;
                    ss << sequence;
                    _print(ss.str());
                }
                DJV_ASSERT(0 == sequence.isValid());
            }
            
            {
                std::vector<Frame::Number> frames = { 1 };
                const auto sequence = Frame::fromFrames(frames);
                {
                    std::stringstream ss;
                    ss << sequence;
                    _print(ss.str());
                }
                DJV_ASSERT(1 == sequence.isValid());
                DJV_ASSERT(1 == sequence.getRanges()[0].getMin());
                DJV_ASSERT(1 == sequence.getRanges()[0].getMax());
            }
            
            {
                std::vector<Frame::Number> frames = { 1, 2, 3 };
                const auto sequence = Frame::fromFrames(frames);
                {
                    std::stringstream ss;
                    ss << sequence;
                    _print(ss.str());
                }
                DJV_ASSERT(1 == sequence.isValid());
                DJV_ASSERT(1 == sequence.getRanges()[0].getMin());
                DJV_ASSERT(3 == sequence.getRanges()[0].getMax());
            }
            
            {
                std::vector<Frame::Number> frames = { 1, 3 };
                const auto sequence = Frame::fromFrames(frames);
                {
                    std::stringstream ss;
                    ss << sequence;
                    _print(ss.str());
                }
                const auto& ranges = sequence.getRanges();
                DJV_ASSERT(2 == ranges.size());
                DJV_ASSERT(1 == ranges[0].getMin());
                DJV_ASSERT(1 == ranges[0].getMax());
                DJV_ASSERT(3 == ranges[1].getMin());
                DJV_ASSERT(3 == ranges[1].getMax());
            }
            
            {
                std::vector<Frame::Number> frames = { 1, 2, 3, 5, 6, 8 };
                const auto sequence = Frame::fromFrames(frames);
                {
                    std::stringstream ss;
                    ss << sequence;
                    _print(ss.str());
                }
                const auto& ranges = sequence.getRanges();
                DJV_ASSERT(3 == ranges.size());
                DJV_ASSERT(1 == ranges[0].getMin());
                DJV_ASSERT(3 == ranges[0].getMax());
                DJV_ASSERT(5 == ranges[1].getMin());
                DJV_ASSERT(6 == ranges[1].getMax());
                DJV_ASSERT(8 == ranges[2].getMin());
                DJV_ASSERT(8 == ranges[2].getMax());
            }
            
            {
                const Frame::Sequence sequence(
                {
                    Frame::Range(1, 3),
                    Frame::Range(5, 6),
                    Frame::Range(8)
                });
                const size_t size = sequence.getFrameCount();
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
            }
            
            {
                const Frame::Range r(1, 10);
                DJV_ASSERT("1-10" == Frame::toString(r));
                Frame::Range r2;
                size_t pad = 0;
                Frame::fromString("1-10", r2, pad);
                DJV_ASSERT(r == r2);
                DJV_ASSERT(0 == pad);
                DJV_ASSERT("0001-0010" == Frame::toString(r, 4));
                Frame::fromString("0001-0010", r2, pad);
                DJV_ASSERT(r == r2);
                DJV_ASSERT(4 == pad);
            }
                        
            {
                const Frame::Sequence s(Frame::Range(1, 10));
                DJV_ASSERT("1-10" == Frame::toString(s));
                Frame::Sequence s2;
                Frame::fromString("1-10", s2);
                DJV_ASSERT(s == s2);
            }
                        
            {
                const Frame::Sequence s(Frame::Range(1, 10), 4);
                DJV_ASSERT("0001-0010" == Frame::toString(s));
                Frame::Sequence s2;
                Frame::fromString("0001-0010", s2);
                DJV_ASSERT(s == s2);
            }
            
            {
                Frame::Range range;
                size_t pad = 0;
                Frame::fromString("01-0010", range, pad);
                DJV_ASSERT(range == Frame::Range(1, 10));
                DJV_ASSERT(4 == pad);
            }
        }
        
        void FrameNumberFuncTest::_serialize()
        {
            {
                const Frame::Sequence s(Frame::Range(1, 3));
                std::stringstream ss;
                ss << s;
                Frame::Sequence s2;
                ss >> s2;
                DJV_ASSERT(s == s2);
            }
            
            try
            {
                Frame::Sequence s;
                std::stringstream ss;
                ss >> s;
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}

            {
                const Frame::Sequence s(Frame::Range(1, 10), 4);
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(s, allocator);
                Frame::Sequence s2;
                fromJSON(json, s2);
                DJV_ASSERT(s == s2);
            }
            
            try
            {
                auto json = rapidjson::Value();
                Frame::Sequence s;
                fromJSON(json, s);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }
                
    } // namespace CoreTest
} // namespace djv

