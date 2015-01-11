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

//! \file djvTimerTest.cpp

#include <djvTimerTest.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvMath.h>
#include <djvTime.h>
#include <djvTimer.h>

#include <QString>

void djvTimerTest::run(int &, char **)
{
    DJV_DEBUG("djvTimerTest::run");
    
    ctors();
    operators();
}

void djvTimerTest::ctors()
{
    DJV_DEBUG("djvTimerTest::ctors");
    
    {
        const djvTimer timer;
        
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, timer.seconds()));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, timer.fps()));
    }
    
    {
        djvTimer a;
        
        a.start();
        djvTime::msleep(1);
        a.check();
        
        djvTimer b(a);
        
        DJV_ASSERT(djvMath::fuzzyCompare(a.seconds(), b.seconds()));
        DJV_ASSERT(djvMath::fuzzyCompare(a.fps(), b.fps()));
    }
}

void djvTimerTest::operators()
{
    DJV_DEBUG("djvTimerTest::operators");
    
    {
        djvTimer a;
        
        a.start();
        djvTime::msleep(1);
        a.check();
        
        djvTimer b;
        
        b = a;
        
        DJV_ASSERT(djvMath::fuzzyCompare(a.seconds(), b.seconds()));
        DJV_ASSERT(djvMath::fuzzyCompare(a.fps(), b.fps()));
    }
}

