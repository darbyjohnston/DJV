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

//! \file djvRangeTest.cpp

#include <djvRangeTest.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvRange.h>
#include <djvRangeUtil.h>

void djvRangeTest::run(int &, char **)
{
    DJV_DEBUG("djvRangeTest::run");
    
    ctors();
    convert();
    operators();
}

void djvRangeTest::ctors()
{
    DJV_DEBUG("djvRangeTest::ctors");
    
    {
        const djvRange<int> range;
        
        DJV_ASSERT(0 == range.min);
        DJV_ASSERT(0 == range.max);
    }
    
    {
        const djvRange<int> range(1, 10);
        
        DJV_ASSERT( 1 == range.min);
        DJV_ASSERT(10 == range.max);
    }
}

void djvRangeTest::convert()
{
    DJV_DEBUG("djvRangeTest::convert");
    
    const struct Data
    {
        djvFrameList           a;
        QVector<djvFrameRange> b;
    }
        data [] =
    {
        {
            djvFrameList() << 1,
            QVector<djvFrameRange>() << djvFrameRange(1, 1)
        },
        {
            djvFrameList() << 1 << 2,
            QVector<djvFrameRange>() << djvFrameRange(1, 2)
        },
        {
            djvFrameList() << 1 << 2 << 3,
            QVector<djvFrameRange>() << djvFrameRange(1, 3)
        },
        {
            djvFrameList() << 1 << 3,
            QVector<djvFrameRange>() << djvFrameRange(1) << djvFrameRange(3)
        },
        {
            djvFrameList() << 1 << 2 << 4,
            QVector<djvFrameRange>() << djvFrameRange(1, 2) << djvFrameRange(4)
        },
        {
            djvFrameList() << 1 << 2 << 4 << 5,
            QVector<djvFrameRange>() << djvFrameRange(1, 2) << djvFrameRange(4, 5)
        }
    };
    
    const int dataCount = sizeof(data) / sizeof(Data);

    for (int i = 0; i < dataCount; ++i)
    {
        const djvFrameRangeList tmp = djvRangeUtil::range(data[i].a);
        
        DJV_ASSERT(tmp == data[i].b);
    }
}

void djvRangeTest::operators()
{
    DJV_DEBUG("djvRangeTest::operators");
    
    {
        const djvRange<int> range(1, 10);
        
        DJV_ASSERT(range == range);
        DJV_ASSERT(range != djvRange<int>());
    }
}

