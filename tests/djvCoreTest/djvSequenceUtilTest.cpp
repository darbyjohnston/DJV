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

//! \file djvSequenceUtilTest.cpp

#include <djvSequenceUtilTest.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvSequence.h>
#include <djvSequenceUtil.h>

void djvSequenceUtilTest::run(int &, char **)
{
    DJV_DEBUG("djvSequenceUtilTest::run");
    
    frameToString();
    stringToFrame();
    sequenceToString();
    stringToSequence();
}

void djvSequenceUtilTest::frameToString()
{
    DJV_DEBUG("djvSequenceUtilTest::frameToString");

    const struct Data
    {
        qint64  a;
        QString b;
        int     pad;
    }
        data [] =
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
        const QString tmp = djvSequenceUtil::frameToString(data[i].a, data[i].pad);

        DJV_DEBUG_PRINT(data[i].a << "(" << data[i].pad << ") = " << tmp);

        DJV_ASSERT(tmp == data[i].b);
    }
}

void djvSequenceUtilTest::stringToFrame()
{
    DJV_DEBUG("djvSequenceUtilTest::stringToFrame");

    const struct Data
    {
        QString a;
        qint64  b;
        int     pad;
    }
        data [] =
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
        
        const qint64 tmp = djvSequenceUtil::stringToFrame(data[i].a, &pad);

        DJV_DEBUG_PRINT(data[i].a << " = " << tmp << "(" << pad << ")");

        DJV_ASSERT(tmp == data[i].b);
        DJV_ASSERT(pad == data[i].pad);
    }
}

void djvSequenceUtilTest::sequenceToString()
{
    DJV_DEBUG("djvSequenceUtilTest::sequenceToString");

    const struct Data
    {
        djvSequence a;
        QString     b;
    }
        data [] =
    {
        { djvSequence(djvFrameList()), "" },

        { djvSequence(djvFrameList() << 1), "1" },
        { djvSequence(djvFrameList() << 1 << 2), "1-2" },
        { djvSequence(djvFrameList() << 1 << 2 << 3), "1-3" },
        { djvSequence(djvFrameList() << 1 << 2 << 3, 4), "0001-0003" },
        { djvSequence(djvFrameList() << 3 << 2 << 1), "3-1" },
        { djvSequence(djvFrameList() << 1 << 2 << 3 << 5), "1-3,5" },
        { djvSequence(djvFrameList() << 1 << 2 << 3 << 5 << 6), "1-3,5-6" },
        { djvSequence(djvFrameList() << 1 << 2 << 3 << 5 << 6 << 7), "1-3,5-7" },
        { djvSequence(djvFrameList() << 1 << 2 << 3 << 3 << 2 << 1), "1-3,3-1" },

        { djvSequence(djvFrameList() << -1), "-1" },
        { djvSequence(djvFrameList() << -1 << -2), "-1--2" },
        { djvSequence(djvFrameList() << -1 << -2 << -3), "-1--3" },
        { djvSequence(djvFrameList() << -1 << -2 << -3, 4), "-0001--0003" },
        { djvSequence(djvFrameList() << -3 << -2 << -1), "-3--1" },
        { djvSequence(djvFrameList() << -1 << -2 << -3 << -5), "-1--3,-5" },
        { djvSequence(djvFrameList() << -1 << -2 << -3 << -5 << -6), "-1--3,-5--6" },
        { djvSequence(djvFrameList() << -1 << -2 << -3 << -5 << -6 << -7), "-1--3,-5--7" },
        { djvSequence(djvFrameList() << -1 << -2 << -3 << -3 << -2 << -1), "-1--3,-3--1" },
        
        { djvSequence(djvFrameList() << -3 << -2 << -1 << 0 << 1 << 2 << 3), "-3-3" },
        { djvSequence(djvFrameList() << 3 << 2 << 1 << 0 << -1 << -2 << -3), "3--3" }
    };
    
    const int dataCount = sizeof(data) / sizeof(Data);

    for (int i = 0; i < dataCount; ++i)
    {
        const QString tmp = djvSequenceUtil::sequenceToString(data[i].a);

        DJV_DEBUG_PRINT(data[i].a.frames << " = " << tmp);

        DJV_ASSERT(tmp == data[i].b);
    }
}

void djvSequenceUtilTest::stringToSequence()
{
    DJV_DEBUG("djvSequenceUtilTest::stringToSequence");

    const struct Data
    {
        QString     a;
        djvSequence b;
    }
        data [] =
    {
        { "", djvSequence(djvFrameList()) },
        
        { "1", djvSequence(djvFrameList() << 1) },
        { "1-2", djvSequence(djvFrameList() << 1 << 2) },
        { "1-3", djvSequence(djvFrameList() << 1 << 2 << 3) },
        { "0001-0003", djvSequence(djvFrameList() << 1 << 2 << 3, 4) },
        { "3-1", djvSequence(djvFrameList() << 3 << 2 << 1) },
        { "1,2,3", djvSequence(djvFrameList() << 1 << 2 << 3) },
        { "1-3,5", djvSequence(djvFrameList() << 1 << 2 << 3 << 5) },
        { "1-3,5-6", djvSequence(djvFrameList() << 1 << 2 << 3 << 5 << 6) },
        { "1-3,5-7", djvSequence(djvFrameList() << 1 << 2 << 3 << 5 << 6 << 7) },
        { "1-3,3-1", djvSequence(djvFrameList() << 1 << 2 << 3 << 3 << 2 << 1) },
        
        { "-1", djvSequence(djvFrameList() << -1) },
        { "-1--2", djvSequence(djvFrameList() << -1 << -2) },
        { "-1--3", djvSequence(djvFrameList() << -1 << -2 << -3) },
        { "-0001--0003", djvSequence(djvFrameList() << -1 << -2 << -3, 4) },
        { "-3--1", djvSequence(djvFrameList() << -3 << -2 << -1) },
        { "-1,-2,-3", djvSequence(djvFrameList() << -1 << -2 << -3) },
        { "-1--3,-5", djvSequence(djvFrameList() << -1 << -2 << -3 << -5) },
        { "-1--3,-5--6", djvSequence(djvFrameList() << -1 << -2 << -3 << -5 << -6) },
        { "-1--3,-5--7", djvSequence(djvFrameList() << -1 << -2 << -3 << -5 << -6 << -7) },
        { "-1--3,-3--1", djvSequence(djvFrameList() << -1 << -2 << -3 << -3 << -2 << -1) },
        
        { "-3-3", djvSequence(djvFrameList() << -3 << -2 << -1 << 0 << 1 << 2 << 3) },
        { "3--3", djvSequence(djvFrameList() << 3 << 2 << 1 << 0 << -1 << -2 << -3) }
    };
    
    const int dataCount = sizeof(data) / sizeof(Data);

    for (int i = 0; i < dataCount; ++i)
    {
        const djvSequence tmp = djvSequenceUtil::stringToSequence(data[i].a);

        DJV_DEBUG_PRINT(data[i].a << " = " << tmp);

        DJV_ASSERT(tmp == data[i].b);
    }
}

