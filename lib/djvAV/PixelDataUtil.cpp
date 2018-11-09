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

#include <djvAV/PixelDataUtil.h>

#include <djvCore/Assert.h>

namespace djv
{
    namespace AV
    {
        PixelDataUtil::~PixelDataUtil()
        {}

        quint64 PixelDataUtil::scanlineByteCount(const PixelDataInfo & in)
        {
            return (in.size.x * Pixel::byteCount(in.pixel) * in.align) / in.align;
        }

        quint64 PixelDataUtil::dataByteCount(const PixelDataInfo & in)
        {
            return in.size.y * scanlineByteCount(in);
        }

        void PixelDataUtil::proxyScale(
            const PixelData &    in,
            PixelData &          out,
            PixelDataInfo::PROXY proxy)
        {
            //DJV_DEBUG("PixelDataUtil::proxyScale");
            //DJV_DEBUG_PRINT("in = " << in);
            //DJV_DEBUG_PRINT("out = " << out);
            //DJV_DEBUG_PRINT("proxy = " << proxy);

            const int  w = out.w();
            const int  h = out.h();
            const int  proxyScale = PixelDataUtil::proxyScale(proxy);
            const bool bgr = in.info().bgr != out.info().bgr;
            const bool endian = in.info().endian != out.info().endian;
            //DJV_DEBUG_PRINT("bgr = " << bgr);
            //DJV_DEBUG_PRINT("endian = " << endian);

            const bool fast = in.pixel() == out.pixel() && !bgr && !endian;
            //DJV_DEBUG_PRINT("fast = " << fast);

            std::vector<quint8> tmp;
            if (!fast)
            {
                tmp.resize(w * proxyScale * Pixel::byteCount(in.pixel()));
                //DJV_DEBUG_PRINT("tmp size = " << tmp.size());
            }

            for (int y = 0; y < h; ++y)
            {
                const quint8 * inP = in.data(0, y * proxyScale);
                quint8 * outP = out.data(0, y);
                if (fast)
                {
                    const quint64 pixelByteCount = in.pixelByteCount();
                    const quint64 inStride = pixelByteCount * proxyScale;
                    const quint64 outStride = pixelByteCount;
                    for (
                        int x = 0;
                        x < w;
                        ++x, inP += inStride, outP += outStride)
                    {
                        switch (pixelByteCount)
                        {
                        case 16: outP[15] = inP[15];
                        case 15: outP[14] = inP[14];
                        case 14: outP[13] = inP[13];
                        case 13: outP[12] = inP[12];
                        case 12: outP[11] = inP[11];
                        case 11: outP[10] = inP[10];
                        case 10: outP[9] = inP[9];
                        case 9:  outP[8] = inP[8];
                        case 8:  outP[7] = inP[7];
                        case 7:  outP[6] = inP[6];
                        case 6:  outP[5] = inP[5];
                        case 5:  outP[4] = inP[4];
                        case 4:  outP[3] = inP[3];
                        case 3:  outP[2] = inP[2];
                        case 2:  outP[1] = inP[1];
                        case 1:  outP[0] = inP[0];
                        }
                    }
                }
                else
                {
                    if (endian)
                    {
                        const int size = w * proxyScale;
                        const int wordSize = Pixel::byteCount(in.pixel());
                        //DJV_DEBUG_PRINT("endian size = " << size);
                        //DJV_DEBUG_PRINT("endian word size = " << wordSize);
                        Core::Memory::convertEndian(inP, tmp.data(), size, wordSize);
                        inP = tmp.data();
                    }

                    //DJV_DEBUG_PRINT("convert");
                    Pixel::convert(
                        inP,
                        in.pixel(),
                        outP,
                        out.pixel(),
                        w,
                        proxyScale,
                        bgr);
                }
            }
        }

        int PixelDataUtil::proxyScale(PixelDataInfo::PROXY proxy)
        {
            return proxy ? Core::Math::pow(2, static_cast<int>(proxy)) : 1;
        }

        glm::ivec2 PixelDataUtil::proxyScale(const glm::ivec2 & in, PixelDataInfo::PROXY proxy)
        {
            const int scale = proxyScale(proxy);
            return glm::ivec2(
                Core::Math::ceil(in.x / static_cast<float>(scale)),
                Core::Math::ceil(in.y / static_cast<float>(scale)));
        }

