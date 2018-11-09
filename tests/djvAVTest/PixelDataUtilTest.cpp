//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <djvAVTest/PixelDataUtilTest.h>

#include <djvAV/AVContext.h>
#include <djvAV/PixelData.h>
#include <djvAV/PixelDataUtil.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>

#include <QPixmap>
#include <QString>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        void PixelDataUtilTest::run(int & argc, char ** argv)
        {
            DJV_DEBUG("PixelDataUtilTest::run");
            byteCount();
            proxy();
            interleave();
            gradient();
        }

        void PixelDataUtilTest::byteCount()
        {
            DJV_DEBUG("PixelDataUtilTest::byteCount");
            {
                const AV::PixelDataInfo info(1, 2, AV::Pixel::L_U8);
                DJV_ASSERT(1 == AV::PixelDataUtil::scanlineByteCount(info));
                DJV_ASSERT(2 == AV::PixelDataUtil::dataByteCount(info));
            }
        }

        void PixelDataUtilTest::proxy()
        {
            DJV_DEBUG("PixelDataUtilTest::proxy");
            QList<AV::PixelDataInfo::PROXY> proxies;
            QList<Memory::ENDIAN>                 endians;
            QList<AV::Pixel::PIXEL>         pixels;
            for (int i = 0; i < AV::PixelDataInfo::PROXY_COUNT; ++i)
            {
                proxies += static_cast<AV::PixelDataInfo::PROXY>(i);
            }
            for (int i = 0; i < Memory::ENDIAN_COUNT; ++i)
            {
                endians += static_cast<Memory::ENDIAN>(i);
            }
            for (int i = 0; i < AV::Pixel::PIXEL_COUNT; ++i)
            {
                pixels += static_cast<AV::Pixel::PIXEL>(i);
            }
            Q_FOREACH(AV::Pixel::PIXEL pixel, pixels)
            {
                AV::PixelData data(AV::PixelDataInfo(32, 32, pixel));
                data.zero();
                Q_FOREACH(AV::Pixel::PIXEL proxyPixel, pixels)
                {
                    Q_FOREACH(Memory::ENDIAN proxyEndian, endians)
                    {
                        Q_FOREACH(AV::PixelDataInfo::PROXY proxy, proxies)
                        {
                            AV::PixelDataInfo proxyInfo(
                                AV::PixelDataUtil::proxyScale(data.size(), proxy),
                                proxyPixel);
                            proxyInfo.endian = proxyEndian;
                            DJV_DEBUG_PRINT("proxy = " << proxy);
                            DJV_DEBUG_PRINT("info = " << proxyInfo);
                            AV::PixelData proxyData(proxyInfo);
                            AV::PixelDataUtil::proxyScale(data, proxyData, proxy);
                        }
                    }
                }
            }
            const Box2i box(16, 32, 64, 128);
            DJV_DEBUG_PRINT("box = " << box);
            Q_FOREACH(AV::PixelDataInfo::PROXY proxy, proxies)
            {
                DJV_DEBUG_PRINT("proxy = " << proxy);
                DJV_DEBUG_PRINT("box = " << AV::PixelDataUtil::proxyScale(box, proxy));
            }
        }

        void PixelDataUtilTest::interleave()
        {
            DJV_DEBUG("PixelDataUtilTest::interleave");
            QList<AV::Pixel::PIXEL> pixels;
            for (int i = 0; i < AV::Pixel::PIXEL_COUNT; ++i)
            {
                const AV::Pixel::PIXEL pixel = static_cast<AV::Pixel::PIXEL>(i);
                if (pixel != AV::Pixel::RGB_U10)
                {
                    pixels += pixel;
                }
            }
            Q_FOREACH(AV::Pixel::PIXEL pixel, pixels)
            {
                AV::PixelData data(AV::PixelDataInfo(32, 32, pixel));
                data.zero();
                DJV_DEBUG_PRINT("info = " << data.info());
                AV::PixelData interleaveData(data.info());
                AV::PixelDataUtil::planarInterleave(data, interleaveData);
                AV::PixelData deinterleaveData(data.info());
                AV::PixelDataUtil::planarDeinterleave(interleaveData, deinterleaveData);
                DJV_ASSERT(0 == memcmp(
                    data.data(),
                    deinterleaveData.data(),
                    data.dataByteCount()));
            }
        }

        void PixelDataUtilTest::gradient()
        {
            DJV_DEBUG("PixelDataUtilTest::gradient");
            AV::PixelData data(AV::PixelDataInfo(32, 32, AV::Pixel::L_F32));
            AV::PixelDataUtil::gradient(data);
        }

    } // namespace AVTest
} // namespace djv
