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

//! \file djvBoxTest.cpp

#include <djvBoxTest.h>

#include <djvAssert.h>
#include <djvBox.h>
#include <djvDebug.h>
#include <djvMath.h>
#include <djvStringUtil.h>

void djvBoxTest::run(int &, char **)
{
    DJV_DEBUG("djvBoxTest::run");
    
    ctors();
    members();
    operators();
}

void djvBoxTest::ctors()
{
    DJV_DEBUG("djvBoxTest::ctors");
    
    {
        const djvBox2i box;
        
        DJV_ASSERT(0 == box.position.x);
        DJV_ASSERT(0 == box.position.y);
        DJV_ASSERT(0 == box.size.x);
        DJV_ASSERT(0 == box.size.y);
        
        DJV_ASSERT(0 == box.x);
        DJV_ASSERT(0 == box.y);
        DJV_ASSERT(0 == box.w);
        DJV_ASSERT(0 == box.h);
    }
    
    {
        const djvBox3i box;
        
        DJV_ASSERT(0 == box.position.x);
        DJV_ASSERT(0 == box.position.y);
        DJV_ASSERT(0 == box.position.z);
        DJV_ASSERT(0 == box.size.x);
        DJV_ASSERT(0 == box.size.y);
        DJV_ASSERT(0 == box.size.z);
        
        DJV_ASSERT(0 == box.x);
        DJV_ASSERT(0 == box.y);
        DJV_ASSERT(0 == box.z);
        DJV_ASSERT(0 == box.w);
        DJV_ASSERT(0 == box.h);
        DJV_ASSERT(0 == box.d);
    }
    
    {
        const djvBox2i tmp(1, 2, 3, 4);
        
        const djvBox2i box(tmp);
        
        DJV_ASSERT(1 == box.position.x);
        DJV_ASSERT(2 == box.position.y);
        DJV_ASSERT(3 == box.size.x);
        DJV_ASSERT(4 == box.size.y);
        
        DJV_ASSERT(1 == box.x);
        DJV_ASSERT(2 == box.y);
        DJV_ASSERT(3 == box.w);
        DJV_ASSERT(4 == box.h);
    }
    
    {
        const djvBox3i tmp(1, 2, 3, 4, 5, 6);
        
        const djvBox3i box(tmp);
        
        DJV_ASSERT(1 == box.position.x);
        DJV_ASSERT(2 == box.position.y);
        DJV_ASSERT(3 == box.position.z);
        DJV_ASSERT(4 == box.size.x);
        DJV_ASSERT(5 == box.size.y);
        DJV_ASSERT(6 == box.size.z);
        
        DJV_ASSERT(1 == box.x);
        DJV_ASSERT(2 == box.y);
        DJV_ASSERT(3 == box.z);
        DJV_ASSERT(4 == box.w);
        DJV_ASSERT(5 == box.h);
        DJV_ASSERT(6 == box.d);
    }
    
    {
        const djvVector2i tmp(1, 2);
        
        const djvBox2i box(tmp);
        
        DJV_ASSERT(0 == box.position.x);
        DJV_ASSERT(0 == box.position.y);
        DJV_ASSERT(1 == box.size.x);
        DJV_ASSERT(2 == box.size.y);
        
        DJV_ASSERT(0 == box.x);
        DJV_ASSERT(0 == box.y);
        DJV_ASSERT(1 == box.w);
        DJV_ASSERT(2 == box.h);
    }
    
    {
        const djvVector3i tmp(1, 2, 3);
        
        const djvBox3i box(tmp);
        
        DJV_ASSERT(0 == box.position.x);
        DJV_ASSERT(0 == box.position.y);
        DJV_ASSERT(0 == box.position.z);
        DJV_ASSERT(1 == box.size.x);
        DJV_ASSERT(2 == box.size.y);
        DJV_ASSERT(3 == box.size.z);
        
        DJV_ASSERT(0 == box.x);
        DJV_ASSERT(0 == box.y);
        DJV_ASSERT(0 == box.z);
        DJV_ASSERT(1 == box.w);
        DJV_ASSERT(2 == box.h);
        DJV_ASSERT(3 == box.d);
    }
    
    {
        const djvBox2i box(djvVector2i(1, 2), djvVector2i(3, 4));
        
        DJV_ASSERT(1 == box.position.x);
        DJV_ASSERT(2 == box.position.y);
        DJV_ASSERT(3 == box.size.x);
        DJV_ASSERT(4 == box.size.y);
        
        DJV_ASSERT(1 == box.x);
        DJV_ASSERT(2 == box.y);
        DJV_ASSERT(3 == box.w);
        DJV_ASSERT(4 == box.h);
    }
    
    {
        const djvBox2i box(1, 2);
        
        DJV_ASSERT(0 == box.position.x);
        DJV_ASSERT(0 == box.position.y);
        DJV_ASSERT(1 == box.size.x);
        DJV_ASSERT(2 == box.size.y);
        
        DJV_ASSERT(0 == box.x);
        DJV_ASSERT(0 == box.y);
        DJV_ASSERT(1 == box.w);
        DJV_ASSERT(2 == box.h);
    }
    
    {
        const djvBox3i box(1, 2, 3);
        
        DJV_ASSERT(0 == box.position.x);
        DJV_ASSERT(0 == box.position.y);
        DJV_ASSERT(0 == box.position.z);
        DJV_ASSERT(1 == box.size.x);
        DJV_ASSERT(2 == box.size.y);
        DJV_ASSERT(3 == box.size.z);
        
        DJV_ASSERT(0 == box.x);
        DJV_ASSERT(0 == box.y);
        DJV_ASSERT(0 == box.z);
        DJV_ASSERT(1 == box.w);
        DJV_ASSERT(2 == box.h);
        DJV_ASSERT(3 == box.d);
    }
    
    {
        const djvBox2i box(djvVector2i(1, 2), djvVector2i(3, 4));
        
        DJV_ASSERT(1 == box.position.x);
        DJV_ASSERT(2 == box.position.y);
        DJV_ASSERT(3 == box.size.x);
        DJV_ASSERT(4 == box.size.y);
        
        DJV_ASSERT(1 == box.x);
        DJV_ASSERT(2 == box.y);
        DJV_ASSERT(3 == box.w);
        DJV_ASSERT(4 == box.h);
    }
    
    {
        const djvBox3i box(djvVector3i(1, 2, 3), djvVector3i(4, 5, 6));
        
        DJV_ASSERT(1 == box.position.x);
        DJV_ASSERT(2 == box.position.y);
        DJV_ASSERT(3 == box.position.z);
        DJV_ASSERT(4 == box.size.x);
        DJV_ASSERT(5 == box.size.y);
        DJV_ASSERT(6 == box.size.z);
        
        DJV_ASSERT(1 == box.x);
        DJV_ASSERT(2 == box.y);
        DJV_ASSERT(3 == box.z);
        DJV_ASSERT(4 == box.w);
        DJV_ASSERT(5 == box.h);
        DJV_ASSERT(6 == box.d);
    }
    
    {
        const djvBox2i box(djvVector2i(1, 2));
        
        DJV_ASSERT(0 == box.position.x);
        DJV_ASSERT(0 == box.position.y);
        DJV_ASSERT(1 == box.size.x);
        DJV_ASSERT(2 == box.size.y);
        
        DJV_ASSERT(0 == box.x);
        DJV_ASSERT(0 == box.y);
        DJV_ASSERT(1 == box.w);
        DJV_ASSERT(2 == box.h);
    }
    
    {
        const djvBox3i box(djvVector3i(1, 2, 3));
        
        DJV_ASSERT(0 == box.position.x);
        DJV_ASSERT(0 == box.position.y);
        DJV_ASSERT(0 == box.position.z);
        DJV_ASSERT(1 == box.size.x);
        DJV_ASSERT(2 == box.size.y);
        DJV_ASSERT(3 == box.size.z);
        
        DJV_ASSERT(0 == box.x);
        DJV_ASSERT(0 == box.y);
        DJV_ASSERT(0 == box.z);
        DJV_ASSERT(1 == box.w);
        DJV_ASSERT(2 == box.h);
        DJV_ASSERT(3 == box.d);
    }
}

