// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ImageViewPrivate.h>

#include <djvUI/DrawUtil.h>
#include <djvUI/ImageWidget.h>
#include <djvUI/Style.h>

#include <djvAV/Render2D.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        void ImageViewGridOverlay::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            for (size_t i = 0; i < 26; ++i)
            {
                _letters.push_back('A' + i);
            }
            _fontSystem = context->getSystemT<AV::Font::System>();
        }

        ImageViewGridOverlay::ImageViewGridOverlay()
        {}

        std::shared_ptr<ImageViewGridOverlay> ImageViewGridOverlay::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ImageViewGridOverlay>(new ImageViewGridOverlay);
            out->_init(context);
            return out;
        }

        void ImageViewGridOverlay::setOptions(const GridOptions& value)
        {
            _options = value;
            _textUpdate();
            _redraw();
        }

        void ImageViewGridOverlay::setImagePosAndZoom(const glm::vec2& pos, float zoom)
        {
            if (pos == _imagePos && zoom == _imageZoom)
                return;
            _imagePos = pos;
            _imageZoom = zoom;
            _textUpdate();
            _redraw();
        }

        void ImageViewGridOverlay::setImageRotate(UI::ImageRotate value)
        {
            if (value == _imageRotate)
                return;
            _imageRotate = value;
            _textUpdate();
            _redraw();
        }

        void ImageViewGridOverlay::setImageAspectRatio(
            UI::ImageAspectRatio imageAspectRatio,
            float aspectRatio,
            float pixelAspectRatio)
        {
            if (imageAspectRatio == _imageAspectRatio &&
                aspectRatio == _aspectRatio &&
                pixelAspectRatio == _pixelAspectRatio)
                return;
            _imageAspectRatio = imageAspectRatio;
            _aspectRatio = aspectRatio;
            _pixelAspectRatio = pixelAspectRatio;
            _textUpdate();
            _redraw();
        }

        void ImageViewGridOverlay::setImageBBox(const BBox2f& value)
        {
            if (value == _imageBBox)
                return;
            _imageBBox = value;
            _textUpdate();
            _redraw();
        }

        void ImageViewGridOverlay::setImageFrame(const BBox2f& value)
        {
            if (value == _imageFrame)
                return;
            _imageFrame = value;
            _textUpdate();
            _redraw();
        }

        void ImageViewGridOverlay::_layoutEvent(Event::Layout&)
        {
            const auto& style = _getStyle();
            const BBox2f& g = getMargin().bbox(getGeometry(), style);
            const glm::vec2& size = g.getSize();
            if (size != _widgetSize)
            {
                _widgetSize = size;
                _textUpdate();
                _redraw();
            }
        }

        void ImageViewGridOverlay::_paintEvent(Event::Paint&)
        {
            const float gridCellSizeZoom = _options.size * _imageZoom;
            if (_options.enabled && gridCellSizeZoom > 2.f)
            {
                const auto& style = _getStyle();
                const float s = style->getMetric(UI::MetricsRole::SpacingSmall);
                const float b = style->getMetric(UI::MetricsRole::Border);
                const BBox2f& g = getMargin().bbox(getGeometry(), style);

                auto render = _getRender();

                /*const BBox2f bbox = BBox2f(
                    _imageBBox.min.x * _imageZoom + _imagePos.x,
                    _imageBBox.min.y * _imageZoom + _imagePos.y,
                    _imageBBox.w() * _imageZoom,
                    _imageBBox.h() * _imageZoom);
                render->setFillColor(AV::Image::Color(1.F, 0.F, 0.F));
                UI::drawBorder(render, bbox, b);*/

                const BBox2f viewportWorld = _getViewportWorld();
                const BBox2f viewport = _getViewport();
                //render->setFillColor(AV::Image::Color(1.F, 1.F, 0.F));
                //UI::drawBorder(render, viewport, b);

                const float opacity = Math::clamp((gridCellSizeZoom - 2.F) / 10.F, 0.F, 1.F);
                auto gridColor = _options.color.convert(AV::Image::Type::RGBA_F32);
                gridColor.setF32(gridColor.getF32(3) * opacity, 3);
                render->setFillColor(gridColor);
                for (int x = viewportWorld.min.x / _options.size; x <= viewportWorld.max.x / _options.size; ++x)
                {
                    render->drawRect(BBox2f(
                        floorf(g.min.x + x * _options.size * _imageZoom + _imagePos.x),
                        floorf(g.min.y + viewport.min.y),
                        b,
                        ceilf(viewport.h())));
                }
                for (int y = viewportWorld.min.y / _options.size; y <= viewportWorld.max.y / _options.size; ++y)
                {
                    render->drawRect(BBox2f(
                        floorf(g.min.x + viewport.min.x),
                        floorf(g.min.y + y * _options.size * _imageZoom + _imagePos.y),
                        viewport.w(),
                        ceilf(b)));
                }

                if (_text.size() > 0 && (_textWidthMax + b * 2.F) < gridCellSizeZoom)
                {
                    render->setFillColor(_options.color);
                    for (const auto& i : _text)
                    {
                        switch (i.first.first)
                        {
                        case ImageViewGrid::Column:
                            render->drawRect(BBox2f(
                                floorf(g.min.x + (i.first.second * _options.size + _options.size / 2.F) * _imageZoom + _imagePos.x - i.second.size.x / 2.F),
                                floorf(std::max(_imageFrame.min.y, g.min.y + viewport.min.y)),
                                ceilf(i.second.size.x + b * 2.F),
                                ceilf(i.second.size.y + b * 2.F)));
                            break;
                        case ImageViewGrid::Row:
                            render->drawRect(BBox2f(
                                floorf(std::max(_imageFrame.min.x, g.min.x + viewport.min.x)),
                                floorf(g.min.y + (i.first.second * _options.size + _options.size / 2.F) * _imageZoom + _imagePos.y - i.second.size.y / 2.F),
                                ceilf(i.second.size.x + b * 2.F),
                                ceilf(i.second.size.y + b * 2.F)));
                            break;
                        default: break;
                        }
                    }

                    render->setFillColor(_options.labelsColor);
                    for (const auto& i : _text)
                    {
                        switch (i.first.first)
                        {
                        case ImageViewGrid::Column:
                            render->drawText(
                                i.second.glyphs,
                                glm::vec2(
                                    floorf(g.min.x + b + (i.first.second * _options.size + _options.size / 2.F) * _imageZoom + _imagePos.x - i.second.size.x / 2.F),
                                    floorf(std::max(_imageFrame.min.y, g.min.y + viewport.min.y) + b + _fontMetrics.ascender - 1.F)));
                            break;
                        case ImageViewGrid::Row:
                            render->drawText(
                                i.second.glyphs,
                                glm::vec2(
                                    floorf(std::max(_imageFrame.min.x, g.min.x + viewport.min.x) + b),
                                    floorf(g.min.y + b + (i.first.second * _options.size + _options.size / 2.F) * _imageZoom + _imagePos.y - i.second.size.y / 2.F + _fontMetrics.ascender - 1.F)));
                            break;
                        default: break;
                        }
                    }
                }
            }
        }

        BBox2f ImageViewGridOverlay::_getViewportWorld() const
        {
            const auto& style = _getStyle();
            const BBox2f& g = getMargin().bbox(getGeometry(), style);
            return BBox2f(
                -_imagePos.x / _imageZoom,
                -_imagePos.y / _imageZoom,
                (g.w() - 1.F) / _imageZoom,
                (g.h() - 1.F) / _imageZoom).intersect(_imageBBox);
        }

        BBox2f ImageViewGridOverlay::_getViewport() const
        {
            const BBox2f viewportWorld = _getViewportWorld();
            return BBox2f(
                viewportWorld.min.x * _imageZoom + _imagePos.x,
                viewportWorld.min.y * _imageZoom + _imagePos.y,
                viewportWorld.w() * _imageZoom,
                viewportWorld.h() * _imageZoom);
        }

        void ImageViewGridOverlay::_initEvent(Event::Init& event)
        {
            Widget::_initEvent(event);
            const auto& style = _getStyle();
            const auto fontInfo = style->getFontInfo(AV::Font::familyMono, AV::Font::faceDefault, UI::MetricsRole::FontSmall);
            _fontMetricsFuture = _fontSystem->getMetrics(fontInfo);
        }

        void ImageViewGridOverlay::_updateEvent(Event::Update& event)
        {
            Widget::_updateEvent(event);
            if (_fontMetricsFuture.valid() &&
                _fontMetricsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                try
                {
                    _fontMetrics = _fontMetricsFuture.get();
                    _textUpdate();
                    _redraw();
                }
                catch (const std::exception & e)
                {
                    _log(e.what(), LogLevel::Error);
                }
            }
            auto textSizeFuturesIt = _textSizeFutures.begin();
            while (textSizeFuturesIt != _textSizeFutures.end())
            {
                if (textSizeFuturesIt->second.valid() &&
                    textSizeFuturesIt->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    try
                    {
                        const glm::vec2 size = textSizeFuturesIt->second.get();
                        _text[textSizeFuturesIt->first].size = size;
                        _textWidthMax = std::max(_textWidthMax, size.x);
                        _redraw();
                    }
                    catch (const std::exception & e)
                    {
                        _log(e.what(), LogLevel::Error);
                    }
                    textSizeFuturesIt = _textSizeFutures.erase(textSizeFuturesIt);
                }
                else
                {
                    ++textSizeFuturesIt;
                }
            }
            auto textGlyphsFuturesIt = _textGlyphsFutures.begin();
            while (textGlyphsFuturesIt != _textGlyphsFutures.end())
            {
                if (textGlyphsFuturesIt->second.valid() &&
                    textGlyphsFuturesIt->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    try
                    {
                        _text[textGlyphsFuturesIt->first].glyphs = textGlyphsFuturesIt->second.get();
                        _redraw();
                    }
                    catch (const std::exception & e)
                    {
                        _log(e.what(), LogLevel::Error);
                    }
                    textGlyphsFuturesIt = _textGlyphsFutures.erase(textGlyphsFuturesIt);
                }
                else
                {
                    ++textGlyphsFuturesIt;
                }
            }
        }

        std::string ImageViewGridOverlay::_getLabel(const ImageViewGridPos& value) const
        {
            std::string out;
            switch (_options.labels)
            {
            case ImageViewGridLabels::X_Y:
            {
                std::stringstream ss;
                ss << value.second;
                out = ss.str();
                break;
            }
            case ImageViewGridLabels::A_Y:
            {
                std::stringstream ss;
                switch (value.first)
                {
                case ImageViewGrid::Column:
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
                case ImageViewGrid::Row:
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

        void ImageViewGridOverlay::_textCreate(const ImageViewGridPos& pos)
        {
            auto& text = _text[pos];
            const std::string label = _getLabel(pos);
            text.text = label;
            const auto& style = _getStyle();
            const auto fontInfo = style->getFontInfo(AV::Font::familyMono, AV::Font::faceDefault, UI::MetricsRole::FontSmall);
            _textSizeFutures[pos] = _fontSystem->measure(label, fontInfo);
            _textGlyphsFutures[pos] = _fontSystem->getGlyphs(label, fontInfo);
        }

        void ImageViewGridOverlay::_textUpdate()
        {
            _textSizeFutures.clear();
            _textGlyphsFutures.clear();
            if (_options.enabled &&
                _options.labels != ImageViewGridLabels::None &&
                _fontMetrics.ascender > 0 &&
                _options.size * _imageZoom > _fontMetrics.ascender * 2.F)
            {
                const BBox2f viewportWorld = _getViewportWorld();
                const BBox2f viewport = _getViewport();
                std::set<ImageViewGridPos> erase;
                for (const auto& i : _text)
                {
                    erase.insert(i.first);
                }
                for (int x = viewportWorld.min.x / _options.size; x <= viewportWorld.max.x / _options.size; ++x)
                {
                    const auto pos = std::make_pair(ImageViewGrid::Column, x);
                    _textCreate(pos);
                    const auto i = erase.find(pos);
                    if (i != erase.end())
                    {
                        erase.erase(i);
                    }
                }
                for (int y = viewportWorld.min.y / _options.size; y <= viewportWorld.max.y / _options.size; ++y)
                {
                    const auto pos = std::make_pair(ImageViewGrid::Row, y);
                    _textCreate(pos);
                    const auto i = erase.find(pos);
                    if (i != erase.end())
                    {
                        erase.erase(i);
                    }
                }
                for (const auto& i : erase)
                {
                    const auto j = _text.find(i);
                    if (j != _text.end())
                    {
                        _text.erase(j);
                    }
                }
            }
            else
            {
                _text.clear();
            }
        }

    } // namespace ViewApp
} // namespace djv

