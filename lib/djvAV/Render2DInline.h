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

        } // namespace Render
    } // namespace AV
} // namespace djv

