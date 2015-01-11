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

//! \file djvMemoryBufferTest.cpp

#include <djvMemoryBufferTest.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvMemoryBuffer.h>

#include <QString>

void djvMemoryBufferTest::run(int &, char **)
{
    DJV_DEBUG("djvMemoryBufferTest::run");
    
    ctors();
    members();
    operators();
}

void djvMemoryBufferTest::ctors()
{
    DJV_DEBUG("djvMemoryBufferTest::ctors");
    
    {
        const djvMemoryBuffer<quint8> buf;
        
        DJV_ASSERT(! buf.data());
        DJV_ASSERT(! buf.size());
    }
    
    {
        djvMemoryBuffer<quint8> tmp(2);
        tmp()[0] = 127;
        tmp()[1] = 255;
        
        const djvMemoryBuffer<quint8> buf(tmp);
        
        DJV_ASSERT(2 == buf.size());
        DJV_ASSERT(127 == buf()[0]);
        DJV_ASSERT(255 == buf()[1]);
    }
}

void djvMemoryBufferTest::members()
{
    DJV_DEBUG("djvMemoryBufferTest::members");
    
    {
        djvMemoryBuffer<quint8> buf;
        
        DJV_ASSERT(! buf.data());

        buf.setSize(2);
        
        DJV_ASSERT(2 == buf.size());
        
        buf.zero();
        
        DJV_ASSERT(0 == buf()[0]);
        DJV_ASSERT(0 == buf()[1]);
    }
}

void djvMemoryBufferTest::operators()
{
    DJV_DEBUG("djvMemoryBufferTest::operators");
    
    {
        djvMemoryBuffer<quint8> tmp(2);
        tmp()[0] = 127;
        tmp()[1] = 255;
        
        djvMemoryBuffer<quint8> buf;
        
        buf = tmp;
    
        DJV_ASSERT(2 == buf.size());
        DJV_ASSERT(127 == buf()[0]);
        DJV_ASSERT(255 == buf()[1]);
    }
}

