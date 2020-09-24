// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvImage/Color.h>

namespace djv
{
    namespace Render2D
    {
        inline bool ImageColor::operator == (const ImageColor& other) const
        {
            return
                brightness == other.brightness &&
                contrast   == other.contrast   &&
                saturation == other.saturation &&
                invert     == other.invert;
        }
        
        inline bool ImageColor::operator != (const ImageColor& other) const
        {
            return !(*this == other);
        }
        
        inline bool ImageLevels::operator == (const ImageLevels& other) const
        {
            return
                inLow   == other.inLow   &&
                inHigh  == other.inHigh  &&
                gamma   == other.gamma   &&
                outLow  == other.outLow  &&
                outHigh == other.outHigh;
        }
        
        inline bool ImageLevels::operator != (const ImageLevels& other) const
        {
            return !(*this == other);
        }

        inline bool ImageExposure::operator == (const ImageExposure& other) const
        {
            return
                exposure == other.exposure &&
                defog    == other.defog    &&
                kneeLow  == other.kneeLow  &&
                kneeHigh == other.kneeHigh;
        }

        inline bool ImageExposure::operator != (const ImageExposure& other) const
        {
            return !(*this == other);
        }

        inline bool ImageOptions::operator == (const ImageOptions& other) const
        {
            return
                channelDisplay  == other.channelDisplay  &&
                alphaBlend      == other.alphaBlend      &&
                mirror          == other.mirror          &&
                colorSpace      == other.colorSpace      &&
                colorEnabled    == other.colorEnabled    &&
                color           == other.color           &&
                levelsEnabled   == other.levelsEnabled   &&
                levels          == other.levels          &&
                exposureEnabled == other.exposureEnabled &&
                exposure        == other.exposure        &&
                softClipEnabled == other.softClipEnabled &&
                softClip        == other.softClip        &&
                cache           == other.cache;
        }

        inline bool ImageOptions::operator != (const ImageOptions& other) const
        {
            return !(*this == other);
        }

        inline GLenum toGL(ImageFilter value)
        {
            GLenum out = GL_NONE;
            switch (value)
            {
            case ImageFilter::Nearest: out = GL_NEAREST; break;
            case ImageFilter::Linear:  out = GL_LINEAR;  break;
            default: break;
            }
            return out;
        }

        inline bool ImageFilterOptions::operator == (const ImageFilterOptions& other) const
        {
            return min == other.min && mag == other.mag;
        }

        inline bool ImageFilterOptions::operator != (const ImageFilterOptions& other) const
        {
            return !(*this == other);
        }

    } // namespace Render2D
} // namespace djv

