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

#include <djvMathTest.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/Math.h>

#include <QString>

void djvMathTest::run(int &, char **)
{
    DJV_DEBUG("djvMathTest::run");
    members();
}

void djvMathTest::members()
{
    DJV_DEBUG("djvMathTest::members");
    {
        DJV_ASSERT(10 == djvMath::abs<int>( 10));
        DJV_ASSERT(10 == djvMath::abs<int>(-10));
    }
    {
        DJV_ASSERT(4 == djvMath::pow(2, 2));
        DJV_ASSERT(djvMath::fuzzyCompare(4.f, djvMath::pow(2.f, 2.f)));
    }
    {
        DJV_ASSERT(djvMath::fuzzyCompare(2.f, djvMath::sqrt(4.f)));
    }
    {
        DJV_ASSERT(0 == djvMath::mod<int>(2, 1));
    }
    {
        DJV_ASSERT(1 == djvMath::wrap<int>(-1, 0, 1));
        DJV_ASSERT(0 == djvMath::wrap<int>( 0, 0, 1));
        DJV_ASSERT(1 == djvMath::wrap<int>( 1, 0, 1));
        DJV_ASSERT(0 == djvMath::wrap<int>( 2, 0, 1));
    }
    {
        DJV_ASSERT(-1 == djvMath::step<int>(-1, 1));
        DJV_ASSERT( 0 == djvMath::step<int>( 0, 1));
        DJV_ASSERT( 1 == djvMath::step<int>( 1, 1));
        DJV_ASSERT( 1 == djvMath::step<int>( 2, 1));
    }
    {
        DJV_ASSERT(0 == djvMath::pulse<int>(-1, 0, 1));
        DJV_ASSERT(0 == djvMath::pulse<int>( 0, 0, 1));
        DJV_ASSERT(1 == djvMath::pulse<int>( 1, 0, 1));
        DJV_ASSERT(1 == djvMath::pulse<int>( 2, 0, 1));
    }
    {
        DJV_ASSERT(0 == djvMath::min(0, 1));
        DJV_ASSERT(1 == djvMath::max(0, 1));
    }
    {
        DJV_ASSERT(0 == djvMath::clamp(-1, 0, 1));
        DJV_ASSERT(0 == djvMath::clamp( 0, 0, 1));
        DJV_ASSERT(1 == djvMath::clamp( 1, 0, 1));
        DJV_ASSERT(1 == djvMath::clamp( 2, 0, 1));
    }
    {
        DJV_ASSERT(-1 == djvMath::lerp(-1, 0, 1));
        DJV_ASSERT( 0 == djvMath::lerp( 0, 0, 1));
        DJV_ASSERT( 1 == djvMath::lerp( 1, 0, 1));
        DJV_ASSERT( 2 == djvMath::lerp( 2, 0, 1));
    }
    {
        DJV_ASSERT(1 == djvMath::floor(1.5f));
        DJV_ASSERT(2 == djvMath::ceil (1.5f));
        DJV_ASSERT(2 == djvMath::round(1.5f));
    }
    {
        DJV_ASSERT(djvMath::fuzzyCompare(.5f, djvMath::fraction(1.5f)));
    }
    {
        DJV_ASSERT(4 == djvMath::toPow2(3));
    }
    {
        DJV_DEBUG_PRINT(djvMath::degreesToRadians(180.f));
        DJV_DEBUG_PRINT(djvMath::radiansToDegrees(djvMath::pi));
        //DJV_ASSERT(djvMath::fuzzyCompare(djvMath::pi, djvMath::degreesToRadians(180.f)));
        //DJV_ASSERT(djvMath::fuzzyCompare(180.f, djvMath::radiansToDegrees(djvMath::pi)));
    }
}