void djvBoxTest::members()
{
    DJV_DEBUG("djvBoxTest::members");
    
    {
        djvBox2i box(1, 2, 3, 4);
        
        box.zero();
        
        DJV_ASSERT(0 == box.x);
        DJV_ASSERT(0 == box.y);
        DJV_ASSERT(0 == box.w);
        DJV_ASSERT(0 == box.h);
    }
    
    {
        djvBox3i box(1, 2, 3, 4, 5, 6);
        
        box.zero();
        
        DJV_ASSERT(0 == box.x);
        DJV_ASSERT(0 == box.y);
        DJV_ASSERT(0 == box.z);
        DJV_ASSERT(0 == box.w);
        DJV_ASSERT(0 == box.h);
        DJV_ASSERT(0 == box.d);
    }
    
    {
        djvBox2i box(1, 2, 3, 4);
        
        DJV_ASSERT(3 == box.lowerRight().x);
        DJV_ASSERT(5 == box.lowerRight().y);
        
        box.setLowerRight(djvVector2i(4, 6));
        
        DJV_ASSERT(4 == box.w);
        DJV_ASSERT(5 == box.h);
    }
    
    {
        djvBox3i box(1, 2, 3, 4, 5, 6);
        
        DJV_ASSERT(4 == box.lowerRight().x);
        DJV_ASSERT(6 == box.lowerRight().y);
        DJV_ASSERT(8 == box.lowerRight().z);
        
        box.setLowerRight(djvVector3i(5, 7, 9));
        
        DJV_ASSERT(5 == box.w);
        DJV_ASSERT(6 == box.h);
        DJV_ASSERT(7 == box.d);
    }
}

