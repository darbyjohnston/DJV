// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/Enum.h>
#include <djvAV/FontSystem.h>
#include <djvAV/Image.h>
#include <djvAV/OCIO.h>

#include <djvCore/BBox.h>
#include <djvCore/ISystem.h>
#include <djvCore/RapidJSON.h>
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
        namespace Render2D
        {
            //! This enumeration provides which image channels are displayed.
            enum class ImageChannelDisplay
            {
                Color,
                Red,
                Green,
                Blue,
                Alpha,

                Count,
                First = Color
            };
            DJV_ENUM_HELPERS(ImageChannelDisplay);

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
                
                ImageChannelDisplay channelDisplay    = ImageChannelDisplay::Color;
                AlphaBlend          alphaBlend        = AlphaBlend::Straight;
                AV::Image::Mirror   mirror;
                AV::OCIO::Convert   colorSpace;
                ImageColor          color;
                bool                colorEnabled      = false;
                ImageLevels         levels;
                bool                levelsEnabled     = false;
                ImageExposure       exposure;
                bool                exposureEnabled   = false;
                float               softClip          = 0.F;
                bool                softClipEnabled   = false;
                ImageCache          cache             = ImageCache::Atlas;

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
                explicit ImageFilterOptions(ImageFilter minMag);
                ImageFilterOptions(ImageFilter min, ImageFilter mag);

                ImageFilter min = ImageFilter::Linear;
                ImageFilter mag = ImageFilter::Nearest;

                bool operator == (const ImageFilterOptions&) const;
                bool operator != (const ImageFilterOptions&) const;
            };

            //! This class provides a 2D render system.
            class Render : public Core::ISystem
            {
                DJV_NON_COPYABLE(Render);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                Render();

            public:
                ~Render();

                static std::shared_ptr<Render> create(const std::shared_ptr<Core::Context>&);

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

                void setTextLCDRendering(bool);

                void drawText(const std::vector<std::shared_ptr<Font::Glyph> >& glyphs, const glm::vec2& position);

                ///@}

                //! \name Shadows
                ///@{

                void drawShadow(const Core::BBox2f&, Side);
                void drawShadow(const Core::BBox2f&, float radius, size_t facets = 16);

                ///@}

                //! \name Textures
                ///@{

                void drawTexture(const Core::BBox2f&, GLuint, GLenum = GL_TEXTURE_2D);

                ///@}

                //! \name Diagnostics
                ///@{

                float getTextureAtlasPercentage() const;
                size_t getDynamicTextureCount() const;
                size_t getVBOSize() const;

                ///@}

            private:
                const glm::mat3x3& _getCurrentTransform() const;
                void _updateCurrentClipRect();
                void _updateCurrentTransform();
                void _updateImageFilter();

                Image::Size             _size;
                std::list<glm::mat3x3>  _transforms;
                const glm::mat3x3       _identity         = glm::mat3x3(1.F);
                std::list<Core::BBox2f> _clipRects;
                Core::BBox2f            _currentClipRect  = Core::BBox2f(0.F, 0.F, 0.F, 0.F);
                float                   _fillColor[4]     = { 1.F, 1.F, 1.F, 1.F };
                float                   _colorMult        = 1.F;
                float                   _alphaMult        = 1.F;
                float                   _finalColor[4]    = { 1.F, 1.F, 1.F, 1.F };
                float                   _lineWidth        = 1.0F;
                
                DJV_PRIVATE();
            };

        } // namespace Render2D
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::Render2D::ImageChannelDisplay);
    DJV_ENUM_SERIALIZE_HELPERS(AV::Render2D::ImageCache);
    DJV_ENUM_SERIALIZE_HELPERS(AV::Render2D::ImageFilter);

    rapidjson::Value toJSON(AV::Render2D::ImageFilter, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const AV::Render2D::ImageFilterOptions&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::Render2D::ImageFilter&);
    void fromJSON(const rapidjson::Value&, AV::Render2D::ImageFilterOptions&);

} // namespace djv

#include <djvAV/Render2DInline.h>
