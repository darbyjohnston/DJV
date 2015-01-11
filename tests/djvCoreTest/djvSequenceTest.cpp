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

//! \file djvSequenceTest.cpp

#include <djvSequenceTest.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvSequence.h>
#include <djvSequenceUtil.h>

void djvSequenceTest::run(int &, char **)
{
    DJV_DEBUG("djvSequenceTest::run");
    
    ctors();
    members();
    operators();
}

void djvSequenceTest::ctors()
{
    DJV_DEBUG("djvSequenceTest::ctors");
    
    {
        const djvSequence seq;
        
        DJV_ASSERT(seq.frames.isEmpty());
        DJV_ASSERT(0 == seq.pad);
        DJV_ASSERT(djvSpeed() == seq.speed);
    }
    
    {
        const djvFrameList frameList = djvFrameList() << 1 << 2 << 3;
        const djvSequence seq(frameList, 4, djvSpeed::FPS_24);
        
        DJV_ASSERT(frameList == seq.frames);
        DJV_ASSERT(4 == seq.pad);
        DJV_ASSERT(djvSpeed::FPS_24 == seq.speed);
    }
    
    {
        const djvFrameList frameList = djvFrameList() << 1 << 2 << 3;
        const djvSequence seq(1, 3, 4, djvSpeed::FPS_24);
        
        DJV_ASSERT(frameList == seq.frames);
        DJV_ASSERT(4 == seq.pad);
        DJV_ASSERT(djvSpeed::FPS_24 == seq.speed);
    }
}

void djvSequenceTest::members()
{
    DJV_DEBUG("djvSequenceTest::members");
    
    {
        djvSequence seq;

        DJV_ASSERT(0 == seq.start());
        DJV_ASSERT(0 == seq.end());
    }
    
    {
        djvSequence seq;
        
        seq.setFrames(1, 3);
        
        DJV_ASSERT(djvFrameList() << 1 << 2 << 3 == seq.frames);
        DJV_ASSERT(1 == seq.start());
        DJV_ASSERT(3 == seq.end());

        seq.setFrames(3, 1);
        
        DJV_ASSERT(djvFrameList() << 3 << 2 << 1 == seq.frames);
        DJV_ASSERT(3 == seq.start());
        DJV_ASSERT(1 == seq.end());

        seq.setFrames(0, djvSequence::maxFrames() - 1);

        DJV_ASSERT(0 == seq.start());
        DJV_ASSERT((djvSequence::maxFrames() - 1) == seq.end());

        seq.setFrames(0, djvSequence::maxFrames());

        DJV_ASSERT(0 == seq.start());
        DJV_ASSERT((djvSequence::maxFrames() - 1) == seq.end());

        seq.setFrames(djvSequence::maxFrames(), 0);

        DJV_ASSERT(djvSequence::maxFrames() == seq.start());
        DJV_ASSERT(1 == seq.end());
    }
    
    {
        djvSequence seq;
        
        seq.setFrames(-1, -3);
        
        DJV_ASSERT(djvFrameList() << -1 << -2 << -3 == seq.frames);
        DJV_ASSERT(-1 == seq.start());
        DJV_ASSERT(-3 == seq.end());

        seq.setFrames(-3, -1);
        
        DJV_ASSERT(djvFrameList() << -3 << -2 << -1 == seq.frames);
        DJV_ASSERT(-3 == seq.start());
        DJV_ASSERT(-1 == seq.end());

        seq.setFrames(-(djvSequence::maxFrames() - 1), 0);

        DJV_ASSERT(-(djvSequence::maxFrames() - 1) == seq.start());
        DJV_ASSERT(0 == seq.end());

        seq.setFrames(-djvSequence::maxFrames(), 0);

        DJV_ASSERT(-djvSequence::maxFrames() == seq.start());
        DJV_ASSERT(-1 == seq.end());

        seq.setFrames(0, -djvSequence::maxFrames());

        DJV_ASSERT(0 == seq.start());
        DJV_ASSERT(-(djvSequence::maxFrames() - 1) == seq.end());
    }
    
    {
        djvSequence seq(djvFrameList() << 2 << 3 << 1);
        seq.sort();

        DJV_ASSERT(djvFrameList() << 1 << 2 << 3 == seq.frames);
    }
    
    {
        djvSequence seq(djvFrameList() << -2 << -3 << -1);
        seq.sort();

        DJV_ASSERT(djvFrameList() << -3 << -2 << -1 == seq.frames);
    }
    
    {
        const djvSequence seq(djvFrameList() << 1 << 5 << 15);
        
        DJV_ASSERT(0 == djvSequenceUtil::findClosest( 1, seq.frames));
        DJV_ASSERT(1 == djvSequenceUtil::findClosest( 4, seq.frames));
        DJV_ASSERT(1 == djvSequenceUtil::findClosest( 6, seq.frames));
        DJV_ASSERT(2 == djvSequenceUtil::findClosest(12, seq.frames));
        DJV_ASSERT(2 == djvSequenceUtil::findClosest(15, seq.frames));
    }
    
    {
        const djvSequence seq(djvFrameList() << -15 << -5 << -1);
        
        DJV_ASSERT(2 == djvSequenceUtil::findClosest( -1, seq.frames));
        DJV_ASSERT(1 == djvSequenceUtil::findClosest( -4, seq.frames));
        DJV_ASSERT(1 == djvSequenceUtil::findClosest( -6, seq.frames));
        DJV_ASSERT(0 == djvSequenceUtil::findClosest(-12, seq.frames));
        DJV_ASSERT(0 == djvSequenceUtil::findClosest(-15, seq.frames));
    }
}

void djvSequenceTest::operators()
{
    DJV_DEBUG("djvSequenceTest::operators");

    {
        djvSequence a(1, 3), b(a);

        DJV_ASSERT(a == b);

        DJV_ASSERT(a != djvSequence());
    }

    {
        djvSequence seq;
        
        QStringList s = QStringList() << "1-3";
        s >> seq;
        
        DJV_ASSERT(djvFrameList() << 1 << 2 << 3 == seq.frames);
    }
    
    {
        djvSequence seq;
        
        QStringList s = QStringList() << "0001-0003";
        s >> seq;
        
        DJV_ASSERT(djvFrameList() << 1 << 2 << 3 == seq.frames);
    }
    
    {
        djvSequence seq(djvFrameList() << 1 << 2 << 3);
        
        QStringList s;
        s << seq;
        
        DJV_ASSERT(QStringList() << "1-3" == s);
    }

    {
        djvSequence seq(djvFrameList() << 1 << 2 << 3, 4);
        
        QStringList s;
        s << seq;
        
        DJV_ASSERT(QStringList() << "0001-0003" == s);
    }
    
    {
        DJV_DEBUG_PRINT(djvSequence::COMPRESS_RANGE);
    }
}

