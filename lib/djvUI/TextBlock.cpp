//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvCore/Cache.h>
#include <djvCore/Math.h>
#include <djvCore/Memory.h>

#include <djvAV/FontSystem.h>
#include <djvAV/Render2D.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct TextBlock::Private
        {
            std::string text;
            TextHAlign textHAlign = TextHAlign::Left;
            Style::ColorRole textColorRole = Style::ColorRole::Foreground;
            Style::MetricsRole textSizeRole = Style::MetricsRole::TextColumn;
            std::string fontFace = AV::Font::Info::faceDefault;
            Style::MetricsRole fontSizeRole = Style::MetricsRole::FontMedium;
            AV::Font::Metrics fontMetrics;
            std::future<AV::Font::Metrics> fontMetricsFuture;
            float heightForWidth = 0.f;
            size_t heightForWidthHash = 0;
            glm::vec2 textSize = glm::vec2(0.f, 0.f);
            size_t textSizeHash = 0;
            std::future<glm::vec2> textSizeFuture;
            std::vector<AV::Font::TextLine> breakLines;
            size_t breakLinesHash = 0;
            std::future<std::vector<AV::Font::TextLine> > breakLinesFuture;
            BBox2f clipRect;
        };

        void TextBlock::_init(Context * context)
        {
            Widget::_init(context);
            
            setClassName("djv::UI::TextBlock");
        }
        
        TextBlock::TextBlock() :
            _p(new Private)
        {}

        TextBlock::~TextBlock()
        {}

        std::shared_ptr<TextBlock> TextBlock::create(Context * context)
        {
            auto out = std::shared_ptr<TextBlock>(new TextBlock);
            out->_init(context);
            return out;
        }

        std::shared_ptr<TextBlock> TextBlock::create(const std::string& text, Context * context)
        {
            auto out = TextBlock::create(context);
            out->setText(text);
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
            p.heightForWidthHash = 0;
            p.textSizeHash = 0;
            p.breakLinesHash = 0;
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

        Style::ColorRole TextBlock::getTextColorRole() const
        {
            return _p->textColorRole;
        }

        void TextBlock::setTextColorRole(Style::ColorRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.textColorRole)
                return;
            p.textColorRole = value;
            _redraw();
        }

        Style::MetricsRole TextBlock::getTextSizeRole() const
        {
            return _p->textSizeRole;
        }

        void TextBlock::setTextSizeRole(Style::MetricsRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.textSizeRole)
                return;
            p.textSizeRole = value;
            _textUpdate();
        }

        const std::string & TextBlock::getFontFace() const
        {
            return _p->fontFace;
        }

        Style::MetricsRole TextBlock::getFontSizeRole() const
        {
            return _p->fontSizeRole;
        }

        void TextBlock::setFontFace(const std::string & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.fontFace)
                return;
            p.fontFace = value;
            _textUpdate();
        }

        void TextBlock::setFontSizeRole(Style::MetricsRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.fontSizeRole)
                return;
            p.fontSizeRole = value;
            _textUpdate();
        }

        float TextBlock::getHeightForWidth(float value) const
        {
            float out = 0.f;
            if (auto style = _getStyle().lock())
            {
                DJV_PRIVATE_PTR();
                if (auto fontSystem = _getFontSystem().lock())
                {
                    const auto fontInfo = style->getFontInfo(p.fontFace, p.fontSizeRole);
                    const float w = value - getMargin().getWidth(style);

                    size_t hash = 0;
                    Memory::hashCombine(hash, fontInfo.family);
                    Memory::hashCombine(hash, fontInfo.face);
                    Memory::hashCombine(hash, fontInfo.size);
                    Memory::hashCombine(hash, w);
                    if (!p.heightForWidthHash || p.heightForWidthHash != hash)
                    {
                        p.heightForWidthHash = hash;
                        p.heightForWidth = fontSystem->measure(p.text, w, fontInfo).get().y;
                    }
                }
                out = p.heightForWidth + getMargin().getHeight(style);
            }
            return out;
        }

        void TextBlock::_styleEvent(Event::Style& event)
        {
            _textUpdate();
        }

        void TextBlock::_preLayoutEvent(Event::PreLayout& event)
        {
            if (auto style = _getStyle().lock())
            {
                DJV_PRIVATE_PTR();
                if (p.fontMetricsFuture.valid())
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
                if (p.textSizeFuture.valid())
                {
                    try
                    {
                        p.textSize = p.textSizeFuture.get();
                        _resize();
                    }
                    catch (const std::exception& e)
                    {
                        _log(e.what(), LogLevel::Error);
                    }
                }

                auto size = p.textSize;
                size.x = std::max(size.x, style->getMetric(p.textSizeRole));
                _setMinimumSize(size + getMargin().getSize(style));
            }
        }

        void TextBlock::_layoutEvent(Event::Layout& event)
        {
            if (auto style = _getStyle().lock())
            {
                if (auto fontSystem = _getFontSystem().lock())
                {
                    DJV_PRIVATE_PTR();
                    const BBox2f& g = getMargin().bbox(getGeometry(), style);
                    const auto fontInfo = style->getFontInfo(p.fontFace, p.fontSizeRole);

                    size_t hash = 0;
                    Memory::hashCombine(hash, fontInfo.family);
                    Memory::hashCombine(hash, fontInfo.face);
                    Memory::hashCombine(hash, fontInfo.size);
                    Memory::hashCombine(hash, g.w());
                    if (!p.breakLinesHash || p.breakLinesHash != hash)
                    {
                        p.breakLinesHash = hash;
                        p.breakLinesFuture = fontSystem->breakLines(p.text, g.w(), fontInfo);
                    }
                }
            }
        }

        void TextBlock::_clipEvent(Event::Clip& event)
        {
            _p->clipRect = event.getClipRect();
        }

        void TextBlock::_paintEvent(Event::Paint& event)
        {
            Widget::_paintEvent(event);
            if (auto render = _getRender().lock())
            {
                if (auto style = _getStyle().lock())
                {
                    DJV_PRIVATE_PTR();
                    if (p.textSize.x > 0.f && p.textSize.y > 0.f)
                    {
                        const BBox2f& g = getMargin().bbox(getGeometry(), style);
                        const glm::vec2 c = g.getCenter();

                        if (p.breakLinesFuture.valid())
                        {
                            p.breakLines = p.breakLinesFuture.get();
                        }
                        glm::vec2 pos = g.min;
                        render->setCurrentFont(style->getFontInfo(p.fontFace, p.fontSizeRole));
                        for (const auto& line : p.breakLines)
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

                                //render->setFillColor(AV::Image::Color(1.f, 0.f, 0.f));
                                //render->drawRectangle(BBox2f(pos.x, pos.y, line.size.x, line.size.y));

                                render->setFillColor(_getColorWithOpacity(style->getColor(p.textColorRole)));
                                render->drawText(line.text, glm::vec2(pos.x, pos.y + p.fontMetrics.ascender));
                            }
                            pos.y += line.size.y;
                        }
                    }
                }
            }
        }

        void TextBlock::_textUpdate()
        {
            if (auto style = _getStyle().lock())
            {
                if (auto fontSystem = _getFontSystem().lock())
                {
                    const BBox2f& g = getMargin().bbox(getGeometry(), style);

                    DJV_PRIVATE_PTR();
                    auto fontInfo = style->getFontInfo(p.fontFace, p.fontSizeRole);
                    p.fontMetricsFuture = fontSystem->getMetrics(fontInfo);

                    size_t hash = 0;
                    Memory::hashCombine(hash, fontInfo.family);
                    Memory::hashCombine(hash, fontInfo.face);
                    Memory::hashCombine(hash, fontInfo.size);
                    const float w = g.w() > 0 ? g.w() : style->getMetric(p.textSizeRole);
                    Memory::hashCombine(hash, w);
                    if (!p.textSizeHash || p.textSizeHash != hash)
                    {
                        p.textSizeHash = hash;
                        p.textSizeFuture = fontSystem->measure(p.text, w, fontInfo);
                    }
                }
            }
        }

    } // namespace UI
} // namespace djv
