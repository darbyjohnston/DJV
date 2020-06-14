// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/Style.h>

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            inline Margin::Margin()
            {}

            inline Margin::Margin(MetricsRole value)
            {
                set(value);
            }

            inline Margin::Margin(MetricsRole left, MetricsRole right, MetricsRole top, MetricsRole bottom)
            {
                set(left, right, top, bottom);
            }

            inline void Margin::set(MetricsRole value)
            {
                _value[static_cast<size_t>(Side::Left)] = value;
                _value[static_cast<size_t>(Side::Top)] = value;
                _value[static_cast<size_t>(Side::Right)] = value;
                _value[static_cast<size_t>(Side::Bottom)] = value;
            }

            inline void Margin::set(MetricsRole left, MetricsRole right, MetricsRole top, MetricsRole bottom)
            {
                _value[static_cast<size_t>(Side::Left)] = left;
                _value[static_cast<size_t>(Side::Top)] = top;
                _value[static_cast<size_t>(Side::Right)] = right;
                _value[static_cast<size_t>(Side::Bottom)] = bottom;
            }

            inline float Margin::get(Side side, const std::shared_ptr<Style::Style>& style) const
            {
                return style->getMetric(_value[static_cast<size_t>(side)]);
            }
            
            inline glm::vec2 Margin::getSize(const std::shared_ptr<Style::Style>& style) const
            {
                return glm::vec2(getWidth(style), getHeight(style));
            }

            inline float Margin::getWidth(const std::shared_ptr<Style::Style>& style) const
            {
                return get(Side::Left, style) + get(Side::Right, style);
            }

            inline float Margin::getHeight(const std::shared_ptr<Style::Style>& style) const
            {
                return get(Side::Top, style) + get(Side::Bottom, style);
            }

            inline Core::BBox2f Margin::bbox(const Core::BBox2f& value, const std::shared_ptr<Style::Style>& style) const
            {
                return value.margin(
                    -get(Side::Left, style),
                    -get(Side::Top, style),
                    -get(Side::Right, style),
                    -get(Side::Bottom, style));
            }

            inline MetricsRole Margin::operator [] (Side side) const
            {
                return _value[static_cast<size_t>(side)];
            }

            inline MetricsRole& Margin::operator [] (Side side)
            {
                return _value[static_cast<size_t>(side)];
            }

            inline bool Margin::operator == (const Margin& other) const
            {
                return
                    _value[static_cast<size_t>(Side::Left)] == other._value[static_cast<size_t>(Side::Left)] &&
                    _value[static_cast<size_t>(Side::Top)] == other._value[static_cast<size_t>(Side::Top)] &&
                    _value[static_cast<size_t>(Side::Right)] == other._value[static_cast<size_t>(Side::Right)] &&
                    _value[static_cast<size_t>(Side::Bottom)] == other._value[static_cast<size_t>(Side::Bottom)];
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
