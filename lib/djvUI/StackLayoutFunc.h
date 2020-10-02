// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Margin.h>

#include <vector>

namespace djv
{
    namespace UI
    {
        class Widget;

        namespace Layout
        {
            //! \name Stack Layout
            ///@{

            float stackHeightForWidth(
                    float,
                    const std::vector<std::shared_ptr<Widget> >&,
                    const Margin&,
                    const std::shared_ptr<Style::Style>&);

            glm::vec2 stackMinimumSize(
                const std::vector<std::shared_ptr<Widget> >&,
                const Margin&,
                const std::shared_ptr<Style::Style>&);

            void stackLayout(
                const Math::BBox2f&,
                const std::vector<std::shared_ptr<Widget> >&,
                const Margin&,
                const std::shared_ptr<Style::Style>&);

            ///@}

        } // namespace Layout
    } // namespace UI
} // namespace djv
