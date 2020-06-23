// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/TextBlock.h>

#include <djvUI/Style.h>

#include <djvAV/FontSystem.h>
#include <djvAV/Render2D.h>

#include <djvCore/Cache.h>
#include <djvCore/Context.h>
#include <djvCore/Math.h>
#include <djvCore/Memory.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct TextBlock::Private
        {
            std::shared_ptr<AV::Font::System> fontSystem;

            std::string text;
            TextHAlign textHAlign = TextHAlign::Left;
            ColorRole textColorRole = ColorRole::Foreground;
            MetricsRole textSizeRole = MetricsRole::TextColumn;

            std::string fontFace;
            std::string fontFamily;
            MetricsRole fontSizeRole = MetricsRole::FontMedium;
            AV::Font::FontInfo fontInfo;
            AV::Font::Metrics fontMetrics;
            std::future<AV::Font::Metrics> fontMetricsFuture;

            typedef std::pair<AV::Font::FontInfo, float> TextCacheKey;
            typedef std::pair<std::vector<AV::Font::TextLine>, glm::vec2> TextCacheValue;
            Memory::Cache<TextCacheKey, TextCacheValue> textCache;

            BBox2f clipRect;

            TextCacheValue textLines(float);
        };

        void TextBlock::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();
            setClassName("djv::UI::TextBlock");
            p.fontSystem = context->getSystemT<AV::Font::System>();
            p.textCache.setMax(5);
        }
        
        TextBlock::TextBlock() :
            _p(new Private)
        {}

        TextBlock::~TextBlock()
        {}

        std::shared_ptr<TextBlock> TextBlock::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<TextBlock>(new TextBlock);
            out->_init(context);
            return out;
        }

        const std::string& TextBlock::getText() const
        {
            return _p->text;
        }

        void TextBlock::setText(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.text)
                return;
            p.text = value;
            _textUpdate();
        }

        void TextBlock::appendText(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            p.text.append(value);
            _textUpdate();
        }

        void TextBlock::clearText()
        {
            setText(std::string());
        }

        TextHAlign TextBlock::getTextHAlign() const
        {
            return _p->textHAlign;
        }
        
        void TextBlock::setTextHAlign(TextHAlign value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.textHAlign)
                return;
            p.textHAlign = value;
            _resize();
        }

        ColorRole TextBlock::getTextColorRole() const
        {
            return _p->textColorRole;
        }

        void TextBlock::setTextColorRole(ColorRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.textColorRole)
                return;
            p.textColorRole = value;
            _redraw();
        }

        MetricsRole TextBlock::getTextSizeRole() const
        {
            return _p->textSizeRole;
        }

        void TextBlock::setTextSizeRole(MetricsRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.textSizeRole)
                return;
            p.textSizeRole = value;
            _textUpdate();
        }

        const std::string& TextBlock::getFontFamily() const
        {
            return _p->fontFace;
        }

        const std::string& TextBlock::getFontFace() const
        {
            return _p->fontFace;
        }

        MetricsRole TextBlock::getFontSizeRole() const
        {
            return _p->fontSizeRole;
        }

        void TextBlock::setFontFamily(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.fontFamily)
                return;
            p.fontFamily = value;
            _textUpdate();
        }

        void TextBlock::setFontFace(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.fontFace)
                return;
            p.fontFace = value;
            _textUpdate();
        }

        void TextBlock::setFontSizeRole(MetricsRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.fontSizeRole)
                return;
            p.fontSizeRole = value;
            _textUpdate();
        }

        float TextBlock::getHeightForWidth(float value) const
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float w = value - getMargin().getWidth(style);
            const glm::vec2 textSize = p.textLines(w).second;
            return textSize.y + getMargin().getHeight(style);
        }

        void TextBlock::_preLayoutEvent(Event::PreLayout& event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float w = style->getMetric(p.textSizeRole);
            const glm::vec2 textSize = p.textLines(w).second;
            glm::vec2 size = textSize;
            size.x = std::max(size.x, w);
            _setMinimumSize(size + getMargin().getSize(style));
        }

        void TextBlock::_layoutEvent(Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const BBox2f& g = getMargin().bbox(getGeometry(), style);
            const float w = g.w();
            p.textLines(w);
        }

        void TextBlock::_clipEvent(Event::Clip& event)
        {
            _p->clipRect = event.getClipRect();
        }

        void TextBlock::_paintEvent(Event::Paint& event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const BBox2f& g = getMargin().bbox(getGeometry(), style);
            const auto key = std::make_pair(p.fontInfo, g.w());
            Private::TextCacheValue cacheValue;
            if (p.textCache.get(key, cacheValue))
            {
                const glm::vec2 c = g.getCenter();
                glm::vec2 pos = g.min;
                
                const auto& render = _getRender();
                //render->setFillColor(AV::Image::Color(1.F, 0.F, 0.F, .5F));
                //render->drawRect(g);

                render->setFillColor(style->getColor(p.textColorRole));
                for (const auto& line : cacheValue.first)
                {
                    if (pos.y + line.size.y >= p.clipRect.min.y && pos.y <= p.clipRect.max.y)
                    {
                        switch (p.textHAlign)
                        {
                        case TextHAlign::Center:
                            pos.x = ceilf(c.x - line.size.x / 2.F);
                            break;
                        case TextHAlign::Right:
                            pos.x = g.max.x - line.size.x;
                            break;
                        default: break;
                        }

                        render->setFillColor(style->getColor(p.textColorRole));
                        //! \bug Why the extra subtract by one here?
                        render->drawText(
                            line.glyphs,
                            glm::vec2(floorf(pos.x), floorf(pos.y + p.fontMetrics.ascender - 1.F)));
                    }
                    pos.y += line.size.y;
                }
            }
        }

        void TextBlock::_initEvent(Event::Init& event)
        {
            if (event.getData().resize || event.getData().font)
            {
                _textUpdate();
            }
        }

        void TextBlock::_updateEvent(Event::Update& event)
        {
            DJV_PRIVATE_PTR();
            const bool fontMetricsFutureValid = p.fontMetricsFuture.valid();
            if (fontMetricsFutureValid &&
                p.fontMetricsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                try
                {
                    p.fontMetrics = p.fontMetricsFuture.get();
                    _resize();
                }
                catch (const std::exception& e)
                {
                    _log(e.what(), LogLevel::Error);
                }
            }
        }

        void TextBlock::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            p.fontInfo = p.fontFamily.empty() ?
                style->getFontInfo(p.fontFace, p.fontSizeRole) :
                style->getFontInfo(p.fontFamily, p.fontFace, p.fontSizeRole);
            p.fontMetricsFuture = p.fontSystem->getMetrics(p.fontInfo);
            p.fontSystem->cacheGlyphs(p.text, p.fontInfo);
            p.textCache.clear();
            _resize();
        }

        TextBlock::Private::TextCacheValue TextBlock::Private::textLines(float value)
        {
            Private::TextCacheValue out;
            const auto key = std::make_pair(fontInfo, value);
            if (!textCache.get(key, out))
            {
                auto textLines = fontSystem->textLines(text, value, fontInfo).get();
                glm::vec2 textSize = glm::vec2(0.F, 0.F);
                for (const auto& i : textLines)
                {
                    textSize.x = std::max(textSize.x, i.size.x);
                    textSize.y += i.size.y;
                }
                out.first = textLines;
                out.second = textSize;
                textCache.add(key, out);
            }
            return out;
        }

    } // namespace UI
} // namespace djv
