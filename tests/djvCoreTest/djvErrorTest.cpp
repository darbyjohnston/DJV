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

//! \file djvErrorTest.cpp

#include <djvErrorTest.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvError.h>
#include <djvErrorUtil.h>

#include <QString>

void djvErrorTest::run(int &, char **)
{
    DJV_DEBUG("djvErrorTest::run");
    
    ctors();
    members();
    operators();
}

void djvErrorTest::ctors()
{
    DJV_DEBUG("djvErrorTest::ctors");
    
    {
        const djvError error;
        
        DJV_ASSERT(error.messages().isEmpty());
    }
    
    {
        const djvError error("string");
        
        DJV_ASSERT(error.messages()[0].prefix.isEmpty());
        DJV_ASSERT("string" == error.messages()[0].string);
    }
    
    {
        const djvError error("prefix", "string");
        
        DJV_ASSERT("prefix" == error.messages()[0].prefix);
        DJV_ASSERT("string" == error.messages()[0].string);
    }
    
    {
        const djvError error("prefix", "string");
        
        const djvError other(error);
        
        DJV_ASSERT(other.messages()[0].prefix == error.messages()[0].prefix);
        DJV_ASSERT(other.messages()[0].string == error.messages()[0].string);
    }
}

void djvErrorTest::members()
{
    DJV_DEBUG("djvErrorTest::members");
    
    {
        DJV_DEBUG_PRINT("last error = " << djvErrorUtil::lastError());
    }
}

void djvErrorTest::operators()
{
    DJV_DEBUG("djvErrorTest::operators");
    
    {
        djvError a;

        const djvError b("prefix", "string");        
        
        a = b;
        
        DJV_ASSERT(a.messages()[0].prefix == b.messages()[0].prefix);
        DJV_ASSERT(a.messages()[0].string == b.messages()[0].string);
    }
}

