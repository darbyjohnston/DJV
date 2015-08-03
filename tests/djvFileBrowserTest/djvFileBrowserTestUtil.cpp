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

//! \file djvFileBrowserTestUtil.cpp

#include <djvFileBrowserTestUtil.h>

#include <djvAssert.h>
#include <djvPixelDataUtil.h>

#include <QApplication>
#include <QVector>

//------------------------------------------------------------------------------
// djvFileBrowserTestUtil
//------------------------------------------------------------------------------

const QStringList & djvFileBrowserTestUtil::thumbnailsLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvFileBrowserTestUtil", "Off") <<
        qApp->translate("djvFileBrowserTestUtil", "Low Quality") <<
        qApp->translate("djvFileBrowserTestUtil", "High Quality");

    DJV_ASSERT(data.count() == THUMBNAILS_COUNT);

    return data;
}

const QStringList & djvFileBrowserTestUtil::thumbnailSizeLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvFileBrowserTestUtil", "Small") <<
        qApp->translate("djvFileBrowserTestUtil", "Medium") <<
        qApp->translate("djvFileBrowserTestUtil", "Large");

    DJV_ASSERT(data.count() == THUMBNAILS_COUNT);

    return data;
}

int djvFileBrowserTestUtil::thumbnailSize(THUMBNAIL_SIZE size)
{
    static const QVector<int> data = QVector<int>() <<
        100 <<
        200 <<
        300;

    DJV_ASSERT(data.count() == THUMBNAIL_SIZE_COUNT);

    return data[size];
}

djvVector2i djvFileBrowserTestUtil::thumbnailSize(
    THUMBNAILS                thumbnails,
    int                       thumbnailSize,
    const djvVector2i &       imageSize,
    djvPixelDataInfo::PROXY * proxy)
{
    const int tmp = djvMath::max(imageSize.x, imageSize.y);

    if (tmp <= 0)
        return djvVector2i();

    int _proxy = 0;
    
    double proxyScale = static_cast<double>(
        djvPixelDataUtil::proxyScale(djvPixelDataInfo::PROXY(_proxy)));

    if (THUMBNAILS_LOW == thumbnails)
    {
        while (
            (tmp / proxyScale) > thumbnailSize * 2 &&
            _proxy < djvPixelDataInfo::PROXY_COUNT)
        {
            proxyScale = static_cast<double>(
                djvPixelDataUtil::proxyScale(djvPixelDataInfo::PROXY(++_proxy)));
        }
    }
    
    if (proxy)
    {
        *proxy = djvPixelDataInfo::PROXY(_proxy);
    }

    const double scale = thumbnailSize / static_cast<double>(tmp / proxyScale);
    
    return djvVectorUtil::ceil<double, int>(djvVector2f(imageSize) / proxyScale * scale);
}

