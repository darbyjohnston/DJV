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

#include <djvCoreTest/RangeTest.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/Range.h>
#include <djvCore/RangeUtil.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        void RangeTest::run(int &, char **)
        {
            DJV_DEBUG("RangeTest::run");
            ctors();
            convert();
            operators();
        }

        void RangeTest::ctors()
        {
            DJV_DEBUG("RangeTest::ctors");
            {
                const Range<int> range;
                DJV_ASSERT(0 == range.min);
                DJV_ASSERT(0 == range.max);
            }
            {
                const Range<int> range(1, 10);
                DJV_ASSERT(1 == range.min);
                DJV_ASSERT(10 == range.max);
            }
        }

        void RangeTest::convert()
        {
            DJV_DEBUG("RangeTest::convert");
            const struct Data
            {
                FrameList           a;
                QVector<FrameRange> b;
            }
            data[] =
            {
                {
                    FrameList() << 1,
                    QVector<FrameRange>() << FrameRange(1, 1)
                },
                {
                    FrameList() << 1 << 2,
                    QVector<FrameRange>() << FrameRange(1, 2)
                },
                {
                    FrameList() << 1 << 2 << 3,
                    QVector<FrameRange>() << FrameRange(1, 3)
                },
                {
                    FrameList() << 1 << 3,
                    QVector<FrameRange>() << FrameRange(1) << FrameRange(3)
                },
                {
                    FrameList() << 1 << 2 << 4,
                    QVector<FrameRange>() << FrameRange(1, 2) << FrameRange(4)
                },
                {
                    FrameList() << 1 << 2 << 4 << 5,
                    QVector<FrameRange>() << FrameRange(1, 2) << FrameRange(4, 5)
                }
            };
            const int dataCount = sizeof(data) / sizeof(Data);
            for (int i = 0; i < dataCount; ++i)
            {
                const FrameRangeList tmp = RangeUtil::range(data[i].a);
                //! \bug Error C2893 on Windows?
                //DJV_ASSERT(tmp == data[i].b);
                DJV_ASSERT(tmp.size() == data[i].b.size());
                for (int j = 0; j < tmp.size(); ++j)
                {
                    DJV_ASSERT(tmp[j] == data[i].b[j]);
                }
            }
        }

        void RangeTest::operators()
        {
            DJV_DEBUG("RangeTest::operators");
            {
                const Range<int> range(1, 10);
                DJV_ASSERT(range == range);
                DJV_ASSERT(range != Range<int>());
            }
        }

    } // namespace CoreTest
} // namespace djv
