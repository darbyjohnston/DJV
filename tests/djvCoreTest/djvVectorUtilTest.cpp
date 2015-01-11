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

//! \file djvVectorUtilTest.cpp

#include <djvVectorUtilTest.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvStringUtil.h>
#include <djvVector.h>
#include <djvVectorUtil.h>

void djvVectorUtilTest::run(int &, char **)
{
    DJV_DEBUG("djvVectorUtilTest::run");
    
    abs();
    swap();
    range();
    isSizeValid();
    length();
    normalize();
    dot();
    aspect();
    convert();
}

void djvVectorUtilTest::abs()
{
    DJV_DEBUG("djvVectorUtilTest::abs");
    
    DJV_ASSERT(djvVectorUtil::abs(djvVector2i(-1, -2)) == djvVector2i(1, 2));
}

void djvVectorUtilTest::swap()
{
    DJV_DEBUG("djvVectorUtilTest::swap");
    
    DJV_ASSERT(djvVectorUtil::swap(djvVector2i(1, 2)) == djvVector2i(2, 1));
}

void djvVectorUtilTest::range()
{
    DJV_DEBUG("djvVectorUtilTest::range");
    
    DJV_ASSERT(
        djvVectorUtil::min(djvVector2i(1, 2), djvVector2i(3, 4)) == djvVector2i(1, 2));
    DJV_ASSERT(
        djvVectorUtil::max(djvVector2i(1, 2), djvVector2i(3, 4)) == djvVector2i(3, 4));
    DJV_ASSERT(
        djvVectorUtil::clamp(djvVector2i(-10, 10), djvVector2i(1, 2), djvVector2i(3, 4)) == djvVector2i(1, 4));
}

void djvVectorUtilTest::isSizeValid()
{
    DJV_DEBUG("djvVectorUtilTest::isSizeValid");
    
    DJV_ASSERT(
        djvVectorUtil::isSizeValid(djvVector2i(1, 2)) &&
        !djvVectorUtil::isSizeValid(djvVector2i(-1, -2)));
}

void djvVectorUtilTest::length()
{
    DJV_DEBUG("djvVectorUtilTest::length");
    
    DJV_ASSERT(djvVectorUtil::length(djvVector2i(0, 2)) == 2);
}

void djvVectorUtilTest::normalize()
{
    DJV_DEBUG("djvVectorUtilTest::normalize");
    
    DJV_ASSERT(djvVectorUtil::normalize(djvVector2f(0.0, 2.0)) == djvVector2f(0.0, 1.0));
}

void djvVectorUtilTest::dot()
{
    DJV_DEBUG("djvVectorUtilTest::dot");
    
    DJV_ASSERT(djvVectorUtil::dot(djvVector2f(1.0, 0.0), djvVector2f(0.0, 1.0)) == 0.0);
}

void djvVectorUtilTest::aspect()
{
    DJV_DEBUG("djvVectorUtilTest::aspect");
    
    DJV_ASSERT(djvVectorUtil::aspect(djvVector2i(2, 1)) == 2.0);
}

void djvVectorUtilTest::convert()
{
    DJV_DEBUG("djvVectorUtilTest::convert");
    
    djvVector2i v2i = djvVectorUtil::convert<double, int, 2>(djvVector2f(1.0, 2.0));
    DJV_ASSERT(v2i == djvVector2i(1, 2));
    v2i = djvVectorUtil::ceil<double, int, 2>(djvVector2f(0.5, 0.5));
    DJV_ASSERT(v2i == djvVector2i(1, 1));
    v2i = djvVectorUtil::floor<double, int, 2>(djvVector2f(0.5, 0.5));
    DJV_ASSERT(v2i == djvVector2i(0, 0));

    DJV_ASSERT((QStringList() << "1" << "2") ==
        djvStringUtil::label(djvVector2i(1, 2)));
}