void djvBoxTest::operators()
{
    DJV_DEBUG("djvBoxTest::operators");
    
    {
        const djvBox2i tmp(1, 2, 3, 4);
        
        djvBox2i box;
        
        box = tmp;
        
        DJV_ASSERT(1 == box.x);
        DJV_ASSERT(2 == box.y);
        DJV_ASSERT(3 == box.w);
        DJV_ASSERT(4 == box.h);
    }
    
    {
        const djvBox3i tmp(1, 2, 3, 4, 5, 6);
        
        djvBox3i box;
        
        box = tmp;
        
        DJV_ASSERT(1 == box.x);
        DJV_ASSERT(2 == box.y);
        DJV_ASSERT(3 == box.z);
        DJV_ASSERT(4 == box.w);
        DJV_ASSERT(5 == box.h);
        DJV_ASSERT(6 == box.d);
    }
    
    {
        djvBox2i box(1, 2, 3, 4);
        
        box *= djvVector2i(2, 3);
        
        DJV_ASSERT( 2 == box.x);
        DJV_ASSERT( 6 == box.y);
        DJV_ASSERT( 6 == box.w);
        DJV_ASSERT(12 == box.h);
    }
    
    {
        djvBox3i box(1, 2, 3, 4, 5, 6);
        
        box *= djvVector3i(2, 3, 4);
        
        DJV_ASSERT( 2 == box.x);
        DJV_ASSERT( 6 == box.y);
        DJV_ASSERT(12 == box.z);
        DJV_ASSERT( 8 == box.w);
        DJV_ASSERT(15 == box.h);
        DJV_ASSERT(24 == box.d);
    }
    
    {
        djvBox2i box(4, 6, 8, 9);
        
        box /= djvVector2i(2, 3);
        
        DJV_ASSERT(2 == box.x);
        DJV_ASSERT(2 == box.y);
        DJV_ASSERT(4 == box.w);
        DJV_ASSERT(3 == box.h);
    }
    
    {
        djvBox3i box(4, 6, 8, 10, 12, 16);
        
        box /= djvVector3i(2, 3, 4);
        
        DJV_ASSERT(2 == box.x);
        DJV_ASSERT(2 == box.y);
        DJV_ASSERT(2 == box.z);
        DJV_ASSERT(5 == box.w);
        DJV_ASSERT(4 == box.h);
        DJV_ASSERT(4 == box.d);
    }
    
    {
        djvBox2i box(4, 6, 8, 10);
        
        box /= 2;
        
        DJV_ASSERT(2 == box.x);
        DJV_ASSERT(3 == box.y);
        DJV_ASSERT(4 == box.w);
        DJV_ASSERT(5 == box.h);
    }
    
    {
        djvBox3i box(4, 6, 8, 10, 12, 14);
        
        box /= 2;
        
        DJV_ASSERT(2 == box.x);
        DJV_ASSERT(3 == box.y);
        DJV_ASSERT(4 == box.z);
        DJV_ASSERT(5 == box.w);
        DJV_ASSERT(6 == box.h);
        DJV_ASSERT(7 == box.d);
    }
    
    {
        const djvBox2f box = djvBox2i(1, 2, 3, 4);

        DJV_ASSERT(djvMath::fuzzyCompare(1.0, box.x));
        DJV_ASSERT(djvMath::fuzzyCompare(2.0, box.y));
        DJV_ASSERT(djvMath::fuzzyCompare(3.0, box.w));
        DJV_ASSERT(djvMath::fuzzyCompare(4.0, box.h));
    }
    
    {
        const djvBox3f box = djvBox3i(1, 2, 3, 4, 5, 6);

        DJV_ASSERT(djvMath::fuzzyCompare(1.0, box.x));
        DJV_ASSERT(djvMath::fuzzyCompare(2.0, box.y));
        DJV_ASSERT(djvMath::fuzzyCompare(3.0, box.z));
        DJV_ASSERT(djvMath::fuzzyCompare(4.0, box.w));
        DJV_ASSERT(djvMath::fuzzyCompare(5.0, box.h));
        DJV_ASSERT(djvMath::fuzzyCompare(6.0, box.d));
    }
    
    {
        const djvBox2i box = djvBox2f(1.0, 2.0, 3.0, 4.0);

        DJV_ASSERT(1 == box.x);
        DJV_ASSERT(2 == box.y);
        DJV_ASSERT(3 == box.w);
        DJV_ASSERT(4 == box.h);
    }
    
    {
        const djvBox3i box = djvBox3f(1.0, 2.0, 3.0, 4.0, 5.0, 6.0);

        DJV_ASSERT(1 == box.x);
        DJV_ASSERT(2 == box.y);
        DJV_ASSERT(3 == box.z);
        DJV_ASSERT(4 == box.w);
        DJV_ASSERT(5 == box.h);
        DJV_ASSERT(6 == box.d);
    }
    
    {
        const djvBox2i box = djvBox2i(1, 2, 3, 4) + 1;
        
        DJV_ASSERT(2 == box.x);
        DJV_ASSERT(3 == box.y);
        DJV_ASSERT(4 == box.w);
        DJV_ASSERT(5 == box.h);
    }
    
    {
        const djvBox3i box = djvBox3i(1, 2, 3, 4, 5, 6) + 1;
        
        DJV_ASSERT(2 == box.x);
        DJV_ASSERT(3 == box.y);
        DJV_ASSERT(4 == box.z);
        DJV_ASSERT(5 == box.w);
        DJV_ASSERT(6 == box.h);
        DJV_ASSERT(7 == box.d);
    }
    
    {
        const djvBox2i box = djvBox2i(1, 2, 3, 4) - 1;
        
        DJV_ASSERT(0 == box.x);
        DJV_ASSERT(1 == box.y);
        DJV_ASSERT(2 == box.w);
        DJV_ASSERT(3 == box.h);
    }
    
    {
        const djvBox3i box = djvBox3i(1, 2, 3, 4, 5, 6) - 1;
        
        DJV_ASSERT(0 == box.x);
        DJV_ASSERT(1 == box.y);
        DJV_ASSERT(2 == box.z);
        DJV_ASSERT(3 == box.w);
        DJV_ASSERT(4 == box.h);
        DJV_ASSERT(5 == box.d);
    }
    
    {
        const djvBox2i box = djvBox2i(1, 2, 3, 4) * 2;
        
        DJV_ASSERT(2 == box.x);
        DJV_ASSERT(4 == box.y);
        DJV_ASSERT(6 == box.w);
        DJV_ASSERT(8 == box.h);
    }
    
    {
        const djvBox3i box = djvBox3i(1, 2, 3, 4, 5, 6) * 2;
        
        DJV_ASSERT( 2 == box.x);
        DJV_ASSERT( 4 == box.y);
        DJV_ASSERT( 6 == box.z);
        DJV_ASSERT( 8 == box.w);
        DJV_ASSERT(10 == box.h);
        DJV_ASSERT(12 == box.d);
    }
    
    {
        const djvBox2i box = djvBox2i(2, 4, 6, 8) / 2;
        
        DJV_ASSERT(1 == box.x);
        DJV_ASSERT(2 == box.y);
        DJV_ASSERT(3 == box.w);
        DJV_ASSERT(4 == box.h);
    }
    
    {
        const djvBox3i box = djvBox3i(2, 4, 6, 8, 10, 12) / 2;
        
        DJV_ASSERT(1 == box.x);
        DJV_ASSERT(2 == box.y);
        DJV_ASSERT(3 == box.z);
        DJV_ASSERT(4 == box.w);
        DJV_ASSERT(5 == box.h);
        DJV_ASSERT(6 == box.d);
    }

    {
        const djvBox2i box = djvBox2i(1, 2, 3, 4) + djvVector2i(1, 2);
        
        DJV_ASSERT(2 == box.x);
        DJV_ASSERT(4 == box.y);
        DJV_ASSERT(4 == box.w);
        DJV_ASSERT(6 == box.h);
    }
    
    {
        const djvBox3i box = djvBox3i(1, 2, 3, 4, 5, 6) + djvVector3i(1, 2, 3);
        
        DJV_ASSERT(2 == box.x);
        DJV_ASSERT(4 == box.y);
        DJV_ASSERT(6 == box.z);
        DJV_ASSERT(5 == box.w);
        DJV_ASSERT(7 == box.h);
        DJV_ASSERT(9 == box.d);
    }
    
    {
        const djvBox2i box = djvBox2i(1, 2, 3, 4) - djvVector2i(1, 2);
        
        DJV_ASSERT(0 == box.x);
        DJV_ASSERT(0 == box.y);
        DJV_ASSERT(2 == box.w);
        DJV_ASSERT(2 == box.h);
    }
    
    {
        const djvBox3i box = djvBox3i(1, 2, 3, 4, 5, 6) - djvVector3i(1, 2, 3);
        
        DJV_ASSERT(0 == box.x);
        DJV_ASSERT(0 == box.y);
        DJV_ASSERT(0 == box.z);
        DJV_ASSERT(3 == box.w);
        DJV_ASSERT(3 == box.h);
        DJV_ASSERT(3 == box.d);
    }
    
    {
        const djvBox2i box = djvBox2i(1, 2, 3, 4) * djvVector2i(2, 3);
        
        DJV_ASSERT( 2 == box.x);
        DJV_ASSERT( 6 == box.y);
        DJV_ASSERT( 6 == box.w);
        DJV_ASSERT(12 == box.h);
    }
    
    {
        const djvBox3i box = djvBox3i(1, 2, 3, 4, 5, 6) * djvVector3i(2, 3, 4);
        
        DJV_ASSERT( 2 == box.x);
        DJV_ASSERT( 6 == box.y);
        DJV_ASSERT(12 == box.z);
        DJV_ASSERT( 8 == box.w);
        DJV_ASSERT(15 == box.h);
        DJV_ASSERT(24 == box.d);
    }
    
    {
        const djvBox2i box = djvBox2i(2, 3, 4, 6) / djvVector2i(2, 3);
        
        DJV_ASSERT(1 == box.x);
        DJV_ASSERT(1 == box.y);
        DJV_ASSERT(2 == box.w);
        DJV_ASSERT(2 == box.h);
    }
    
    {
        const djvBox3i box = djvBox3i(2, 3, 4, 4, 6, 8) / djvVector3i(2, 3, 4);
        
        DJV_ASSERT(1 == box.x);
        DJV_ASSERT(1 == box.y);
        DJV_ASSERT(1 == box.z);
        DJV_ASSERT(2 == box.w);
        DJV_ASSERT(2 == box.h);
        DJV_ASSERT(2 == box.d);
    }
    
    {
        DJV_ASSERT(djvBox2i(1, 2, 3, 4) == djvBox2i(1, 2, 3, 4));
        DJV_ASSERT(djvBox2i(1, 2, 3, 4) != djvBox2i());
    }
    
    {
        djvBox2i box;
        
        QStringList s = QStringList() << "1" << "2" << "3" << "4";
        s >> box;
        
        DJV_ASSERT(1 == box.x);
        DJV_ASSERT(2 == box.y);
        DJV_ASSERT(3 == box.w);
        DJV_ASSERT(4 == box.h);
        DJV_ASSERT(s.isEmpty());
    }
    
    {
        djvBox3i box;
        
        QStringList s = QStringList() << "1" << "2" << "3" << "4" << "5" << "6";
        s >> box;
        
        DJV_ASSERT(1 == box.x);
        DJV_ASSERT(2 == box.y);
        DJV_ASSERT(3 == box.z);
        DJV_ASSERT(4 == box.w);
        DJV_ASSERT(5 == box.h);
        DJV_ASSERT(6 == box.d);
        DJV_ASSERT(s.isEmpty());
    }
    
    {
        djvBox2i box(1, 2, 3, 4);
        
        QStringList s;
        s << box;
        
        DJV_ASSERT((QStringList() << "1" << "2" << "3" << "4") == s);
    }
    
    {
        djvBox3i box(1, 2, 3, 4, 5, 6);
        
        QStringList s;
        s << box;
        
        DJV_ASSERT((QStringList() << "1" << "2" << "3" << "4" << "5" << "6") == s);
    }
    
    {
        DJV_DEBUG_PRINT(djvBox2i());
        
        DJV_DEBUG_PRINT(djvBox2f());
        
        DJV_DEBUG_PRINT(djvBox3i());
        
        DJV_DEBUG_PRINT(djvBox3f());
    }
}

