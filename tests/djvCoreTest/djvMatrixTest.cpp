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

//! \file djvMatrixTest.cpp

#include <djvMatrixTest.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvMatrix.h>

void djvMatrixTest::run(int &, char **)
{
    DJV_DEBUG("djvMatrixTest::run");

    ctors();
    members();
    operators();
}

void djvMatrixTest::ctors()
{
    DJV_DEBUG("djvMatrixTest::ctors");
    
    {
        const djvMatrix3f m;
        
        DJV_ASSERT(djvMath::fuzzyCompare(1.0, m.e[0]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[1]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[2]));

        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[3]));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0, m.e[4]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[5]));

        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[6]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[7]));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0, m.e[8]));
    }
    
    {
        const djvMatrix4f m;
        
        DJV_ASSERT(djvMath::fuzzyCompare(1.0, m.e[0]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[1]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[2]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[3]));

        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[4]));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0, m.e[5]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[6]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[7]));

        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[8]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[9]));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0, m.e[10]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[11]));

        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[12]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[13]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[14]));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0, m.e[15]));
    }
    
    {
        const djvMatrix3f m(
            djvMatrix3f(
                0.0, 1.0, 2.0,
                3.0, 4.0, 5.0,
                6.0, 7.0, 8.0));
        
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[0]));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0, m.e[1]));
        DJV_ASSERT(djvMath::fuzzyCompare(2.0, m.e[2]));

        DJV_ASSERT(djvMath::fuzzyCompare(3.0, m.e[3]));
        DJV_ASSERT(djvMath::fuzzyCompare(4.0, m.e[4]));
        DJV_ASSERT(djvMath::fuzzyCompare(5.0, m.e[5]));

        DJV_ASSERT(djvMath::fuzzyCompare(6.0, m.e[6]));
        DJV_ASSERT(djvMath::fuzzyCompare(7.0, m.e[7]));
        DJV_ASSERT(djvMath::fuzzyCompare(8.0, m.e[8]));
    }
    
    {
        const djvMatrix4f m(
            djvMatrix4f(
                 0.0,  1.0,  2.0,  3.0,
                 4.0,  5.0,  6.0,  7.0,
                 8.0,  9.0, 10.0, 11.0,
                12.0, 13.0, 14.0, 15.0));
        
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[0]));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0, m.e[1]));
        DJV_ASSERT(djvMath::fuzzyCompare(2.0, m.e[2]));
        DJV_ASSERT(djvMath::fuzzyCompare(3.0, m.e[3]));

        DJV_ASSERT(djvMath::fuzzyCompare(4.0, m.e[4]));
        DJV_ASSERT(djvMath::fuzzyCompare(5.0, m.e[5]));
        DJV_ASSERT(djvMath::fuzzyCompare(6.0, m.e[6]));
        DJV_ASSERT(djvMath::fuzzyCompare(7.0, m.e[7]));

        DJV_ASSERT(djvMath::fuzzyCompare(8.0, m.e[8]));
        DJV_ASSERT(djvMath::fuzzyCompare(9.0, m.e[9]));
        DJV_ASSERT(djvMath::fuzzyCompare(10.0, m.e[10]));
        DJV_ASSERT(djvMath::fuzzyCompare(11.0, m.e[11]));

        DJV_ASSERT(djvMath::fuzzyCompare(12.0, m.e[12]));
        DJV_ASSERT(djvMath::fuzzyCompare(13.0, m.e[13]));
        DJV_ASSERT(djvMath::fuzzyCompare(14.0, m.e[14]));
        DJV_ASSERT(djvMath::fuzzyCompare(15.0, m.e[15]));
    }
    
    {
        const djvMatrix3f m(
            0.0, 1.0, 2.0,
            3.0, 4.0, 5.0,
            6.0, 7.0, 8.0);
        
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[0]));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0, m.e[1]));
        DJV_ASSERT(djvMath::fuzzyCompare(2.0, m.e[2]));

        DJV_ASSERT(djvMath::fuzzyCompare(3.0, m.e[3]));
        DJV_ASSERT(djvMath::fuzzyCompare(4.0, m.e[4]));
        DJV_ASSERT(djvMath::fuzzyCompare(5.0, m.e[5]));

        DJV_ASSERT(djvMath::fuzzyCompare(6.0, m.e[6]));
        DJV_ASSERT(djvMath::fuzzyCompare(7.0, m.e[7]));
        DJV_ASSERT(djvMath::fuzzyCompare(8.0, m.e[8]));
    }
    
    {
        const djvMatrix4f m(
             0.0,  1.0,  2.0,  3.0,
             4.0,  5.0,  6.0,  7.0,
             8.0,  9.0, 10.0, 11.0,
            12.0, 13.0, 14.0, 15.0);
        
        DJV_ASSERT(djvMath::fuzzyCompare( 0.0, m.e[ 0]));
        DJV_ASSERT(djvMath::fuzzyCompare( 1.0, m.e[ 1]));
        DJV_ASSERT(djvMath::fuzzyCompare( 2.0, m.e[ 2]));
        DJV_ASSERT(djvMath::fuzzyCompare( 3.0, m.e[ 3]));

        DJV_ASSERT(djvMath::fuzzyCompare( 4.0, m.e[ 4]));
        DJV_ASSERT(djvMath::fuzzyCompare( 5.0, m.e[ 5]));
        DJV_ASSERT(djvMath::fuzzyCompare( 6.0, m.e[ 6]));
        DJV_ASSERT(djvMath::fuzzyCompare( 7.0, m.e[ 7]));

        DJV_ASSERT(djvMath::fuzzyCompare( 8.0, m.e[ 8]));
        DJV_ASSERT(djvMath::fuzzyCompare( 9.0, m.e[ 9]));
        DJV_ASSERT(djvMath::fuzzyCompare(10.0, m.e[10]));
        DJV_ASSERT(djvMath::fuzzyCompare(11.0, m.e[11]));

        DJV_ASSERT(djvMath::fuzzyCompare(12.0, m.e[12]));
        DJV_ASSERT(djvMath::fuzzyCompare(13.0, m.e[13]));
        DJV_ASSERT(djvMath::fuzzyCompare(14.0, m.e[14]));
        DJV_ASSERT(djvMath::fuzzyCompare(15.0, m.e[15]));
    }
    
    {
        const double v[] =
        {
            0.0, 1.0, 2.0,
            3.0, 4.0, 5.0,
            6.0, 7.0, 8.0
        };
        const djvMatrix3f m(v);
        
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[0]));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0, m.e[1]));
        DJV_ASSERT(djvMath::fuzzyCompare(2.0, m.e[2]));

        DJV_ASSERT(djvMath::fuzzyCompare(3.0, m.e[3]));
        DJV_ASSERT(djvMath::fuzzyCompare(4.0, m.e[4]));
        DJV_ASSERT(djvMath::fuzzyCompare(5.0, m.e[5]));

        DJV_ASSERT(djvMath::fuzzyCompare(6.0, m.e[6]));
        DJV_ASSERT(djvMath::fuzzyCompare(7.0, m.e[7]));
        DJV_ASSERT(djvMath::fuzzyCompare(8.0, m.e[8]));
    }
    
    {
        const double v[] =
        {
             0.0,  1.0,  2.0,  3.0,
             4.0,  5.0,  6.0,  7.0,
             8.0,  9.0, 10.0, 11.0,
            12.0, 13.0, 14.0, 15.0
        };
        const djvMatrix4f m(v);
        
        DJV_ASSERT(djvMath::fuzzyCompare( 0.0, m.e[ 0]));
        DJV_ASSERT(djvMath::fuzzyCompare( 1.0, m.e[ 1]));
        DJV_ASSERT(djvMath::fuzzyCompare( 2.0, m.e[ 2]));
        DJV_ASSERT(djvMath::fuzzyCompare( 3.0, m.e[ 3]));

        DJV_ASSERT(djvMath::fuzzyCompare( 4.0, m.e[ 4]));
        DJV_ASSERT(djvMath::fuzzyCompare( 5.0, m.e[ 5]));
        DJV_ASSERT(djvMath::fuzzyCompare( 6.0, m.e[ 6]));
        DJV_ASSERT(djvMath::fuzzyCompare( 7.0, m.e[ 7]));

        DJV_ASSERT(djvMath::fuzzyCompare( 8.0, m.e[ 8]));
        DJV_ASSERT(djvMath::fuzzyCompare( 9.0, m.e[ 9]));
        DJV_ASSERT(djvMath::fuzzyCompare(10.0, m.e[10]));
        DJV_ASSERT(djvMath::fuzzyCompare(11.0, m.e[11]));

        DJV_ASSERT(djvMath::fuzzyCompare(12.0, m.e[12]));
        DJV_ASSERT(djvMath::fuzzyCompare(13.0, m.e[13]));
        DJV_ASSERT(djvMath::fuzzyCompare(14.0, m.e[14]));
        DJV_ASSERT(djvMath::fuzzyCompare(15.0, m.e[15]));
    }
}

