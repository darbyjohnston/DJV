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

//! \file djvVectorTest.cpp

#include <djvVectorTest.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvStringUtil.h>
#include <djvVector.h>
#include <djvVectorUtil.h>

void djvVectorTest::run(int &, char **)
{
    DJV_DEBUG("djvVectorTest::run");
    
    ctors();
    members();
    operators();
}

void djvVectorTest::ctors()
{
    DJV_DEBUG("djvVectorTest::ctors");
    
    {
        const djvVector2i v;
        
        DJV_ASSERT(0 == v.x);
        DJV_ASSERT(0 == v.y);
    }
    
    {
        const djvVector3i v;
        
        DJV_ASSERT(0 == v.x);
        DJV_ASSERT(0 == v.y);
        DJV_ASSERT(0 == v.z);
    }
    
    {
        const djvVector2i v(djvVector2i(1, 2));
        
        DJV_ASSERT(1 == v.x);
        DJV_ASSERT(2 == v.y);
    }
    
    {
        const djvVector3i v(djvVector3i(1, 2, 3));
        
        DJV_ASSERT(1 == v.x);
        DJV_ASSERT(2 == v.y);
        DJV_ASSERT(3 == v.z);
    }
    
    {
        const djvVector2i v(1, 2);
        
        DJV_ASSERT(1 == v.x);
        DJV_ASSERT(2 == v.y);
    }
    
    {
        const djvVector3i v(1, 2, 3);
        
        DJV_ASSERT(1 == v.x);
        DJV_ASSERT(2 == v.y);
        DJV_ASSERT(3 == v.z);
    }
    
    {
        const djvVector2i v(1);
        
        DJV_ASSERT(1 == v.x);
        DJV_ASSERT(1 == v.y);
    }
    
    {
        const djvVector3i v(1);
        
        DJV_ASSERT(1 == v.x);
        DJV_ASSERT(1 == v.y);
        DJV_ASSERT(1 == v.z);
    }
}

void djvVectorTest::members()
{
    DJV_DEBUG("djvVectorTest::members");
    
    {
        djvVector2i v;
        v.set(1);
        
        DJV_ASSERT(1 == v.x);
        DJV_ASSERT(1 == v.y);
    }

    {
        djvVector3i v;
        v.set(1);
        
        DJV_ASSERT(1 == v.x);
        DJV_ASSERT(1 == v.y);
        DJV_ASSERT(1 == v.z);
    }
    
    {
        djvVector2i v(1);
        v.zero();
        
        DJV_ASSERT(0 == v.x);
        DJV_ASSERT(0 == v.y);
    }

    {
        djvVector3i v(1);
        v.zero();
        
        DJV_ASSERT(0 == v.x);
        DJV_ASSERT(0 == v.y);
        DJV_ASSERT(0 == v.z);
    }
}

