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

//! \file djvPixelDataUtilTest.cpp

#include <djvPixelDataUtilTest.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvOpenGlContext.h>
#include <djvPixelData.h>
#include <djvPixelDataUtil.h>

#include <QApplication>
#include <QPixmap>
#include <QString>

void djvPixelDataUtilTest::run(int & argc, char ** argv)
{
    DJV_DEBUG("djvPixelDataUtilTest::run");

    // A QApplication must be created before any QPixmaps are created.
    
    QApplication app(argc, argv);
    
    byteCount();
    proxy();
    interleave();
    gradient();
    qt();
}

void djvPixelDataUtilTest::byteCount()
{
    DJV_DEBUG("djvPixelDataUtilTest::byteCount");
    
    {
        const djvPixelDataInfo info(1, 2, djvPixel::L_U8);
        
        DJV_ASSERT(1 == djvPixelDataUtil::scanlineByteCount(info));
        DJV_ASSERT(2 == djvPixelDataUtil::dataByteCount(info));
    }
}

void djvPixelDataUtilTest::proxy()
{
    DJV_DEBUG("djvPixelDataUtilTest::proxy");
    
    QList<djvPixelDataInfo::PROXY> proxies;
    QList<djvMemory::ENDIAN>       endians;
    QList<djvPixel::PIXEL>         pixels;

    for (int i = 0; i < djvPixelDataInfo::PROXY_COUNT; ++i)
    {
        proxies += static_cast<djvPixelDataInfo::PROXY>(i);
    }

    for (int i = 0; i < djvMemory::ENDIAN_COUNT; ++i)
    {
        endians += static_cast<djvMemory::ENDIAN>(i);
    }

    for (int i = 0; i < djvPixel::PIXEL_COUNT; ++i)
    {
        pixels += static_cast<djvPixel::PIXEL>(i);
    }    

    Q_FOREACH(djvPixel::PIXEL pixel, pixels)
    {
        djvPixelData data(djvPixelDataInfo(32, 32, pixel));
        data.zero();
        
        Q_FOREACH(djvPixel::PIXEL proxyPixel, pixels)
        {
            Q_FOREACH(djvMemory::ENDIAN proxyEndian, endians)
            {
                Q_FOREACH(djvPixelDataInfo::PROXY proxy, proxies)
                {
                    djvPixelDataInfo proxyInfo(
                        djvPixelDataUtil::proxyScale(data.size(), proxy),
                        proxyPixel);
                    proxyInfo.endian = proxyEndian;
                    
                    DJV_DEBUG_PRINT("proxy = " << proxy);
                    DJV_DEBUG_PRINT("info = " << proxyInfo);
                
                    djvPixelData proxyData(proxyInfo);
                
                    djvPixelDataUtil::proxyScale(data, proxyData, proxy);
                }
            }
        }
    }
    
    const djvBox2i box(16, 32, 64, 128);
    
    DJV_DEBUG_PRINT("box = " << box);
    
    Q_FOREACH(djvPixelDataInfo::PROXY proxy, proxies)
    {
        DJV_DEBUG_PRINT("proxy = " << proxy);
        DJV_DEBUG_PRINT("box = " << djvPixelDataUtil::proxyScale(box, proxy));
    }
}

void djvPixelDataUtilTest::interleave()
{
    DJV_DEBUG("djvPixelDataUtilTest::interleave");
    
    QList<djvPixel::PIXEL> pixels;

    for (int i = 0; i < djvPixel::PIXEL_COUNT; ++i)
    {
        const djvPixel::PIXEL pixel = static_cast<djvPixel::PIXEL>(i);
        
        if (pixel != djvPixel::RGB_U10)
        {
            pixels += pixel;
        }
    }    

    Q_FOREACH(djvPixel::PIXEL pixel, pixels)
    {
        djvPixelData data(djvPixelDataInfo(32, 32, pixel));
        data.zero();
        
        DJV_DEBUG_PRINT("info = " << data.info());
        
        djvPixelData interleaveData(data.info());
        
        djvPixelDataUtil::planarInterleave(data, interleaveData);

        djvPixelData deinterleaveData(data.info());

        djvPixelDataUtil::planarDeinterleave(interleaveData, deinterleaveData);
        
        DJV_ASSERT(0 == djvMemory::compare(
            data.data(),
            deinterleaveData.data(),
            data.dataByteCount()));
    }
}

void djvPixelDataUtilTest::gradient()
{
    DJV_DEBUG("djvPixelDataUtilTest::gradient");
    
    djvPixelData data(djvPixelDataInfo(32, 32, djvPixel::L_F32));
    
    djvPixelDataUtil::gradient(data);
}

void djvPixelDataUtilTest::qt()
{
    DJV_DEBUG("djvPixelDataUtilTest::qt");
    
    QScopedPointer<djvOpenGlContext> context(djvOpenGlContextFactory::create());
        
    djvOpenGlContextScope contextScope(context.data());
    
    djvPixelData data(djvPixelDataInfo(32, 32, djvPixel::L_F32));
    
    djvPixelDataUtil::toQt(data);
}

