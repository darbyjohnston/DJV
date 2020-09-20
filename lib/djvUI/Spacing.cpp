// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUI/Spacing.h>

#include <djvUI/Style.h>

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            Spacing::Spacing()
            {}

            Spacing::Spacing(MetricsRole value)
            {
                set(value);
            }

            Spacing::Spacing(MetricsRole horizontal, MetricsRole vertical)
            {
                set(horizontal, vertical);
            }

            void Spacing::set(MetricsRole value)
            {
                _value[static_cast<size_t>(Orientation::Horizontal)] = value;
                _value[static_cast<size_t>(Orientation::Vertical)]   = value;
            }

            void Spacing::set(MetricsRole horizontal, MetricsRole vertical)
            {
                _value[static_cast<size_t>(Orientation::Horizontal)] = horizontal;
                _value[static_cast<size_t>(Orientation::Vertical)]   = vertical;
            }

            glm::vec2 Spacing::get(const std::shared_ptr<Style::Style>& style) const
            {
                return glm::vec2(get(Orientation::Horizontal, style), get(Orientation::Vertical, style));
            }

            float Spacing::get(Orientation value, const std::shared_ptr<Style::Style>& style) const
            {
                return style->getMetric(_value[static_cast<size_t>(value)]);
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv

