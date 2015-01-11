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

//! \file djvMatrixUtilTest.cpp

#include <djvMatrixUtilTest.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvMatrixUtil.h>

void djvMatrixUtilTest::run(int &, char **)
{
    DJV_DEBUG("djvMatrixUtilTest::run");

    transpose();
    translate();
    scale();
    rotate();
    convert();
}

void djvMatrixUtilTest::transpose()
{
    DJV_DEBUG("djvMatrixUtilTest::transpose");
    
    DJV_ASSERT(
        djvMatrixUtil::transpose(djvMatrix3f(0, 1, 2, 3, 4, 5, 6, 7, 8)) ==
        djvMatrix3f(0, 3, 6,
            1, 4, 7,
            2, 5, 8));
}

void djvMatrixUtilTest::translate()
{
    DJV_DEBUG("djvMatrixUtilTest::translate");
    
    DJV_ASSERT(
        djvMatrixUtil::translate3f(djvVector2f(1, 2)) ==
        djvMatrix3f(1, 0, 0,
            0, 1, 0,
            1, 2, 1));
}

void djvMatrixUtilTest::scale()
{
    DJV_DEBUG("djvMatrixUtilTest::scale");
    
    DJV_ASSERT(
        djvMatrixUtil::scale3f(djvVector2f(2.0, 4.0)) ==
        djvMatrix3f(2, 0, 0,
            0, 4, 0,
            0, 0, 1));
}

void djvMatrixUtilTest::rotate()
{
    DJV_DEBUG("djvMatrixUtilTest::rotate");
    
    //! \todo Fix this confidence test.
    
    /*DJV_ASSERT(
      djvMatrixUtil::rotate3f(90.0) ==
      djvMatrix3f( 0, 1, 0,
          -1, 0, 0,
           0, 0, 1));*/
}

void djvMatrixUtilTest::convert()
{
}

