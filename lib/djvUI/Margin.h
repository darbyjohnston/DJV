// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Enum.h>

#include <djvCore/BBox.h>

#include <memory>

namespace djv
{
    namespace UI
    {
        namespace Style
        {
            class Style;
            
        } // namespace Style
        
        namespace Layout
        {
            //! This class provides margins.
            class Margin
            {
            public:
                Margin();
                Margin(MetricsRole);
                //! \todo Change the ordering to match the Side enum? (left, top, right, bottom)
                Margin(MetricsRole left, MetricsRole right, MetricsRole top, MetricsRole bottom);

                void set(MetricsRole);
                void set(MetricsRole left, MetricsRole right, MetricsRole top, MetricsRole bottom);

                float get(Side, const std::shared_ptr<Style::Style>&) const;
                glm::vec2 getSize(const std::shared_ptr<Style::Style>&) const;
                float getWidth(const std::shared_ptr<Style::Style>&) const;
                float getHeight(const std::shared_ptr<Style::Style>&) const;

                Core::BBox2f bbox(const Core::BBox2f&, const std::shared_ptr<Style::Style>&) const;

                MetricsRole operator [] (Side) const;
                MetricsRole& operator [] (Side);

                bool operator == (const Margin&) const;

            private:
                MetricsRole _value[4] =
                {
                    MetricsRole::None,
                    MetricsRole::None,
                    MetricsRole::None,
                    MetricsRole::None
                };
            };

        } // namespace Layout
    } // namespace UI
} // namespace djv

#include <djvUI/MarginInline.h>
