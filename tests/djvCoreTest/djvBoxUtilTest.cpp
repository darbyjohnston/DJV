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

//! \file djvBoxUtilTest.cpp

#include <djvBoxUtilTest.h>

#include <djvAssert.h>
#include <djvBoxUtil.h>
#include <djvDebug.h>

void djvBoxUtilTest::run(int &, char **)
{
    DJV_DEBUG("djvBoxUtilTest::run");
    
    corners();
    swap();
    intersect();
    bound();
    expand();
    border();
    isSizeValid();
    convert();
}

void djvBoxUtilTest::corners()
{
    DJV_DEBUG("djvBoxUtilTest::corners");
    
    DJV_ASSERT(
        djvBoxUtil::corners(djvVector2i(0, 10), djvVector2i(9, 29)) == djvBox2i(0, 10, 10, 20));
    DJV_ASSERT(
        djvBoxUtil::corners(djvVector2f(0.0, 10.0), djvVector2f(10.0, 30.0)) ==
        djvBox2f(0.0, 10.0, 10.0, 20.0));
}

void djvBoxUtilTest::swap()
{
    DJV_DEBUG("djvBoxUtilTest::swap");
    
    DJV_ASSERT(djvBoxUtil::swap(djvBox2i(0, 10, 10, 20)) == djvBox2i(10, 0, 20, 10));
}

void djvBoxUtilTest::intersect()
{
    DJV_DEBUG("djvBoxUtilTest::intersect");
    
    DJV_ASSERT(
        djvBoxUtil::intersect(djvBox2i(0, 5, 10, 15), djvBox2i(5, 10, 15, 20)) ==
        djvBox2i(5, 10, 5, 10));
    DJV_ASSERT(djvBoxUtil::intersect(djvBox2i(0, 5, 10, 15), djvVector2i(5, 10)));
    DJV_ASSERT(! djvBoxUtil::intersect(djvBox2i(0, 5, 10, 15), djvVector2i(-1, -1)));
}

void djvBoxUtilTest::bound()
{
    DJV_DEBUG("djvBoxUtilTest::bound");
    
    DJV_ASSERT(
        djvBoxUtil::bound(djvBox2i(0, 5, 5, 10), djvBox2i(5, 10, 10, 15)) ==
        djvBox2i(0, 5, 15, 20));
}

void djvBoxUtilTest::expand()
{
    DJV_DEBUG("djvBoxUtilTest::expand");
    
    DJV_ASSERT(
        djvBoxUtil::expand(djvBox2i(0, 5, 5, 10), djvVector2i(9, 19)) ==
        djvBox2i(0, 5, 10, 15));
}

void djvBoxUtilTest::border()
{
    DJV_DEBUG("djvBoxUtilTest::border");
    
    DJV_ASSERT(
        djvBoxUtil::border(djvBox2i(0, 5, 5, 10), djvVector2i(5, 5)) ==
        djvBox2i(-5, 0, 15, 20));
}

void djvBoxUtilTest::isSizeValid()
{
    DJV_DEBUG("djvBoxUtilTest::isSizeValid");
    
    DJV_ASSERT(djvBoxUtil::isSizeValid(djvBox2i(0, 5, 10, 15)));
}

void djvBoxUtilTest::convert()
{
    DJV_DEBUG("djvBoxUtilTest::convert");
    
    {
        const djvBox2i box =
            djvBoxUtil::convert<double, int, 2>(djvBox2f(0.0, 1.0, 2.0, 3.0));

        DJV_ASSERT(box == djvBox2i(0, 1, 2, 3));
    }
}

