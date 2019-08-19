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

#include <djvUI/Border.h>

#include <djvUI/DrawUtil.h>
#include <djvUI/StackLayout.h>
#include <djvUI/Style.h>

#include <djvAV/Render2D.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            struct Border::Private
            {
                MetricsRole borderSize = MetricsRole::Border;
                ColorRole borderColor = ColorRole::Border;
                Layout::Margin insideMargin;
            };

            void Border::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);
                setClassName("djv::UI::Layout::Border");
            }

            Border::Border() :
                _p(new Private)
            {}

            Border::~Border()
            {}

            std::shared_ptr<Border> Border::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<Border>(new Border);
                out->_init(context);
                return out;
            }

            MetricsRole Border::getBorderSize() const
            {
                return _p->borderSize;
            }

            void Border::setBorderSize(MetricsRole value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.borderSize)
                    return;
                p.borderSize = value;
                _resize();
            }

            ColorRole Border::getBorderColorRole() const
            {
                return _p->borderColor;
            }

            void Border::setBorderColorRole(ColorRole value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.borderColor)
                    return;
                p.borderColor = value;
                _redraw();
            }

            const Margin & Border::getInsideMargin() const
            {
                return _p->insideMargin;
            }

            void Border::setInsideMargin(const Margin & value)
            {
                if (value == _p->insideMargin)
                    return;
                _p->insideMargin = value;
                _resize();
            }

            float Border::getHeightForWidth(float value) const
            {
                DJV_PRIVATE_PTR();
                float out = 0.f;
                const auto& style = _getStyle();
                const glm::vec2 m = getMargin().getSize(style);
                const float b = style->getMetric(MetricsRole::Border);
                out = StackLayout::heightForWidth(value - b * 2.f - m.x, getChildWidgets(), p.insideMargin, style) + b * 2.f + m.y;
                return out;
            }

            void Border::_preLayoutEvent(Event::PreLayout & event)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                _setMinimumSize(StackLayout::minimumSize(getChildWidgets(), p.insideMargin, style) +
                    style->getMetric(p.borderSize) * 2.f + getMargin().getSize(style));
            }

            void Border::_layoutEvent(Event::Layout & event)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const BBox2f & g = getGeometry().margin(-style->getMetric(p.borderSize));
                StackLayout::layout(getMargin().bbox(g, style), getChildWidgets(), p.insideMargin, style);
            }

            void Border::_paintEvent(Event::Paint & event)
            {
                Widget::_paintEvent(event);
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const BBox2f & g = getMargin().bbox(getGeometry(), style);
                const float b = style->getMetric(p.borderSize);
                auto render = _getRender();
                render->setFillColor(style->getColor(p.borderColor));
                drawBorder(render, g, b);
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
