// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/TextBlock.h>

#include <djvUI/Style.h>

#include <djvRender2D/FontSystem.h>
#include <djvRender2D/Render.h>

#include <djvSystem/Context.h>

#include <djvMath/Math.h>

#include <djvCore/Cache.h>
#include <djvCore/Memory.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Text
        {
            struct Block::Private
            {
                std::shared_ptr<Render2D::Font::FontSystem> fontSystem;

                std::string text;
                TextHAlign textHAlign = TextHAlign::Left;
                ColorRole textColorRole = ColorRole::Foreground;
                MetricsRole textSizeRole = MetricsRole::TextColumn;

                std::string fontFace;
                std::string fontFamily;
                MetricsRole fontSizeRole = MetricsRole::FontMedium;
                Render2D::Font::FontInfo fontInfo;
                Render2D::Font::Metrics fontMetrics;
                std::future<Render2D::Font::Metrics> fontMetricsFuture;

                bool wordWrap = true;

                typedef std::pair<Render2D::Font::FontInfo, float> TextCacheKey;
                typedef std::pair<std::vector<Render2D::Font::TextLine>, glm::vec2> TextCacheValue;
                Memory::Cache<TextCacheKey, TextCacheValue> textCache;

                Math::BBox2f clipRect;

                TextCacheValue textLines(float);
            };

            void Block::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();
                setClassName("djv::UI::Text::Block");
                p.fontSystem = context->getSystemT<Render2D::Font::FontSystem>();
                p.textCache.setMax(5);
            }

            Block::Block() :
                _p(new Private)
            {}

            Block::~Block()
            {}

            std::shared_ptr<Block> Block::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Block>(new Block);
                out->_init(context);
                return out;
            }

            const std::string& Block::getText() const
            {
                return _p->text;
            }

            void Block::setText(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.text)
                    return;
                p.text = value;
                _textUpdate();
            }

            void Block::appendText(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                p.text.append(value);
                _textUpdate();
            }

            void Block::clearText()
            {
                setText(std::string());
            }

            const std::string& Block::getFontFamily() const
            {
                return _p->fontFace;
            }

            const std::string& Block::getFontFace() const
            {
                return _p->fontFace;
            }

            MetricsRole Block::getFontSizeRole() const
            {
                return _p->fontSizeRole;
            }

            void Block::setFontFamily(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.fontFamily)
                    return;
                p.fontFamily = value;
                _textUpdate();
            }

            void Block::setFontFace(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.fontFace)
                    return;
                p.fontFace = value;
                _textUpdate();
            }

            void Block::setFontSizeRole(MetricsRole value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.fontSizeRole)
                    return;
                p.fontSizeRole = value;
                _textUpdate();
            }

            TextHAlign Block::getTextHAlign() const
            {
                return _p->textHAlign;
            }

            ColorRole Block::getTextColorRole() const
            {
                return _p->textColorRole;
            }

            MetricsRole Block::getTextSizeRole() const
            {
                return _p->textSizeRole;
            }

            void Block::setTextHAlign(TextHAlign value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.textHAlign)
                    return;
                p.textHAlign = value;
                _resize();
            }

            void Block::setTextColorRole(ColorRole value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.textColorRole)
                    return;
                p.textColorRole = value;
                _redraw();
            }

            void Block::setTextSizeRole(MetricsRole value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.textSizeRole)
                    return;
                p.textSizeRole = value;
                _textUpdate();
            }

            void Block::setWordWrap(bool value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.wordWrap)
                    return;
                p.wordWrap = value;
                _textUpdate();
            }

            float Block::getHeightForWidth(float value) const
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const float w = value - getMargin().getWidth(style);
                const glm::vec2 textSize = p.textLines(w).second;
                return textSize.y + getMargin().getHeight(style);
            }

            void Block::_preLayoutEvent(System::Event::PreLayout& event)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const float w = style->getMetric(p.textSizeRole);
                const glm::vec2 textSize = p.textLines(w).second;
                glm::vec2 size = textSize;
                size.x = std::max(size.x, w);
                _setMinimumSize(size + getMargin().getSize(style));
            }

            void Block::_layoutEvent(System::Event::Layout&)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const Math::BBox2f& g = getMargin().bbox(getGeometry(), style);
                const float w = g.w();
                p.textLines(w);
            }

            void Block::_clipEvent(System::Event::Clip& event)
            {
                _p->clipRect = event.getClipRect();
            }

            void Block::_paintEvent(System::Event::Paint& event)
            {
                Widget::_paintEvent(event);
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const Math::BBox2f& g = getMargin().bbox(getGeometry(), style);
                const auto key = std::make_pair(p.fontInfo, g.w());
                Private::TextCacheValue cacheValue;
                if (p.textCache.get(key, cacheValue))
                {
                    const glm::vec2 c = g.getCenter();
                    glm::vec2 pos = g.min;

                    const auto& render = _getRender();
                    //render->setFillColor(Image::Color(1.F, 0.F, 0.F, .5F));
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

            void Block::_initEvent(System::Event::Init& event)
            {
                if (event.getData().resize || event.getData().font)
                {
                    _textUpdate();
                }
            }

            void Block::_updateEvent(System::Event::Update& event)
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
                        _log(e.what(), System::LogLevel::Error);
                    }
                }
            }

            void Block::_textUpdate()
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

            Block::Private::TextCacheValue Block::Private::textLines(float value)
            {
                Private::TextCacheValue out;
                const auto key = std::make_pair(fontInfo, value);
                if (!textCache.get(key, out))
                {
                    auto textLines = fontSystem->textLines(
                        text,
                        wordWrap ? static_cast<uint16_t>(std::min(value, static_cast<float>(std::numeric_limits<uint16_t>::max()))) : 0.F,
                        fontInfo).get();
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

        } // namespace Text
    } // namespace UI
} // namespace djv
