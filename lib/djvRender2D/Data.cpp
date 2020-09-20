// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvRender2D/Data.h>

namespace djv
{
    namespace Render2D
    {
        ImageColor::ImageColor()
        {}
        
        ImageLevels::ImageLevels()
        {}

        ImageExposure::ImageExposure()
        {}
        
        ImageOptions::ImageOptions()
        {}

        ImageFilterOptions::ImageFilterOptions()
        {}

        ImageFilterOptions::ImageFilterOptions(ImageFilter minMag) :
            min(minMag),
            mag(minMag)
        {}

        ImageFilterOptions::ImageFilterOptions(ImageFilter min, ImageFilter mag) :
            min(min),
            mag(mag)
        {}
        
    } // namespace Render2D
} // namespace djv
