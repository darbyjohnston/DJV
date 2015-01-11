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

//! \file djvPixelDataUtil.cpp

#include <djvPixelDataUtil.h>

#include <djvAssert.h>
#include <djvOpenGlOffscreenBuffer.h>

#include <QPixmap>

//------------------------------------------------------------------------------
// djvPixelDataUtil
//------------------------------------------------------------------------------

djvPixelDataUtil::~djvPixelDataUtil()
{}

quint64 djvPixelDataUtil::scanlineByteCount(const djvPixelDataInfo & in)
{
    return (in.size.x * djvPixel::byteCount(in.pixel) * in.align) / in.align;
}

quint64 djvPixelDataUtil::dataByteCount(const djvPixelDataInfo & in)
{
    return in.size.y * scanlineByteCount(in);
}

void djvPixelDataUtil::proxyScale(
    const djvPixelData &    in,
    djvPixelData &          out,
    djvPixelDataInfo::PROXY proxy)
{
    //DJV_DEBUG("djvPixelDataUtil::proxyScale");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("out = " << out);
    //DJV_DEBUG_PRINT("proxy = " << proxy);

    const int  w          = out.w();
    const int  h          = out.h();
    const int  proxyScale = djvPixelDataUtil::proxyScale(proxy);
    const bool bgr        = in.info().bgr != out.info().bgr;
    const bool endian     = in.info().endian != out.info().endian;
    
    //DJV_DEBUG_PRINT("bgr = " << bgr);
    //DJV_DEBUG_PRINT("endian = " << endian);

    const bool fast = in.pixel() == out.pixel() && ! bgr && ! endian;
    
    //DJV_DEBUG_PRINT("fast = " << fast);

    djvMemoryBuffer<quint8> tmp;

    if (! fast)
    {
        tmp.setSize(w * proxyScale * djvPixel::byteCount(in.pixel()));
    }

    for (int y = 0; y < h; ++y)
    {
        const quint8 * inP = in.data(0, y * proxyScale);
        quint8 * outP = out.data(0, y);

        if (fast)
        {
            const quint64 pixelByteCount = in.pixelByteCount();
            const quint64 inStride       = pixelByteCount * proxyScale;
            const quint64 outStride      = pixelByteCount;

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
                    case 10: outP[ 9] = inP[ 9];
                    case 9:  outP[ 8] = inP[ 8];
                    case 8:  outP[ 7] = inP[ 7];
                    case 7:  outP[ 6] = inP[ 6];
                    case 6:  outP[ 5] = inP[ 5];
                    case 5:  outP[ 4] = inP[ 4];
                    case 4:  outP[ 3] = inP[ 3];
                    case 3:  outP[ 2] = inP[ 2];
                    case 2:  outP[ 1] = inP[ 1];
                    case 1:  outP[ 0] = inP[ 0];
                }
            }
        }
        else
        {
            if (endian)
            {
                const int size     = w * proxyScale;
                const int wordSize = djvPixel::byteCount(in.pixel());
                
                //DJV_DEBUG_PRINT("endian size = " << size);
                //DJV_DEBUG_PRINT("endian word size = " << wordSize);
                
                djvMemory::convertEndian(inP, tmp(), size, wordSize);
                
                inP = tmp();
            }

            djvPixel::convert(
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

int djvPixelDataUtil::proxyScale(djvPixelDataInfo::PROXY proxy)
{
    return proxy ? djvMath::pow(2, static_cast<int>(proxy)) : 1;
}

djvVector2i djvPixelDataUtil::proxyScale(const djvVector2i & in, djvPixelDataInfo::PROXY proxy)
{
    const int scale = proxyScale(proxy);

    return djvVector2i(
        djvMath::ceil(in.x / static_cast<double>(scale)),
        djvMath::ceil(in.y / static_cast<double>(scale)));
}

djvBox2i djvPixelDataUtil::proxyScale(const djvBox2i & in, djvPixelDataInfo::PROXY proxy)
{
    const int scale = proxyScale(proxy);

    return djvBox2i(
        djvMath::ceil(in.x / static_cast<double>(scale)),
        djvMath::ceil(in.y / static_cast<double>(scale)),
        djvMath::ceil(in.size.x / static_cast<double>(scale)),
        djvMath::ceil(in.size.y / static_cast<double>(scale)));
}

void djvPixelDataUtil::planarInterleave(
    const djvPixelData &    in,
    djvPixelData &          out,
    djvPixelDataInfo::PROXY proxy)
{
    DJV_ASSERT(in.pixel() == out.pixel());
    DJV_ASSERT(in.pixel() != djvPixel::RGB_U10);

    //DJV_DEBUG("djvPixelDataUtil::planarInterleave");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("out = " << out);
    //DJV_DEBUG_PRINT("proxy = " << proxy);

    const int     w                = out.w();
    const int     h                = out.h();
    const int     proxyScale       = djvPixelDataUtil::proxyScale(proxy);
    const int     channels         = out.channels();
    const quint64 pixelByteCount   = out.pixelByteCount();
    const int     channelByteCount = djvPixel::channelByteCount(out.pixel());

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

void djvPixelDataUtil::planarDeinterleave(const djvPixelData & in, djvPixelData & out)
{
    DJV_ASSERT(in.pixel() == out.pixel());
    DJV_ASSERT(in.pixel() != djvPixel::RGB_U10);

    //DJV_DEBUG("djvPixelDataUtil::planarDeinterleave");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("out = " << out);

    const int     w                = out.w();
    const int     h                = out.h();
    const int     channels         = out.channels();
    const quint64 pixelByteCount   = out.pixelByteCount();
    const int     channelByteCount = djvPixel::channelByteCount(out.pixel());

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

void djvPixelDataUtil::gradient(djvPixelData & out)
{
    //DJV_DEBUG("gradient");

    const djvPixelDataInfo info(out.size(), djvPixel::L_F32);
    
    out.set(info);
    
    //DJV_DEBUG_PRINT("out = " << out);

    for (int y = 0; y < info.size.y; ++y)
    {
        djvPixel::F32_T * p = reinterpret_cast<djvPixel::F32_T *>(out.data(0, y));

        for (int x = 0; x < info.size.x; ++x, ++p)
        {
            *p = static_cast<djvPixel::F32_T>(
                x / static_cast<double>(info.size.x - 1));
        }
    }
}

QPixmap djvPixelDataUtil::toQt(
    const djvPixelData &          pixelData,
    const djvOpenGlImageOptions & options,
    djvOpenGlImageState *         state,
    djvOpenGlOffscreenBuffer *    buffer)
{
    //DJV_DEBUG("djvPixelDataUtil::toQt");
    //DJV_DEBUG_PRINT("pixelData = " << pixelData);

    // Convert the pixel data to an 8-bit format.
    
    const int w = pixelData.w();
    const int h = pixelData.h();
    
    const djvPixelDataInfo info(w, h, djvPixel::RGBA_U8);
    
    const djvPixelData * pixelDataP = &pixelData;

    djvPixelData tmp;
    
    if (pixelDataP->pixel() != info.pixel)
    {
        tmp.set(info);

        QScopedPointer<djvOpenGlOffscreenBuffer> _buffer;

        if (! buffer)
        {
            //DJV_DEBUG_PRINT("create buffer");

            _buffer.reset(new djvOpenGlOffscreenBuffer(info));

            buffer = _buffer.data();
        }
        
        djvOpenGlImage::copy(pixelData, tmp, options, state, buffer);
        
        pixelDataP = &tmp;
    }
    
    // Convert to a QImage.
    
    QImage qImage(w, h, QImage::Format_ARGB32);

    //! \todo It looks like when RGB_U10 is being converted to RGBA_U8
    //! the alpha channel is 0 instead of 255. This is a hack to work
    //! around the issue but how do we fix the underlying problem?

    const bool alpha = pixelData.pixel() != djvPixel::RGB_U10;

    //DJV_DEBUG_PRINT("alpha = " << alpha);
    
    for (int y = 0; y < h; ++y)
    {
        QRgb * qRgb = (QRgb *)qImage.scanLine(y);
        
        const uchar * p = pixelDataP->data(0, h - 1 - y);
        
        for (int x = 0; x < w; ++x, p += 4)
        {
            //const uchar * p = pixelDataP->data(x, h - 1 - y);

            //DJV_DEBUG_PRINT("pixel (" << x << ", " << y << ") = " <<
            //    p[0] << " " << p[1] << " " << p[2] << " " << p[3]);
        
            qRgb[x] = qRgba(p[0], p[1], p[2], alpha ? p[3] : 255);
        }
    }
    
    return QPixmap::fromImage(qImage);
}

