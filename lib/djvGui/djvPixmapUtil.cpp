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

#include <djvPixmapUtil.h>

#include <djvOpenGlOffscreenBuffer.h>

#include <QPixmap>

//------------------------------------------------------------------------------
// djvPixmapUtil
//------------------------------------------------------------------------------

QPixmap djvPixmapUtil::toQt(
    const djvPixelData &          pixelData,
    const djvOpenGlImageOptions & options,
    djvOpenGlImageState *         state,
    djvOpenGlOffscreenBuffer *    buffer)
{
    //DJV_DEBUG("djvPixmapUtil::toQt");
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

