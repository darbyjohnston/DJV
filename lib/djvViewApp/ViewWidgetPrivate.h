// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/View.h>
#include <djvViewApp/ViewWidget.h>

namespace djv
{
    namespace ViewApp
    {
        enum class Grid
        {
            Column,
            Row
        };

        typedef std::pair<Grid, int> GridPos;

        class GridOverlay : public UI::Widget
        {
            DJV_NON_COPYABLE(GridOverlay);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            GridOverlay();

        public:
            static std::shared_ptr<GridOverlay> create(const std::shared_ptr<Core::Context>&);

            void setOptions(const GridOptions&);

            void setImagePosAndZoom(const glm::vec2&, float);
            void setImageRotate(UI::ImageRotate);
            void setImageAspectRatio(UI::ImageAspectRatio, float aspectRatio, float pixelAspectRatio);
            void setImageBBox(const Core::BBox2f&);

        protected:
            void _layoutEvent(Core::Event::Layout&) override;
            void _paintEvent(Core::Event::Paint&) override;

            void _initEvent(Core::Event::Init&) override;
            void _updateEvent(Core::Event::Update&) override;

        private:
            Core::BBox2f _getViewportWorld() const;
            Core::BBox2f _getViewport() const;
            std::string _getLabel(const GridPos&) const;
            void _textCreate(const GridPos&);
            void _textUpdate();

            GridOptions _options;
            glm::vec2 _imagePos = glm::vec2(0.F, 0.F);
            float _imageZoom = 1.F;
            UI::ImageRotate _imageRotate = UI::ImageRotate::First;
            UI::ImageAspectRatio _imageAspectRatio = UI::ImageAspectRatio::First;
            float _aspectRatio = 1.F;
            float _pixelAspectRatio = 1.F;
            Core::BBox2f _imageBBox = Core::BBox2f(0.F, 0.F, 0.F, 0.F);
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
            std::map<GridPos, Text> _text;
            std::map<GridPos, std::future<glm::vec2> > _textSizeFutures;
            std::map<GridPos, std::future<std::vector<std::shared_ptr<AV::Font::Glyph> > > > _textGlyphsFutures;
            float _textWidthMax = 0.F;
        };

    } // namespace ViewApp
} // namespace djv

