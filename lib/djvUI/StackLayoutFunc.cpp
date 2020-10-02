// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUI/StackLayoutFunc.h>

#include <djvUI/LayoutUtil.h>
#include <djvUI/Widget.h>

#include <glm/glm.hpp>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            float stackHeightForWidth(float value, const std::vector<std::shared_ptr<Widget> >& children, const Layout::Margin& margin, const std::shared_ptr<Style::Style>& style)
            {
                float out = 0.F;
                const glm::vec2 m = margin.getSize(style);
                for (const auto& child : children)
                {
                    if (child->isVisible())
                    {
                        out = glm::max(out, child->getHeightForWidth(value - m.x));
                    }
                }
                out += m.y;
                return out;
            }

            glm::vec2 stackMinimumSize(const std::vector<std::shared_ptr<Widget> >& children, const Layout::Margin& margin, const std::shared_ptr<Style::Style>& style)
            {
                glm::vec2 minimumSize = glm::vec2(0.F, 0.F);
                for (const auto& child : children)
                {
                    if (child->isVisible())
                    {
                        minimumSize = glm::max(minimumSize, child->getMinimumSize());
                    }
                }
                return minimumSize + margin.getSize(style);
            }

            void stackLayout(const Math::BBox2f& geometry, const std::vector<std::shared_ptr<Widget> >& children, const Layout::Margin& margin, const std::shared_ptr<Style::Style>& style)
            {
                const Math::BBox2f& g = margin.bbox(geometry, style);
                for (const auto& child : children)
                {
                    const Math::BBox2f childGeometry = getAlign(g, child->getMinimumSize(), child->getHAlign(), child->getVAlign());
                    child->setGeometry(childGeometry);
                }
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
