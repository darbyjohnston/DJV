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

#include <djvCoreTest/SequenceUtilTest.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/Sequence.h>
#include <djvCore/SequenceUtil.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        void SequenceUtilTest::run(int &, char **)
        {
            DJV_DEBUG("SequenceUtilTest::run");
            frameToString();
            stringToFrame();
            sequenceToString();
            stringToSequence();
        }

        void SequenceUtilTest::frameToString()
        {
            DJV_DEBUG("SequenceUtilTest::frameToString");
            const struct Data
            {
                qint64  a;
                QString b;
                int     pad;
            }
            data[] =
            {
                {      0,       "0", 0 },
                {      0,    "0000", 4 },
                {    100,    "0100", 4 },
                {   1000,    "1000", 4 },
                {  10000,   "10000", 4 },
                {     -1,      "-1", 0 },
                {     -1,   "-0001", 4 },
                {   -100,   "-0100", 4 },
                {  -1000,   "-1000", 4 },
                { -10000,  "-10000", 4 },

                {  1370468628437,  "1370468628437", 0 },
                { -1370468628437, "-1370468628437", 0 }
            };
            const int dataCount = sizeof(data) / sizeof(Data);
            for (int i = 0; i < dataCount; ++i)
            {
                const QString tmp = SequenceUtil::frameToString(data[i].a, data[i].pad);
                DJV_DEBUG_PRINT(data[i].a << "(" << data[i].pad << ") = " << tmp);
                DJV_ASSERT(tmp == data[i].b);
            }
        }

        void SequenceUtilTest::stringToFrame()
        {
            DJV_DEBUG("SequenceUtilTest::stringToFrame");
            const struct Data
            {
                QString a;
                qint64  b;
                int     pad;
            }
            data[] =
            {
                {     "0",     0, 0 },
                {  "0000",     0, 4 },
                {  "0100",   100, 4 },
                {  "1000",  1000, 0 },
                {    "-1",    -1, 0 },
                { "-0001",    -1, 4 },
                { "-0100",  -100, 4 },
                { "-1000", -1000, 0 },

                {  "1370468628437",  1370468628437, 0 },
                { "-1370468628437", -1370468628437, 0 },

                { "#", -1, 0 }
            };
            const int dataCount = sizeof(data) / sizeof(Data);
            for (int i = 0; i < dataCount; ++i)
            {
                int pad = 0;
                const qint64 tmp = SequenceUtil::stringToFrame(data[i].a, &pad);
                DJV_DEBUG_PRINT(data[i].a << " = " << tmp << "(" << pad << ")");
                DJV_ASSERT(tmp == data[i].b);
                DJV_ASSERT(pad == data[i].pad);
            }
        }

        void SequenceUtilTest::sequenceToString()
        {
            DJV_DEBUG("SequenceUtilTest::sequenceToString");
            const struct Data
            {
                Sequence a;
                QString  b;
            }
            data[] =
            {
                { Sequence(FrameList()), "" },
                { Sequence(FrameList() << 1), "1" },
                { Sequence(FrameList() << 1 << 2), "1-2" },
                { Sequence(FrameList() << 1 << 2 << 3), "1-3" },
                { Sequence(FrameList() << 1 << 2 << 3, 4), "0001-0003" },
                { Sequence(FrameList() << 3 << 2 << 1), "3-1" },
                { Sequence(FrameList() << 1 << 2 << 3 << 5), "1-3,5" },
                { Sequence(FrameList() << 1 << 2 << 3 << 5 << 6), "1-3,5-6" },
                { Sequence(FrameList() << 1 << 2 << 3 << 5 << 6 << 7), "1-3,5-7" },
                { Sequence(FrameList() << 1 << 2 << 3 << 3 << 2 << 1), "1-3,3-1" },
                { Sequence(FrameList() << -1), "-1" },
                { Sequence(FrameList() << -1 << -2), "-1--2" },
                { Sequence(FrameList() << -1 << -2 << -3), "-1--3" },
                { Sequence(FrameList() << -1 << -2 << -3, 4), "-0001--0003" },
                { Sequence(FrameList() << -3 << -2 << -1), "-3--1" },
                { Sequence(FrameList() << -1 << -2 << -3 << -5), "-1--3,-5" },
                { Sequence(FrameList() << -1 << -2 << -3 << -5 << -6), "-1--3,-5--6" },
                { Sequence(FrameList() << -1 << -2 << -3 << -5 << -6 << -7), "-1--3,-5--7" },
                { Sequence(FrameList() << -1 << -2 << -3 << -3 << -2 << -1), "-1--3,-3--1" },
                { Sequence(FrameList() << -3 << -2 << -1 << 0 << 1 << 2 << 3), "-3-3" },
                { Sequence(FrameList() << 3 << 2 << 1 << 0 << -1 << -2 << -3), "3--3" }
            };
            const int dataCount = sizeof(data) / sizeof(Data);
            for (int i = 0; i < dataCount; ++i)
            {
                const QString tmp = SequenceUtil::sequenceToString(data[i].a);
                DJV_DEBUG_PRINT(data[i].a.frames << " = " << tmp);
                DJV_ASSERT(tmp == data[i].b);
            }
        }

        void SequenceUtilTest::stringToSequence()
        {
            DJV_DEBUG("SequenceUtilTest::stringToSequence");
            const struct Data
            {
                QString  a;
                Sequence b;
            }
            data[] =
            {
                { "", Sequence(FrameList()) },
                { "1", Sequence(FrameList() << 1) },
                { "1-2", Sequence(FrameList() << 1 << 2) },
                { "1-3", Sequence(FrameList() << 1 << 2 << 3) },
                { "0001-0003", Sequence(FrameList() << 1 << 2 << 3, 4) },
                { "3-1", Sequence(FrameList() << 3 << 2 << 1) },
                { "1,2,3", Sequence(FrameList() << 1 << 2 << 3) },
                { "1-3,5", Sequence(FrameList() << 1 << 2 << 3 << 5) },
                { "1-3,5-6", Sequence(FrameList() << 1 << 2 << 3 << 5 << 6) },
                { "1-3,5-7", Sequence(FrameList() << 1 << 2 << 3 << 5 << 6 << 7) },
                { "1-3,3-1", Sequence(FrameList() << 1 << 2 << 3 << 3 << 2 << 1) },
                { "-1", Sequence(FrameList() << -1) },
                { "-1--2", Sequence(FrameList() << -1 << -2) },
                { "-1--3", Sequence(FrameList() << -1 << -2 << -3) },
                { "-0001--0003", Sequence(FrameList() << -1 << -2 << -3, 4) },
                { "-3--1", Sequence(FrameList() << -3 << -2 << -1) },
                { "-1,-2,-3", Sequence(FrameList() << -1 << -2 << -3) },
                { "-1--3,-5", Sequence(FrameList() << -1 << -2 << -3 << -5) },
                { "-1--3,-5--6", Sequence(FrameList() << -1 << -2 << -3 << -5 << -6) },
                { "-1--3,-5--7", Sequence(FrameList() << -1 << -2 << -3 << -5 << -6 << -7) },
                { "-1--3,-3--1", Sequence(FrameList() << -1 << -2 << -3 << -3 << -2 << -1) },
                { "-3-3", Sequence(FrameList() << -3 << -2 << -1 << 0 << 1 << 2 << 3) },
                { "3--3", Sequence(FrameList() << 3 << 2 << 1 << 0 << -1 << -2 << -3) }
            };
            const int dataCount = sizeof(data) / sizeof(Data);
            for (int i = 0; i < dataCount; ++i)
            {
                const Sequence tmp = SequenceUtil::stringToSequence(data[i].a);
                DJV_DEBUG_PRINT(data[i].a << " = " << tmp);
                DJV_ASSERT(tmp == data[i].b);
            }
        }

    } // namespace CoreTest
} // namespace djv