void djvMatrixTest::members()
{
    DJV_DEBUG("djvMatrixTest::members");
    
    {
        const double v[] =
        {
            0.0, 1.0, 2.0,
            3.0, 4.0, 5.0,
            6.0, 7.0, 8.0
        };
        djvMatrix3f m;
        m.set(v);
        
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[0]));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0, m.e[1]));
        DJV_ASSERT(djvMath::fuzzyCompare(2.0, m.e[2]));

        DJV_ASSERT(djvMath::fuzzyCompare(3.0, m.e[3]));
        DJV_ASSERT(djvMath::fuzzyCompare(4.0, m.e[4]));
        DJV_ASSERT(djvMath::fuzzyCompare(5.0, m.e[5]));

        DJV_ASSERT(djvMath::fuzzyCompare(6.0, m.e[6]));
        DJV_ASSERT(djvMath::fuzzyCompare(7.0, m.e[7]));
        DJV_ASSERT(djvMath::fuzzyCompare(8.0, m.e[8]));
    }
    
    {
        const double v[] =
        {
             0.0,  1.0,  2.0,  3.0,
             4.0,  5.0,  6.0,  7.0,
             8.0,  9.0, 10.0, 11.0,
            12.0, 13.0, 14.0, 15.0
        };
        djvMatrix4f m;
        m.set(v);
        
        DJV_ASSERT(djvMath::fuzzyCompare( 0.0, m.e[ 0]));
        DJV_ASSERT(djvMath::fuzzyCompare( 1.0, m.e[ 1]));
        DJV_ASSERT(djvMath::fuzzyCompare( 2.0, m.e[ 2]));
        DJV_ASSERT(djvMath::fuzzyCompare( 3.0, m.e[ 3]));

        DJV_ASSERT(djvMath::fuzzyCompare( 4.0, m.e[ 4]));
        DJV_ASSERT(djvMath::fuzzyCompare( 5.0, m.e[ 5]));
        DJV_ASSERT(djvMath::fuzzyCompare( 6.0, m.e[ 6]));
        DJV_ASSERT(djvMath::fuzzyCompare( 7.0, m.e[ 7]));

        DJV_ASSERT(djvMath::fuzzyCompare( 8.0, m.e[ 8]));
        DJV_ASSERT(djvMath::fuzzyCompare( 9.0, m.e[ 9]));
        DJV_ASSERT(djvMath::fuzzyCompare(10.0, m.e[10]));
        DJV_ASSERT(djvMath::fuzzyCompare(11.0, m.e[11]));

        DJV_ASSERT(djvMath::fuzzyCompare(12.0, m.e[12]));
        DJV_ASSERT(djvMath::fuzzyCompare(13.0, m.e[13]));
        DJV_ASSERT(djvMath::fuzzyCompare(14.0, m.e[14]));
        DJV_ASSERT(djvMath::fuzzyCompare(15.0, m.e[15]));
    }
    
    {
        djvMatrix3f m;
        m.set(
            0.0, 1.0, 2.0,
            3.0, 4.0, 5.0,
            6.0, 7.0, 8.0);
        
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[0]));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0, m.e[1]));
        DJV_ASSERT(djvMath::fuzzyCompare(2.0, m.e[2]));

        DJV_ASSERT(djvMath::fuzzyCompare(3.0, m.e[3]));
        DJV_ASSERT(djvMath::fuzzyCompare(4.0, m.e[4]));
        DJV_ASSERT(djvMath::fuzzyCompare(5.0, m.e[5]));

        DJV_ASSERT(djvMath::fuzzyCompare(6.0, m.e[6]));
        DJV_ASSERT(djvMath::fuzzyCompare(7.0, m.e[7]));
        DJV_ASSERT(djvMath::fuzzyCompare(8.0, m.e[8]));
    }
    
    {
        djvMatrix4f m;
        m.set(
             0.0,  1.0,  2.0,  3.0,
             4.0,  5.0,  6.0,  7.0,
             8.0,  9.0, 10.0, 11.0,
            12.0, 13.0, 14.0, 15.0);
        
        DJV_ASSERT(djvMath::fuzzyCompare( 0.0, m.e[ 0]));
        DJV_ASSERT(djvMath::fuzzyCompare( 1.0, m.e[ 1]));
        DJV_ASSERT(djvMath::fuzzyCompare( 2.0, m.e[ 2]));
        DJV_ASSERT(djvMath::fuzzyCompare( 3.0, m.e[ 3]));

        DJV_ASSERT(djvMath::fuzzyCompare( 4.0, m.e[ 4]));
        DJV_ASSERT(djvMath::fuzzyCompare( 5.0, m.e[ 5]));
        DJV_ASSERT(djvMath::fuzzyCompare( 6.0, m.e[ 6]));
        DJV_ASSERT(djvMath::fuzzyCompare( 7.0, m.e[ 7]));

        DJV_ASSERT(djvMath::fuzzyCompare( 8.0, m.e[ 8]));
        DJV_ASSERT(djvMath::fuzzyCompare( 9.0, m.e[ 9]));
        DJV_ASSERT(djvMath::fuzzyCompare(10.0, m.e[10]));
        DJV_ASSERT(djvMath::fuzzyCompare(11.0, m.e[11]));

        DJV_ASSERT(djvMath::fuzzyCompare(12.0, m.e[12]));
        DJV_ASSERT(djvMath::fuzzyCompare(13.0, m.e[13]));
        DJV_ASSERT(djvMath::fuzzyCompare(14.0, m.e[14]));
        DJV_ASSERT(djvMath::fuzzyCompare(15.0, m.e[15]));
    }
    
    {
        djvMatrix3f m;
        m.zero();
        
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[0]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[1]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[2]));

        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[3]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[4]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[5]));

        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[6]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[7]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[8]));
    }
    
    {
        djvMatrix4f m;
        m.zero();
        
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[ 0]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[ 1]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[ 2]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[ 3]));

        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[ 4]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[ 5]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[ 6]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[ 7]));

        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[ 8]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[ 9]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[10]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[11]));

        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[12]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[13]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[14]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[15]));
    }
    
    {
        djvMatrix3f m;
        m.zero();
        m.identity();
        
        DJV_ASSERT(djvMath::fuzzyCompare(1.0, m.e[0]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[1]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[2]));

        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[3]));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0, m.e[4]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[5]));

        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[6]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[7]));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0, m.e[8]));
    }
    
    {
        djvMatrix4f m;
        m.zero();
        m.identity();
        
        DJV_ASSERT(djvMath::fuzzyCompare(1.0, m.e[ 0]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[ 1]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[ 2]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[ 3]));

        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[ 4]));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0, m.e[ 5]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[ 6]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[ 7]));

        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[ 8]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[ 9]));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0, m.e[10]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[11]));

        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[12]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[13]));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[14]));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0, m.e[15]));
    }
}

