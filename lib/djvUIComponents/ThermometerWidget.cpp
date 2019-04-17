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

#include <djvAV/Render2D.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ThermometerWidget::Private
        {
            float percentage = 0.f;
        };

        void ThermometerWidget::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();

            setBackgroundRole(ColorRole::Border);
        }

        ThermometerWidget::ThermometerWidget() :
            _p(new Private)
        {}

        ThermometerWidget::~ThermometerWidget()
        {}

        std::shared_ptr<ThermometerWidget> ThermometerWidget::create(Context * context)
        {
            auto out = std::shared_ptr<ThermometerWidget>(new ThermometerWidget);
            out->_init(context);
            return out;
        }

        void ThermometerWidget::setPercentage(float value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.percentage)
                return;
            p.percentage = value;
            _redraw();
        }

        void ThermometerWidget::_preLayoutEvent(Event::PreLayout & event)
        {
            auto style = _getStyle();
            const float sl = style->getMetric(MetricsRole::Slider);
            const float hw = style->getMetric(MetricsRole::Handle);
            _setMinimumSize(glm::vec2(sl, hw));
        }

        void ThermometerWidget::_layoutEvent(Event::Layout&)
        {}

        void ThermometerWidget::_paintEvent(Event::Paint& event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            auto style = _getStyle();
            const BBox2f& g = getMargin().bbox(getGeometry(), style);
            auto render = _getRender();
            render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Checked)));
            const float w = p.percentage / 100.f * g.w();
            render->drawRect(BBox2f(g.min.x, g.min.y, w, g.h()));
        }

    } // namespace UI
} // namespace djv

