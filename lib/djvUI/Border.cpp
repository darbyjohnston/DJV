// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/Border.h>

#include <djvUI/DrawUtil.h>
#include <djvUI/StackLayout.h>
#include <djvUI/Style.h>

#include <djvRender2D/Render.h>

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

            void Border::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                setClassName("djv::UI::Layout::Border");
            }

            Border::Border() :
                _p(new Private)
            {}

            Border::~Border()
            {}

            std::shared_ptr<Border> Border::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Border>(new Border);
                out->_init(context);
                return out;
            }

            MetricsRole Border::getBorderSize() const
            {
                return _p->borderSize;
            }

            ColorRole Border::getBorderColorRole() const
            {
                return _p->borderColor;
            }

            void Border::setBorderSize(MetricsRole value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.borderSize)
                    return;
                p.borderSize = value;
                _resize();
            }

            void Border::setBorderColorRole(ColorRole value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.borderColor)
                    return;
                p.borderColor = value;
                _redraw();
            }

            const Margin& Border::getInsideMargin() const
            {
                return _p->insideMargin;
            }

            void Border::setInsideMargin(const Margin& value)
            {
                if (value == _p->insideMargin)
                    return;
                _p->insideMargin = value;
                _resize();
            }

            float Border::getHeightForWidth(float value) const
            {
                DJV_PRIVATE_PTR();
                float out = 0.F;
                const auto& style = _getStyle();
                const glm::vec2 m = getMargin().getSize(style);
                const float b = style->getMetric(p.borderSize);
                out = StackLayout::heightForWidth(value - b * 2.F - m.x, getChildWidgets(), p.insideMargin, style) + b * 2.F + m.y;
                return out;
            }

            void Border::_preLayoutEvent(System::Event::PreLayout& event)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const glm::vec2 m = getMargin().getSize(style);
                const float b = style->getMetric(p.borderSize);
                _setMinimumSize(StackLayout::minimumSize(getChildWidgets(), p.insideMargin, style) + b * 2.F + m);
            }

            void Border::_layoutEvent(System::Event::Layout& event)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const float b = style->getMetric(p.borderSize);
                const Math::BBox2f& g = getGeometry().margin(-b);
                StackLayout::layout(getMargin().bbox(g, style), getChildWidgets(), p.insideMargin, style);
            }

            void Border::_paintEvent(System::Event::Paint& event)
            {
                Widget::_paintEvent(event);
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const Math::BBox2f& g = getMargin().bbox(getGeometry(), style);
                const float b = style->getMetric(p.borderSize);
                const auto& render = _getRender();
                render->setFillColor(style->getColor(p.borderColor));
                drawBorder(render, g, b);
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
