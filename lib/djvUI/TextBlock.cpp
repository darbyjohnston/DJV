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
            AV::Font::Metrics fontMetrics;
            std::future<AV::Font::Metrics> fontMetricsFuture;
            glm::vec2 textSize = glm::vec2(0.f, 0.f);
            size_t textSizeHash = 0;
            std::vector<AV::Font::TextLine> textLines;
            BBox2f clipRect;
        };

        void TextBlock::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            setClassName("djv::UI::TextBlock");
            _p->fontSystem = context->getSystemT<AV::Font::System>();
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

        void TextBlock::setText(const std::string & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.text)
                return;
            p.text = value;
            p.textSizeHash = 0;
            _textUpdate();
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
            float out = 0.f;
            const auto& style = _getStyle();
            const float w = value - getMargin().getWidth(style);
            _calcSize(w);
            out = p.textSize.y + getMargin().getHeight(style);
            return out;
        }

        void TextBlock::_styleEvent(Event::Style & event)
        {
            _textUpdate();
        }

        void TextBlock::_preLayoutEvent(Event::PreLayout & event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            _calcSize(style->getMetric(p.textSizeRole));
            _setMinimumSize(p.textSize + getMargin().getSize(style));
        }

        void TextBlock::_clipEvent(Event::Clip & event)
        {
            _p->clipRect = event.getClipRect();
        }

        void TextBlock::_paintEvent(Event::Paint & event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            if (p.textSize.x > 0.f && p.textSize.y > 0.f)
            {
                const auto& style = _getStyle();
                const BBox2f & g = getMargin().bbox(getGeometry(), style);
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
                render->setCurrentFont(p.fontFamily.empty() ?
                    style->getFontInfo(p.fontFace, p.fontSizeRole) :
                    style->getFontInfo(p.fontFamily, p.fontFace, p.fontSizeRole));

                render->setFillColor(style->getColor(p.textColorRole));
                for (const auto & line : p.textLines)
                {
                    if (pos.y + line.size.y >= p.clipRect.min.y && pos.y <= p.clipRect.max.y)
                    {
                        switch (p.textHAlign)
                        {
                        case TextHAlign::Center:
                            pos.x = ceilf(c.x - line.size.x / 2.f);
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
                            glm::vec2(floorf(pos.x), floorf(pos.y + p.fontMetrics.ascender - 1.f)));
                    }
                    pos.y += line.size.y;
                }
            }
        }

        void TextBlock::_calcSize(float value) const
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const auto fontInfo = p.fontFamily.empty() ?
                style->getFontInfo(p.fontFace, p.fontSizeRole) :
                style->getFontInfo(p.fontFamily, p.fontFace, p.fontSizeRole);
            size_t hash = 0;
            Memory::hashCombine(hash, fontInfo.family);
            Memory::hashCombine(hash, fontInfo.face);
            Memory::hashCombine(hash, fontInfo.size);
            Memory::hashCombine(hash, value);
            if (!p.textSizeHash || p.textSizeHash != hash)
            {
                p.textSizeHash = hash;
                p.textLines = p.fontSystem->textLines(p.text, value, fontInfo).get();
                p.textSize = glm::vec2(0.f, 0.f);
                for (const auto& i : p.textLines)
                {
                    p.textSize.x = std::max(p.textSize.x, i.size.x);
                    p.textSize.y += i.size.y;
                }
            }
        }

        void TextBlock::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            auto fontInfo = p.fontFamily.empty() ?
                style->getFontInfo(p.fontFace, p.fontSizeRole) :
                style->getFontInfo(p.fontFamily, p.fontFace, p.fontSizeRole);
            p.fontMetricsFuture = p.fontSystem->getMetrics(fontInfo);
            p.fontSystem->cacheGlyphs(p.text, fontInfo);
            _resize();
        }

    } // namespace UI
} // namespace djv
