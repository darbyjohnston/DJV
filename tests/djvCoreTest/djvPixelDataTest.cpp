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

//! \file djvPixelDataTest.cpp

#include <djvPixelDataTest.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvPixelData.h>

#include <QString>

void djvPixelDataTest::run(int &, char **)
{
    DJV_DEBUG("djvPixelDataTest::run");
    
    ctors();
    members();
    operators();
}

void djvPixelDataTest::ctors()
{
    DJV_DEBUG("djvPixelDataTest::ctors");
    
    {
        const djvPixelDataInfo info;
        
        DJV_ASSERT(djvVector2i() == info.size);
    }
    
    {
        const djvPixelDataInfo info(djvVector2i(1, 2), djvPixel::LA_U8);
        
        DJV_ASSERT(djvVector2i(1, 2) == info.size);
        DJV_ASSERT(djvPixel::LA_U8 == info.pixel);
    }
    
    {
        const djvPixelDataInfo info(1, 2, djvPixel::LA_U8);
        
        DJV_ASSERT(djvVector2i(1, 2) == info.size);
        DJV_ASSERT(djvPixel::LA_U8 == info.pixel);
    }
    
    {
        const djvPixelDataInfo info("name", djvVector2i(1, 2), djvPixel::LA_U8);
        
        DJV_ASSERT("name" == info.fileName);
        DJV_ASSERT(djvVector2i(1, 2) == info.size);
        DJV_ASSERT(djvPixel::LA_U8 == info.pixel);
    }
    
    {
        const djvPixelDataInfo info("name", 1, 2, djvPixel::LA_U8);
        
        DJV_ASSERT("name" == info.fileName);
        DJV_ASSERT(djvVector2i(1, 2) == info.size);
        DJV_ASSERT(djvPixel::LA_U8 == info.pixel);
    }
    
    {
        const djvPixelData data;
        
        DJV_ASSERT(djvVector2i() == data.size());
    }
    
    {
        const djvPixelData data(djvPixelDataInfo(1, 2, djvPixel::LA_U8));
        
        DJV_ASSERT(djvVector2i(1, 2) == data.size());
        DJV_ASSERT(djvPixel::LA_U8 == data.pixel());
    }
}

void djvPixelDataTest::members()
{
    DJV_DEBUG("djvPixelDataTest::members");
    
    {
        djvPixelData data;
        
        DJV_ASSERT(! data.isValid());

        const djvPixelDataInfo info(1, 2, djvPixel::LA_U8);
        
        data.set(info);
        
        DJV_ASSERT(info == data.info());
        DJV_ASSERT(data.isValid());
        
        data.zero();

        DJV_ASSERT(0 == *data.data());
        DJV_ASSERT(0 == *data.data(0, 1));
        
        const djvPixelData & tmp = data;

        DJV_ASSERT(0 == *tmp.data());
        DJV_ASSERT(0 == *tmp.data(0, 1));
        
        DJV_ASSERT(2 == data.pixelByteCount());
        DJV_ASSERT(2 == data.scanlineByteCount());
        DJV_ASSERT(4 == data.dataByteCount());
    }
}

void djvPixelDataTest::operators()
{
    DJV_DEBUG("djvPixelDataTest::operators");
    
    {
        djvPixelDataInfo::Mirror a, b;
        
        a.x = b.x = true;
        
        DJV_ASSERT(a == b);
        DJV_ASSERT(a != djvPixelDataInfo::Mirror());
    }
    
    {
        djvPixelDataInfo a, b;
        
        a.size = b.size = djvVector2i(1, 2);
        
        DJV_ASSERT(a == b);
        DJV_ASSERT(a != djvPixelDataInfo());
    }
    
    {
        djvPixelData
            a(djvPixelDataInfo(1, 2, djvPixel::LA_U8)),
            b(djvPixelDataInfo(1, 2, djvPixel::LA_U8));
        
        a.zero();
        b.zero();
        
        DJV_ASSERT(a == b);
        DJV_ASSERT(a != djvPixelData());
    }
    
    {
        DJV_DEBUG_PRINT(djvPixelDataInfo::PROXY_1_2);
        
        DJV_DEBUG_PRINT(djvPixelDataInfo::Mirror(true, true));
    }
}


