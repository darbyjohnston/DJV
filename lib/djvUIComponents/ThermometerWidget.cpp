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

#include <djvUIComponents/ThermometerWidget.h>

#include <djvUI/Style.h>

#include <djvAV/Render2D.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ThermometerWidget::Private
        {
            Orientation orientation = Orientation::Horizontal;
            float percentage = 0.F;
            ColorRole colorRole = ColorRole::Checked;
            MetricsRole sizeRole = MetricsRole::Slider;
        };

        void ThermometerWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            setClassName("djv::UI::ThermometerWidget");
            setBackgroundRole(ColorRole::Trough);
        }

        ThermometerWidget::ThermometerWidget() :
            _p(new Private)
        {}

        ThermometerWidget::~ThermometerWidget()
        {}

        std::shared_ptr<ThermometerWidget> ThermometerWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ThermometerWidget>(new ThermometerWidget);
            out->_init(context);
            return out;
        }

        Orientation ThermometerWidget::getOrientation() const
        {
            return _p->orientation;
        }

        void ThermometerWidget::setOrientation(Orientation value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.orientation)
                return;
            p.orientation = value;
            _resize();
        }

        float ThermometerWidget::getPercentage() const
        {
            return _p->percentage;
        }

        void ThermometerWidget::setPercentage(float value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.percentage)
                return;
            p.percentage = value;
            _redraw();
        }

        ColorRole ThermometerWidget::getColorRole() const
        {
            return _p->colorRole;
        }

        void ThermometerWidget::setColorRole(ColorRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.colorRole)
                return;
            p.colorRole = value;
            _resize();
        }

        MetricsRole ThermometerWidget::getSizeRole() const
        {
            return _p->sizeRole;
        }

        void ThermometerWidget::setSizeRole(MetricsRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.sizeRole)
                return;
            p.sizeRole = value;
            _resize();
        }

        void ThermometerWidget::_preLayoutEvent(Event::PreLayout & event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float sr = style->getMetric(p.sizeRole);
            const float hw = style->getMetric(MetricsRole::Handle);
            glm::vec2 size(0.F, 0.F);
            switch (p.orientation)
            {
            case Orientation::Horizontal: size = glm::vec2(sr, hw); break;
            case Orientation::Vertical:   size = glm::vec2(hw, sr); break;
            default: break;
            }
            _setMinimumSize(size);
        }

        void ThermometerWidget::_layoutEvent(Event::Layout&)
        {}

        void ThermometerWidget::_paintEvent(Event::Paint& event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const BBox2f& g = getMargin().bbox(getGeometry(), style);
            auto render = _getRender();
            render->setFillColor(style->getColor(p.colorRole));
            switch (p.orientation)
            {
            case Orientation::Horizontal:
            {
                const float w = p.percentage / 100.F * g.w();
                render->drawRect(BBox2f(g.min.x, g.min.y, w, g.h()));
                break;
            }
            case Orientation::Vertical:
            {
                const float h = p.percentage / 100.F * g.h();
                render->drawRect(BBox2f(g.min.x, g.max.y - h, g.w(), h));
                break;
            }
            default: break;
            }
        }

    } // namespace UI
} // namespace djv

