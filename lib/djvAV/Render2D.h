//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#pragma once

#include <djvAV/Enum.h>
#include <djvAV/FontSystem.h>
#include <djvAV/Image.h>
#include <djvAV/OCIO.h>

#include <djvCore/BBox.h>
#include <djvCore/ISystem.h>
#include <djvCore/PicoJSON.h>
#include <djvCore/Range.h>

#include <list>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            class Color;

        } // namespace Image

        //! This namespace provides rendering functionality.
        namespace Render
        {
            //! This enumeration provides which image channels are displayed.
            enum class ImageChannel
            {
                None,
                Red,
                Green,
                Blue,
                Alpha,

                Count,
                First = None
            };
            DJV_ENUM_HELPERS(ImageChannel);

            //! This eumeration provides the image cache options.
            enum class ImageCache
            {
                Atlas,
                Dynamic,

                Count,
                First = Atlas
            };
            DJV_ENUM_HELPERS(ImageCache);
            
            //! This class provides color values.
            class ImageColor
            {
            public:
                ImageColor();
                
                float brightness = 1.F;
                float contrast   = 1.F;
                float saturation = 1.F;
                bool  invert     = false;

                bool operator == (const ImageColor&) const;
                bool operator != (const ImageColor&) const;
            };
            
            //! Create a brightness matrix.
            glm::mat4x4 brightnessMatrix(float r, float g, float b);

            //! Create a contrast matrix.
            glm::mat4x4 contrastMatrix(float r, float g, float b);

            //! Create a saturation matrix.
            glm::mat4x4 saturationMatrix(float r, float g, float b);

            //! Create a color matrix.
            glm::mat4x4 colorMatrix(const ImageColor&);
            
            //! This class provides levels values.
            class ImageLevels
            {
            public:
                ImageLevels();
                
                float inLow   = 0.F;
                float inHigh  = 1.F;
                float gamma   = 1.F;
                float outLow  = 0.F;
                float outHigh = 1.F;

                bool operator == (const ImageLevels&) const;
                bool operator != (const ImageLevels&) const;
            };
            
            //! This class provides exposure values.
            class ImageExposure
            {
            public:
                ImageExposure();
                
                float exposure = 0.F;
                float defog    = 0.F;
                float kneeLow  = 0.F;
                float kneeHigh = 5.F;

                bool operator == (const ImageExposure&) const;
                bool operator != (const ImageExposure&) const;
            };

            //! This class provides image options.
            class ImageOptions
            {
            public:
                ImageOptions();
                
                ImageChannel      channel           = ImageChannel::None;
                AlphaBlend        alphaBlend        = AlphaBlend::Straight;
                AV::Image::Mirror mirror;
                AV::OCIO::Convert colorSpace;
                ImageColor        color;
                bool              colorEnabled      = false;
                ImageLevels       levels;
                bool              levelsEnabled     = false;
                float             softClip          = 0.F;
                ImageExposure     exposure;
                bool              exposureEnabled   = false;
                ImageCache        cache             = ImageCache::Atlas;

                bool operator == (const ImageOptions&) const;
                bool operator != (const ImageOptions&) const;
            };

            //! This eumeration provides the image filtering options.
            enum class ImageFilter
            {
                Nearest,
                Linear,

                Count,
                First = Nearest
            };
            DJV_ENUM_HELPERS(ImageFilter);
            GLenum toGL(ImageFilter);

            class ImageFilterOptions
            {
            public:
                ImageFilterOptions();
                ImageFilterOptions(ImageFilter minMag);
                ImageFilterOptions(ImageFilter min, ImageFilter mag);

                ImageFilter min = ImageFilter::Linear;
                ImageFilter mag = ImageFilter::Nearest;

                bool operator == (const ImageFilterOptions&) const;
                bool operator != (const ImageFilterOptions&) const;
            };

            //! This class provides a 2D renderer.
            class Render2D : public Core::ISystem
            {
                DJV_NON_COPYABLE(Render2D);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                Render2D();

            public:
                ~Render2D();

                static std::shared_ptr<Render2D> create(const std::shared_ptr<Core::Context>&);

                //! \name Begin and End
                ///@{

                void beginFrame(const Image::Size&);
                void endFrame();

                ///@}

                //! \name Transform
                ///@{

                void pushTransform(const glm::mat3x3&);
                void popTransform();

                ///@}

                //! \name Clipping Rectangle
                ///@{

                void pushClipRect(const Core::BBox2f &);
                void popClipRect();

                ///@}

                //! \name Color
                ///@{

                void setFillColor(const Image::Color&);
                void setColorMult(float);
                void setAlphaMult(float);

                ///@}

                //! \name Line Width
                ///@{

                void setLineWidth(float);

                ///@}

                //! \name Primitives
                ///@{

                void drawPolyline(const std::vector<glm::vec2>&);
                void drawRect(const Core::BBox2f&);
                void drawRects(const std::vector<Core::BBox2f>&);
                void drawPill(const Core::BBox2f&, size_t facets = 32);
                void drawCircle(const glm::vec2 & pos, float radius, size_t facets = 64);

                ///@}

                //! \name Images
                ///@{

                const ImageFilterOptions& getImageFilterOptions() const;

                //! This function should only be called outside of beginFrame()/endFrame().
                void setImageFilterOptions(const ImageFilterOptions&);

                void drawImage(
                    const std::shared_ptr<Image::Image> &,
                    const glm::vec2& pos,
                    const ImageOptions& = ImageOptions());

                void drawFilledImage(
                    const std::shared_ptr<Image::Image> &,
                    const glm::vec2& pos,
                    const ImageOptions & = ImageOptions());

                ///@}

                //! \name Text
                ///@{

                void setCurrentFont(const Font::Info &);
                void setLCDText(bool);

                void drawText(const std::vector<std::shared_ptr<Font::Glyph> >& glyphs, const glm::vec2& position);

                ///@}

                //! \name Shadows
                ///@{

                void drawShadow(const Core::BBox2f&, Side);
                void drawShadow(const Core::BBox2f&, float radius, size_t facets = 16);

                ///@}

                //! \name Diagnostics
                ///@{

                float getTextureAtlasPercentage() const;
                size_t getDynamicTextureCount() const;
                size_t getVBOSize() const;

                ///@}

            private:
                void _updateCurrentTransform();
                void _updateImageFilter();

                Image::Size             _size;
                std::list<glm::mat3x3>  _transforms;
                glm::mat3x3             _currentTransform = glm::mat3x3(1.F);
                std::list<Core::BBox2f> _clipRects;
                Core::BBox2f            _currentClipRect  = Core::BBox2f(0.F, 0.F, 0.F, 0.F);
                float                   _fillColor[4]     = { 1.F, 1.F, 1.F, 1.F };
                float                   _colorMult        = 1.F;
                float                   _alphaMult        = 1.F;
                float                   _finalColor[4]    = { 1.F, 1.F, 1.F, 1.F };
                float                   _lineWidth        = 1.0F;
                
                DJV_PRIVATE();
            };

        } // namespace Render
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::Render::ImageChannel);
    DJV_ENUM_SERIALIZE_HELPERS(AV::Render::ImageCache);
    DJV_ENUM_SERIALIZE_HELPERS(AV::Render::ImageFilter);

    picojson::value toJSON(AV::Render::ImageFilter);
    picojson::value toJSON(const AV::Render::ImageFilterOptions&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, AV::Render::ImageFilter&);
    void fromJSON(const picojson::value&, AV::Render::ImageFilterOptions&);

} // namespace djv

#include <djvAV/Render2DInline.h>
