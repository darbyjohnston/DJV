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

#include <djvViewApp/ImageView.h>

namespace djv
{
    namespace ViewApp
    {
        enum class ImageViewGrid
        {
            Column,
            Row
        };

        typedef std::pair<ImageViewGrid, int> ImageViewGridPos;

        class ImageViewGridOverlay : public UI::Widget
        {
            DJV_NON_COPYABLE(ImageViewGridOverlay);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ImageViewGridOverlay();

        public:
            static std::shared_ptr<ImageViewGridOverlay> create(const std::shared_ptr<Core::Context>&);

            void setOptions(const GridOptions&);

            void setImagePosAndZoom(const glm::vec2&, float);
            void setImageRotate(UI::ImageRotate);
            void setImageAspectRatio(UI::ImageAspectRatio, float aspectRatio, float pixelAspectRatio);
            void setImageBBox(const Core::BBox2f&);
            void setImageFrame(const Core::BBox2f&);

        protected:
            void _layoutEvent(Core::Event::Layout&) override;
            void _paintEvent(Core::Event::Paint&) override;

            void _initEvent(Core::Event::Init&) override;
            void _updateEvent(Core::Event::Update&) override;

        private:
            Core::BBox2f _getViewportWorld() const;
            Core::BBox2f _getViewport() const;
            std::string _getLabel(const ImageViewGridPos&) const;
            void _textCreate(const ImageViewGridPos&);
            void _textUpdate();

            GridOptions _options;
            glm::vec2 _imagePos = glm::vec2(0.F, 0.F);
            float _imageZoom = 1.F;
            UI::ImageRotate _imageRotate = UI::ImageRotate::First;
            UI::ImageAspectRatio _imageAspectRatio = UI::ImageAspectRatio::First;
            float _aspectRatio = 1.F;
            float _pixelAspectRatio = 1.F;
            Core::BBox2f _imageBBox = Core::BBox2f(0.F, 0.F, 0.F, 0.F);
            Core::BBox2f _imageFrame = Core::BBox2f(0.F, 0.F, 0.F, 0.F);
            glm::vec2 _widgetSize = glm::vec2(0.F, 0.F);
            std::vector<char> _letters;
            std::shared_ptr<AV::Font::System> _fontSystem;
            AV::Font::Metrics _fontMetrics;
            std::future<AV::Font::Metrics> _fontMetricsFuture;
            struct Text
            {
                std::string text;
                glm::vec2 size = glm::vec2(0.F, 0.F);
                std::vector<std::shared_ptr<AV::Font::Glyph> > glyphs;
            };
            std::map<ImageViewGridPos, Text> _text;
            std::map<ImageViewGridPos, std::future<glm::vec2> > _textSizeFutures;
            std::map<ImageViewGridPos, std::future<std::vector<std::shared_ptr<AV::Font::Glyph> > > > _textGlyphsFutures;
            float _textWidthMax = 0.F;
        };

    } // namespace ViewApp
} // namespace djv

