// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            inline Spacing::Spacing()
            {}

            inline Spacing::Spacing(MetricsRole value)
            {
                set(value);
            }

            inline Spacing::Spacing(MetricsRole horizontal, MetricsRole vertical)
            {
                set(horizontal, vertical);
            }

            inline void Spacing::set(MetricsRole value)
            {
                _value[static_cast<size_t>(Orientation::Horizontal)] = value;
                _value[static_cast<size_t>(Orientation::Vertical)] = value;
            }

            inline void Spacing::set(MetricsRole horizontal, MetricsRole vertical)
            {
                _value[static_cast<size_t>(Orientation::Horizontal)] = horizontal;
                _value[static_cast<size_t>(Orientation::Vertical)] = vertical;
            }

            inline glm::vec2 Spacing::get(const std::shared_ptr<Style::Style> & style) const
            {
                return glm::vec2(get(Orientation::Horizontal, style), get(Orientation::Vertical, style));
            }

            inline float Spacing::get(Orientation value, const std::shared_ptr<Style::Style> & style) const
            {
                return style->getMetric(_value[static_cast<size_t>(value)]);
            }

            inline MetricsRole Spacing::operator [] (Orientation value) const
            {
                return _value[static_cast<size_t>(value)];
            }

            inline MetricsRole & Spacing::operator [] (Orientation value)
            {
                return _value[static_cast<size_t>(value)];
            }

            inline bool Spacing::operator == (const Spacing & other) const
            {
                return
                    _value[static_cast<size_t>(Orientation::Horizontal)] == other._value[static_cast<size_t>(Orientation::Horizontal)] &&
                    _value[static_cast<size_t>(Orientation::Vertical)] == other._value[static_cast<size_t>(Orientation::Vertical)];
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv

