// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ImageData.h>

namespace djv
{
    namespace ViewApp
    {
        bool ImageData::operator == (const ImageData& other) const
        {
            return channelsDisplay == other.channelsDisplay &&
                alphaBlend == other.alphaBlend &&mirror == other.mirror &&
                rotate == other.rotate &&
                aspectRatio == other.aspectRatio &&
                colorEnabled == other.colorEnabled &&
                color == other.color &&
                levelsEnabled == other.levelsEnabled &&
                levels == other.levels &&
                exposureEnabled == other.exposureEnabled &&
                exposure == other.exposure &&
                softClipEnabled == other.softClipEnabled &&
                softClip == other.softClip;
        }

        bool ImageData::operator != (const ImageData& other) const
        {
            return !(*this == other);
        }

    } // namespace ViewApp
} // namespace djv