void djvVectorTest::operators()
{
    DJV_DEBUG("djvVectorTest::operators");
    
    {
        const djvVector2i v = djvVector2i(1, 2);
        
        DJV_ASSERT(1 == v.x);
        DJV_ASSERT(2 == v.y);
    }
    
    {
        const djvVector3i v = djvVector3i(1, 2, 3);
        
        DJV_ASSERT(1 == v.x);
        DJV_ASSERT(2 == v.y);
        DJV_ASSERT(3 == v.z);
    }
    
    {
        djvVector2i v(1, 2);
        v += djvVector2i(3, 4);
        
        DJV_ASSERT(4 == v.x);
        DJV_ASSERT(6 == v.y);
    }
    
    {
        djvVector3i v(1, 2, 3);
        v += djvVector3i(4, 5, 6);
        
        DJV_ASSERT(5 == v.x);
        DJV_ASSERT(7 == v.y);
        DJV_ASSERT(9 == v.z);
    }
    
    {
        djvVector2i v(3, 4);
        v -= djvVector2i(1, 2);
        
        DJV_ASSERT(2 == v.x);
        DJV_ASSERT(2 == v.y);
    }
    
    {
        djvVector3i v(4, 5, 6);
        v -= djvVector3i(1, 2, 3);
        
        DJV_ASSERT(3 == v.x);
        DJV_ASSERT(3 == v.y);
        DJV_ASSERT(3 == v.z);
    }
    
    {
        djvVector2i v(1, 2);
        v *= djvVector2i(3, 4);
        
        DJV_ASSERT(3 == v.x);
        DJV_ASSERT(8 == v.y);
    }
    
    {
        djvVector3i v(1, 2, 3);
        v *= djvVector3i(4, 5, 6);
        
        DJV_ASSERT( 4 == v.x);
        DJV_ASSERT(10 == v.y);
        DJV_ASSERT(18 == v.z);
    }
    
    {
        djvVector2i v(4, 6);
        v /= djvVector2i(2, 3);
        
        DJV_ASSERT(2 == v.x);
        DJV_ASSERT(2 == v.y);
    }
    
    {
        djvVector3i v(4, 6, 8);
        v /= djvVector3i(2, 3, 4);
        
        DJV_ASSERT(2 == v.x);
        DJV_ASSERT(2 == v.y);
        DJV_ASSERT(2 == v.z);
    }

    {
        djvVector2i v(1, 2);
        v += 1;
        
        DJV_ASSERT(2 == v.x);
        DJV_ASSERT(3 == v.y);
    }
    
    {
        djvVector3i v(1, 2, 3);
        v += 1;
        
        DJV_ASSERT(2 == v.x);
        DJV_ASSERT(3 == v.y);
        DJV_ASSERT(4 == v.z);
    }
    
    {
        djvVector2i v(1, 2);
        v -= 1;
        
        DJV_ASSERT(0 == v.x);
        DJV_ASSERT(1 == v.y);
    }
    
    {
        djvVector3i v(1, 2, 3);
        v -= 1;
        
        DJV_ASSERT(0 == v.x);
        DJV_ASSERT(1 == v.y);
        DJV_ASSERT(2 == v.z);
    }
    
    {
        djvVector2i v(1, 2);
        v *= 2;
        
        DJV_ASSERT(2 == v.x);
        DJV_ASSERT(4 == v.y);
    }
    
    {
        djvVector3i v(1, 2, 3);
        v *= 2;
        
        DJV_ASSERT(2 == v.x);
        DJV_ASSERT(4 == v.y);
        DJV_ASSERT(6 == v.z);
    }
    
    {
        djvVector2i v(4, 6);
        v /= 2;
        
        DJV_ASSERT(2 == v.x);
        DJV_ASSERT(3 == v.y);
    }
    
    {
        djvVector3i v(4, 6, 8);
        v /= 2;
        
        DJV_ASSERT(2 == v.x);
        DJV_ASSERT(3 == v.y);
        DJV_ASSERT(4 == v.z);
    }
    
    {
        const djvVector2f v = djvVector2i(1, 2);

        DJV_ASSERT(djvMath::fuzzyCompare(1.0, v.x));
        DJV_ASSERT(djvMath::fuzzyCompare(2.0, v.y));
    }
    
    {
        const djvVector3f v = djvVector3i(1, 2, 3);

        DJV_ASSERT(djvMath::fuzzyCompare(1.0, v.x));
        DJV_ASSERT(djvMath::fuzzyCompare(2.0, v.y));
        DJV_ASSERT(djvMath::fuzzyCompare(3.0, v.z));
    }
    
    {
        const djvVector2i v = djvVector2f(1.0, 2.0);

        DJV_ASSERT(1 == v.x);
        DJV_ASSERT(2 == v.y);
    }
    
    {
        const djvVector3i v = djvVector3f(1.0, 2.0, 3.0);

        DJV_ASSERT(1 == v.x);
        DJV_ASSERT(2 == v.y);
        DJV_ASSERT(3 == v.z);
    }
    
    {
        const djvVector2i v = djvVector2i(1, 2) + djvVector2i(3, 4);
        
        DJV_ASSERT(4 == v.x);
        DJV_ASSERT(6 == v.y);
    }
    
    {
        const djvVector3i v = djvVector3i(1, 2, 3) + djvVector3i(4, 5, 6);
        
        DJV_ASSERT(5 == v.x);
        DJV_ASSERT(7 == v.y);
        DJV_ASSERT(9 == v.z);
    }
    
    {
        const djvVector2i v = djvVector2i(3, 4) - djvVector2i(1, 2);
        
        DJV_ASSERT(2 == v.x);
        DJV_ASSERT(2 == v.y);
    }
    
    {
        const djvVector3i v = djvVector3i(4, 5, 6) - djvVector3i(1, 2, 3);
        
        DJV_ASSERT(3 == v.x);
        DJV_ASSERT(3 == v.y);
        DJV_ASSERT(3 == v.z);
    }
    
    {
        const djvVector2i v = djvVector2i(1, 2) * djvVector2i(3, 4);
        
        DJV_ASSERT(3 == v.x);
        DJV_ASSERT(8 == v.y);
    }
    
    {
        const djvVector3i v = djvVector3i(1, 2, 3) * djvVector3i(4, 5, 6);
        
        DJV_ASSERT( 4 == v.x);
        DJV_ASSERT(10 == v.y);
        DJV_ASSERT(18 == v.z);
    }
    
    {
        const djvVector2i v = djvVector2i(4, 6) / djvVector2i(2, 3);
        
        DJV_ASSERT(2 == v.x);
        DJV_ASSERT(2 == v.y);
    }
    
    {
        const djvVector3i v = djvVector3i(4, 6, 8) / djvVector3i(2, 3, 4);
        
        DJV_ASSERT(2 == v.x);
        DJV_ASSERT(2 == v.y);
        DJV_ASSERT(2 == v.z);
    }

    {
        const djvVector2i v = djvVector2i(1, 2) + 1;
        
        DJV_ASSERT(2 == v.x);
        DJV_ASSERT(3 == v.y);
    }
    
    {
        const djvVector3i v = djvVector3i(1, 2, 3) + 1;
        
        DJV_ASSERT(2 == v.x);
        DJV_ASSERT(3 == v.y);
        DJV_ASSERT(4 == v.z);
    }
    
    {
        const djvVector2i v = djvVector2i(1, 2) - 1;
        
        DJV_ASSERT(0 == v.x);
        DJV_ASSERT(1 == v.y);
    }
    
    {
        const djvVector3i v = djvVector3i(1, 2, 3) - 1;
        
        DJV_ASSERT(0 == v.x);
        DJV_ASSERT(1 == v.y);
        DJV_ASSERT(2 == v.z);
    }
    
    {
        const djvVector2i v = djvVector2i(1, 2) * 2;
        
        DJV_ASSERT(2 == v.x);
        DJV_ASSERT(4 == v.y);
    }
    
    {
        const djvVector3i v = djvVector3i(1, 2, 3) * 2;
        
        DJV_ASSERT(2 == v.x);
        DJV_ASSERT(4 == v.y);
        DJV_ASSERT(6 == v.z);
    }
    
    {
        const djvVector2i v = djvVector2i(4, 6) / 2;
        
        DJV_ASSERT(2 == v.x);
        DJV_ASSERT(3 == v.y);
    }
    
    {
        const djvVector3i v = djvVector3i(4, 6, 8) / 2;
        
        DJV_ASSERT(2 == v.x);
        DJV_ASSERT(3 == v.y);
        DJV_ASSERT(4 == v.z);
    }
    
    {
        djvVector2i v;
        
        QStringList s = QStringList() << "1" << "2";
        s >> v;
        
        DJV_ASSERT(1 == v.x);
        DJV_ASSERT(2 == v.y);
        DJV_ASSERT(s.isEmpty());
    }
    
    {
        djvVector3i v;
        
        QStringList s = QStringList() << "1" << "2" << "3";
        s >> v;
        
        DJV_ASSERT(1 == v.x);
        DJV_ASSERT(2 == v.y);
        DJV_ASSERT(3 == v.z);
        DJV_ASSERT(s.isEmpty());
    }
    
    {
        djvVector2i v(1, 2);
        
        QStringList s;
        s << v;
        
        DJV_ASSERT((QStringList() << "1" << "2") == s);
    }
    
    {
        djvVector3i v(1, 2, 3);
        
        QStringList s;
        s << v;
        
        DJV_ASSERT((QStringList() << "1" << "2" << "3") == s);
    }

    {
        const djvVector2i a(1, 2);
        const djvVector2i b(2, 3);
        
        DJV_ASSERT(a == a);
        DJV_ASSERT(a != djvVector2i());
        DJV_ASSERT(a <  b);
        DJV_ASSERT(a <= a);
        DJV_ASSERT(b >  a);
        DJV_ASSERT(a >= a);
    }
}

