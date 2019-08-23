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

#include <djvUI/Label.h>

#include <djvUI/Style.h>

#include <djvAV/Render2D.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct Label::Private
        {
            std::string text;
            TextHAlign textHAlign = TextHAlign::Center;
            TextVAlign textVAlign = TextVAlign::Center;
            ColorRole textColorRole = ColorRole::Foreground;
            std::string font;
            std::string fontFace = AV::Font::faceDefault;
            MetricsRole fontSizeRole = MetricsRole::FontMedium;
            AV::Font::Metrics fontMetrics;
            std::future<AV::Font::Metrics> fontMetricsFuture;
            glm::vec2 textSize = glm::vec2(0.f, 0.f);
            std::future<glm::vec2> textSizeFuture;
            std::string sizeString;
            glm::vec2 sizeStringSize = glm::vec2(0.f, 0.f);
            std::future<glm::vec2> sizeStringFuture;
        };

        void Label::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            setClassName("djv::UI::Label");
        }
        
        Label::Label() :
            _p(new Private)
        {}

        Label::~Label()
        {}

        std::shared_ptr<Label> Label::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<Label>(new Label);
            out->_init(context);
            return out;
        }

        /*std::shared_ptr<Label> Label::create(const std::string & text, const std::shared_ptr<Core::Context>& context)
        {
            auto out = Label::create(context);
            out->setText(text);
            return out;
        }*/

        const std::string & Label::getText() const
        {
            return _p->text;
        }

        void Label::setText(const std::string & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.text)
                return;
            p.text = value;
            _textUpdate();
        }

        TextHAlign Label::getTextHAlign() const
        {
            return _p->textHAlign;
        }
        
        TextVAlign Label::getTextVAlign() const
        {
            return _p->textVAlign;
        }
        
        void Label::setTextHAlign(TextHAlign value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.textHAlign)
                return;
            p.textHAlign = value;
            _resize();
        }
        
        void Label::setTextVAlign(TextVAlign value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.textVAlign)
                return;
            p.textVAlign = value;
            _resize();
        }
            
        ColorRole Label::getTextColorRole() const
        {
            return _p->textColorRole;
        }

        void Label::setTextColorRole(ColorRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.textColorRole)
                return;
            p.textColorRole = value;
            _redraw();
        }

        const std::string & Label::getFont() const
        {
            return _p->font;
        }

        const std::string & Label::getFontFace() const
        {
            return _p->fontFace;
        }

        MetricsRole Label::getFontSizeRole() const
        {
            return _p->fontSizeRole;
        }

        void Label::setFont(const std::string & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.font)
                return;
            p.font = value;
            _textUpdate();
        }

        void Label::setFontFace(const std::string & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.fontFace)
                return;
            p.fontFace = value;
            _textUpdate();
        }

        void Label::setFontSizeRole(MetricsRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.fontSizeRole)
                return;
            p.fontSizeRole = value;
            _textUpdate();
        }

        const std::string & Label::getSizeString() const
        {
            return _p->sizeString;
        }

        void Label::setSizeString(const std::string & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.sizeString)
                return;
            p.sizeString = value;
            _textUpdate();
        }

        void Label::_styleEvent(Event::Style & event)
        {
            _textUpdate();
        }

        void Label::_preLayoutEvent(Event::PreLayout &)
        {
            DJV_PRIVATE_PTR();
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
            if (p.textSizeFuture.valid())
            {
                try
                {
                    p.textSize = p.textSizeFuture.get();
                }
                catch (const std::exception & e)
                {
                    _log(e.what(), LogLevel::Error);
                }
            }
            if (p.sizeStringFuture.valid())
            {
                try
                {
                    p.sizeStringSize = p.sizeStringFuture.get();
                }
                catch (const std::exception & e)
                {
                    _log(e.what(), LogLevel::Error);
                }
            }
            const glm::vec2 size(glm::max(p.textSize.x, p.sizeStringSize.x), p.fontMetrics.lineHeight);
            const auto& style = _getStyle();
            _setMinimumSize(size + getMargin().getSize(style));
        }

        void Label::_paintEvent(Event::Paint & event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const BBox2f & g = getMargin().bbox(getGeometry(), style);
            const glm::vec2 c = g.getCenter();

            auto fontInfo = p.font.empty() ?
                style->getFontInfo(p.fontFace, p.fontSizeRole) :
                style->getFontInfo(p.font, p.fontFace, p.fontSizeRole);
            auto render = _getRender();
            render->setCurrentFont(fontInfo);
            glm::vec2 pos = g.min;
            switch (p.textHAlign)
            {
            case TextHAlign::Center:
                pos.x = c.x - p.textSize.x / 2.f;
                break;
            case TextHAlign::Right:
                pos.x = g.max.x - p.textSize.x;
                break;
            default: break;
            }
            switch (p.textVAlign)
            {
            case TextVAlign::Center:
                pos.y = c.y - p.textSize.y / 2.f;
                break;
            case TextVAlign::Top:
                pos.y = g.min.y;
                break;
            case TextVAlign::Bottom:
                pos.y = g.max.y - p.textSize.y;
                break;
            case TextVAlign::Baseline:
                pos.y = c.y - p.fontMetrics.ascender / 2.f;
                break;
            default: break;
            }

            //render->setFillColor(AV::Image::Color(1.f, 0.f, 0.f));
            //render->drawRect(BBox2f(pos.x, pos.y, p.textSize.x, p.textSize.y));

            render->setFillColor(style->getColor(p.textColorRole));
            //! \bug Why the extra subtract by one here?
            render->drawText(p.text, glm::vec2(floorf(pos.x), floorf(pos.y + p.fontMetrics.ascender - 1.f)));
        }

        void Label::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const auto fontInfo = p.font.empty() ?
                style->getFontInfo(p.fontFace, p.fontSizeRole) :
                style->getFontInfo(p.font, p.fontFace, p.fontSizeRole);
            auto fontSystem = _getFontSystem();
            p.fontMetricsFuture = fontSystem->getMetrics(fontInfo);
            p.textSizeFuture = fontSystem->measure(p.text, fontInfo);
            if (!p.sizeString.empty())
            {
                p.sizeStringFuture = fontSystem->measure(p.sizeString, fontInfo);
            }
            _resize();
        }

    } // namespace UI
} // namespace djv
