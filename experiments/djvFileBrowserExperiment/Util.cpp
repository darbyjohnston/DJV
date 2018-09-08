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

#include <Util.h>

#include <djvAssert.h>
#include <djvPixelDataUtil.h>

#include <QApplication>
#include <QVector>

//------------------------------------------------------------------------------
// Util
//------------------------------------------------------------------------------

const ActionDataList & Util::sequenceData()
{
    static const ActionDataList data =
        ActionDataList() <<
        ActionData(
            djvSequence::compressLabels()[djvSequence::COMPRESS_OFF],
            qApp->translate("djvFileBrowserExperiment", "Disable file sequencing"),
            ":djvFileIcon.png") <<
        ActionData(
            djvSequence::compressLabels()[djvSequence::COMPRESS_SPARSE],
            qApp->translate("djvFileBrowserExperiment", "Enable sparse file sequencing"),
            ":djvSequenceIcon.png") <<
        ActionData(
            djvSequence::compressLabels()[djvSequence::COMPRESS_RANGE],
            qApp->translate("djvFileBrowserExperiment", "Enable range file sequencing"),
            ":djvSequenceRangeIcon.png");
    DJV_ASSERT(data.count() == djvSequence::COMPRESS_COUNT);
    return data;
}

const QStringList & Util::thumbnailsLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvFileBrowserExperiment", "Off") <<
        qApp->translate("djvFileBrowserExperiment", "Low Quality") <<
        qApp->translate("djvFileBrowserExperiment", "High Quality");
    DJV_ASSERT(data.count() == THUMBNAILS_COUNT);
    return data;
}

glm::ivec2 Util::thumbnailSize(
    THUMBNAILS                thumbnails,
    int                       thumbnailSize,
    const glm::ivec2 &        imageSize,
    djvPixelDataInfo::PROXY * proxy)
{
    const int tmp = djvMath::max(imageSize.x, imageSize.y);
    if (tmp <= 0)
        return glm::ivec2(0, 0);
    int _proxy = 0;
    float proxyScale = static_cast<float>(
        djvPixelDataUtil::proxyScale(djvPixelDataInfo::PROXY(_proxy)));
    if (THUMBNAILS_LOW == thumbnails)
    {
        while (
            (tmp / proxyScale) > thumbnailSize * 2 &&
            _proxy < djvPixelDataInfo::PROXY_COUNT)
        {
            proxyScale = static_cast<float>(
                djvPixelDataUtil::proxyScale(djvPixelDataInfo::PROXY(++_proxy)));
        }
    }
    if (proxy)
    {
        *proxy = djvPixelDataInfo::PROXY(_proxy);
    }
    const float scale = thumbnailSize / static_cast<float>(tmp / proxyScale);
    return djvVectorUtil::ceil(glm::vec2(imageSize) / proxyScale * scale);
}

const QStringList & Util::columnsLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvFileBrowserExperiment", "Name") <<
        qApp->translate("djvFileBrowserExperiment", "Size") <<
#if ! defined(DJV_WINDOWS)
        qApp->translate("djvFileBrowserExperiment", "User") <<
#endif // DJV_WINDOWS
        qApp->translate("djvFileBrowserExperiment", "Permissions") <<
        qApp->translate("djvFileBrowserExperiment", "Time");
    DJV_ASSERT(data.count() == COLUMNS_COUNT);
    return data;
}

