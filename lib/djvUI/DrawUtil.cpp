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
        void drawBorder(
            const std::shared_ptr<AV::Render2D::Render>& render,
            const Core::BBox2f& box,
            float width)
        {
            render->drawRects(
                {
                    BBox2f(
                        glm::vec2(box.min.x, box.min.y),
                        glm::vec2(box.max.x, box.min.y + width)),
                    BBox2f(
                        glm::vec2(box.min.x, box.max.y - width),
                        glm::vec2(box.max.x, box.max.y)),
                    BBox2f(
                        glm::vec2(box.min.x, box.min.y + width),
                        glm::vec2(box.min.x + width, box.max.y - width)),
                    BBox2f(
                        glm::vec2(box.max.x - width, box.min.y + width),
                        glm::vec2(box.max.x, box.max.y - width))
                });
        }

        void drawCheckers(
            const std::shared_ptr<AV::Render2D::Render>& render,
            const Core::BBox2f& box,
            float size,
            const AV::Image::Color& color0,
            const AV::Image::Color& color1)
        {
            const size_t columns = static_cast<size_t>(ceilf((box.max.x - box.min.x) / size));
            const size_t rows = static_cast<size_t>(ceilf((box.max.y - box.min.y) / size));
            for (size_t j = 0; j < rows; ++j)
            {
                for (size_t i = 0; i < columns; ++i)
                {
                    if (0 == ((j + i) % 2))
                    {
                        render->setFillColor(color0);
                    }
                    else
                    {
                        render->setFillColor(color1);
                    }
                    render->drawRect(BBox2f(
                        floorf(box.min.x + i * size),
                        floorf(box.min.y + j * size),
                        ceilf(size),
                        ceilf(size)).intersect(box));
                }
            }
        }

    } // namespace UI
} // namespace djv
