// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ViewWidgetPrivate.h>

#include <djvUI/DrawUtil.h>
#include <djvUI/ImageWidget.h>
#include <djvUI/Style.h>

#include <djvRender2D/FontSystem.h>
#include <djvRender2D/Render.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct GridOverlay::Private
        {
            GridOptions options;
            glm::vec2 imagePos = glm::vec2(0.F, 0.F);
            float imageZoom = 1.F;
            UI::ImageRotate imageRotate = UI::ImageRotate::First;
            UI::ImageAspectRatio imageAspectRatio = UI::ImageAspectRatio::First;
            float aspectRatio = 1.F;
            float pixelAspectRatio = 1.F;
            Math::BBox2f imageBBox = Math::BBox2f(0.F, 0.F, 0.F, 0.F);
            glm::vec2 widgetSize = glm::vec2(0.F, 0.F);
            std::vector<char> letters;
            std::shared_ptr<Render2D::Font::FontSystem> fontSystem;
            Render2D::Font::Metrics fontMetrics;
            std::future<Render2D::Font::Metrics> fontMetricsFuture;
            struct Text
            {
                std::string text;
                glm::vec2 size = glm::vec2(0.F, 0.F);
                std::vector<std::shared_ptr<Render2D::Font::Glyph> > glyphs;
            };
            std::map<GridPos, Text> text;
            std::map<GridPos, std::future<glm::vec2> > textSizeFutures;
            std::map<GridPos, std::future<std::vector<std::shared_ptr<Render2D::Font::Glyph> > > > textGlyphsFutures;
            float textWidthMax = 0.F;
        };

        void GridOverlay::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();
            for (size_t i = 0; i < 26; ++i)
            {
                p.letters.push_back('A' + i);
            }
            p.fontSystem = context->getSystemT<Render2D::Font::FontSystem>();
        }

        GridOverlay::GridOverlay() :
            _p(new Private)
        {}

        std::shared_ptr<GridOverlay> GridOverlay::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<GridOverlay>(new GridOverlay);
            out->_init(context);
            return out;
        }

        void GridOverlay::setOptions(const GridOptions& value)
        {
            _p->options = value;
            _textUpdate();
            _redraw();
        }

        void GridOverlay::setImagePosAndZoom(const glm::vec2& pos, float zoom)
        {
            DJV_PRIVATE_PTR();
            if (pos == p.imagePos && zoom == p.imageZoom)
                return;
            p.imagePos = pos;
            p.imageZoom = zoom;
            _textUpdate();
            _redraw();
        }

        void GridOverlay::setImageRotate(UI::ImageRotate value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.imageRotate)
                return;
            p.imageRotate = value;
            _textUpdate();
            _redraw();
        }

        void GridOverlay::setImageAspectRatio(
            UI::ImageAspectRatio imageAspectRatio,
            float aspectRatio,
            float pixelAspectRatio)
        {
            DJV_PRIVATE_PTR();
            if (imageAspectRatio == p.imageAspectRatio &&
                aspectRatio == p.aspectRatio &&
                pixelAspectRatio == p.pixelAspectRatio)
                return;
            p.imageAspectRatio = imageAspectRatio;
            p.aspectRatio = aspectRatio;
            p.pixelAspectRatio = pixelAspectRatio;
            _textUpdate();
            _redraw();
        }

        void GridOverlay::setImageBBox(const Math::BBox2f& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.imageBBox)
                return;
            p.imageBBox = value;
            _textUpdate();
            _redraw();
        }

        void GridOverlay::_layoutEvent(System::Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const Math::BBox2f& g = getMargin().bbox(getGeometry(), style);
            const glm::vec2& size = g.getSize();
            if (size != p.widgetSize)
            {
                p.widgetSize = size;
                _textUpdate();
                _redraw();
            }
        }

        void GridOverlay::_paintEvent(System::Event::Paint&)
        {
            DJV_PRIVATE_PTR();
            const float gridCellSizeZoom = p.options.size * p.imageZoom;
            if (p.options.enabled && gridCellSizeZoom > 2.f)
            {
                const auto& style = _getStyle();
                const float b = style->getMetric(UI::MetricsRole::Border);
                const Math::BBox2f& g = getMargin().bbox(getGeometry(), style);

                const auto& render = _getRender();

                /*const Math::BBox2f bbox = Math::BBox2f(
                    p.imageBBox.min.x * p.imageZoom + p.imagePos.x,
                    p.imageBBox.min.y * p.imageZoom + p.imagePos.y,
                    p.imageBBox.w() * p.imageZoom,
                    p.imageBBox.h() * p.imageZoom);
                render->setFillColor(Image::Color(1.F, 0.F, 0.F));
                UI::drawBorder(render, bbox, b);*/

                const Math::BBox2f viewportWorld = _getViewportWorld();
                const Math::BBox2f viewport = _getViewport();
                //render->setFillColor(Image::Color(1.F, 1.F, 0.F));
                //UI::drawBorder(render, viewport, b);

                std::vector<Math::BBox2f> rects;
                for (int x = viewportWorld.min.x / p.options.size; x <= viewportWorld.max.x / p.options.size; ++x)
                {
                    rects.emplace_back(Math::BBox2f(
                        floorf(g.min.x + x * p.options.size * p.imageZoom + p.imagePos.x),
                        floorf(g.min.y + viewport.min.y),
                        b,
                        ceilf(viewport.h())));
                }
                for (int y = viewportWorld.min.y / p.options.size; y <= viewportWorld.max.y / p.options.size; ++y)
                {
                    rects.emplace_back(Math::BBox2f(
                        floorf(g.min.x + viewport.min.x),
                        floorf(g.min.y + y * p.options.size * p.imageZoom + p.imagePos.y),
                        viewport.w(),
                        ceilf(b)));
                }
                const float opacity = Math::clamp((gridCellSizeZoom - 2.F) / 10.F, 0.F, 1.F);
                auto gridColor = p.options.color.convert(Image::Type::RGBA_F32);
                gridColor.setF32(gridColor.getF32(3) * opacity, 3);
                render->setFillColor(gridColor);
                render->drawRects(rects);

                if (p.text.size() > 0 && (p.textWidthMax + b * 2.F) < gridCellSizeZoom)
                {
                    rects.clear();
                    for (const auto& i : p.text)
                    {
                        switch (i.first.first)
                        {
                        case Grid::Column:
                            rects.emplace_back(Math::BBox2f(
                                floorf(g.min.x + (i.first.second * p.options.size + p.options.size / 2.F) * p.imageZoom + p.imagePos.x - i.second.size.x / 2.F),
                                floorf(g.min.y + viewport.min.y),
                                ceilf(i.second.size.x + b * 2.F),
                                ceilf(i.second.size.y + b * 2.F)));
                            break;
                        case Grid::Row:
                            rects.emplace_back(Math::BBox2f(
                                floorf(g.min.x + viewport.min.x),
                                floorf(g.min.y + (i.first.second * p.options.size + p.options.size / 2.F) * p.imageZoom + p.imagePos.y - i.second.size.y / 2.F),
                                ceilf(i.second.size.x + b * 2.F),
                                ceilf(i.second.size.y + b * 2.F)));
                            break;
                        default: break;
                        }
                    }
                    render->setFillColor(p.options.color);
                    render->drawRects(rects);

                    render->setFillColor(p.options.labelsColor);
                    for (const auto& i : p.text)
                    {
                        switch (i.first.first)
                        {
                        case Grid::Column:
                            render->drawText(
                                i.second.glyphs,
                                glm::vec2(
                                    floorf(g.min.x + b + (i.first.second * p.options.size + p.options.size / 2.F) * p.imageZoom + p.imagePos.x - i.second.size.x / 2.F),
                                    floorf(g.min.y + viewport.min.y + b + p.fontMetrics.ascender - 1.F)));
                            break;
                        case Grid::Row:
                            render->drawText(
                                i.second.glyphs,
                                glm::vec2(
                                    floorf(g.min.x + viewport.min.x + b),
                                    floorf(g.min.y + b + (i.first.second * p.options.size + p.options.size / 2.F) * p.imageZoom + p.imagePos.y - i.second.size.y / 2.F + p.fontMetrics.ascender - 1.F)));
                            break;
                        default: break;
                        }
                    }
                }
            }
        }

        Math::BBox2f GridOverlay::_getViewportWorld() const
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const Math::BBox2f& g = getMargin().bbox(getGeometry(), style);
            return Math::BBox2f(
                -p.imagePos.x / p.imageZoom,
                -p.imagePos.y / p.imageZoom,
                (g.w() - 1.F) / p.imageZoom,
                (g.h() - 1.F) / p.imageZoom).intersect(p.imageBBox);
        }

        Math::BBox2f GridOverlay::_getViewport() const
        {
            DJV_PRIVATE_PTR();
            const Math::BBox2f viewportWorld = _getViewportWorld();
            return Math::BBox2f(
                viewportWorld.min.x * p.imageZoom + p.imagePos.x,
                viewportWorld.min.y * p.imageZoom + p.imagePos.y,
                viewportWorld.w() * p.imageZoom,
                viewportWorld.h() * p.imageZoom);
        }

        void GridOverlay::_initEvent(System::Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().resize ||
                event.getData().font)
            {
                const auto& style = _getStyle();
                const auto fontInfo = style->getFontInfo(Render2D::Font::familyMono, Render2D::Font::faceDefault, UI::MetricsRole::FontSmall);
                p.fontMetricsFuture = p.fontSystem->getMetrics(fontInfo);
            }
        }

        void GridOverlay::_updateEvent(System::Event::Update& event)
        {
            DJV_PRIVATE_PTR();
            if (p.fontMetricsFuture.valid() &&
                p.fontMetricsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                try
                {
                    p.fontMetrics = p.fontMetricsFuture.get();
                    _textUpdate();
                    _redraw();
                }
                catch (const std::exception & e)
                {
                    _log(e.what(), System::LogLevel::Error);
                }
            }
            auto textSizeFuturesIt = p.textSizeFutures.begin();
            while (textSizeFuturesIt != p.textSizeFutures.end())
            {
                if (textSizeFuturesIt->second.valid() &&
                    textSizeFuturesIt->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    try
                    {
                        const glm::vec2 size = textSizeFuturesIt->second.get();
                        p.text[textSizeFuturesIt->first].size = size;
                        p.textWidthMax = std::max(p.textWidthMax, size.x);
                        _redraw();
                    }
                    catch (const std::exception & e)
                    {
                        _log(e.what(), System::LogLevel::Error);
                    }
                    textSizeFuturesIt = p.textSizeFutures.erase(textSizeFuturesIt);
                }
                else
                {
                    ++textSizeFuturesIt;
                }
            }
            auto textGlyphsFuturesIt = p.textGlyphsFutures.begin();
            while (textGlyphsFuturesIt != p.textGlyphsFutures.end())
            {
                if (textGlyphsFuturesIt->second.valid() &&
                    textGlyphsFuturesIt->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    try
                    {
                        p.text[textGlyphsFuturesIt->first].glyphs = textGlyphsFuturesIt->second.get();
                        _redraw();
                    }
                    catch (const std::exception & e)
                    {
                        _log(e.what(), System::LogLevel::Error);
                    }
                    textGlyphsFuturesIt = p.textGlyphsFutures.erase(textGlyphsFuturesIt);
                }
                else
                {
                    ++textGlyphsFuturesIt;
                }
            }
        }

        std::string GridOverlay::_getLabel(const GridPos& value) const
        {
            DJV_PRIVATE_PTR();
            std::string out;
            switch (p.options.labels)
            {
            case GridLabels::X_Y:
            {
                std::stringstream ss;
                ss << value.second;
                out = ss.str();
                break;
            }
            case GridLabels::A_Y:
            {
                std::stringstream ss;
                switch (value.first)
                {
                case Grid::Column:
                {
                    std::string tmp;
                    bool negative = false;
                    int v = value.second;
                    if (v < 0)
                    {
                        negative = true;
                        v = -v;
                    }
                    while (v >= 26)
                    {
                        tmp.insert(tmp.begin(), 'A' + (v % 26));
                        v /= 26;
                    }
                    tmp.insert(tmp.begin(), 'A' + (v % 26));
                    if (negative)
                    {
                        tmp.insert(tmp.begin(), '-');
                    }
                    ss << tmp;
                    break;
                }
                case Grid::Row:
                    ss << value.second;
                    break;
                default: break;
                }
                out = ss.str();
                break;
            }
            default: break;
            }
            return out;
        }

        void GridOverlay::_textCreate(const GridPos& pos)
        {
            DJV_PRIVATE_PTR();
            auto& text = p.text[pos];
            const std::string label = _getLabel(pos);
            text.text = label;
            const auto& style = _getStyle();
            const auto fontInfo = style->getFontInfo(Render2D::Font::familyMono, Render2D::Font::faceDefault, UI::MetricsRole::FontSmall);
            p.textSizeFutures[pos] = p.fontSystem->measure(label, fontInfo);
            p.textGlyphsFutures[pos] = p.fontSystem->getGlyphs(label, fontInfo);
        }

        void GridOverlay::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            p.textSizeFutures.clear();
            p.textGlyphsFutures.clear();
            if (p.options.enabled &&
                p.options.labels != GridLabels::None &&
                p.fontMetrics.ascender > 0 &&
                p.options.size * p.imageZoom > p.fontMetrics.ascender * 2.F)
            {
                const Math::BBox2f viewportWorld = _getViewportWorld();
                std::set<GridPos> erase;
                for (const auto& i : p.text)
                {
                    erase.insert(i.first);
                }
                for (int x = viewportWorld.min.x / p.options.size; x <= viewportWorld.max.x / p.options.size; ++x)
                {
                    const auto pos = std::make_pair(Grid::Column, x);
                    _textCreate(pos);
                    const auto i = erase.find(pos);
                    if (i != erase.end())
                    {
                        erase.erase(i);
                    }
                }
                for (int y = viewportWorld.min.y / p.options.size; y <= viewportWorld.max.y / p.options.size; ++y)
                {
                    const auto pos = std::make_pair(Grid::Row, y);
                    _textCreate(pos);
                    const auto i = erase.find(pos);
                    if (i != erase.end())
                    {
                        erase.erase(i);
                    }
                }
                for (const auto& i : erase)
                {
                    const auto j = p.text.find(i);
                    if (j != p.text.end())
                    {
                        p.text.erase(j);
                    }
                }
            }
            else
            {
                p.text.clear();
            }
        }

    } // namespace ViewApp
} // namespace djv

