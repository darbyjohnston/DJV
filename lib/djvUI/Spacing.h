// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Enum.h>
#include <djvUI/Style.h>

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            //! This class provides spacing information.
            class Spacing
            {
            public:
                Spacing();
                Spacing(MetricsRole);
                Spacing(MetricsRole horizontal, MetricsRole vertical);

                void set(MetricsRole);
                void set(MetricsRole horizontal, MetricsRole vertical);

                glm::vec2 get(const std::shared_ptr<Style::Style> &) const;
                float get(Orientation, const std::shared_ptr<Style::Style> &) const;

                MetricsRole operator [] (Orientation) const;
                MetricsRole & operator [] (Orientation);

                bool operator == (const Spacing &) const;

            private:
                MetricsRole _value[2] =
                {
                    MetricsRole::None,
                    MetricsRole::None
                };
            };

        } // namespace Layout
    } // namespace UI
} // namespace djv

#include <djvUI/SpacingInline.h>

