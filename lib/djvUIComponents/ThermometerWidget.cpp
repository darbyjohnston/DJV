// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/ThermometerWidget.h>

#include <djvUI/Style.h>

#include <djvRender2D/Render.h>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        struct ThermometerWidget::Private
        {
            UI::Orientation orientation = UI::Orientation::Horizontal;
            float percentage = 0.F;
            UI::ColorRole colorRole = UI::ColorRole::Checked;
            UI::MetricsRole sizeRole = UI::MetricsRole::Slider;
        };

        void ThermometerWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);

            setClassName("djv::UIComponents::ThermometerWidget");
            setBackgroundColorRole(UI::ColorRole::Trough);
        }

        ThermometerWidget::ThermometerWidget() :
            _p(new Private)
        {}

        ThermometerWidget::~ThermometerWidget()
        {}

        std::shared_ptr<ThermometerWidget> ThermometerWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ThermometerWidget>(new ThermometerWidget);
            out->_init(context);
            return out;
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

        UI::Orientation ThermometerWidget::getOrientation() const
        {
            return _p->orientation;
        }

        UI::ColorRole ThermometerWidget::getColorRole() const
        {
            return _p->colorRole;
        }

        UI::MetricsRole ThermometerWidget::getSizeRole() const
        {
            return _p->sizeRole;
        }

        void ThermometerWidget::setOrientation(UI::Orientation value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.orientation)
                return;
            p.orientation = value;
            _resize();
        }

        void ThermometerWidget::setColorRole(UI::ColorRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.colorRole)
                return;
            p.colorRole = value;
            _resize();
        }

        void ThermometerWidget::setSizeRole(UI::MetricsRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.sizeRole)
                return;
            p.sizeRole = value;
            _resize();
        }

        void ThermometerWidget::_preLayoutEvent(System::Event::PreLayout& event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float sr = style->getMetric(p.sizeRole);
            const float hw = style->getMetric(UI::MetricsRole::Handle);
            glm::vec2 size(0.F, 0.F);
            switch (p.orientation)
            {
            case UI::Orientation::Horizontal: size = glm::vec2(sr, hw); break;
            case UI::Orientation::Vertical:   size = glm::vec2(hw, sr); break;
            default: break;
            }
            _setMinimumSize(size);
        }

        void ThermometerWidget::_layoutEvent(System::Event::Layout&)
        {}

        void ThermometerWidget::_paintEvent(System::Event::Paint& event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const Math::BBox2f& g = getMargin().bbox(getGeometry(), style);
            const auto& render = _getRender();
            render->setFillColor(style->getColor(p.colorRole));
            switch (p.orientation)
            {
            case UI::Orientation::Horizontal:
            {
                const float w = p.percentage / 100.F * g.w();
                render->drawRect(Math::BBox2f(g.min.x, g.min.y, w, g.h()));
                break;
            }
            case UI::Orientation::Vertical:
            {
                const float h = p.percentage / 100.F * g.h();
                render->drawRect(Math::BBox2f(g.min.x, g.max.y - h, g.w(), h));
                break;
            }
            default: break;
            }
        }

    } // namespace UIComponents
} // namespace djv

