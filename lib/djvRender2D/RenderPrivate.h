// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvRender2D/Data.h>
#include <djvRender2D/Enum.h>

#include <djvGL/Shader.h>

#include <djvImage/Info.h>

#include <djvMath/BBox.h>

namespace djv
{
    namespace Render2D
    {
        //! \todo Should this be configurable?
        const uint8_t  textureAtlasCount      = 4;
        const uint16_t textureAtlasSize       = 8192;
        const size_t   dynamicTextureCount    = 16;
        const size_t   dynamicTextureCacheMax = 16;
#if !defined(DJV_GL_ES2)
        const size_t   lut3DSize              = 32;
        const size_t   colorSpaceCacheMax     = 32;
#endif // DJV_GL_ES2

        // This enumeration provides how the color is used to draw the render primitive.
        enum class ColorMode
        {
            SolidColor,             // Use the uniform variable "color"
            ColorWithTextureAlpha,  // Use the uniform variable "color" with the alpha multiplied
                                    // by the red channel from the texture (e.g., used for
                                    // drawing text)
            ColorWithTextureAlphaR, // Used for drawing text with LCD sub-sampling
            ColorWithTextureAlphaG,
            ColorWithTextureAlphaB,
            ColorAndTexture,        // Use the uniform variable "color" multiplied by the texture     
            Shadow                  // Use the uniform variable "color" multiplied by the "U" texture coordinate
        };

        //! This struct provides data used to draw the render primitive.
        struct PrimitiveData
        {
            // Used as an offset to find textures.
            uint8_t textureAtlasCount       = 0;

            // Shader uniform variable locations.
            GLint imageChannelsLoc          = 0;
            GLint colorModeLoc              = 0;
            GLint colorLoc                  = 0;
#if !defined(DJV_GL_ES2)
            GLint colorSpaceLoc             = 0;
            GLint colorSpaceSamplerLoc      = 0;
#endif // DJV_GL_ES2
            GLint colorMatrixLoc            = 0;
            GLint colorMatrixEnabledLoc     = 0;
            GLint colorInvertLoc            = 0;
            GLint levelsInLowLoc            = 0;
            GLint levelsInHighLoc           = 0;
            GLint levelsGammaLoc            = 0;
            GLint levelsOutLowLoc           = 0;
            GLint levelsOutHighLoc          = 0;
            GLint levelsEnabledLoc          = 0;
            GLint exposureVLoc              = 0;
            GLint exposureDLoc              = 0;
            GLint exposureKLoc              = 0;
            GLint exposureFLoc              = 0;
            GLint exposureEnabledLoc        = 0;
            GLint softClipLoc               = 0;
            GLint imageChannelsDisplayLoc   = 0;
            GLint textureSamplerLoc         = 0;
        };

        //! This class provides the base functionality for render primitives.
        class Primitive
        {
        public:
            virtual ~Primitive();

            Math::BBox2f clipRect;
            float        color[4]           = { 0.F, 0.F, 0.F, 0.F };
            GLenum       type               = GL_TRIANGLES;
            size_t       vaoOffset          = 0;
            size_t       vaoSize            = 0;
            AlphaBlend   alphaBlend         = AlphaBlend::Straight;
            bool         textLCDRendering   = false;

            virtual void bind(const PrimitiveData&, const std::shared_ptr<GL::Shader>&);
        };

        //! This class provides a text render primitive.
        class TextPrimitive : public Primitive
        {
        public:
            uint8_t atlasIndex = 0;

            void bind(const PrimitiveData&, const std::shared_ptr<GL::Shader>&) override;
        };

        //! This class provides an image render primitive.
        class ImagePrimitive : public Primitive
        {
        public:
            ColorMode            colorMode            = ColorMode::ColorAndTexture;
            Image::Channels      imageChannels        = Image::Channels::RGBA;
#if !defined(DJV_GL_ES2)
            uint8_t              colorSpace           = 0;
            GLuint               colorSpaceTextureID  = 0;
#endif // DJV_GL_ES2
            glm::mat4x4          colorMatrix;
            bool                 colorMatrixEnabled   = false;
            bool                 colorInvert          = false;
            ImageLevels          levels;
            bool                 levelsEnabled        = false;
            float                exposureV            = 0.F;
            float                exposureD            = 0.F;
            float                exposureK            = 0.F;
            float                exposureF            = 0.F;
            bool                 exposureEnabled      = false;
            float                softClip             = 0.F;
            ImageChannelsDisplay imageChannelsDisplay = ImageChannelsDisplay::Color;
            ImageCache           imageCache           = ImageCache::Atlas;
            uint8_t              atlasIndex           = 0;
            GLuint               textureID            = 0;

            void bind(const PrimitiveData&, const std::shared_ptr<GL::Shader>&) override;
        };

        //! This class provides a shadow render primitive.
        class ShadowPrimitive : public Primitive
        {
        public:
            void bind(const PrimitiveData&, const std::shared_ptr<GL::Shader>&) override;
        };

        //! This class provides a texture render primitive.
        class TexturePrimitive : public Primitive
        {
        public:
            GLuint textureID    = 0;
            GLenum target       = GL_TEXTURE_2D;

            void bind(const PrimitiveData&, const std::shared_ptr<GL::Shader>&) override;
        };

        //! This struct provides the layout for a VBO vertex.
        struct VBOVertex
        {
            float    vx;
            float    vy;
            uint16_t tx;
            uint16_t ty;
        };

#if !defined(DJV_GL_ES2)

        //! This class provides a 3D lookup table for color space conversions.
        class LUT3D
        {
            DJV_NON_COPYABLE(LUT3D);

        public:
            explicit LUT3D(size_t edgeLen = lut3DSize);
            ~LUT3D();

            size_t getEdgeLen() const;
            size_t getSize() const;
            float* getData();
            const float* getData() const;
            GLuint getID() const;

            void copy();
            void bind();

        private:
            size_t _edgeLen = 0;
            size_t _size = 0;
            float* _data = nullptr;
            GLuint _id = 0;
        };

        //! This struct provides data for color space conversions.
        struct ColorSpaceData
        {
            size_t                  id;
            std::string             shaderSource;
            std::shared_ptr<LUT3D>  lut3D;
        };

#endif // DJV_GL_ES2

        // Utility function to flip the y-coordinate.
        Math::BBox2f flip(const Math::BBox2f&, const Image::Size&);

        float knee(float x, float f);
        float knee2(float x, float y);

        glm::mat4x4 brightnessMatrix(float r, float g, float b);
        glm::mat4x4 contrastMatrix(float r, float g, float b);
        glm::mat4x4 saturationMatrix(float r, float g, float b);
        glm::mat4x4 colorMatrix(const ImageColor&);

    } // namespace Render2D
} // namespace djv

