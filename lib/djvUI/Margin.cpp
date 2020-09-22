// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUI/Margin.h>

#include <djvUI/Style.h>

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            Margin::Margin()
            {}

            Margin::Margin(MetricsRole value)
            {
                set(value);
            }

            Margin::Margin(MetricsRole left, MetricsRole right, MetricsRole top, MetricsRole bottom)
            {
                set(left, right, top, bottom);
            }
            
            void Margin::set(MetricsRole value)
            {
                _value[static_cast<size_t>(Side::Left)]   = value;
                _value[static_cast<size_t>(Side::Top)]    = value;
                _value[static_cast<size_t>(Side::Right)]  = value;
                _value[static_cast<size_t>(Side::Bottom)] = value;
            }

            void Margin::set(MetricsRole left, MetricsRole right, MetricsRole top, MetricsRole bottom)
            {
                _value[static_cast<size_t>(Side::Left)]   = left;
                _value[static_cast<size_t>(Side::Top)]    = top;
                _value[static_cast<size_t>(Side::Right)]  = right;
                _value[static_cast<size_t>(Side::Bottom)] = bottom;
            }

            float Margin::get(Side side, const std::shared_ptr<Style::Style>& style) const
            {
                return style->getMetric(_value[static_cast<size_t>(side)]);
            }
            
            glm::vec2 Margin::getSize(const std::shared_ptr<Style::Style>& style) const
            {
                return glm::vec2(getWidth(style), getHeight(style));
            }

            float Margin::getWidth(const std::shared_ptr<Style::Style>& style) const
            {
                return get(Side::Left, style) + get(Side::Right, style);
            }

            float Margin::getHeight(const std::shared_ptr<Style::Style>& style) const
            {
                return get(Side::Top, style) + get(Side::Bottom, style);
            }
            
            Math::BBox2f Margin::bbox(const Math::BBox2f& value, const std::shared_ptr<Style::Style>& style) const
            {
                return value.margin(
                    -get(Side::Left, style),
                    -get(Side::Top, style),
                    -get(Side::Right, style),
                    -get(Side::Bottom, style));
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
