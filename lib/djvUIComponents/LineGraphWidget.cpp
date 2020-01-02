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

#include <djvUIComponents/LineGraphWidget.h>

#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Style.h>

#include <djvAV/Render2D.h>

#include <list>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct LineGraphWidget::Private
        {
            std::list<float> samples;
            size_t samplesSize = 0;
            FloatRange samplesRange = FloatRange(0.F, 0.F);
            size_t precision = 2;
            std::shared_ptr<Label> label;
            std::shared_ptr<Label> labelValue;
            std::shared_ptr<HorizontalLayout> layout;
        };

        void LineGraphWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::LineGraphWidget");

            setBackgroundRole(ColorRole::Trough);

            p.label = Label::create(context);
            p.label->setFontSizeRole(MetricsRole::FontSmall);
            
            p.labelValue = Label::create(context);
            p.labelValue->setFont(AV::Font::familyMono);
            p.labelValue->setFontSizeRole(MetricsRole::FontSmall);
            
            p.layout = HorizontalLayout::create(context);
            p.layout->setMargin(Layout::Margin(MetricsRole::Border));
            p.layout->setSpacing(Layout::Spacing(MetricsRole::SpacingSmall));
            p.layout->setBackgroundRole(ColorRole::OverlayLight);
            p.layout->addChild(p.label);
            p.layout->addChild(p.labelValue);
            addChild(p.layout);
        }

        LineGraphWidget::LineGraphWidget() :
            _p(new Private)
        {}

        LineGraphWidget::~LineGraphWidget()
        {}

        std::shared_ptr<LineGraphWidget> LineGraphWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<LineGraphWidget>(new LineGraphWidget);
            out->_init(context);
            return out;
        }

        const FloatRange& LineGraphWidget::getSampleRange() const
        {
            return _p->samplesRange;
        }

        void LineGraphWidget::addSample(float value)
        {
            DJV_PRIVATE_PTR();
            p.samples.push_front(value);
            p.samplesRange.min = std::min(value, p.samplesRange.min);
            p.samplesRange.max = std::max(value, p.samplesRange.max);
            while (p.samples.size() > p.samplesSize)
            {
                p.samples.pop_back();
            }
            p.samplesRange.min = p.samplesRange.max = 0.F;
            auto i = p.samples.begin();
            if (i != p.samples.end())
            {
                p.samplesRange.min = p.samplesRange.max = p.samples.front();
                ++i;
            }
            for (; i != p.samples.end(); ++i)
            {
                p.samplesRange.min = std::min(p.samplesRange.min, *i);
                p.samplesRange.max = std::max(p.samplesRange.max, *i);
            }
            _updateWidget();
            _redraw();
        }

        void LineGraphWidget::resetSamples()
        {
            DJV_PRIVATE_PTR();
            p.samples.clear();
            p.samplesRange.zero();
            _updateWidget();
            _redraw();
        }

        void LineGraphWidget::setPrecision(size_t value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.precision)
                return;
            p.precision = value;
            _updateWidget();
            _redraw();
        }

        void LineGraphWidget::_preLayoutEvent(Event::PreLayout & event)
        {
            const auto& style = _getStyle();
            const float tc = style->getMetric(MetricsRole::TextColumn);
            _setMinimumSize(glm::vec2(tc, tc / 3.F));
        }

        void LineGraphWidget::_layoutEvent(Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const BBox2f& g = getGeometry();
            const float b = style->getMetric(MetricsRole::Border) * 2.F;
            const float w = g.w();
            p.samplesSize = static_cast<size_t>(w > 0.F ? (g.w() / b) : 0.F);
            while (p.samples.size() > p.samplesSize)
            {
                p.samples.pop_back();
            }
            const glm::vec2 layoutSize = p.layout->getMinimumSize();
            p.layout->setGeometry(BBox2f(g.min.x, g.max.y - layoutSize.y, layoutSize.x, layoutSize.y));
        }

        void LineGraphWidget::_paintEvent(Event::Paint& event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float b = style->getMetric(MetricsRole::Border) * 2.F;
            const BBox2f& g = getMargin().bbox(getGeometry(), style).margin(0, 0, 0, -b);
            auto render = _getRender();
            auto color1 = style->getColor(ColorRole::Checked);
            auto color2 = style->getColor(ColorRole::Checked);
            color2.setF32(color2.getF32(3) * .5F, 3);
            float x = g.min.x;
            const float range = p.samplesRange.max - p.samplesRange.min;
            std::vector<BBox2f> boxes1(p.samples.size());
            std::vector<BBox2f> boxes2(p.samples.size());
            for (const auto& i : p.samples)
            {
                float h = (i - p.samplesRange.min) / range * g.h();
                boxes1.emplace_back(BBox2f(x, g.min.y + g.h() - h, b, b));
                boxes2.emplace_back(BBox2f(x, g.min.y + g.h() - h + b, b, h));
                x += b;
            }
            render->setFillColor(color1);
            render->drawRects(boxes1);
            render->setFillColor(color2);
            render->drawRects(boxes2);
        }

        void LineGraphWidget::_initEvent(Event::Init& event)
        {
            Widget::_initEvent(event);
            _updateWidget();
        }

        void LineGraphWidget::_updateWidget()
        {
            DJV_PRIVATE_PTR();
            {
                std::stringstream ss;
                ss << _getText(DJV_TEXT("Current")) << ", " <<
                    _getText(DJV_TEXT("min")) << ", " <<
                    _getText(DJV_TEXT("max")) << ": ";
                p.label->setText(ss.str());
            }
            {
                std::stringstream ss;
                ss.precision(p.precision);
                ss << std::fixed;
                float v = p.samples.size() ? p.samples.front() : 0.F;
                ss << v << "/" << p.samplesRange.min << "/" << p.samplesRange.max;
                p.labelValue->setText(ss.str());
            }
        }

    } // namespace UI
} // namespace djv