void djvMatrixTest::operators()
{
    DJV_DEBUG("djvMatrixTest::operators");

    {
        const djvMatrix3f a(
            0.0, 1.0, 2.0,
            3.0, 4.0, 5.0,
            6.0, 7.0, 8.0);

        DJV_ASSERT(a == a);
        DJV_ASSERT(a != djvMatrix3f());
    }
    
    {
        djvMatrix3f m;
        
        QStringList s = QStringList() <<
            "0.0" << "1.0" << "2.0" <<
            "3.0" << "4.0" << "5.0" <<
            "6.0" << "7.0" << "8.0";
        s >> m;
        
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, m.e[0]));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0, m.e[1]));
        DJV_ASSERT(djvMath::fuzzyCompare(2.0, m.e[2]));

        DJV_ASSERT(djvMath::fuzzyCompare(3.0, m.e[3]));
        DJV_ASSERT(djvMath::fuzzyCompare(4.0, m.e[4]));
        DJV_ASSERT(djvMath::fuzzyCompare(5.0, m.e[5]));

        DJV_ASSERT(djvMath::fuzzyCompare(6.0, m.e[6]));
        DJV_ASSERT(djvMath::fuzzyCompare(7.0, m.e[7]));
        DJV_ASSERT(djvMath::fuzzyCompare(8.0, m.e[8]));
        
        DJV_ASSERT(s.isEmpty());
    }
    
    {
        djvMatrix3f m(
            0.0, 1.0, 2.0,
            3.0, 4.0, 5.0,
            6.0, 7.0, 8.0);
        
        QStringList s;
        s << m;
        
        DJV_ASSERT((QStringList() <<
            "0" << "1" << "2" <<
            "3" << "4" << "5" <<
            "6" << "7" << "8") == s);
    }
}

