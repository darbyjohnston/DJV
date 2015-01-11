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

//! \file djvColorProfileTest.cpp

#include <djvColorProfileTest.h>

#include <djvAssert.h>
#include <djvColorProfile.h>
#include <djvDebug.h>

void djvColorProfileTest::run(int &, char **)
{
    DJV_DEBUG("djvColorProfileTest::run");
    
    ctors();
    operators();
}

void djvColorProfileTest::ctors()
{
    DJV_DEBUG("djvColorProfileTest::ctors");
    
    {
        const djvColorProfile::Exposure exposure;
        
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, exposure.value));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, exposure.defog));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0, exposure.kneeLow));
        DJV_ASSERT(djvMath::fuzzyCompare(5.0, exposure.kneeHigh));
    }
    
    {
        const djvColorProfile::Exposure exposure(1.0, 2.0, 3.0, 4.0);
        
        DJV_ASSERT(djvMath::fuzzyCompare(1.0, exposure.value));
        DJV_ASSERT(djvMath::fuzzyCompare(2.0, exposure.defog));
        DJV_ASSERT(djvMath::fuzzyCompare(3.0, exposure.kneeLow));
        DJV_ASSERT(djvMath::fuzzyCompare(4.0, exposure.kneeHigh));
    }
    
    {
        const djvColorProfile colorProfile;
        
        DJV_ASSERT(djvColorProfile::RAW == colorProfile.type);
        DJV_ASSERT(djvMath::fuzzyCompare(2.2, colorProfile.gamma));
        DJV_ASSERT(! colorProfile.lut.isValid());
    }
}

void djvColorProfileTest::operators()
{
    DJV_DEBUG("djvColorProfileTest::operators");
    
    {
        djvColorProfile a, b;
        a.type     = b.type     = djvColorProfile::LUT;
        a.gamma    = b.gamma    = 1.0;
        a.lut      = b.lut      = djvPixelData(djvPixelDataInfo(16, 1, djvPixel::L_U8));
        a.exposure = b.exposure = djvColorProfile::Exposure(1.0, 2.0, 3.0, 4.0);
        
        a.lut.zero();
        b.lut.zero();
        
        DJV_ASSERT(a.exposure == b.exposure);
        DJV_ASSERT(a.exposure != djvColorProfile::Exposure());
        
        DJV_ASSERT(a == b);
        DJV_ASSERT(a != djvColorProfile());
    }
    
    {
        djvColorProfile::Exposure exposure;
        
        QStringList s = QStringList() << "1.0" << "2.0" << "3.0" << "4.0";
        s >> exposure;
        
        DJV_ASSERT(djvMath::fuzzyCompare(1.0, exposure.value));
        DJV_ASSERT(djvMath::fuzzyCompare(2.0, exposure.defog));
        DJV_ASSERT(djvMath::fuzzyCompare(3.0, exposure.kneeLow));
        DJV_ASSERT(djvMath::fuzzyCompare(4.0, exposure.kneeHigh));
    }
    
    {
        djvColorProfile::Exposure exposure(1.0, 2.0, 3.0, 4.0);
        
        QStringList s;
        s << exposure;
        
        DJV_ASSERT((QStringList() << "1" << "2" << "3" << "4") == s);
    }
    
    {
        const djvColorProfile::Exposure a(1.0, 2.0, 3.0, 4.0);
        
        QStringList tmp;
        
        tmp << a;
        
        djvColorProfile::Exposure b;
        
        tmp >> b;
        
        DJV_ASSERT(a == b);
    }
    
    {
        const djvColorProfile::PROFILE a = djvColorProfile::LUT;
        
        QStringList tmp;
        
        tmp << a;
        
        djvColorProfile::PROFILE b = static_cast<djvColorProfile::PROFILE>(0);
        
        tmp >> b;
        
        DJV_ASSERT(a == b);
    }
    
    {
        DJV_DEBUG_PRINT(djvColorProfile::Exposure());
        
        DJV_DEBUG_PRINT(djvColorProfile::RAW);
        
        DJV_DEBUG_PRINT(djvColorProfile());
    }
}

