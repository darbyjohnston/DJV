// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/LineGraphWidget.h>

#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Style.h>

#include <djvRender2D/Render.h>

#include <list>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        struct LineGraphWidget::Private
        {
            std::list<float> samples;
            size_t samplesSize = 0;
            Math::FloatRange samplesRange = Math::FloatRange(0.F, 0.F);
            size_t precision = 2;
            std::shared_ptr<UI::Text::Label> label;
            std::shared_ptr<UI::Text::Label> labelValue;
            std::shared_ptr<UI::HorizontalLayout> layout;
        };

        void LineGraphWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UIComponents::LineGraphWidget");

            setBackgroundColorRole(UI::ColorRole::Trough);

            p.label = UI::Text::Label::create(context);
            p.label->setFontSizeRole(UI::MetricsRole::FontSmall);
            
            p.labelValue = UI::Text::Label::create(context);
            p.labelValue->setFontFamily(Render2D::Font::familyMono);
            p.labelValue->setFontSizeRole(UI::MetricsRole::FontSmall);
            
            p.layout = UI::HorizontalLayout::create(context);
            p.layout->setMargin(UI::MetricsRole::Border);
            p.layout->setSpacing(UI::MetricsRole::SpacingSmall);
            p.layout->setBackgroundColorRole(UI::ColorRole::OverlayLight);
            p.layout->addChild(p.label);
            p.layout->addChild(p.labelValue);
            addChild(p.layout);
        }

        LineGraphWidget::LineGraphWidget() :
            _p(new Private)
        {}

        LineGraphWidget::~LineGraphWidget()
        {}

        std::shared_ptr<LineGraphWidget> LineGraphWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<LineGraphWidget>(new LineGraphWidget);
            out->_init(context);
            return out;
        }

        const Math::FloatRange& LineGraphWidget::getSampleRange() const
        {
            return _p->samplesRange;
        }

        void LineGraphWidget::addSample(float value)
        {
            DJV_PRIVATE_PTR();
            p.samples.push_front(value);
            while (p.samples.size() > p.samplesSize)
            {
                p.samples.pop_back();
            }
            p.samplesRange = Math::FloatRange(0.F);
            auto i = p.samples.begin();
            if (i != p.samples.end())
            {
                p.samplesRange = Math::FloatRange(p.samples.front());
                ++i;
            }
            for (; i != p.samples.end(); ++i)
            {
                p.samplesRange = Math::FloatRange(
                    std::min(p.samplesRange.getMin(), *i),
                    std::max(p.samplesRange.getMax(), *i));
            }
            _widgetUpdate();
            _redraw();
        }

        void LineGraphWidget::resetSamples()
        {
            DJV_PRIVATE_PTR();
            p.samples.clear();
            p.samplesRange.zero();
            _widgetUpdate();
            _redraw();
        }

        void LineGraphWidget::setPrecision(size_t value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.precision)
                return;
            p.precision = value;
            _widgetUpdate();
            _redraw();
        }

        void LineGraphWidget::_preLayoutEvent(System::Event::PreLayout& event)
        {
            const auto& style = _getStyle();
            const float tc = style->getMetric(UI::MetricsRole::TextColumn);
            _setMinimumSize(glm::vec2(tc, tc / 3.F));
        }

        void LineGraphWidget::_layoutEvent(System::Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const Math::BBox2f& g = getGeometry();
            const float b = style->getMetric(UI::MetricsRole::Border) * 2.F;
            const float w = g.w();
            p.samplesSize = static_cast<size_t>(w > 0.F ? (g.w() / b) : 0.F);
            while (p.samples.size() > p.samplesSize)
            {
                p.samples.pop_back();
            }
            const glm::vec2 layoutSize = p.layout->getMinimumSize();
            p.layout->setGeometry(Math::BBox2f(g.min.x, g.max.y - layoutSize.y, layoutSize.x, layoutSize.y));
        }

        void LineGraphWidget::_paintEvent(System::Event::Paint& event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float b = style->getMetric(UI::MetricsRole::Border) * 2.F;
            const Math::BBox2f& g = getMargin().bbox(getGeometry(), style).margin(0, 0, 0, -b);
            const auto& render = _getRender();
            auto color1 = style->getColor(UI::ColorRole::Checked);
            auto color2 = style->getColor(UI::ColorRole::Checked);
            color2.setF32(color2.getF32(3) * .5F, 3);
            float x = g.min.x;
            const float range = p.samplesRange.getMax() - p.samplesRange.getMin();
            std::vector<Math::BBox2f> rects[2];
            for (const auto& i : p.samples)
            {
                float h = (i - p.samplesRange.getMin()) / range * g.h();
                rects[0].emplace_back(Math::BBox2f(x, g.min.y + g.h() - h, b, b));
                rects[1].emplace_back(Math::BBox2f(x, g.min.y + g.h() - h + b, b, h));
                x += b;
            }
            render->setFillColor(color1);
            render->drawRects(rects[0]);
            render->setFillColor(color2);
            render->drawRects(rects[1]);
        }

        void LineGraphWidget::_initEvent(System::Event::Init& event)
        {
            if (event.getData().text)
            {
                _widgetUpdate();
            }
        }

        void LineGraphWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            {
                std::stringstream ss;
                ss << _getText(DJV_TEXT("line_graph_current")) << ", " <<
                    _getText(DJV_TEXT("line_graph_min")) << ", " <<
                    _getText(DJV_TEXT("line_graph_max")) << ": ";
                p.label->setText(ss.str());
            }
            {
                std::stringstream ss;
                ss.precision(p.precision);
                ss << std::fixed;
                float v = p.samples.size() ? p.samples.front() : 0.F;
                ss << v << "/" << p.samplesRange.getMin() << "/" << p.samplesRange.getMax();
                p.labelValue->setText(ss.str());
            }
        }

    } // namespace UIComponents
} // namespace djv

