// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/Color.h>

namespace djv
{
    namespace AV
    {
        namespace Render2D
        {
            inline ImageColor::ImageColor()
            {}
            
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
            
            inline ImageLevels::ImageLevels()
            {}
            
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

            inline ImageExposure::ImageExposure()
            {}

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
            
            inline ImageOptions::ImageOptions()
            {}

            inline bool ImageOptions::operator == (const ImageOptions& other) const
            {
                return
                    channelDisplay  == other.channelDisplay  &&
                    alphaBlend      == other.alphaBlend      &&
                    mirror          == other.mirror          &&
                    colorSpace      == other.colorSpace      &&
                    color           == other.color           &&
                    colorEnabled    == other.colorEnabled    &&
                    levels          == other.levels          &&
                    levelsEnabled   == other.levelsEnabled   &&
                    exposure        == other.exposure        &&
                    exposureEnabled == other.exposureEnabled &&
                    softClip        == other.softClip        &&
                    softClipEnabled == other.softClipEnabled &&
                    cache           == other.cache;
            }

            inline bool ImageOptions::operator != (const ImageOptions& other) const
            {
                return !(*this == other);
            }

            inline ImageFilterOptions::ImageFilterOptions()
            {}

            inline ImageFilterOptions::ImageFilterOptions(ImageFilter minMag) :
                min(minMag),
                mag(minMag)
            {}

            inline ImageFilterOptions::ImageFilterOptions(ImageFilter min, ImageFilter mag) :
                min(min),
                mag(mag)
            {}

            inline bool ImageFilterOptions::operator == (const ImageFilterOptions& other) const
            {
                return min == other.min && mag == other.mag;
            }

            inline bool ImageFilterOptions::operator != (const ImageFilterOptions& other) const
            {
                return !(*this == other);
            }

            inline void Render::pushTransform(const glm::mat3x3& value)
            {
                _transforms.push_back(_transforms.size() ? _transforms.back() * value : value);
            }

            inline void Render::popTransform()
            {
                if (_transforms.size())
                {
                    _transforms.pop_back();
                }
            }

            inline void Render::pushClipRect(const Core::BBox2f& value)
            {
                if (!_clipRects.size())
                {
                    _currentClipRect = value;
                }
                else
                {
                    _currentClipRect = _currentClipRect.intersect(value);
                }
                _clipRects.push_back(_currentClipRect);
            }

            inline void Render::popClipRect()
            {
                _clipRects.pop_back();
                if (_clipRects.size())
                {
                    _currentClipRect = _clipRects.back();
                }
                else
                {
                    _currentClipRect.zero();
                }
            }

            inline void Render::setFillColor(const Image::Color& value)
            {
                if (Image::Type::RGBA_F32 == value.getType())
                {
                    const float* d = reinterpret_cast<const float*>(value.getData());
                    _fillColor[0] = d[0];
                    _fillColor[1] = d[1];
                    _fillColor[2] = d[2];
                    _fillColor[3] = d[3];
                }
                else
                {
                    const Image::Color tmp = value.convert(Image::Type::RGBA_F32);
                    const float* d = reinterpret_cast<const float*>(tmp.getData());
                    _fillColor[0] = d[0];
                    _fillColor[1] = d[1];
                    _fillColor[2] = d[2];
                    _fillColor[3] = d[3];
                }
                _finalColor[0] = _fillColor[0] * _colorMult;
                _finalColor[1] = _fillColor[1] * _colorMult;
                _finalColor[2] = _fillColor[2] * _colorMult;
                _finalColor[3] = _fillColor[3] * _alphaMult;
            }

            inline void Render::setColorMult(float value)
            {
                if (value == _colorMult)
                    return;
                _colorMult = value;
                _finalColor[0] = _fillColor[0] * _colorMult;
                _finalColor[1] = _fillColor[1] * _colorMult;
                _finalColor[2] = _fillColor[2] * _colorMult;
                _finalColor[3] = _fillColor[3] * _alphaMult;
            }

            inline void Render::setAlphaMult(float value)
            {
                if (value == _alphaMult)
                    return;
                _alphaMult = value;
                _finalColor[0] = _fillColor[0] * _colorMult;
                _finalColor[1] = _fillColor[1] * _colorMult;
                _finalColor[2] = _fillColor[2] * _colorMult;
                _finalColor[3] = _fillColor[3] * _alphaMult;
            }

            inline void Render::setLineWidth(float value)
            {
                _lineWidth = value;
            }

            inline const glm::mat3x3& Render::_getCurrentTransform() const
            {
                return _transforms.size() ? _transforms.back() : _identity;
            }

            inline void Render::_updateCurrentClipRect()
            {
                _currentClipRect.min.x = 0.F;
                _currentClipRect.min.y = 0.F;
                _currentClipRect.max.x = static_cast<float>(_size.w);
                _currentClipRect.max.y = static_cast<float>(_size.h);
                for (const auto& i : _clipRects)
                {
                    _currentClipRect = _currentClipRect.intersect(i);
                }
            }

        } // namespace Render2D
    } // namespace AV
} // namespace djv

