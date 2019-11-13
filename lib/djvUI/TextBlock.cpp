//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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
            std::string fontFace = AV::Font::faceDefault;
            std::string fontFamily;
            MetricsRole fontSizeRole = MetricsRole::FontMedium;
            AV::Font::Info fontInfo;
            AV::Font::Metrics fontMetrics;
            std::future<AV::Font::Metrics> fontMetricsFuture;
            typedef std::pair<AV::Font::Info, float> TextCacheKey;
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

        const std::string & TextBlock::getText() const
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

        const std::string & TextBlock::getFontFamily() const
        {
            return _p->fontFace;
        }

        const std::string & TextBlock::getFontFace() const
        {
            return _p->fontFace;
        }

        MetricsRole TextBlock::getFontSizeRole() const
        {
            return _p->fontSizeRole;
        }

        void TextBlock::setFontFamily(const std::string & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.fontFamily)
                return;
            p.fontFamily = value;
            _textUpdate();
        }

        void TextBlock::setFontFace(const std::string & value)
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

        void TextBlock::_clipEvent(Event::Clip & event)
        {
            _p->clipRect = event.getClipRect();
        }

        void TextBlock::_paintEvent(Event::Paint & event)
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

                if (p.fontMetricsFuture.valid())
                {
                    try
                    {
                        p.fontMetrics = p.fontMetricsFuture.get();
                    }
                    catch (const std::exception & e)
                    {
                        _log(e.what(), LogLevel::Error);
                    }
                }

                glm::vec2 pos = g.min;
                auto render = _getRender();
                render->setCurrentFont(p.fontInfo);
                render->setFillColor(style->getColor(p.textColorRole));
                for (const auto & line : cacheValue.first)
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
                            line.text,
                            glm::vec2(floorf(pos.x), floorf(pos.y + p.fontMetrics.ascender - 1.F)));
                    }
                    pos.y += line.size.y;
                }
            }
        }

        void TextBlock::_initEvent(Event::Init & event)
        {
            Widget::_initEvent(event);
            _textUpdate();
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
