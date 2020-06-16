// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/Enum.h>
#include <djvAV/FontSystem.h>
#include <djvAV/Image.h>
#include <djvAV/Render2DData.h>

#include <djvCore/BBox.h>
#include <djvCore/ISystem.h>
#include <djvCore/RapidJSON.h>

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

                void pushClipRect(const Core::BBox2f&);
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
                void drawCircle(const glm::vec2& pos, float radius, size_t facets = 64);

                ///@}

                //! \name Images
                ///@{

                const ImageFilterOptions& getImageFilterOptions() const;

                //! This function should only be called outside of beginFrame()/endFrame().
                void setImageFilterOptions(const ImageFilterOptions&);

                void drawImage(
                    const std::shared_ptr<Image::Image>&,
                    const glm::vec2& pos,
                    const ImageOptions& = ImageOptions());

                void drawFilledImage(
                    const std::shared_ptr<Image::Image>&,
                    const glm::vec2& pos,
                    const ImageOptions& = ImageOptions());

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
} // namespace djv

#include <djvAV/Render2DInline.h>
