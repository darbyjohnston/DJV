//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvAV/Color.h>

namespace djv
{
    namespace AV
    {
        namespace Render
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
                    channel         == other.channel         &&
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
                    cache           == other.cache;
            }

            inline bool ImageOptions::operator != (const ImageOptions& other) const
            {
                return !(*this == other);
            }

            inline void Render2D::pushTransform(const glm::mat3x3& value)
            {
                _transforms.push_back(value);
                _currentTransform *= value;
            }

            inline void Render2D::popTransform()
            {
                _transforms.pop_back();
                _updateCurrentTransform();
            }

            inline void Render2D::pushClipRect(const Core::BBox2f & value)
            {
                _clipRects.push_back(value);
                _currentClipRect = _currentClipRect.intersect(value);
            }

            inline void Render2D::popClipRect()
            {
                _clipRects.pop_back();
                _updateCurrentClipRect();
            }

            inline void Render2D::setFillColor(const Image::Color & value)
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

            inline void Render2D::setColorMult(float value)
            {
                if (value == _colorMult)
                    return;
                _colorMult = value;
                _finalColor[0] = _fillColor[0] * _colorMult;
                _finalColor[1] = _fillColor[1] * _colorMult;
                _finalColor[2] = _fillColor[2] * _colorMult;
                _finalColor[3] = _fillColor[3] * _alphaMult;
            }

            inline void Render2D::setAlphaMult(float value)
            {
                if (value == _alphaMult)
                    return;
                _alphaMult = value;
                _finalColor[0] = _fillColor[0] * _colorMult;
                _finalColor[1] = _fillColor[1] * _colorMult;
                _finalColor[2] = _fillColor[2] * _colorMult;
                _finalColor[3] = _fillColor[3] * _alphaMult;
            }

            inline void Render2D::setLineWidth(float value)
            {
                _lineWidth = value;
            }

            inline void Render2D::_updateCurrentTransform()
            {
                _currentTransform = glm::mat3x3(1.F);
                for (const auto& i : _transforms)
                {
                    _currentTransform *= i;
                }
            }

            inline void Render2D::_updateCurrentClipRect()
            {
                _currentClipRect.min.x = 0.F;
                _currentClipRect.min.y = 0.F;
                _currentClipRect.max.x = static_cast<float>(_size.w);
                _currentClipRect.max.y = static_cast<float>(_size.h);
                for (const auto & i : _clipRects)
                {
                    _currentClipRect = _currentClipRect.intersect(i);
                }
            }

        } // namespace Render
    } // namespace AV
} // namespace djv

