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

#include <djvPixelDataUtilTest.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvGraphics/GraphicsContext.h>
#include <djvGraphics/PixelData.h>
#include <djvGraphics/PixelDataUtil.h>

#include <QPixmap>
#include <QString>

using namespace djv;

void djvPixelDataUtilTest::run(int & argc, char ** argv)
{
    DJV_DEBUG("djvPixelDataUtilTest::run");
    byteCount();
    proxy();
    interleave();
    gradient();
}

void djvPixelDataUtilTest::byteCount()
{
    DJV_DEBUG("djvPixelDataUtilTest::byteCount");
    {
        const Graphics::PixelDataInfo info(1, 2, Graphics::Pixel::L_U8);
        DJV_ASSERT(1 == Graphics::PixelDataUtil::scanlineByteCount(info));
        DJV_ASSERT(2 == Graphics::PixelDataUtil::dataByteCount(info));
    }
}

void djvPixelDataUtilTest::proxy()
{
    DJV_DEBUG("djvPixelDataUtilTest::proxy");
    QList<Graphics::PixelDataInfo::PROXY> proxies;
    QList<djvMemory::ENDIAN>              endians;
    QList<Graphics::Pixel::PIXEL>         pixels;
    for (int i = 0; i < Graphics::PixelDataInfo::PROXY_COUNT; ++i)
    {
        proxies += static_cast<Graphics::PixelDataInfo::PROXY>(i);
    }
    for (int i = 0; i < djvMemory::ENDIAN_COUNT; ++i)
    {
        endians += static_cast<djvMemory::ENDIAN>(i);
    }
    for (int i = 0; i < Graphics::Pixel::PIXEL_COUNT; ++i)
    {
        pixels += static_cast<Graphics::Pixel::PIXEL>(i);
    }
    Q_FOREACH(Graphics::Pixel::PIXEL pixel, pixels)
    {
        Graphics::PixelData data(Graphics::PixelDataInfo(32, 32, pixel));
        data.zero();
        Q_FOREACH(Graphics::Pixel::PIXEL proxyPixel, pixels)
        {
            Q_FOREACH(djvMemory::ENDIAN proxyEndian, endians)
            {
                Q_FOREACH(Graphics::PixelDataInfo::PROXY proxy, proxies)
                {
                    Graphics::PixelDataInfo proxyInfo(
                        Graphics::PixelDataUtil::proxyScale(data.size(), proxy),
                        proxyPixel);
                    proxyInfo.endian = proxyEndian;
                    DJV_DEBUG_PRINT("proxy = " << proxy);
                    DJV_DEBUG_PRINT("info = " << proxyInfo);
                    Graphics::PixelData proxyData(proxyInfo);
                    Graphics::PixelDataUtil::proxyScale(data, proxyData, proxy);
                }
            }
        }
    }
    const djvBox2i box(16, 32, 64, 128);
    DJV_DEBUG_PRINT("box = " << box);
    Q_FOREACH(Graphics::PixelDataInfo::PROXY proxy, proxies)
    {
        DJV_DEBUG_PRINT("proxy = " << proxy);
        DJV_DEBUG_PRINT("box = " << Graphics::PixelDataUtil::proxyScale(box, proxy));
    }
}

void djvPixelDataUtilTest::interleave()
{
    DJV_DEBUG("djvPixelDataUtilTest::interleave");
    QList<Graphics::Pixel::PIXEL> pixels;
    for (int i = 0; i < Graphics::Pixel::PIXEL_COUNT; ++i)
    {
        const Graphics::Pixel::PIXEL pixel = static_cast<Graphics::Pixel::PIXEL>(i);
        if (pixel != Graphics::Pixel::RGB_U10)
        {
            pixels += pixel;
        }
    }
    Q_FOREACH(Graphics::Pixel::PIXEL pixel, pixels)
    {
        Graphics::PixelData data(Graphics::PixelDataInfo(32, 32, pixel));
        data.zero();
        DJV_DEBUG_PRINT("info = " << data.info());
        Graphics::PixelData interleaveData(data.info());
        Graphics::PixelDataUtil::planarInterleave(data, interleaveData);
        Graphics::PixelData deinterleaveData(data.info());
        Graphics::PixelDataUtil::planarDeinterleave(interleaveData, deinterleaveData);
        DJV_ASSERT(0 == memcmp(
            data.data(),
            deinterleaveData.data(),
            data.dataByteCount()));
    }
}

void djvPixelDataUtilTest::gradient()
{
    DJV_DEBUG("djvPixelDataUtilTest::gradient");
    Graphics::PixelData data(Graphics::PixelDataInfo(32, 32, Graphics::Pixel::L_F32));
    Graphics::PixelDataUtil::gradient(data);
}

