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

#include <djvColorProfileTest.h>

#include <djvCore/Assert.h>
#include <djvGraphics/ColorProfile.h>
#include <djvCore/Debug.h>

using namespace djv;

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
        const Graphics::ColorProfile::Exposure exposure;
        DJV_ASSERT(djvMath::fuzzyCompare(0.f, exposure.value));
        DJV_ASSERT(djvMath::fuzzyCompare(0.f, exposure.defog));
        DJV_ASSERT(djvMath::fuzzyCompare(0.f, exposure.kneeLow));
        DJV_ASSERT(djvMath::fuzzyCompare(5.f, exposure.kneeHigh));
    }
    {
        const Graphics::ColorProfile::Exposure exposure(1.f, 2.f, 3.f, 4.f);
        DJV_ASSERT(djvMath::fuzzyCompare(1.f, exposure.value));
        DJV_ASSERT(djvMath::fuzzyCompare(2.f, exposure.defog));
        DJV_ASSERT(djvMath::fuzzyCompare(3.f, exposure.kneeLow));
        DJV_ASSERT(djvMath::fuzzyCompare(4.f, exposure.kneeHigh));
    }
    {
        const Graphics::ColorProfile colorProfile;
        DJV_ASSERT(Graphics::ColorProfile::RAW == colorProfile.type);
        DJV_ASSERT(djvMath::fuzzyCompare(2.2f, colorProfile.gamma));
        DJV_ASSERT(! colorProfile.lut.isValid());
    }
}

void djvColorProfileTest::operators()
{
    DJV_DEBUG("djvColorProfileTest::operators");
    {
        Graphics::ColorProfile a, b;
        a.type     = b.type     = Graphics::ColorProfile::LUT;
        a.gamma    = b.gamma    = 1.f;
        a.lut      = b.lut      = Graphics::PixelData(Graphics::PixelDataInfo(16, 1, Graphics::Pixel::L_U8));
        a.exposure = b.exposure = Graphics::ColorProfile::Exposure(1.f, 2.f, 3.f, 4.f);
        a.lut.zero();
        b.lut.zero();
        DJV_ASSERT(a.exposure == b.exposure);
        DJV_ASSERT(a.exposure != Graphics::ColorProfile::Exposure());
        DJV_ASSERT(a == b);
        DJV_ASSERT(a != Graphics::ColorProfile());
    }
    {
        Graphics::ColorProfile::Exposure exposure;
        QStringList s = QStringList() << "1.0" << "2.0" << "3.0" << "4.0";
        s >> exposure;
        DJV_ASSERT(djvMath::fuzzyCompare(1.f, exposure.value));
        DJV_ASSERT(djvMath::fuzzyCompare(2.f, exposure.defog));
        DJV_ASSERT(djvMath::fuzzyCompare(3.f, exposure.kneeLow));
        DJV_ASSERT(djvMath::fuzzyCompare(4.f, exposure.kneeHigh));
    }
    {
        Graphics::ColorProfile::Exposure exposure(1.f, 2.f, 3.f, 4.f);
        QStringList s;
        s << exposure;
        DJV_ASSERT((QStringList() << "1" << "2" << "3" << "4") == s);
    }
    {
        const Graphics::ColorProfile::Exposure a(1.f, 2.f, 3.f, 4.f);
        QStringList tmp;
        tmp << a;
        Graphics::ColorProfile::Exposure b;
        tmp >> b;
        DJV_ASSERT(a == b);
    }
    {
        const Graphics::ColorProfile::PROFILE a = Graphics::ColorProfile::LUT;
        QStringList tmp;
        tmp << a;
        Graphics::ColorProfile::PROFILE b = static_cast<Graphics::ColorProfile::PROFILE>(0);
        tmp >> b;
        DJV_ASSERT(a == b);
    }
    {
        DJV_DEBUG_PRINT(Graphics::ColorProfile::Exposure());
        DJV_DEBUG_PRINT(Graphics::ColorProfile::RAW);
        DJV_DEBUG_PRINT(Graphics::ColorProfile());
    }
}

