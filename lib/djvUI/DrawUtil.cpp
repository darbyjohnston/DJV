// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/DrawUtil.h>

#include <djvAV/Render2D.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        void drawBorder(const std::shared_ptr<AV::Render2D::Render>& render, const Core::BBox2f& box, float border)
        {
            render->drawRects(
                {
                    BBox2f(
                        glm::vec2(box.min.x, box.min.y),
                        glm::vec2(box.max.x, box.min.y + border)),
                    BBox2f(
                        glm::vec2(box.min.x, box.max.y - border),
                        glm::vec2(box.max.x, box.max.y)),
                    BBox2f(
                        glm::vec2(box.min.x, box.min.y + border),
                        glm::vec2(box.min.x + border, box.max.y - border)),
                    BBox2f(
                        glm::vec2(box.max.x - border, box.min.y + border),
                        glm::vec2(box.max.x, box.max.y - border))
                });
        }

    } // namespace UI
} // namespace djv
