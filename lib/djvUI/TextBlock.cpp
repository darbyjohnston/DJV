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
#include <djvAV/Render2DSystem.h>

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
            std::string fontFace = AV::Font::Info::defaultFace;
            Style::MetricsRole fontSizeRole = Style::MetricsRole::FontMedium;
            std::future<AV::Font::Metrics> fontMetricsFuture;
            AV::Font::Metrics fontMetrics;
            float heightForWidth = 0.f;
            size_t heightForWidthHash = 0;
            glm::vec2 textSize = glm::vec2(0.f, 0.f);
            size_t textSizeHash = 0;
            std::future<glm::vec2> textSizeFuture;
            std::vector<AV::Font::TextLine> breakText;
            size_t breakTextHash = 0;
            std::future<std::vector<AV::Font::TextLine> > breakTextFuture;
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
            _p->text = value;
            _p->heightForWidthHash = 0;
            _p->textSizeHash = 0;
            _p->breakTextHash = 0;
        }

        TextHAlign TextBlock::getTextHAlign() const
        {
            return _p->textHAlign;
        }
        
        void TextBlock::setTextHAlign(TextHAlign value)
        {
            _p->textHAlign = value;
        }

        Style::ColorRole TextBlock::getTextColorRole() const
        {
            return _p->textColorRole;
        }

        void TextBlock::setTextColorRole(Style::ColorRole value)
        {
            _p->textColorRole = value;
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
            _p->fontFace = value;
        }

        void TextBlock::setFontSizeRole(Style::MetricsRole value)
        {
            _p->fontSizeRole = value;
        }

        float TextBlock::getHeightForWidth(float value) const
        {
            float out = 0.f;
            if (auto style = _getStyle().lock())
            {
                if (auto fontSystem = _getFontSystem().lock())
                {
                    const auto font = style->getFont(_p->fontFace, _p->fontSizeRole);
                    const float w = value - getMargin().getWidth(style);

                    size_t hash = 0;
                    Memory::hashCombine(hash, font.family);
                    Memory::hashCombine(hash, font.face);
                    Memory::hashCombine(hash, font.size);
                    Memory::hashCombine(hash, w);
                    if (!_p->heightForWidthHash || _p->heightForWidthHash != hash)
                    {
                        _p->heightForWidthHash = hash;
                        _p->heightForWidth = fontSystem->measure(_p->text, w, font).get().y;
                    }
                }
                out = _p->heightForWidth + getMargin().getHeight(style);
            }
            return out;
        }

        void TextBlock::_preLayoutEvent(Event::PreLayout& event)
        {
            if (auto style = _getStyle().lock())
            {
                if (_p->textSizeFuture.valid())
                {
                    _p->textSize = _p->textSizeFuture.get();
                }
                glm::vec2 size = _p->textSize;
                size.x = std::max(size.x, style->getMetric(Style::MetricsRole::TextColumn) - getMargin().getWidth(style));
                _setMinimumSize(size + getMargin().getSize(style));
            }
        }

        void TextBlock::_layoutEvent(Event::Layout& event)
        {
            if (auto style = _getStyle().lock())
            {
                if (auto fontSystem = _getFontSystem().lock())
                {
                    const BBox2f& g = getMargin().bbox(getGeometry(), style);
                    const auto font = style->getFont(_p->fontFace, _p->fontSizeRole);

                    size_t hash = 0;
                    Memory::hashCombine(hash, font.family);
                    Memory::hashCombine(hash, font.face);
                    Memory::hashCombine(hash, font.size);
                    Memory::hashCombine(hash, g.w());
                    if (!_p->breakTextHash || _p->breakTextHash != hash)
                    {
                        _p->breakTextHash = hash;
                        _p->breakTextFuture = fontSystem->breakLines(_p->text, g.w(), font);
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
            if (auto render = _getRenderSystem().lock())
            {
                if (auto style = _getStyle().lock())
                {
                    const BBox2f& g = getMargin().bbox(getGeometry(), style);
                    const glm::vec2 c = g.getCenter();

                    float ascender = 0.f;
                    if (_p->fontMetricsFuture.valid())
                    {
                        ascender = _p->fontMetricsFuture.get().ascender;
                    }
                    if (_p->breakTextFuture.valid())
                    {
                        _p->breakText = _p->breakTextFuture.get();
                    }
                    glm::vec2 pos = g.min;
                    render->setCurrentFont(style->getFont(_p->fontFace, _p->fontSizeRole));
                    render->setFillColor(_getColorWithOpacity(style->getColor(_p->textColorRole)));
                    for (const auto& line : _p->breakText)
                    {
                        if (pos.y + line.size.y >= _p->clipRect.min.y && pos.y <= _p->clipRect.max.y)
                        {
                            switch (_p->textHAlign)
                            {
                            case TextHAlign::Center:
                                pos.x = c.x - line.size.x / 2.f;
                                break;
                            case TextHAlign::Right:
                                pos.x = g.max.x - line.size.x;
                                break;
                            default: break;
                            }
                            render->drawText(line.text, glm::vec2(pos.x, pos.y + ascender));
                        }
                        pos.y += line.size.y;
                    }
                }
            }
        }

        void TextBlock::_updateEvent(Event::Update& event)
        {
            if (auto style = _getStyle().lock())
            {
                if (auto fontSystem = _getFontSystem().lock())
                {
                    const BBox2f& g = getMargin().bbox(getGeometry(), style);
                    auto font = style->getFont(_p->fontFace, _p->fontSizeRole);

                    _p->fontMetricsFuture = fontSystem->getMetrics(font);

                    size_t hash = 0;
                    Memory::hashCombine(hash, font.family);
                    Memory::hashCombine(hash, font.face);
                    Memory::hashCombine(hash, font.size);
                    Memory::hashCombine(hash, g.w());
                    if (!_p->textSizeHash || _p->textSizeHash != hash)
                    {
                        _p->textSizeHash = hash;
                        _p->textSizeFuture = fontSystem->measure(_p->text, g.w(), font);
                    }
                }
            }
        }

    } // namespace UI
} // namespace djv