        Core::Box2i PixelDataUtil::proxyScale(const Core::Box2i & in, PixelDataInfo::PROXY proxy)
        {
            const int scale = proxyScale(proxy);
            return Core::Box2i(
                Core::Math::ceil(in.x / static_cast<float>(scale)),
                Core::Math::ceil(in.y / static_cast<float>(scale)),
                Core::Math::ceil(in.size.x / static_cast<float>(scale)),
                Core::Math::ceil(in.size.y / static_cast<float>(scale)));
        }

        void PixelDataUtil::planarInterleave(
            const PixelData &    in,
            PixelData &          out,
            PixelDataInfo::PROXY proxy)
        {
            DJV_ASSERT(in.pixel() == out.pixel());
            DJV_ASSERT(in.pixel() != Pixel::RGB_U10);

            //DJV_DEBUG("PixelDataUtil::planarInterleave");
            //DJV_DEBUG_PRINT("in = " << in);
            //DJV_DEBUG_PRINT("out = " << out);
            //DJV_DEBUG_PRINT("proxy = " << proxy);

            const int     w = out.w();
            const int     h = out.h();
            const int     proxyScale = PixelDataUtil::proxyScale(proxy);
            const int     channels = out.channels();
            const quint64 pixelByteCount = out.pixelByteCount();
            const int     channelByteCount = Pixel::channelByteCount(out.pixel());
            for (int c = 0; c < channels; ++c)
            {
                for (int y = 0; y < h; ++y)
                {
                    const quint8 * inP = in.data() + (c * in.h() + y * proxyScale) *
                        in.w() * channelByteCount;
                    quint8 * outP = out.data(0, y) + c * channelByteCount;
                    for (
                        int x = 0;
                        x < w;
                        ++x, inP += channelByteCount * proxyScale,
                        outP += pixelByteCount)
                    {
                        switch (channelByteCount)
                        {
                        case 4: outP[3] = inP[3];
                        case 3: outP[2] = inP[2];
                        case 2: outP[1] = inP[1];
                        case 1: outP[0] = inP[0];
                        }
                    }
                }
            }
        }

        void PixelDataUtil::planarDeinterleave(const PixelData & in, PixelData & out)
        {
            DJV_ASSERT(in.pixel() == out.pixel());
            DJV_ASSERT(in.pixel() != Pixel::RGB_U10);

            //DJV_DEBUG("PixelDataUtil::planarDeinterleave");
            //DJV_DEBUG_PRINT("in = " << in);
            //DJV_DEBUG_PRINT("out = " << out);

            const int     w = out.w();
            const int     h = out.h();
            const int     channels = out.channels();
            const quint64 pixelByteCount = out.pixelByteCount();
            const int     channelByteCount = Pixel::channelByteCount(out.pixel());
            for (int c = 0; c < channels; ++c)
            {
                for (int y = 0; y < h; ++y)
                {
                    const quint8 * inP = in.data(0, y) + c * channelByteCount;
                    quint8 * outP = out.data() + (c * h + y) * w * channelByteCount;
                    for (
                        int x = 0;
                        x < w;
                        ++x, inP += pixelByteCount, outP += channelByteCount)
                    {
                        switch (channelByteCount)
                        {
                        case 4: outP[3] = inP[3];
                        case 3: outP[2] = inP[2];
                        case 2: outP[1] = inP[1];
                        case 1: outP[0] = inP[0];
                        }
                    }
                }
            }
        }

        void PixelDataUtil::gradient(PixelData & out)
        {
            //DJV_DEBUG("gradient");
            const PixelDataInfo info(out.size(), Pixel::L_F32);
            out.set(info);
            //DJV_DEBUG_PRINT("out = " << out);
            for (int y = 0; y < info.size.y; ++y)
            {
                Pixel::F32_T * p = reinterpret_cast<Pixel::F32_T *>(out.data(0, y));
                for (int x = 0; x < info.size.x; ++x, ++p)
                {
                    *p = static_cast<Pixel::F32_T>(x / static_cast<float>(info.size.x - 1));
                }
            }
        }

    } // namespace AV
} // namespace djv
