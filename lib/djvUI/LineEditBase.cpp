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

#include <djvUI/LineEditBase.h>

#include <djvAV/Render2D.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct LineEditBase::Private
        {
            std::string text;
            Style::ColorRole textColorRole = Style::ColorRole::Foreground;
            std::string font;
            std::string fontFace = AV::Font::Info::faceDefault;
            Style::MetricsRole fontSizeRole = Style::MetricsRole::FontMedium;
            AV::Font::Metrics fontMetrics;
            std::future<AV::Font::Metrics> fontMetricsFuture;
            glm::vec2 textSize = glm::vec2(0.f, 0.f);
            std::future<glm::vec2> textSizeFuture;
        };

        void LineEditBase::_init(Context * context)
        {
            Widget::_init(context);

            setClassName("djv::UI::LineEditBase");
            setBackgroundRole(Style::ColorRole::Trough);
            setVAlign(VAlign::Center);
        }

        LineEditBase::LineEditBase() :
            _p(new Private)
        {}

        LineEditBase::~LineEditBase()
        {}

        std::shared_ptr<LineEditBase> LineEditBase::create(Context * context)
        {
            auto out = std::shared_ptr<LineEditBase>(new LineEditBase);
            out->_init(context);
            return out;
        }

        const std::string& LineEditBase::getText() const
        {
            return _p->text;
        }

        void LineEditBase::setText(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.text)
                return;
            p.text = value;
            _textUpdate();
        }

        Style::ColorRole LineEditBase::getTextColorRole() const
        {
            return _p->textColorRole;
        }

        void LineEditBase::setTextColorRole(Style::ColorRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.textColorRole)
                return;
            p.textColorRole = value;
            _redraw();
        }

        const std::string & LineEditBase::getFont() const
        {
            return _p->font;
        }

        const std::string & LineEditBase::getFontFace() const
        {
            return _p->fontFace;
        }

        Style::MetricsRole LineEditBase::getFontSizeRole() const
        {
            return _p->fontSizeRole;
        }

        void LineEditBase::setFont(const std::string & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.font)
                return;
            p.font = value;
            _textUpdate();
        }

        void LineEditBase::setFontFace(const std::string & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.fontFace)
                return;
            p.fontFace = value;
            _textUpdate();
        }

        void LineEditBase::setFontSizeRole(Style::MetricsRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.fontSizeRole)
                return;
            p.fontSizeRole = value;
            _textUpdate();
        }

        void LineEditBase::_styleEvent(Event::Style& event)
        {
            _textUpdate();
        }
        
        void LineEditBase::_preLayoutEvent(Event::PreLayout& event)
        {
			DJV_PRIVATE_PTR();
			if (auto style = _getStyle().lock())
            {
                const float ms = style->getMetric(Style::MetricsRole::MarginSmall);
                const float tc = style->getMetric(Style::MetricsRole::TextColumn);

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

                glm::vec2 size = p.textSize;
                size.x = tc;
                size += ms * 2.f;
                _setMinimumSize(size + getMargin().getSize(style));
            }
        }

        void LineEditBase::_paintEvent(Event::Paint& event)
        {
            Widget::_paintEvent(event);
			DJV_PRIVATE_PTR();
			if (auto render = _getRender().lock())
            {
                if (auto style = _getStyle().lock())
                {
                    const BBox2f& g = getMargin().bbox(getGeometry(), style);
                    const glm::vec2 c = g.getCenter();
                    const float ms = style->getMetric(Style::MetricsRole::MarginSmall);

                    auto fontInfo = p.font.empty() ?
                        style->getFontInfo(p.fontFace, p.fontSizeRole) :
                        style->getFontInfo(p.font, p.fontFace, p.fontSizeRole);
                    render->setCurrentFont(fontInfo);

                    glm::vec2 pos = g.min;
                    pos += ms;
                    pos.y = c.y - p.textSize.y / 2.f;

                    render->setFillColor(_getColorWithOpacity(style->getColor(p.textColorRole)));
                    //! \bug Why the extra subtract by one here?
                    render->drawText(p.text, glm::vec2(floorf(pos.x), floorf(pos.y + p.fontMetrics.ascender - 1.f)));
                }
            }
        }

        void LineEditBase::_textUpdate()
        {
			DJV_PRIVATE_PTR();
			if (auto style = _getStyle().lock())
            {
                if (auto fontSystem = _getFontSystem().lock())
                {
                    const auto fontInfo = p.font.empty() ?
                        style->getFontInfo(p.fontFace, p.fontSizeRole) :
                        style->getFontInfo(p.font, p.fontFace, p.fontSizeRole);
                    p.fontMetricsFuture = fontSystem->getMetrics(fontInfo);
                    p.textSizeFuture = fontSystem->measure(p.text, fontInfo);
                }
            }
        }

    } // namespace UI
} // namespace djv
